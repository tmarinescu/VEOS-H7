.data
orgStack: .word 0
kernelMem: .word 0
HALFlag: .word 0

.thumb
.syntax unified
.section .text, "ax"
.balign 4
.eabi_attribute Tag_ABI_align_preserved, 1

.extern CurrentStackPtr
.extern SP_Limit_Upper
.extern SP_Limit_Lower
.extern StackRAM
.extern KernelState
.extern KernelTick
.extern uwTick
.extern uwTickFreq
.global HALFlag
.global SysTick_Handler
.global KernelJumpStart
.global RunScheduler
.global TM_Handlers
.global KernelHook
.global SysTick_Cont
.global SkipAhead
.global ForceStackMain

ForceStackMain:
	LDR R1,=orgStack
	LDR SP,[R1]
	BX LR

.thumb_func
SysTick_Handler:
	CPSID I					@Disable all interrupts
	LDR R0,=KernelTick
	LDR R0,[R0]
	ADD R0,#1
	LDR R1,=KernelTick
	STR R0,[R1]
	LDR R0,=HALFlag
	LDR R0,[R0]
	CMP R0,#1
	BEQ SkipAhead
	LDR R0,=uwTick
	LDR R0,[R0]
	LDR R1,=uwTickFreq
	LDR R1,[R1]
	ADD R0,R1
	LDR R1,=uwTick
	STR R0,[R1]
	PUSH {R4-R11}
	PUSH {LR}
	BL KernelHook
	POP {LR}
	POP {R4-R11}
	B SkipAhead

SkipAhead:
	LDR R0,=HALFlag
	MOV R1,#0
	STR R1,[R0]
	MOV R0,#0
	B SysTick_Cont

SysTick_Cont:
	CPSID I
	LDR R1,=kernelMem
	STR R0,[R1]
	LDR R1,=KernelState
	LDR R1,[R1]
	CMP R1,#1
	BNE SysTickEnd
	LDR R1,=SP_Limit_Upper	@Load SP_Limit_Upper address
	LDR R1,[R1]				@Get actual value
	CMP SP,R1				@Compare against SP stack address
	BGT StackOverflow		@The SP stack was greater than the limit!
	LDR R1,=SP_Limit_Lower	@Load SP_Limit_Lower address
	LDR R1,[R1]				@Get actual value
	CMP SP,R1				@Compare against SP stack address
	BLT StackUnderflow		@The SP stack was less than the limit!
	PUSH {R4-R11}			@Push registers to stack
	LDR R0,=CurrentStackPtr		@Get address of current tcb
	LDR R1,[R0]				@Get the actual pointer for the address
	STR SP,[R1]				@Store the stack pointer into tcb (this sets the first int32 value inside the structure)
	LDR R1,=orgStack		@Use main stack to branch out since other stacks behave weird
	LDR SP,[R1]
	PUSH {R0,LR}			@Push R0 and LR to the stack
	BL TM_Handlers			@Handle my shit
	LDR R0,=kernelMem
	LDR R0,[R0]
	BL RunScheduler		@Do the actual task switching in the kernel
	POP {R0,LR}				@Pop R0 and LR back
	LDR R1,[R0]				@Get actual pointer back again (the pointer was switched inside SchedulerUpdate)
	LDR SP,[R1]				@Load the stack pointer from the structure (first int32 value)
	POP {R4-R11}			@Pop all the registers from the stack
	B SysTickEnd

SysTickEnd:
	CPSIE I					@Enable all interrupts
	BX LR					@Branch back to the last link register value

StackOverflow:
	LDR R1,=StackRAM
	LDR R2,=0xF0FFFF0F
	STR R2,[R1]
	B StackPanic

StackUnderflow:
	LDR R1,=StackRAM
	LDR R2,=0x00FFFF00
	STR R2,[R1]
	B StackPanic

StackPanic:					@Port of NVIC_SystemReset(), it performs a software reset
	DSB
	MOV R0,#0x5FA
	LSL R0,#16
	LDR R1,=0xE000ED0C		@Magic values, dont ask
	LDR R2,[R1]
	AND R2,R2,#0x700
	ORR R5,R2,#4
	ORR R5,R5,R0
	STR R5,[R1]
	DSB
	B Infinite

Infinite:
	B Infinite

KernelJumpStart:
	LDR R0,=orgStack
	STR SP,[R0]
	LDR R0,=CurrentStackPtr	@Get the address value of the current tcb
	LDR R2,[R0]				@Get the actual pointer for the tcb
	LDR SP,[R2]				@Get the stack pointer from the tcb
	POP {R4-R11}			@Pop all the registers off the stack
	POP {R0-R3}				@Pop empty (can be parameters)
	POP {R12}				@IP register (Intra Procedure)
	ADD SP,SP,#4			@Skip over the LR register
	POP {LR}				@Pops the pointer to the function in the tcb
	ADD SP,SP,#4			@Skip over the PSR register
	PUSH {R0-R1}
	LDR R0,=0xE000E018		@Set systick val to zero
	MOV R1,#0
	STR R1,[R0]
	POP {R0-R1}
	CPSIE I					@Enable all interrupts
	BX LR					@Go to the function
	
.align
.end
			