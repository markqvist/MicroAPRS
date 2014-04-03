#include <cfg/cfg_arch.h>

#if !(ARCH & ARCH_NIGHTTEST)
	#warning This file is deprecated, use switch_ctx_arm.S
	#include "switch_ctx_arm.S"
#endif
