//b7

#include "main.hpp"
#include "kernel.hpp"
#include "GPIO.hpp"
#include "Performance.hpp"

//C:\Users\TM\AppData\Local\VisualGDB\EmbeddedBSPs\arm-eabi\com.sysprogs.arm.stm32

TIM_HandleTypeDef htim4;
Kernel::Mutex mutex;
volatile uint32_t task0 = 0;
volatile uint32_t task1 = 0;
volatile uint32_t task2 = 0;
volatile uint32_t task3 = 0;
volatile uint32_t task4 = 0;
volatile uint32_t task5 = 0;
volatile uint32_t task6 = 0;
volatile uint32_t task7 = 0;
volatile uint32_t task8 = 0;
volatile uint32_t task9 = 0;
volatile uint32_t realTick = 0;

ADC_HandleTypeDef hadc1;

void TickHandler()
{
	realTick++;
}

void Task0(void)
{
	for (int i = 0; i < 10000; i++)
		task0++;
	
	HAL_Delay(1000);
}

void Task1(void)
{
	Kernel::AutoMutex lock(&mutex);
	task1++;
}

volatile uint16_t ts_adc_val = 0;

void Task2(void)
{
	HAL_Delay(1000);
	task2++;
}
	
void Task3(void)
{
	Kernel::StopSwitching();
	task3++;
	Kernel::StartSwitching();
}

void SetBrightness(uint32_t bright)
{
	htim4.Instance->CCR1 = bright;
}

uint32_t level = 0;
bool countUp = true;
void Task4(void)
{
	SetBrightness(level);
	if (countUp)
		level += 8;
	else
		level -= 8;
	
	if (level >= 255)
		countUp = false;
	else if (level == 0)
		countUp = true;
	task4++;
}


void Task5(void)
{
	Kernel::AutoMutex lock(&mutex);
	task5++;
}

void Task6(void)
{
	Kernel::AutoMutex lock(&mutex);
	task6++;
}


void Task7(void)
{
	Kernel::AutoMutex lock(&mutex);
	task7++;
}

void Task8(void)
{
	task8++;
}


void Task9(void)
{
	HAL_Delay(1000);
	task9++;
}

int main(void)
{	
	//SCB_EnableICache();
	//SCB_EnableDCache();
	//Kernel::HandleStartupErrors();
	Kernel::Initialize(CLOCK_480MHZ);

	Performance::Initialize();

	__HAL_RCC_TIM12_CLK_ENABLE();
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };
	TIM_OC_InitTypeDef sConfigOC = { 0 };
	htim4.Instance = TIM12;
	htim4.Init.Prescaler = (SystemCoreClock) / (255 * 255) - 1;
	htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim4.Init.Period = 254;
	htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
	{
		while (true) ;
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
	{
		while (true) ;
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	{
		while (true) ;
	}
	
	PINSystem::GetGroupBPin(14)->AFPPMode()->NoPull()->LowSpeed()->AlternateFunction(2)->Enable()->Set();
	
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
	
	Kernel::EnableThread(0, 256);
	Kernel::EnableThread(1, 256);
	Kernel::EnableThread(2, 256);
	Kernel::EnableThread(3, 256);
	Kernel::EnableThread(4, 256);
	Kernel::EnableThread(5, 256);
	Kernel::EnableThread(6, 256);
	Kernel::EnableThread(7, 256);
	
	Kernel::AddTask(&Task0, 1, true);
	Kernel::AddTask(&Task1, 1, true);
	Kernel::AddTask(&Task2, 1, true);
	Kernel::AddTask(&Task3, 1, true);
	Kernel::AddTask(&Task4, 1, true);
	Kernel::AddTask(&Task5, 1, true);
	Kernel::AddTask(&Task6, 1, true);
	Kernel::AddTask(&Task7, 1, true);
	
	//Kernel::DisableMPU();
	/*Kernel::ConfigureMPURegion(0, (uint32_t)&StackRAM[Kernel::GetThreadByIndex(0)->StackStart], MPU_REGION_SIZE_1KB, true);
	Kernel::ConfigureMPURegion(1, (uint32_t)&StackRAM[Kernel::GetThreadByIndex(1)->StackStart], MPU_REGION_SIZE_1KB, true);
	Kernel::ConfigureMPURegion(2, (uint32_t)&StackRAM[Kernel::GetThreadByIndex(2)->StackStart], MPU_REGION_SIZE_1KB, true);
	Kernel::ConfigureMPURegion(3, (uint32_t)&StackRAM[Kernel::GetThreadByIndex(3)->StackStart], MPU_REGION_SIZE_1KB, true);
	Kernel::ConfigureMPURegion(4, (uint32_t)&StackRAM[Kernel::GetThreadByIndex(4)->StackStart], MPU_REGION_SIZE_1KB, true);
	Kernel::ConfigureMPURegion(5, (uint32_t)&StackRAM[Kernel::GetThreadByIndex(5)->StackStart], MPU_REGION_SIZE_1KB, true);
	Kernel::ConfigureMPURegion(6, (uint32_t)&StackRAM[Kernel::GetThreadByIndex(6)->StackStart], MPU_REGION_SIZE_1KB, true);
	Kernel::ConfigureMPURegion(7, (uint32_t)&StackRAM[Kernel::GetThreadByIndex(7)->StackStart], MPU_REGION_SIZE_1KB, true);
	Kernel::EnableMPU();*/
	//Kernel::AddTask(&Task8, 1, true);
	//Kernel::AddTask(&Task9, 1, true);
	task0 = 5;
	Kernel::AttachToKernelHook(&TickHandler);
	Kernel::Launch();
	return 0;
}
