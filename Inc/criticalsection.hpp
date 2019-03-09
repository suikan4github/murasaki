/**
 * \file criticalsection.hpp
 *
 * \date 2018/01/27
 * \author Seiichi "Suikan" Horie
 * @brief Class to protect a certain section from the interference.
 */

#ifndef CRITICALSECTION_HPP_
#define CRITICALSECTION_HPP_
#include <FreeRTOS.h>
#include <semphr.h>

namespace murasaki {

/**
 * \ingroup MURASAKI_SYNC_GROUP Synchronization and Exclusive access
 * \{
 */
/**
 * \brief A critical section for task context.
 * \details
 * The critical section prevent other task to preempt that critical section. So, a task can modify the
 * shared variable safely inside critical seciton.
 *
 * This class provide a critical section for the task context only. This critical section is not
 * protected from the ISR.
 *
 * The critical section have to start by \ref CriticalSection::Enter() and quit by \ref CriticalSection::Leave().
 */
class CriticalSection
{
 public:
    /**
     * \brief Constructor. Creating semaphore internally.
     */
    CriticalSection();
    /**
     * \brief Destructor. Deleting sempahore internally.
     */
    virtual ~CriticalSection();
    /**
     * \brief Entering critical section
     * \details
     * Entering critical section in task context. No other task can preemptive
     * the task inside critical section.
     */
    void Enter();
    /**
     * \brief Leaving crititical section
     * \details
     * All critical seciton started by CriticalSection::Enter() have to be quit by this
     * member function.
     */
    void Leave();
 private:
    SemaphoreHandle_t const mutex_;
};

/**
 * \} MURASAKI_SYNC_GROUP
 */
} /* namespace murasaki */

#endif /* CRITICALSECTION_HPP_ */
