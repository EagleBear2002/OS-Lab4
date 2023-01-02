
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* EXTERN is defined as extern except in global.c */
#ifdef    GLOBAL_VARIABLES_HERE
#undef    EXTERN
#define    EXTERN
#endif

EXTERN    int ticks;
EXTERN    int disp_pos;
EXTERN    u8 gdt_ptr[6];    // 0~15:Limit  16~47:Base
EXTERN    DESCRIPTOR gdt[GDT_SIZE];
EXTERN    u8 idt_ptr[6];    // 0~15:Limit  16~47:Base
EXTERN    GATE idt[IDT_SIZE];

EXTERN    u32 k_reenter;

EXTERN    TSS tss;
EXTERN    PROCESS *p_proc_ready;

// TODO: added here
EXTERN  int readers;
EXTERN  int writers;
EXTERN  int writing;
EXTERN  int tr;
EXTERN    int nr_current_console;

extern PROCESS proc_table[];
extern char task_stack[];
extern TASK task_table[];
extern irq_handler irq_table[];

// TODO: added here
extern TASK user_proc_table[];
extern TTY tty_table[];
extern CONSOLE console_table[];

extern SEMAPHORE rw_mutex;
extern SEMAPHORE w_mutex;
extern SEMAPHORE r_mutex;
extern SEMAPHORE queue;
extern SEMAPHORE n_r_mutex;

