#ifndef __PERFORMANCE_H__
#define __PERFORMANCE_H__

#include "main.hpp"

class Performance
{
private:
	Performance();
	
public:
	class Clock
	{
	public:
		void Start();
		void Stop();
		void Reset();
		bool Enabled;
		uint32_t StartTime;
		uint32_t StopTime;
		uint32_t TimeLapse;
		
		Clock();
		~Clock();
	};
	
	static TIM_HandleTypeDef TimerHandle;
	static Clock Clocks[10];
	static void StartMeasurements();
	static void StopMeasurements();
	static Clock* GetClock(uint32_t id);
	static void Initialize();
};

#endif
