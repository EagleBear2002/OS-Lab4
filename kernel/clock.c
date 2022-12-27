
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               clock.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "string.h"
#include "proc.h"
#include "global.h"

/*======================================================================*
                           clock_handler
 *======================================================================*/
// TODO: fixed
PUBLIC void clock_handler(int irq) {
	// 时钟中断处理程序
	ticks++;
	
	if (k_reenter != 0) {
		// 处理中断重入
		return;
	}
	
	schedule();
	
	if (disp_number >= 26) {
		clear();
	}
	
}


/*======================================================================*
                              milli_delay
 *======================================================================*/
PUBLIC void milli_delay(int milli_sec) {
	// 使用系统调用完成系统延时，空转
	int t = get_ticks();
	
	while (((get_ticks() - t) * 1000 / HZ) < milli_sec) {}
}
