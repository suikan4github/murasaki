/*
 * bitout.cpp
 *
 *  Created on: 2018/05/07
 *      Author: takemasa
 */

#include <bitout.hpp>
#include "murasaki_assert.hpp"

#ifdef HAL_GPIO_MODULE_ENABLED


murasaki::BitOut::BitOut(GPIO_TypeDef* port, uint16_t pin) :
		gpio_ { port, pin }
{
	MURASAKI_ASSERT(nullptr != port);
}

void murasaki::BitOut::Set(unsigned int state) {
	if (state != 0)
		HAL_GPIO_WritePin(gpio_.port_, gpio_.pin_, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(gpio_.port_, gpio_.pin_, GPIO_PIN_RESET);

}

unsigned int murasaki::BitOut::Get(void) {
	if (HAL_GPIO_ReadPin(gpio_.port_, gpio_.pin_) == GPIO_PIN_SET)
		return 1;
	else
		return 0;
}

void* murasaki::BitOut::GetPeripheralHandle() {
	return const_cast<murasaki::GPIO_type*>(&gpio_);
}

#endif // HAL_GPIO_MODULE_ENABLE
