#include "Performance.hpp"

Performance::Clock Performance::Clocks[10];
TIM_HandleTypeDef Performance::TimerHandle;

void Performance::StartMeasurements()
{
	TIM2->CNT = 0;
}

void Performance::StopMeasurements()
{
	//HAL_TIM_Base_Stop(&TimerHandle);
}

void Performance::Initialize()
{
	for (uint32_t i = 0; i < 10; i++)
	{
		Clocks[i].Enabled = false;
		Clocks[i].TimeLapse = 0;
	}
	
	//(PSC + 1)*(ARR + 1) = TIMclk / Updatefrequency
	//??? = 170000000 / (1 / 0.1) = 17000000
	//PSC = 10000
	//ARR = 1700
	
	__HAL_RCC_TIM2_CLK_ENABLE();
	
	TimerHandle.Instance = TIM2;
	TimerHandle.Init.Prescaler = SystemCoreClock / 1000000 - 1;   //every 100us (170mhz/17000) = 100us
	TimerHandle.Init.Period = 5000000;    //resets every 5 seconds (50000 * 100us) = 5sec
	TimerHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
	TimerHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	TimerHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	TimerHandle.Init.RepetitionCounter = 0;
	
	if (HAL_TIM_Base_Init(&TimerHandle) != HAL_OK)
	{
		return;
	}
	
	HAL_TIM_Base_Start(&TimerHandle);
}

Performance::Clock* Performance::GetClock(uint32_t id)
{
	return &Clocks[id];
}

Performance::Clock::Clock()
{
	TimeLapse = 0;
	Enabled = false;
}

Performance::Clock::~Clock()
{
	
}

void Performance::Clock::Start()
{
	Enabled = true;
	StartTime = TIM2->CNT;
}

void Performance::Clock::Stop()
{
	Enabled = false;
	StopTime = TIM2->CNT;
	TimeLapse = StopTime - StartTime;
}

void Performance::Clock::Reset()
{
	TimeLapse = 0;
}