#ifndef __GPIO_H__
#define __GPIO_H__

#include "main.hpp"

class PIN
{
private:
	bool Enabled;
	GPIO_TypeDef* GPIO;
	uint32_t Number;
	uint32_t Mode;
	uint32_t Pull;
	uint32_t Speed;
	uint32_t AlternateFn;
	friend class PINSystem;
	
public:
	PIN();	
	void Set();
	PIN* Enable();
	PIN* Disable();
	PIN* VeryHighSpeed();
	PIN* HighSpeed();
	PIN* MediumSpeed();
	PIN* LowSpeed();
	PIN* NoPull();
	PIN* PullUp();
	PIN* PullDown();
	
	PIN* InputMode();
	PIN* OutputPPMode();
	PIN* OutputODMode();
	PIN* AFPPMode();
	PIN* AFODMode();
	PIN* AnalogMode();
	PIN* AlternateFunction(uint32_t number);
};

class PINSystem
{
public:
	static PIN GroupAPins[13];
	static PIN GroupBPins[8];
	static PIN GroupFPins[2];
	
	static PIN* GetGroupAPin(uint32_t number);
	static PIN* GetGroupBPin(uint32_t number);
	static PIN* GetGroupFPin(uint32_t number);
	static uint16_t GetActualPin(uint32_t number);
	static void InitializePin(GPIO_TypeDef* gpio, uint32_t pin, uint32_t mode, uint32_t pull, uint32_t speed, uint32_t alternate);
	static void DeinitializePin(GPIO_TypeDef* gpio, uint32_t pin);
	
private:
	PINSystem();
};

#endif
