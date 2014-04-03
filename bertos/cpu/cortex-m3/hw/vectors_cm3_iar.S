  MODULE  ?vectors

  AAPCS INTERWORK, VFP_COMPATIBLE, RWPI_COMPATIBLE
  PRESERVE8

  SECTION IRQSTACK:DATA:NOROOT(3)
  SECTION .vtable:CODE:NOROOT(3)

  EXTERN  __iar_program_start
  PUBLIC  __vector_table

  DATA

__vector_table:
	DCD	SFE(IRQSTACK)
	DCD	__iar_program_start
	DCD	default_isr
	DCD	default_isr

  SECTION .text:CODE:REORDER(1)
  THUMB

default_isr:
	wfi
	b default_isr

  END
