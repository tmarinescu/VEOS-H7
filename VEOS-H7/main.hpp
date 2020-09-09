#ifndef __MAIN_H__
#define __MAIN_H__

#include "stm32h7xx.h"
#include "stm32h7xx_hal.h"

/*Please leave unused defines, they were used at some point during debugging and might be used again*/

/*System handler priority register 3 (holds Systick & PendSV)*/
#define SHPR3							(*((volatile uint32_t*)0xE000ED20))

/*MemManage handler*/
//#define SHPR1							(*((volatile uint32_t*)0xE000ED18))

/*Interrupt control and state register*/
//#define ICSR							(*((volatile uint32_t*)0xE000ED04))

/*System handler control and state register*/
//#define SHCSR							(*((volatile uint32_t*)0xE000ED24))

/*Internal temperature sensor addresses as specified in datasheet*/
#define TEMPSENSE_CAL_ADDRESS_30C		(*((volatile uint16_t*)0x1FFF75A8))
#define TEMPSENSE_CAL_ADDRESS_110C		(*((volatile uint16_t*)0x1FFF75CA))

/*Internal voltage reference (usually around half of VDD)*/
#define VREFINT_CAL_ADDRESS				(*((volatile uint16_t*)0x1FFF75AA))

#endif
