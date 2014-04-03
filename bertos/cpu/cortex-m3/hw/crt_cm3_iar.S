  MODULE  ?cstartup
  
CONTROL_UNPRIVILEGED SET 1
CONTROL_PSP SET 2
  
  AAPCS INTERWORK, VFP_COMPATIBLE, ROPI
  PRESERVE8

  SECTION .vtable:CODE:NOROOT(3)

  RSEG IRQ_STACK:DATA(3)
  RSEG CSTACK:DATA(3)
  RSEG DATABSS:DATA(3)

  EXTERN  __cmain
  EXTERN __init2
  EXTERN  __region_RAM_end__
  PUBLIC  __iar_program_start
  
  SECTION .text:CODE:REORDER(2)
  
  PUBWEAK __dummy_init
  __dummy_init:
  	bx lr
  
  THUMB
  __iar_program_start:
  	cpsid i
  	ldr     r0, =__region_RAM_end__
        sub     r0, r0, #16
  	msr     psp, r0
  
  	movs r0, #CONTROL_PSP
  	msr control, r0
  	isb
  
  	bl __init2
  
  	cpsie i
  	mov r0, #0
  	mov r1, #0
  	bl __cmain
  end:
  	wfi
  	b end

  END
