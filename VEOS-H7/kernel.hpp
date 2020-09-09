/*TM*/
/*VEOS Kernel V3.0*/

#ifndef __KERNEL_H__
#define __KERNEL_H__

#include "main.hpp"

#define THREAD_NUM				8		//Number of threads
/*Special note: Keep in mind the MPU can only do certain increments of addresses for the stack.*/
#define THREAD_STACK_SIZE		256		//256 int32_t values = 4 byte int * 256 = 1024 bytes per thread
#define META_TASK_SIZE			256		//Number of meta tasks
#define KERNEL_QUANTA			1		//Context switch trigger (every 1ms)

/*Needs a better system, used to ensure clock is met*/
#define CORE_CLOCK_DESIRED		480000000

/***************Kernel Errors***************/
#define KERNEL_SAFEGUARD		0x0CFECFE0
#define KERNEL_CHECKSUM_FAIL	0xFFFF0000
#define KERNEL_SAFEGUARD_FAIL	0x0000FFFF
#define KERNEL_OVERFLOW_FAIL	0xF0FFFF0F
#define KERNEL_UNDERFLOW_FAIL	0x00FFFF00
#define KERNEL_HARDFAULT_FAIL	0xF0F0F0F0
#define KERNEL_MEMFAULT_FAIL	0x0F0F0F0F
#define KERNEL_CLOCKSTART_FAIL	0xF00FF00F
#define KERNEL_CLOCKHSE_FAIL	0xFFF00FFF
#define KERNEL_MEM_FAIL			0x12345678
#define KERNEL_BUS_FAIL			0x87654321
#define KERNEL_USAGE_FAIL		0x11111111
/*******************************************/
typedef enum
{
	CLOCK_8MHZ = 1,
	CLOCK_16MHZ = 2,
	//Default boot up
CLOCK_36MHZ = 3,
	CLOCK_72MHZ = 4,
	CLOCK_144MHZ = 5,
	CLOCK_170MHZ = 6, 
	//Datasheet maximum
CLOCK_200MHZ = 7, 
	//Stable
CLOCK_250MHZ = 8, 
	//Stable
CLOCK_285MHZ = 9,
	//PLL near maximum (starts crapping out)
CLOCK_216MHZ = 10, //f7
	CLOCK_480MHZ = 11, //h7
} KernelClockSpeed;

extern "C"
{
	void RunScheduler(uint8_t state);  		//Runs per tick to update scheduler
	void KernelJumpStart(void);  				//Start the first thread
	void ForceStackMain(void);
	void KernelHook(void);  					//You can hook onto kernel tick here
	extern void SysTickCont(uint8_t state);  	//Reserved
	extern volatile uint32_t KernelState;  	//Current kernel state
	extern volatile uint32_t KernelTick;  	//Tick of kernel (SysTick)
	extern uint32_t HALFlag;  				//Used to skip HAL tick
	
	//Size of entire CCMRAM is 10KB
	extern uint32_t StackRAM[THREAD_NUM * THREAD_STACK_SIZE];
}

class Kernel
{
private:
#if THREAD_NUM < 1
#error "At least 1 thread is required"
#endif
	static void Thread1(void);
#if THREAD_NUM > 1
	static void Thread2(void);
#endif
#if THREAD_NUM > 2
	static void Thread3(void);
#endif
#if THREAD_NUM > 3
	static void Thread4(void);
#endif
#if THREAD_NUM > 4
	static void Thread5(void);
#endif
#if THREAD_NUM > 5
	static void Thread6(void);
#endif
#if THREAD_NUM > 6
	static void Thread7(void);
#endif
#if THREAD_NUM > 7
	static void Thread8(void);
#endif
#if THREAD_NUM > 8
	static void Thread9(void);
#endif
#if THREAD_NUM > 9
	static void Thread10(void);
#endif
#if THREAD_NUM > 10 //Expand in future if MCU has more CCMRAM.
#error "More than 10 threads not supported"
#endif
	
	static MPU_Region_InitTypeDef MPUStruct;  	//Memory Protection Unit				->???
	static volatile uint32_t StackOffset;  		//Used to calculate thread stacks		->4 bytes
	static uint32_t MillisPrescaler;  			//Prescaler for SysTick					->4 bytes
	static uint8_t ThreadCount;  					//Thread counter						->1 byte
	static int8_t TaskCount;  					//Task counter							->1 byte
	static volatile bool Switching;  				//Check if kernel is in switch mode		->1 byte
												//										->1 byte	-> alignment waste
	static void(*KernelHookAttachment)(void);  	//Kernel hook extra						->4 bytes
	Kernel() {}
	
public:
	
	class Task	//Task Data
	{
	public:
		volatile bool Initialized;  				//If tasks initialized					->1 byte
		volatile bool Loop;  						//If task loops							->1 byte
		volatile bool AttachedThread;  			//If task is attached					->1 byte
												//										->1 byte	-> alignment waste
		volatile uint32_t AttachedThreadIndex;  	//Easier to access the thread			->4 bytes
		volatile int32_t CurrentQuanta;  			//Current kernel tick					->4 bytes
		volatile int32_t RepeatLevel;  			//Repeat several times					->4 bytes
		void(*volatile Function)(void);  			//Pointer to actual function			->4 bytes
		Task();
		~Task();
	};

	class Thread //Thread Data
	{
	public:
		volatile uint32_t* Stack;  				//Pointer to stack location
		volatile int32_t StackHash;  				//Calculated stack hash
		volatile int8_t Status;  					//Thread status
		volatile bool StackWarning;  				//Stack near maximum			
		volatile bool Initialized;  				//Thread initialized
		volatile uint32_t StackSize;  			//Size of stack in terms of array spaces
		volatile uint32_t StackStart;  			//Address of where stack starts
		volatile uint32_t AttachedTaskIndex;  	//Index of attached task
		volatile Task* AttachedTask;  			//Actual task
		volatile bool Enabled;  					//If enabled
		volatile bool HashCalculated;  			//Hash was calculated
		Thread();
		~Thread();
	};
	
	static Thread Threads[THREAD_NUM];
	static Task Tasks[META_TASK_SIZE];
	static uint32_t TaskCounter;
	static KernelClockSpeed SystemClock;
	static void HandleStartupErrors();
	static void EnableMPU();
	static void DisableMPU();
	static void ConfigureMPURegion(uint8_t number, uint32_t address, uint8_t sizel, bool allow);
	static void DisableMPURegion(uint8_t number);
	
	static void Initialize(KernelClockSpeed clock);
	static void SetSystemClock(KernelClockSpeed clock);
	
	static int32_t AddTask(void(*task)(void), uint32_t quanta, bool loop);
	static void RemoveTask(int32_t id);
	
	static bool FindEquality(int val, int except);
	static void IncrementNextTask(void);
	static void Launch(void);
	static void Yield(void);
	static void StopSwitching();
	static void StartSwitching();
	static bool IsSwitching();
	static bool AnyThreadsEnabled();
	static bool AnyTasks();
	static int32_t FirstEnabledThread();
	static int32_t FindNextTaskOffset(int32_t offset, bool ignoreAttachment);
	static void SetErrorFlag(int32_t flag, uint32_t thread, uint32_t task);
	static int32_t GetErrorFlag();
	static uint32_t GetErrorFlagThread();
	static uint32_t GetErrorFlagTask();
	static void SetStack(uint32_t thread, uint32_t stackSize);
	
	static void ThrowHardFault();
	static void ThrowError();
	static uint32_t DisableAllInterrupts();
	static void EnableAllInterrupts(uint32_t mem);
	static void EnableAllInterrupts();
	
	static void EnableThread(uint32_t thread, uint32_t stackSize);
	static void DisableThread(uint32_t thread, bool forceStop);
	static Thread* GetCurrentThread();
	static Thread* GetThreadByIndex(uint32_t id);
	
	static volatile uint32_t CurrentThread;
	static volatile uint32_t CurrentTask;
	static volatile int32_t LastError;
	static volatile uint32_t LastErrorThread;
	static volatile uint32_t LastErrorTask;
	
	static void SemaphoreInit(int32_t *semaphore, int32_t value);
	static void SignalSet(int32_t *semaphore);
	static void SignalUnset(int32_t *semaphore);
	static void SignalWait(int32_t *semaphore);
	static void AttachToKernelHook(void(*task)(void));
	static void ExecuteKernelAttachment();
	
	class Mutex
	{
	private:
		volatile uint32_t _semaphore;
		bool _testAndSet(uint32_t comp, uint32_t set) volatile;
	public:
		Mutex();
		volatile bool Locked;
		void Lock() volatile;
		void Unlock() volatile;
	};

	class AutoMutex
	{
	public:
		Mutex* mainMutex = 0;
		AutoMutex(Mutex* mtx);
		~AutoMutex();
	};
	
	class CriticalSection
	{
	public:
		CriticalSection();
		~CriticalSection();
		void Enter();
		void Exit();
	
		bool Active;
		uint32_t Memory;
	};
};

#endif
