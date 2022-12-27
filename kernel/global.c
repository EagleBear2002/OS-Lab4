
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define GLOBAL_VARIABLES_HERE

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "proc.h"
#include "global.h"

/* 进程表 */
PUBLIC    PROCESS proc_table[NR_TASKS];

PUBLIC    char task_stack[STACK_SIZE_TOTAL];

/* TODO: fixed 任务列表 */
PUBLIC    TASK task_table[NR_TASKS] = {
		{ReaderA, STACK_SIZE_ReaderA, "ReaderA"},
		{ReaderB, STACK_SIZE_ReaderB, "ReaderB"},
		{ReaderC, STACK_SIZE_ReaderC, "ReaderC"},
		{WriterD, STACK_SIZE_WriterD, "WriterD"},
		{WriterE, STACK_SIZE_WriterD, "WriterE"},
		{F,       STACK_SIZE_F,       "F"}};

PUBLIC    irq_handler irq_table[NR_IRQ];

PUBLIC    system_call sys_call_table[NR_SYS_CALL] = {
		sys_get_ticks,
		sys_disp_str,
		sys_p,
		sys_v,
		sys_delay
};
