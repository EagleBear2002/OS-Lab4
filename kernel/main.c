
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            main.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"

char colors[3] = {'\01', '\03', '\02'};
char signs[3] = {'X', 'Z', 'O'};

PRIVATE void init_tasks() {
	init_screen(tty_table);
	clean(console_table);
	
	// 表驱动，对应进程 0, 1, 2, 3, 4, 5, 6
	int prior[7] = {1, 1, 1, 1, 1, 1, 1};
	for (int i = 0; i < 7; ++i) {
		proc_table[i].ticks = prior[i];
		proc_table[i].priority = prior[i];
	}
	
	// initialization
	k_reenter = 0;
	ticks = 0;
	readers = 0;
	
	p_proc_ready = proc_table;
}

/*======================================================================*
                            kernel_main
 *======================================================================*/
PUBLIC int kernel_main() {
	disp_str("-----\"kernel_main\" begins-----\n");
	
	TASK *p_task = task_table;
	PROCESS *p_proc = proc_table;
	char *p_task_stack = task_stack + STACK_SIZE_TOTAL;
	u16 selector_ldt = SELECTOR_LDT_FIRST;
	u8 privilege;
	u8 rpl;
	int eflags;
	for (int i = 0; i < NR_TASKS + NR_PROCS; i++) {
		if (i < NR_TASKS) {     /* 任务 */
			p_task = task_table + i;
			privilege = PRIVILEGE_TASK;
			rpl = RPL_TASK;
			eflags = 0x1202; /* IF=1, IOPL=1, bit 2 is always 1 */
		} else {                  /* 用户进程 */
			p_task = user_proc_table + (i - NR_TASKS);
			privilege = PRIVILEGE_USER;
			rpl = RPL_USER;
			eflags = 0x202; /* IF=1, bit 2 is always 1 */
		}
		
		strcpy(p_proc->p_name, p_task->name);    // name of the process
		p_proc->pid = i;            // pid
		p_proc->sleeping = 0; // 初始化结构体新增成员
		p_proc->blocked = FALSE;
		p_proc->status = RELAXING;
		
		p_proc->ldt_sel = selector_ldt;
		
		memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3], sizeof(DESCRIPTOR));
		p_proc->ldts[0].attr1 = DA_C | privilege << 5;
		memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3], sizeof(DESCRIPTOR));
		p_proc->ldts[1].attr1 = DA_DRW | privilege << 5;
		p_proc->regs.cs = (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.ds = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.es = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.fs = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.ss = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;
		
		p_proc->regs.eip = (u32) p_task->initial_eip;
		p_proc->regs.esp = (u32) p_task_stack;
		p_proc->regs.eflags = eflags;
		
		p_proc->nr_tty = 0;
		
		p_task_stack -= p_task->stacksize;
		p_proc++;
		p_task++;
		selector_ldt += 1 << 3;
	}
	
	init_tasks();
	
	init_clock();
	init_keyboard();
	
	restart();
	
	while (1) {}
}

PRIVATE void read_proc(int slices) {
	p_proc_ready->status = WORKING;
	sleep_ms(slices * TIME_SLICE); // 读耗时 slices个时间片
}

PRIVATE void write_proc(int slices) {
	p_proc_ready->status = WORKING;
	sleep_ms(slices * TIME_SLICE); // 写耗时 slices个时间片
}

// 读写公平方案
void read_fair(int slices) {
	P(&S);
	
	P(&reader_count_mutex);
	P(&reader_mutex);
	if (++readers == 1)
		P(&rw_mutex); // 有读者，禁止写
	V(&reader_mutex);
	
	V(&S);
	
	read_proc(slices);
	
	P(&reader_mutex);
	if (--readers == 0)
		V(&rw_mutex); // 没有读者，可以开始写了
	V(&reader_mutex);
	V(&reader_count_mutex);
}

void write_fair(int slices) {
	P(&S);
	P(&rw_mutex);
	write_proc(slices);
	V(&rw_mutex);
	V(&S);
}

// 读者优先
void read_rf(int slices) {
	P(&reader_mutex);
	if (++readers == 1)
		P(&writer_mutex);
	V(&reader_mutex);
	
	P(&reader_count_mutex);
	read_proc(slices);
	V(&reader_count_mutex);
	
	P(&reader_mutex);
	if (--readers == 0)
		V(&writer_mutex);
	V(&reader_mutex);
}

void write_rf(int slices) {
	P(&rw_mutex);
	P(&writer_mutex);
	write_proc(slices);
	V(&writer_mutex);
	V(&rw_mutex);
}

// 写者优先
void read_wf(int slices) {
	P(&reader_count_mutex);
	
	P(&S);
	P(&reader_mutex);
	if (++readers == 1)
		P(&rw_mutex); // 有读者时不允许写
	V(&reader_mutex);
	V(&S);
	
	read_proc(slices);
	
	P(&reader_mutex);
	if (--readers == 0)
		V(&rw_mutex); // 没有读者时可以开始写
	V(&reader_mutex);
	
	V(&reader_count_mutex);
}

void write_wf(int slices) {
	P(&writer_mutex);
	if (++writers == 1)
		P(&S);
	V(&writer_mutex);
	
	P(&rw_mutex);
	write_proc(slices);
	V(&rw_mutex);
	
	P(&writer_mutex);
	if (--writers == 0)
		V(&S);
	V(&writer_mutex);
}

read_f read_funcs[3] = {read_rf, read_wf, read_fair};
write_f write_funcs[3] = {write_rf, write_wf, write_fair};

void ReaderB() {
	sleep_ms(3 * TIME_SLICE);
	while (1) {
		read_funcs[STRATEGY](WORKING_SLICES_B);
		p_proc_ready->status = RELAXING;
		sleep_ms(RELAX_SLICES_B * TIME_SLICE);
		p_proc_ready->status = WAITING;
	}
}

void ReaderC() {
	sleep_ms(30 * TIME_SLICE);
	while (1) {
		read_funcs[STRATEGY](WORKING_SLICES_C);
		p_proc_ready->status = RELAXING;
		sleep_ms(RELAX_SLICES_C * TIME_SLICE);
		p_proc_ready->status = WAITING;
	}
}

void ReaderD() {
	sleep_ms(30 * TIME_SLICE);
	while (1) {
		read_funcs[STRATEGY](WORKING_SLICES_D);
		p_proc_ready->status = RELAXING;
		sleep_ms(RELAX_SLICES_D * TIME_SLICE);
		p_proc_ready->status = WAITING;
	}
}

void WriterE() {
//	sleep_ms(1 * TIME_SLICE);
	while (1) {
		write_funcs[STRATEGY](WORKING_SLICES_E);
		p_proc_ready->status = RELAXING;
		sleep_ms(RELAX_SLICES_E * TIME_SLICE);
		p_proc_ready->status = WAITING;
	}
}

void WriterF() {
	sleep_ms(2 * TIME_SLICE);
	while (1) {
		write_funcs[STRATEGY](WORKING_SLICES_F);
		p_proc_ready->status = RELAXING;
		sleep_ms(RELAX_SLICES_F * TIME_SLICE);
		p_proc_ready->status = WAITING;
	}
}

void ReporterA() {
	sleep_ms(TIME_SLICE);

#if STRATEGY == READ_FIRST
//	printf("strategy: Half-Reader First\n");
	printf("strategy: Full-Reader First\n");
#elif STRATEGY == WRITE_FIRST
	printf("strategy: writer first\n");
#elif STRATEGY == FAIR
	printf("strategy: fair\n");
#endif
	
	printf("count of max reader: %d\n", MAX_READERS);
//	printf("relax slices of B: %d\n", RELAX_SLICES_B);
//	printf("relax slices of C: %d\n", RELAX_SLICES_C);
//	printf("relax slices of D: %d\n", RELAX_SLICES_D);
//	printf("relax slices of E: %d\n", RELAX_SLICES_E);
//	printf("relax slices of F: %d\n", RELAX_SLICES_F);
	
	int time_stamp = 0;
	while (++time_stamp <= 20) {
		printf("%c%d%d ", '\06', time_stamp / 10, time_stamp % 10);
		for (int i = NR_TASKS + 1; i < NR_PROCS + NR_TASKS; i++) {
			int proc_status = (proc_table + i)->status;
			printf("%c%c ", colors[proc_status], signs[proc_status]);
		}
		printf("\n");
		sleep_ms(TIME_SLICE);
	}
	while (1);
}