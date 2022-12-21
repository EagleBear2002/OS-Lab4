
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                               syscall.asm
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                                                     Forrest Yu, 2005
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

%include "sconst.inc"

extern disp_str
extern do_sys_p
extern do_sys_v
extern sys_delay_c

_NR_get_ticks       equ 0 ; 要跟 global.c 中 sys_call_table 的定义相对应！
_NR_disp_str_sys	equ 1 
_NR_p 				equ 2
_NR_v				equ 3
_NR_sys_delay		equ 4

INT_VECTOR_SYS_CALL equ 0x90

; 导出符号
global	get_ticks

global 	disp_str_sys
global 	sys_disp_str

global 	p_sys
global 	v_sys
global 	sys_p
global 	sys_v

global 	delay_sys
global 	sys_delay

bits 32
[section .text]

; ====================================================================
;                              get_ticks
; ====================================================================
get_ticks:
	mov	eax, _NR_get_ticks
	int	INT_VECTOR_SYS_CALL
	ret

disp_str_sys:
	; 准备好现场，准备发起中断
	mov eax, _NR_disp_str_sys
	push ebx
	mov ebx, [esp+8]
	int INT_VECTOR_SYS_CALL
	pop ebx
	ret

sys_disp_str:
	; 具体执行中断内容
	pusha 
	push ebx
	call disp_str
	pop ebx
	popa
	ret

p_sys:
	; 准备好现场，准备发起中断
	mov eax, _NR_p
	push ebx
	mov ebx, [esp+8]
	int INT_VECTOR_SYS_CALL
	pop ebx
	ret

sys_p:
	; 信号量执行P操作
	pusha
	push ebx
	call do_sys_p
	pop ebx
	popa
	ret

v_sys:
	; 准备好现场，准备发起中断
	mov eax, _NR_v
	push ebx
	mov ebx, [esp+8]
	int INT_VECTOR_SYS_CALL
	pop ebx
	ret

sys_v:
	; 信号量执行V操作
	pusha
	push ebx
	call do_sys_v
	pop ebx
	popa
	ret

delay_sys:
	; 准备好现场，准备发起中断
	mov eax, _NR_sys_delay
	push ebx
	mov ebx, [esp+8]
	int INT_VECTOR_SYS_CALL
	pop ebx
	ret

sys_delay:
	push ebx
	call sys_delay_c
	pop ebx
	ret
