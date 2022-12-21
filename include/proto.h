
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            proto.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "proc.h"

/* klib.asm */
PUBLIC void	out_byte(u16 port, u8 value);
PUBLIC u8	in_byte(u16 port);
PUBLIC void	disp_str(char * info);
PUBLIC void	disp_color_str(char * info, int color);

/* protect.c */
PUBLIC void	init_prot();
PUBLIC u32	seg2phys(u16 seg);

/* klib.c */
PUBLIC void	delay(int time);

/* kernel.asm */
void restart();

/* main.c 加入函数声明 */
void ReaderA();
void ReaderB();
void ReaderC();
void WriterD();
void WriterE();
void F();

/* i8259.c */
PUBLIC void put_irq_handler(int irq, irq_handler handler);
PUBLIC void spurious_irq(int irq);

/* clock.c */
PUBLIC void clock_handler(int irq);
PUBLIC void milli_delay(int milli_sec);

/* proc.c */
PUBLIC  int     sys_get_ticks();        /* sys_call */
PUBLIC  void    sys_delay_c(int i);
PUBLIC  void    do_sys_p(SEMAPHORE *);
PUBLIC  void    do_sys_v(SEMAPHORE *);

/* syscall.asm 系统调用 */
PUBLIC  void    sys_call();             /* int_handler */
PUBLIC  int     get_ticks();
PUBLIC  void    sys_p(SEMAPHORE *);
PUBLIC  void    sys_v(SEMAPHORE *);
PUBLIC  void    delay_sys(int milli_seconds);
PUBLIC  void    disp_str_sys(char *);

PUBLIC  void    p_sys(SEMAPHORE *);
PUBLIC  void    v_sys(SEMAPHORE *);
PUBLIC  void    sys_delay(int i);
PUBLIC  void    sys_disp_str(char *);

/* main.c */
PUBLIC  void    clear();