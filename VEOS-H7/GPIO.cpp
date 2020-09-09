#include "GPIO.hpp"

PIN::PIN()
{
	Enabled = false;
	GPIO = GPIOA;
	Number = 0;
	Mode = 0;
	Pull = 0;
	Speed = 0;
	AlternateFn = 0;
}

void PINSystem::InitializePin(GPIO_TypeDef* gpio, uint32_t pin, uint32_t mode, uint32_t pull, uint32_t speed, uint32_t alternate)
{
	if (gpio == GPIOA)
		__HAL_RCC_GPIOA_CLK_ENABLE();
	else if (gpio == GPIOB)
		__HAL_RCC_GPIOB_CLK_ENABLE();
	else if (gpio == GPIOC)
		__HAL_RCC_GPIOC_CLK_ENABLE();
	else if (gpio == GPIOD)
		__HAL_RCC_GPIOD_CLK_ENABLE();
	else if (gpio == GPIOF)
		__HAL_RCC_GPIOF_CLK_ENABLE();
	else
		return;
	
	GPIO_InitTypeDef gp = { 0 };
	gp.Pin = pin;
	gp.Mode = mode;
	gp.Pull = pull;
	gp.Speed = speed;
	gp.Alternate = alternate;
	HAL_GPIO_Init(gpio, &gp);
}

void PINSystem::DeinitializePin(GPIO_TypeDef* gpio, uint32_t pin)
{
	HAL_GPIO_DeInit(gpio, pin);
}

void PIN::Set()
{
	if (Enabled)
		PINSystem::InitializePin(GPIO, Number, Mode, Pull, Speed, AlternateFn);
	else
		PINSystem::DeinitializePin(GPIO, Number);
}

PIN* PIN::Enable()
{
	Enabled = true;
	return this;
}

PIN* PIN::Disable()
{
	Enabled = false;
	return this;
}

PIN* PIN::VeryHighSpeed()
{
	Speed = GPIO_SPEED_HIGH;
	return this;
}

PIN* PIN::HighSpeed()
{
	Speed = GPIO_SPEED_FAST;
	return this;
}

PIN* PIN::MediumSpeed()
{
	Speed = GPIO_SPEED_MEDIUM;
	return this;
}

PIN* PIN::LowSpeed()
{
	Speed = GPIO_SPEED_LOW;
	return this;
}

PIN* PIN::NoPull()
{
	Pull = GPIO_NOPULL;
	return this;
}

PIN* PIN::PullUp()
{
	Pull = GPIO_PULLUP;
	return this;
}

PIN* PIN::PullDown()
{
	Pull = GPIO_PULLDOWN;
	return this;
}

PIN* PIN::InputMode()
{
	Mode = GPIO_MODE_INPUT;
	return this;
}

PIN* PIN::OutputPPMode()
{
	Mode = GPIO_MODE_OUTPUT_PP;
	return this;
}

PIN* PIN::OutputODMode()
{
	Mode = GPIO_MODE_OUTPUT_OD;
	return this;
}

PIN* PIN::AFPPMode()
{
	Mode = GPIO_MODE_AF_PP;
	return this;
}

PIN* PIN::AFODMode()
{
	Mode = GPIO_MODE_AF_OD;
	return this;
}

PIN* PIN::AnalogMode()
{
	Mode = GPIO_MODE_ANALOG;
	return this;
}

PIN* PIN::AlternateFunction(uint32_t number)
{
	if (number == 0)
		AlternateFn = 0x00U;
	else if (number == 1)
		AlternateFn = 0x01U;
	else if (number == 2)
		AlternateFn = 0x02U;
	else if (number == 3)
		AlternateFn = 0x03U;
	else if (number == 4)
		AlternateFn = 0x04U;
	else if (number == 5)
		AlternateFn = 0x05U;
	else if (number == 6)
		AlternateFn = 0x06U;
	else if (number == 7)
		AlternateFn = 0x07U;
	else if (number == 8)
		AlternateFn = 0x08U;
	else if (number == 9)
		AlternateFn = 0x09U;
	else if (number == 10)
		AlternateFn = 0x0AU;
	else if (number == 11)
		AlternateFn = 0x0BU;
	else if (number == 12)
		AlternateFn = 0x0CU;
	else if (number == 13)
		AlternateFn = 0x0DU;
	else if (number == 15)
		AlternateFn = 0x0FU;
	else
		AlternateFn = 0;
	return this;
}

PIN PINSystem::GroupAPins[13];
PIN PINSystem::GroupBPins[8];
PIN PINSystem::GroupFPins[2];
	
PIN* PINSystem::GetGroupAPin(uint32_t number)
{
	GroupAPins[number].GPIO = GPIOA;
	GroupAPins[number].Number = GetActualPin(number);
	return &GroupAPins[number];
}

PIN* PINSystem::GetGroupBPin(uint32_t number)
{
	GroupBPins[number].GPIO = GPIOB;
	GroupBPins[number].Number = GetActualPin(number);
	return &GroupBPins[number];
}

PIN* PINSystem::GetGroupFPin(uint32_t number)
{
	GroupFPins[number].GPIO = GPIOF;
	GroupFPins[number].Number = GetActualPin(number);
	return &GroupFPins[number];
}

uint16_t PINSystem::GetActualPin(uint32_t number)
{
	if (number == 0)
		return GPIO_PIN_0;
	else if (number == 1)
		return GPIO_PIN_1;
	else if (number == 2)
		return GPIO_PIN_2;
	else if (number == 3)
		return GPIO_PIN_3;
	else if (number == 4)
		return GPIO_PIN_4;
	else if (number == 5)
		return GPIO_PIN_5;
	else if (number == 6)
		return GPIO_PIN_6;
	else if (number == 7)
		return GPIO_PIN_7;
	else if (number == 8)
		return GPIO_PIN_8;
	else if (number == 9)
		return GPIO_PIN_9;
	else if (number == 10)
		return GPIO_PIN_10;
	else if (number == 11)
		return GPIO_PIN_11;
	else if (number == 12)
		return GPIO_PIN_12;
	else if (number == 13)
		return GPIO_PIN_13;
	else if (number == 14)
		return GPIO_PIN_14;
	else if (number == 15)
		return GPIO_PIN_15;
	return 0;
}