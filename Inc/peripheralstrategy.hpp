/**
 * @file peripheralstrategy.hpp
 *
 * @date 2018/04/26
 * @author: Seiichi "Suikan" Horie
 * @brief Mother of All peripheral
 */

#ifndef PERIPHERALSTRATEGY_HPP_
#define PERIPHERALSTRATEGY_HPP_

#include "murasaki_defs.hpp"

namespace murasaki {

/**
 * \ingroup MURASAKI_ABSTRACT_GROUP
 * @brief Mother of all peripheral class
 * @details
 * This class provides the GetPeripheralHandle() member function as a common stub for
 * the debugging logger. The loggers sometimes refers the
 * raw peripheral to respond to the post mortem situation. By using class, programmer can pass the
 * raw peripheral handler to loggers, while keep it hidden from the application.
 */
class PeripheralStrategy {
public:
	/**
	 * @brief destructor
	 */
	virtual ~PeripheralStrategy(){};
     protected:
	/**
	 * @brief pass the raw peripheral handler
	 * @return pointer to the raw peripheral handler hidden in a class.
	 */
	virtual void * GetPeripheralHandle()=0;
friend class LoggerStrategy;
};

} /* namespace murasaki */

#endif /* PERIPHERALSTRATEGY_HPP_ */
