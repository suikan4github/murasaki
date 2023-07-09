/*
 * bitin.cpp
 *
 *  Created on: 2018/05/07
 *      Author: Seiichi "Suikan" Horie
 */

#include <bitin.hpp>
#include "murasaki_assert.hpp"

namespace murasaki {


BitIn::BitIn(GPIO_TypeDef* port, uint16_t pin) :
		gpio_ { port, pin } {

	MURASAKI_ASSERT(nullptr != port);
}

unsigned int BitIn::Get(void) {
	if (HAL_GPIO_ReadPin(gpio_.port_, gpio_.pin_) == GPIO_PIN_SET)
		return 1;
	else
		return 0;
}

void* BitIn::GetPeripheralHandle() {
	return const_cast<murasaki::GPIO_type*>(&gpio_);
}

} /* namespace murasaki */

