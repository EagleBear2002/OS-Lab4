
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               proc.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef _ORANGE_PROC_H
#define _ORANGE_PROC_H

/* 进程表栈结构 */
typedef struct s_stackframe {	/* proc_ptr points here				↑ Low			*/
	u32	gs;		/* ┓						│			*/
	u32	fs;		/* ┃						│			*/
	u32	es;		/* ┃						│			*/
	u32	ds;		/* ┃						│			*/
	u32	edi;		/* ┃						│			*/
	u32	esi;		/* ┣ pushed by save()				│			*/
	u32	ebp;		/* ┃						│			*/
	u32	kernel_esp;	/* <- 'popad' will ignore it			│			*/
	u32	ebx;		/* ┃						↑栈从高地址往低地址增长*/		
	u32	edx;		/* ┃						│			*/
	u32	ecx;		/* ┃						│			*/
	u32	eax;		/* ┛						│			*/
	u32	retaddr;	/* return address for assembly code save()	│			*/
	u32	eip;		/*  ┓						│			*/
	u32	cs;		/*  ┃						│			*/
	u32	eflags;		/*  ┣ these are pushed by CPU during interrupt	│			*/
	u32	esp;		/*  ┃						│			*/
	u32	ss;		/*  ┛						┷High			*/
}STACK_FRAME;

#define SEMAPHORE_SIZE 32

typedef struct semaphore SEMAPHORE;

/* 进程表结构 */
typedef struct s_proc {
	STACK_FRAME	regs;				/* process' registers saved in stack frame */

	u16			ldt_sel;			/* selector in gdt giving ldt base and limit*/
	DESCRIPTOR	ldts[LDT_SIZE];		/* local descriptors for code and data */
											/* 2 is LDT_SIZE - avoid include protect.h */
	
	int 		ready_tick;			/* 进程可以再次就绪的时间  */
	SEMAPHORE*	waiting_semahore;	/* 进程等待的信号量  */

	u32			pid;				/* process id passed in from MM */
	char		p_name[16];			/* name of the process */
}PROCESS;

/* 一个进程需要一个进程体和堆栈 */
typedef struct s_task {
	task_f	initial_eip;
	int		stacksize;
	char	name[32];
}TASK;

typedef struct semaphore{
	int 		number;			// 值数量
	PROCESS* 	list[SEMAPHORE_SIZE];
	int 		start;
	int 		end;
} SEMAPHORE;

/* Number of tasks */
#define NR_TASKS	6

/* stacks of tasks 定义任务栈的大小 */
#define STACK_SIZE_ReaderA	0x8000
#define STACK_SIZE_ReaderB	0x8000
#define STACK_SIZE_ReaderC	0x8000
#define STACK_SIZE_WriterD	0x8000
#define STACK_SIZE_WriterE	0x8000
#define STACK_SIZE_F		0x8000

#define STACK_SIZE_TOTAL	(STACK_SIZE_ReaderA + \
				STACK_SIZE_ReaderB + \
				STACK_SIZE_ReaderC + \
				STACK_SIZE_WriterD + \
				STACK_SIZE_WriterE + \
				STACK_SIZE_F)

#endif