  SECTION .text:CODE(2)
  
  ; Exported functions
  EXPORT CPU_READ_IPSR
  EXPORT irq_running
  EXPORT asm_switch_context
  
  CPU_READ_IPSR:
  	mrs r0, ipsr
  	bx lr
  
  irq_running:
  	mrs r0, msp
  	cmp sp, r0
  	ite ne
  	movne r0, #0x0
  	moveq r0, #0x1
  	bx lr
  
  asm_switch_context:
  	mrs r12, psp
  	stmdb r12!, {r4-r11, lr}
  	str r12, [r1]
  	ldr r12, [r0]
  	ldmia r12!, {r4-r11, lr}
  	msr psp, r12
  	bx lr
   END
