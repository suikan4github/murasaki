/**
 * @file interruptstrategy.hpp
 *
 * @date Feb 29, 2020
 * @author Seiichi "Suikan" Horie
 */

#ifndef INTERRUPTSTRATEGY_HPP_
#define INTERRUPTSTRATEGY_HPP_

#include "murasaki_defs.hpp"

namespace murasaki {

/**
 * \ingroup MURASAKI_ABSTRACT_GROUP
 * @brief Abstract interrupt class.
 * @details
 * This is a mother of the interrupt classes. The interrut class provides following functionalities :
 * @li Enable/ Disable interrupt
 * @li Synchronization between interrupt and task.
 */
class InterruptStrategy {
 public:
    virtual ~InterruptStrategy() {
    }
    ;
    /**
     * @brief Enable interrupt
     */
    virtual void Enable() = 0;

    /**
     * @brief Disable interrupt
     */
    virtual void Disable() = 0;

    /**
     * @details Wait for an interrupt from task.
     * @param timeout time out parameter
     * @return kisOK : Released by interrupt. kisTimeOut : released by timeout.
     */
    virtual murasaki::InterruptStatus Wait(unsigned int timeout = murasaki::kwmsIndefinitely) = 0;

    /**
     * @details Release the waiting task
     * @param line The identifier of the interrupt
     * @return true if line is matched with this EXTI. false if not matched.
     */
    virtual bool Release(unsigned int line) = 0;
};

}
;
/* namespace murasaki */

#endif /* INTERRUPTSTRATEGY_HPP_ */
