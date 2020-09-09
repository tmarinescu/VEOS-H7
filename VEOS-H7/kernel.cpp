/*TM*/
/*VEOS Kernel V3.0*/

#include "kernel.hpp"
#include "Performance.hpp"

/* USED FOR PERFORMANCE CHECKS */
extern "C"
{
	volatile uint32_t MaxTime = 0;
	volatile uint32_t MinTime = 0;
	
	volatile uint32_t MaxTime2 = 0;
	volatile uint32_t MinTime2 = 0;
}

Kernel::Task::Task()
{
	Initialized = false;
}

Kernel::Task::~Task()
{
	
}

Kernel::Thread::Thread()
{
	Enabled = false;
}

Kernel::Thread::~Thread()
{
	
}

Kernel::Thread Kernel::Threads[THREAD_NUM];
Kernel::Task Kernel::Tasks[META_TASK_SIZE];

extern "C"
{
	volatile uint32_t** CurrentStackPtr = 0;
	volatile uint32_t** CurrentFloatPtr = 0;
	volatile int32_t SP_Limit_Lower;
	volatile int32_t SP_Limit_Upper;
	volatile uint32_t KernelState = 0;
	volatile uint32_t KernelTick = 0;
	volatile int32_t KernelQuanta = 0;
	volatile uint32_t TimerTicks = 0;
	uint32_t __attribute__((section(".ccmram"))) StackRAM[THREAD_NUM * THREAD_STACK_SIZE];
}

uint8_t Kernel::ThreadCount = 0;
int8_t Kernel::TaskCount = 0;
uint32_t Kernel::TaskCounter = 0;
volatile bool Kernel::Switching = true;
uint32_t Kernel::MillisPrescaler = 0;
volatile uint32_t Kernel::CurrentThread = 0;
volatile uint32_t Kernel::CurrentTask = 0;
volatile int32_t Kernel::LastError;
volatile uint32_t Kernel::LastErrorThread;
volatile uint32_t Kernel::LastErrorTask;
volatile uint32_t Kernel::StackOffset = 0;
MPU_Region_InitTypeDef Kernel::MPUStruct;
void(*Kernel::KernelHookAttachment)(void) = 0;

Kernel::Thread* Kernel::GetCurrentThread()
{
	return &Threads[CurrentThread];
}

void Kernel::HandleStartupErrors()
{
	Kernel::SetErrorFlag(0, 0, 0);
	
	__DSB();
	__ISB();
	
	if (StackRAM[0] == KERNEL_CHECKSUM_FAIL)
		Kernel::SetErrorFlag(1, StackRAM[1], StackRAM[2]);
	else if (StackRAM[0] == KERNEL_SAFEGUARD_FAIL)
		Kernel::SetErrorFlag(2, StackRAM[1], StackRAM[2]);
	else if (StackRAM[0] == KERNEL_OVERFLOW_FAIL)
		Kernel::SetErrorFlag(3, StackRAM[1], StackRAM[2]);
	else if (StackRAM[0] == KERNEL_UNDERFLOW_FAIL)
		Kernel::SetErrorFlag(4, StackRAM[1], StackRAM[2]);
	else if (StackRAM[0] == KERNEL_HARDFAULT_FAIL)
		Kernel::SetErrorFlag(5, StackRAM[1], StackRAM[2]);
	else if (StackRAM[0] == KERNEL_CLOCKSTART_FAIL)
		Kernel::SetErrorFlag(6, StackRAM[1], StackRAM[2]);
	else if (StackRAM[0] == KERNEL_MEM_FAIL)
		Kernel::SetErrorFlag(7, StackRAM[1], StackRAM[2]);
	else if (StackRAM[0] == KERNEL_BUS_FAIL)
		Kernel::SetErrorFlag(8, StackRAM[1], StackRAM[2]);
	else if (StackRAM[0] == KERNEL_USAGE_FAIL)
		Kernel::SetErrorFlag(9, StackRAM[1], StackRAM[2]);
	
	__DSB();
	__ISB();
	
	if (Kernel::LastError != 0)
	{
		while (true)
		{
			//Initialize CAN/I2C/UART
			//Send out distress signal
			//Wait for response
		}
	}
	
	StackRAM[0] = 0;
	StackRAM[1] = 0;
	StackRAM[2] = 0;
}

void Kernel::Thread1(void)
{
	uint8_t ID = 0;
	while (true)
	{
		if (Threads[ID].AttachedTask != 0)
		{
			(*Threads[ID].AttachedTask->Function)();
			if (!Threads[ID].AttachedTask->Loop)
			{
				Threads[ID].AttachedTask->Initialized = false;
			}
			
			Threads[ID].AttachedTask->AttachedThread = false;
			Threads[ID].AttachedTask->AttachedThreadIndex = 0;
			Threads[ID].Status = 0;
			Threads[ID].HashCalculated = false;
			Threads[ID].StackWarning = false;
			Threads[ID].StackHash = 0;
			Threads[ID].AttachedTask = 0;
			Threads[ID].AttachedTaskIndex = 0;
		}
	}
}

#if THREAD_NUM > 1
void Kernel::Thread2(void)
{
	uint8_t ID = 1;
	while (true)
	{
		if (Threads[ID].AttachedTask != 0)
		{
			(*Threads[ID].AttachedTask->Function)();
			if (!Threads[ID].AttachedTask->Loop)
			{
				Threads[ID].AttachedTask->Initialized = false;
			}
			
			Threads[ID].AttachedTask->AttachedThread = false;
			Threads[ID].AttachedTask->AttachedThreadIndex = 0;
			Threads[ID].Status = 0;
			Threads[ID].HashCalculated = false;
			Threads[ID].StackWarning = false;
			Threads[ID].StackHash = 0;
			Threads[ID].AttachedTask = 0;
			Threads[ID].AttachedTaskIndex = 0;
		}
	}
}
#endif

#if THREAD_NUM > 2
void Kernel::Thread3(void)
{
	uint8_t ID = 2;
	while (true)
	{
		if (Threads[ID].AttachedTask != 0)
		{
			(*Threads[ID].AttachedTask->Function)();
			if (!Threads[ID].AttachedTask->Loop)
			{
				Threads[ID].AttachedTask->Initialized = false;
			}
			
			Threads[ID].AttachedTask->AttachedThread = false;
			Threads[ID].AttachedTask->AttachedThreadIndex = 0;
			Threads[ID].Status = 0;
			Threads[ID].HashCalculated = false;
			Threads[ID].StackWarning = false;
			Threads[ID].StackHash = 0;
			Threads[ID].AttachedTask = 0;
			Threads[ID].AttachedTaskIndex = 0;
		}
	}
}
#endif

#if THREAD_NUM > 3
void Kernel::Thread4(void)
{
	uint8_t ID = 3;
	while (true)
	{
		if (Threads[ID].AttachedTask != 0)
		{
			(*Threads[ID].AttachedTask->Function)();
			if (!Threads[ID].AttachedTask->Loop)
			{
				Threads[ID].AttachedTask->Initialized = false;
			}
			
			Threads[ID].AttachedTask->AttachedThread = false;
			Threads[ID].AttachedTask->AttachedThreadIndex = 0;
			Threads[ID].Status = 0;
			Threads[ID].HashCalculated = false;
			Threads[ID].StackWarning = false;
			Threads[ID].StackHash = 0;
			Threads[ID].AttachedTask = 0;
			Threads[ID].AttachedTaskIndex = 0;
		}
	}
}
#endif

#if THREAD_NUM > 4
void Kernel::Thread5(void)
{
	uint8_t ID = 4;
	while (true)
	{
		if (Threads[ID].AttachedTask != 0)
		{
			(*Threads[ID].AttachedTask->Function)();
			if (!Threads[ID].AttachedTask->Loop)
			{
				Threads[ID].AttachedTask->Initialized = false;
			}
			
			Threads[ID].AttachedTask->AttachedThread = false;
			Threads[ID].AttachedTask->AttachedThreadIndex = 0;
			Threads[ID].Status = 0;
			Threads[ID].HashCalculated = false;
			Threads[ID].StackWarning = false;
			Threads[ID].StackHash = 0;
			Threads[ID].AttachedTask = 0;
			Threads[ID].AttachedTaskIndex = 0;
		}
	}
}
#endif

#if THREAD_NUM > 5
void Kernel::Thread6(void)
{
	uint8_t ID = 5;
	while (true)
	{
		if (Threads[ID].AttachedTask != 0)
		{
			(*Threads[ID].AttachedTask->Function)();
			if (!Threads[ID].AttachedTask->Loop)
			{
				Threads[ID].AttachedTask->Initialized = false;
			}
			
			Threads[ID].AttachedTask->AttachedThread = false;
			Threads[ID].AttachedTask->AttachedThreadIndex = 0;
			Threads[ID].Status = 0;
			Threads[ID].HashCalculated = false;
			Threads[ID].StackWarning = false;
			Threads[ID].StackHash = 0;
			Threads[ID].AttachedTask = 0;
			Threads[ID].AttachedTaskIndex = 0;
		}
	}
}
#endif

#if THREAD_NUM > 6
void Kernel::Thread7(void)
{
	uint8_t ID = 6;
	while (true)
	{
		if (Threads[ID].AttachedTask != 0)
		{
			(*Threads[ID].AttachedTask->Function)();
			if (!Threads[ID].AttachedTask->Loop)
			{
				Threads[ID].AttachedTask->Initialized = false;
			}
			
			Threads[ID].AttachedTask->AttachedThread = false;
			Threads[ID].AttachedTask->AttachedThreadIndex = 0;
			Threads[ID].Status = 0;
			Threads[ID].HashCalculated = false;
			Threads[ID].StackWarning = false;
			Threads[ID].StackHash = 0;
			Threads[ID].AttachedTask = 0;
			Threads[ID].AttachedTaskIndex = 0;
		}
	}
}
#endif

#if THREAD_NUM > 7
void Kernel::Thread8(void)
{
	uint8_t ID = 7;
	while (true)
	{
		if (Threads[ID].AttachedTask != 0)
		{
			(*Threads[ID].AttachedTask->Function)();
			if (!Threads[ID].AttachedTask->Loop)
			{
				Threads[ID].AttachedTask->Initialized = false;
			}
			
			Threads[ID].AttachedTask->AttachedThread = false;
			Threads[ID].AttachedTask->AttachedThreadIndex = 0;
			Threads[ID].Status = 0;
			Threads[ID].HashCalculated = false;
			Threads[ID].StackWarning = false;
			Threads[ID].StackHash = 0;
			Threads[ID].AttachedTask = 0;
			Threads[ID].AttachedTaskIndex = 0;
		}
	}
}
# endif

void Kernel::SetStack(uint32_t thread, uint32_t stackSize)
{
	for (uint32_t i = StackOffset; i < stackSize + StackOffset; i++)
		StackRAM[i] = 0;
	
	Threads[thread].StackSize = stackSize;
	Threads[thread].StackStart = StackOffset;
	Threads[thread].Stack = &StackRAM[StackOffset + stackSize - 16];     //[num][THREAD_STACK_SIZE - 16];
	Threads[thread].HashCalculated = false;
	StackRAM[StackOffset + stackSize - 1] = 0x01000000;      //thumb mode
	
	for(uint32_t i = StackOffset ; i < StackOffset + 10 ; i++)
		StackRAM[i] = KERNEL_SAFEGUARD;
	
	Threads[thread].StackWarning = false;
	Threads[thread].AttachedTask = 0;
	Threads[thread].AttachedTaskIndex = 0;
	Threads[thread].Initialized = true;
	Threads[thread].Enabled = false;
	Threads[thread].Status = 0;
	Threads[thread].StackHash = 0;
	StackOffset += stackSize;
}

int32_t Kernel::AddTask(void(*task)(void), uint32_t quanta, bool loop)
{
	uint32_t prim = DisableAllInterrupts();
	
	TaskCount = -1;
	for (int i = 0; i < META_TASK_SIZE; i++)
	{
		if (Tasks[i].Initialized == false)
		{
			TaskCount = i;
			break;
		}
	}
	
	if (TaskCount == -1)
	{
		EnableAllInterrupts(prim);
		return -1;
	}
	
	Tasks[TaskCount].Initialized = true;
	Tasks[TaskCount].Loop = loop;
	Tasks[TaskCount].CurrentQuanta = quanta;
	Tasks[TaskCount].Function = task;
	TaskCounter++;
	EnableAllInterrupts(prim);
	return TaskCount;
}

void Kernel::RemoveTask(int32_t id)
{
	if (id < META_TASK_SIZE)
	{
		Tasks[id].Initialized = false;
		TaskCounter--;
	}
}

Kernel::Thread* Kernel::GetThreadByIndex(uint32_t id)
{
	return &Threads[id];
}

void Kernel::Initialize(KernelClockSpeed clock)
{
	HAL_Init();
	
	SetSystemClock(clock);
	__disable_irq();
	KernelHookAttachment = 0;
	TaskCount = 0;
	for (int i = 0; i < META_TASK_SIZE; i++)
	{
		Tasks[i].Initialized = false;
		Tasks[i].Loop = false;
		Tasks[i].AttachedThread = false;
		Tasks[i].AttachedThreadIndex = 0;
		Tasks[i].CurrentQuanta = 0;
		Tasks[i].RepeatLevel = 0;
		Tasks[i].Function = 0;
	}
	
	for (int i = 0; i < THREAD_NUM * THREAD_STACK_SIZE; i++)
	{
		StackRAM[i] = 0;
	}
	
	for (int i = 0; i < THREAD_NUM; i++)
	{
		Threads[i].Stack = 0;
		Threads[i].StackHash = 0;
		Threads[i].Status = -1;
		Threads[i].StackWarning = false;
		Threads[i].Initialized = false;
		Threads[i].StackSize = 0;
		Threads[i].StackStart = 0;
		Threads[i].AttachedTaskIndex = 0;
		Threads[i].AttachedTask = 0;
		Threads[i].Enabled = false;
		Threads[i].HashCalculated = false;
	}
	
	MillisPrescaler = SystemCoreClock / 1000;
	KernelState = 1;
}

int32_t Kernel::FindNextTaskOffset(int32_t offset, bool ignoreAttachment)
{
	for (int i = 1; i <= META_TASK_SIZE + 1; i++)
	{
		uint32_t x = (i + offset) % META_TASK_SIZE;
		if (ignoreAttachment)
		{
			if (Tasks[x].Initialized == true)
			{
				return x;
			}
		}
		else
		{
			if (Tasks[x].Initialized == true && Tasks[x].AttachedThread == false)
			{
				return x;
			}
		}
	}
	return -1;
}

void Kernel::Launch(void)
{
	SysTick->CTRL = 0;
	SysTick->VAL = 0;
		
	//Check the programming manual, (SHPR3 PRI_15)
	SHPR3 = (SHPR3 & 0x00FFFFFF) | 0xE0000000;         //lowest priority
	SysTick->LOAD = (KERNEL_QUANTA * MillisPrescaler) - 1;       //1ms interrupt
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
	//Use CPU clock source		Enable SysTick exception	Enable SysTick timer
	
	int32_t num = Kernel::FirstEnabledThread();
	Kernel::CurrentThread = num;
	for (int i = 0, j = -1; i < THREAD_NUM; i++)
	{
		if (Threads[i].Enabled)
		{
			if (i == num)
			{
				CurrentStackPtr = &Threads[i].Stack;
			}
			
			int32_t tsk = Kernel::FindNextTaskOffset(j, false);
			if (tsk != -1)
			{
				Threads[i].AttachedTask = &Tasks[tsk];
				Threads[i].AttachedTaskIndex = tsk;
				Tasks[tsk].AttachedThread = true;
				Tasks[tsk].AttachedThreadIndex = i;
				j = tsk;
			}
		}
	}
	
	if (Kernel::AnyTasks())
		Kernel::CurrentTask = Kernel::FindNextTaskOffset(-1, true);
	
	//Initial stack boundaries
	SP_Limit_Upper = (int32_t)&StackRAM[Kernel::GetCurrentThread()->StackStart + Kernel::GetCurrentThread()->StackSize - 1];
	SP_Limit_Lower = (int32_t)&StackRAM[Kernel::GetCurrentThread()->StackStart];
	
	//Let's go
	KernelJumpStart();
}


void Kernel::Yield(void)
{
	//uint32_t prim = Utilities::DisableAllInterrupts();
	//SysTick->VAL = 0;
	//INTCTRL = 0x04000000;   //Trigger systick
	//Utilities::EnableAllInterrupts(prim);
	//SysTick_Cont(1);
	//uint32_t prim = Utilities::DisableAllInterrupts();
	if(CurrentStackPtr != 0)
		KernelQuanta = GetCurrentThread()->AttachedTask->CurrentQuanta;
	//KernelState = 2;
	//SkipHAL = 1;
	//SysTick->VAL = 0;
	//SCB->ICSR |= (1 << 26);
	//INTCTRL = 0x04000000;
	//Utilities::EnableAllInterrupts(prim);
}

bool Kernel::FindEquality(int val, int except)
{
	for (int i = 0; i < THREAD_NUM; i++)
		if (i != except)
			if (Threads[i].AttachedTaskIndex == val && Threads[i].Enabled)
				return true;
	return false;
}

void Kernel::IncrementNextTask(void)
{
	CurrentTask++;
	if (CurrentTask >= META_TASK_SIZE)
		CurrentTask = 0;
}

void Kernel::StopSwitching(void)
{
	Switching = false;
	KernelState = 0;
}

void Kernel::StartSwitching(void)
{
	Switching = true;
	KernelState = 1;
}

bool Kernel::IsSwitching(void)
{
	return Switching;
}

void Kernel::EnableThread(uint32_t thread, uint32_t stackSize)
{
	if (thread < 0 || thread >= THREAD_NUM)
	{
		return;
	}
	
	if (Threads[thread].Initialized == false)
	{
		SetStack(thread, stackSize);
		if (thread == 0)
		{
			StackRAM[GetThreadByIndex(thread)->StackStart + GetThreadByIndex(thread)->StackSize - 2] = (int32_t)(&Thread1);
		}
		else if (thread == 1)
		{
#if THREAD_NUM > 1
			StackRAM[GetThreadByIndex(thread)->StackStart + GetThreadByIndex(thread)->StackSize - 2] = (int32_t)(&Thread2);
#endif
		}
		else if (thread == 2)
		{
#if THREAD_NUM > 2
			StackRAM[GetThreadByIndex(thread)->StackStart + GetThreadByIndex(thread)->StackSize - 2] = (int32_t)(&Thread3);
#endif
		}
		else if (thread == 3)
		{
#if THREAD_NUM > 3
			StackRAM[GetThreadByIndex(thread)->StackStart + GetThreadByIndex(thread)->StackSize - 2] = (int32_t)(&Thread4);
#endif
		}
		else if (thread == 4)
		{
#if THREAD_NUM > 4
			StackRAM[GetThreadByIndex(thread)->StackStart + GetThreadByIndex(thread)->StackSize - 2] = (int32_t)(&Thread5);
#endif
		}
		else if (thread == 5)
		{
#if THREAD_NUM > 5
			StackRAM[GetThreadByIndex(thread)->StackStart + GetThreadByIndex(thread)->StackSize - 2] = (int32_t)(&Thread6);
#endif
		}
		else if (thread == 6)
		{
#if THREAD_NUM > 6
			StackRAM[GetThreadByIndex(thread)->StackStart + GetThreadByIndex(thread)->StackSize - 2] = (int32_t)(&Thread7);
#endif
		}
		else if (thread == 7)
		{
#if THREAD_NUM > 7
			StackRAM[GetThreadByIndex(thread)->StackStart + GetThreadByIndex(thread)->StackSize - 2] = (int32_t)(&Thread8);
#endif
		}
		else if (thread == 8)
		{
#if THREAD_NUM > 8
			StackRAM[GetTCBByID(thread)->StackStart + GetTCBByID(thread)->StackSize - 2] = (int32_t)(&Thread9);
#endif
		}
		else if (thread == 9)
		{
#if THREAD_NUM > 9
			StackRAM[GetTCBByID(thread)->StackStart + GetTCBByID(thread)->StackSize - 2] = (int32_t)(&Thread10);
#endif
		}
		Threads[thread].Initialized = true;
	}

	Threads[thread].Enabled = true;
}

bool Kernel::AnyThreadsEnabled()
{
	for (int i = 0; i < THREAD_NUM; i++)
		if (Threads[i].Enabled)
			return true;
	return false;
}

bool Kernel::AnyTasks()
{
	for (int i = 0; i < META_TASK_SIZE; i++)
		if (Tasks[i].Initialized == true)
			return true;
	return false;
}

int32_t Kernel::FirstEnabledThread()
{
	for (int i = 0; i < THREAD_NUM; i++)
		if (Threads[i].Enabled)
			return i;
	return -1;
}

void Kernel::DisableThread(uint32_t thread, bool forceStop)
{
	Threads[thread].Enabled = false;
}

void Kernel::SetErrorFlag(int32_t flag, uint32_t thread, uint32_t task)
{
	Kernel::LastError = flag;
	Kernel::LastErrorThread = thread;
	Kernel::LastErrorTask = task;
}

int32_t Kernel::GetErrorFlag()
{
	return Kernel::LastError;
}

uint32_t Kernel::GetErrorFlagThread()
{
	return Kernel::LastErrorThread;
}

uint32_t Kernel::GetErrorFlagTask()
{
	return Kernel::LastErrorTask;
}

extern "C"
{
	void RunScheduler(uint8_t state)
	{
		if (!Kernel::IsSwitching())
			return;
		
		if (!Kernel::AnyThreadsEnabled())
			return;
		
		Performance::StartMeasurements();
		Performance::GetClock(0)->Start();
		//Kernel::DisableMPU();

		/*StackOverflow Protection*/
		for(uint32_t i = 0 ; i < THREAD_NUM ; i++) //Check all stacks
		{
			if (StackRAM[Kernel::GetThreadByIndex(i)->StackStart] != KERNEL_SAFEGUARD) //Guard was overwritten indicating stackoverflow
				{
					StackRAM[0] = KERNEL_SAFEGUARD_FAIL;
					NVIC_SystemReset();      //Reset stm
				}
			uint32_t ss = Kernel::GetThreadByIndex(i)->StackStart + 10;
			for (uint32_t j = Kernel::GetThreadByIndex(i)->StackStart + 1; j < ss; j++)
			{
				if (StackRAM[j] != KERNEL_SAFEGUARD)
				{
					Kernel::Threads[i].StackWarning = true;
					break;
				}
			}
			
			if (Kernel::Threads[i].Enabled)
			{
				if (Kernel::Threads[i].AttachedTask == 0)
				{
					
				}
			}
		}		

		for (uint32_t i = 0; i < THREAD_NUM; i++)
		{
			if (Kernel::Threads[i].Enabled)
			{
				if (Kernel::Threads[i].AttachedTask == 0)
				{
					uint32_t mem = Kernel::CurrentTask;
					uint8_t memFail = 0;
					while ((Kernel::Tasks[Kernel::CurrentTask].Initialized == false || Kernel::FindEquality(Kernel::CurrentTask, i)))
					{
						Kernel::IncrementNextTask();
						if (mem == Kernel::CurrentTask)
						{
							memFail = 1;
							break;
						}
					}
					if (memFail == 1)
					{
						continue;
					}
					
					Kernel::Threads[i].AttachedTask = &Kernel::Tasks[Kernel::CurrentTask];
					Kernel::Threads[i].AttachedTaskIndex = Kernel::CurrentTask;
					Kernel::Tasks[Kernel::CurrentTask].AttachedThread = true;
					Kernel::Tasks[Kernel::CurrentTask].AttachedThreadIndex = i;
					Kernel::Threads[i].AttachedTaskIndex = Kernel::CurrentTask;
					Kernel::IncrementNextTask();
					Kernel::Threads[i].Status = 1;
				}
			}
		}
		
		if (CurrentStackPtr != 0)
		{
			if (Kernel::GetCurrentThread()->AttachedTask->CurrentQuanta - KernelQuanta <= 0)
			{
				CurrentStackPtr = 0;
				CurrentFloatPtr = 0;
			}
			
			if (state == 1)
			{
				//currentPtr = 0;
				//KernelQuanta++;
			}
		}

		Performance::GetClock(3)->Start();
		Kernel::Thread* thrd = 0;
		if (CurrentStackPtr == 0)
		{
			thrd = Kernel::GetCurrentThread();
			if (thrd->AttachedTask != 0)
				thrd->Status = 1;
			else
				thrd->Status = 0;

			//Compute simple stack checksum for current thread
			uint32_t _stackStart = thrd->StackStart;
			uint32_t _stackSize = thrd->StackSize;
			uint32_t _stackSum = _stackStart + _stackSize;
			thrd->StackHash = StackRAM[_stackStart + 10];
			for (uint32_t i = _stackStart + 11; i < _stackSum; i++)
				thrd->StackHash ^= StackRAM[i];
			thrd->HashCalculated = true;
			
			//Disable editing to memory at current thread
			//Kernel::ConfigureMPURegion(Kernel::CurrentThread, (uint32_t)&StackRAM[_stackStart], MPU_REGION_SIZE_1KB, false);
			
			Kernel::CurrentThread++;
			if (Kernel::CurrentThread >= THREAD_NUM)
				Kernel::CurrentThread = 0;
			
			while (!Kernel::Threads[Kernel::CurrentThread].Enabled || Kernel::Threads[Kernel::CurrentThread].AttachedTask == 0)
			{
				Kernel::CurrentThread++;
				if (Kernel::CurrentThread >= THREAD_NUM)
					Kernel::CurrentThread = 0;
			}
			
			thrd = Kernel::GetCurrentThread();
			_stackStart = thrd->StackStart;
			_stackSize = thrd->StackSize;
			_stackSum = _stackStart + _stackSize;
			
			//Enable memory editing at new thread
			//Kernel::ConfigureMPURegion(Kernel::CurrentThread, (uint32_t)&StackRAM[_stackStart], MPU_REGION_SIZE_1KB, true);
			
			CurrentStackPtr = &thrd->Stack;
			
			//Find the stack boundaries
			SP_Limit_Upper = (int32_t)&StackRAM[_stackSum - 1];
			SP_Limit_Lower = (int32_t)&StackRAM[_stackStart];
			
			//Catch stack illegal accessing from random threads
			if(thrd->HashCalculated == true)
			{
				int32_t stackSum = StackRAM[_stackStart + 10];
				for (uint32_t i = _stackStart + 11; i < _stackSum; i++)
					stackSum ^= StackRAM[i];
				if (thrd->StackHash != stackSum)
				{
					StackRAM[0] = KERNEL_CHECKSUM_FAIL;
					NVIC_SystemReset();     //Stack was changed
				}
			}
			KernelQuanta = 0;
			thrd->Status = 2;
		}
		//Kernel::EnableMPU();
		Performance::GetClock(3)->Stop();
		Performance::GetClock(0)->Stop();
		//Performance::StopMeasurements();
		
		if(Performance::GetClock(3)->TimeLapse > MaxTime)
			MaxTime = Performance::GetClock(3)->TimeLapse;
		if (MinTime == 0)
			MinTime = Performance::GetClock(3)->TimeLapse;
		if (Performance::GetClock(3)->TimeLapse < MinTime)
			MinTime = Performance::GetClock(3)->TimeLapse;
		
		if (Performance::GetClock(0)->TimeLapse > MaxTime2)
			MaxTime2 = Performance::GetClock(0)->TimeLapse;
		if (MinTime2 == 0)
			MinTime2 = Performance::GetClock(0)->TimeLapse;
		if (Performance::GetClock(0)->TimeLapse < MinTime2)
			MinTime2 = Performance::GetClock(0)->TimeLapse;
		
	}
	
	void KernelHook(void)
	{
		if (KernelState == 1)
			KernelQuanta++;
		Kernel::ExecuteKernelAttachment();

	}
	
	void HardFault_Handler()
	{
		//daccviol
		//iaacviol
		ForceStackMain();
		__DSB();
		__ISB();
		Kernel::DisableMPU();
		
		
		//Do some investigation if necessary
		
		StackRAM[0] = KERNEL_HARDFAULT_FAIL;
		StackRAM[1] = Kernel::CurrentThread;
		StackRAM[2] = Kernel::GetCurrentThread()->AttachedTaskIndex;
		NVIC_SystemReset();
	}
	
	void NMI_Handler(void)
	{
		if (__HAL_RCC_GET_IT(RCC_IT_CSS))
		{
			//__HAL_RCC_CLEAR_IT(RCC_IT_CSS);
			StackRAM[0] = KERNEL_CLOCKHSE_FAIL;
			NVIC_SystemReset();
		}
	}
	
	void MemManage_Handler(void)
	{
		StackRAM[0] = KERNEL_MEM_FAIL;
		NVIC_SystemReset();
	}
	
	void BusFault_Handler(void)
	{
		StackRAM[0] = KERNEL_BUS_FAIL;
		NVIC_SystemReset();
	}
	
	void UsageFault_Handler(void)
	{
		StackRAM[0] = KERNEL_USAGE_FAIL;
		NVIC_SystemReset();
	}
}

void Kernel::AttachToKernelHook(void(*task)(void))
{
	KernelHookAttachment = task;
}

void Kernel::ExecuteKernelAttachment()
{
	if (KernelHookAttachment != 0)
	{
		(*KernelHookAttachment)();
	}
}

void Kernel::ThrowHardFault()
{
	//set some LED high
	//send last CAN message
	HardFault_Handler();
}

void Kernel::ThrowError()
{
	//send some CAN message
}

uint32_t Kernel::DisableAllInterrupts()
{
	uint32_t mask = __get_PRIMASK();     //get existing interrupt mask
	__disable_irq();
	return mask;
}

void Kernel::EnableAllInterrupts(uint32_t mem)
{
	if (!mem)
		__enable_irq();
}

void Kernel::EnableAllInterrupts()
{
	__enable_irq();
}

Kernel::CriticalSection::CriticalSection()
{
	Active = false;
	Memory = 0;
}

Kernel::CriticalSection::~CriticalSection()
{
	if (Active)
	{
		Kernel::EnableAllInterrupts(Memory);
		Kernel::StartSwitching();
	}
}

void Kernel::CriticalSection::Enter()
{
	if (!Active)
	{
		Kernel::StopSwitching();
		Active = true;
		Memory = Kernel::DisableAllInterrupts();
	}
}

void Kernel::CriticalSection::Exit()
{
	if (Active)
	{
		Kernel::EnableAllInterrupts(Memory);
		Active = false;
		Kernel::StartSwitching();
	}
}

void Kernel::SemaphoreInit(int32_t *semaphore, int32_t value) //1 = unlocked, 0 = locked
{
	uint32_t prim = DisableAllInterrupts();
	*semaphore = value;
	EnableAllInterrupts(prim);
}

void Kernel::SignalSet(int32_t *semaphore) //signals for it to go through
{
	uint32_t prim = DisableAllInterrupts();
	*semaphore += 1;
	EnableAllInterrupts(prim);
}
	
void Kernel::SignalUnset(int32_t *semaphore) //sets the semaphore to blocking mode
{
	uint32_t prim = DisableAllInterrupts();
	*semaphore = 0;
	EnableAllInterrupts(prim);
}

void Kernel::SignalWait(int32_t *semaphore) //waits for semaphore to be set
{
	uint32_t prim = DisableAllInterrupts();
	while (*semaphore <= 0)
	{
		Kernel::Yield();
	}
		
	*semaphore -= 1;
	EnableAllInterrupts(prim);
}

Kernel::Mutex::Mutex()
{
	__DMB();
	_semaphore = 0;
}

bool Kernel::Mutex::_testAndSet(uint32_t new_val, uint32_t old_val) volatile
{
	do
	{
		if (__LDREXW(&_semaphore) != old_val) //something else changed it
			return false;
	} while (__STREXW(new_val, &_semaphore) != 0); //lock while attempting to set it
	__DMB();     //data memory barrier
	return true;
}

void Kernel::Mutex::Lock() volatile
{
	while (true)
	{
		if (_semaphore == 0)
		{
			if (_testAndSet(1, 0))
			{
				Locked = true;
				return;
			}
		}
		Kernel::Yield();
	}
}

void Kernel::Mutex::Unlock() volatile
{
	__DMB();
	_semaphore = 0;
	Locked = false;
}

Kernel::AutoMutex::AutoMutex(Mutex* mtx)
{
	mainMutex = mtx;
	mtx->Lock();
}
	
Kernel::AutoMutex::~AutoMutex()
{
	mainMutex->Unlock();
}

void Kernel::SetSystemClock(KernelClockSpeed clock)
{	
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	//HAL_PWREx_DisableUSBDeadBatteryPD();
	
	if(clock == KernelClockSpeed::CLOCK_480MHZ)
	{
		RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
		RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
		RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };

		HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);
		__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);
		while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
		__HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);

		RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
		RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
		RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
		RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
		RCC_OscInitStruct.PLL.PLLM = 1;
		RCC_OscInitStruct.PLL.PLLN = 120;
		RCC_OscInitStruct.PLL.PLLP = 2;
		RCC_OscInitStruct.PLL.PLLQ = 2;
		RCC_OscInitStruct.PLL.PLLR = 2;
		RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
		RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
		RCC_OscInitStruct.PLL.PLLFRACN = 0;
		if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
		{
			Kernel::ThrowHardFault();
		}
		
		RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                              | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2
                              | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
		RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
		RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
		RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
		RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
		RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
		RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

		if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
		{
			Kernel::ThrowHardFault();
		}

		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
		{
			Kernel::ThrowHardFault();
		}
		
		if (SystemCoreClock != CORE_CLOCK_DESIRED)
		{
			StackRAM[0] = KERNEL_CLOCKSTART_FAIL;
			NVIC_SystemReset();
		}
		
		HAL_PWREx_EnableUSBVoltageDetector();
		HAL_RCC_EnableCSS();
	}
}

void Kernel::EnableMPU()
{
	HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

void Kernel::DisableMPU()
{
	HAL_MPU_Disable();
}

void Kernel::ConfigureMPURegion(uint8_t number, uint32_t address, uint8_t size, bool allow)
{
	MPUStruct.Enable = MPU_REGION_ENABLE;
	MPUStruct.BaseAddress = address;
	MPUStruct.Size = size;
	if (allow)
		MPUStruct.AccessPermission = MPU_REGION_FULL_ACCESS;   //MPU_REGION_FULL_ACCESS; //MPU_REGION_PRIV_RO_URO
	else
		MPUStruct.AccessPermission = MPU_REGION_NO_ACCESS;    //MPU_REGION_FULL_ACCESS; //MPU_REGION_PRIV_RO_URO
	MPUStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
	MPUStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
	MPUStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
	MPUStruct.Number = number;
	MPUStruct.TypeExtField = MPU_TEX_LEVEL0;
	MPUStruct.SubRegionDisable = 0x00;
	MPUStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
	HAL_MPU_ConfigRegion(&MPUStruct);
}

void Kernel::DisableMPURegion(uint8_t number)
{
	MPUStruct.Enable = MPU_REGION_DISABLE;
	MPUStruct.Number = number;
	HAL_MPU_ConfigRegion(&MPUStruct);
}

extern "C"
{
	void TM_Handlers()
	{
		//Utilities::IncTest();
	}
}