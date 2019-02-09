/**
 * \file synchronizer.hpp
 *
 * \date 2018/01/26
 * \author takemasa
 * \brief Synchronization between a Task and interrupt.
 */

#ifndef SYNCHRONIZER_HPP_
#define SYNCHRONIZER_HPP_

#include <FreeRTOS.h>
#include <semphr.h>
#include <murasaki_defs.hpp>

namespace murasaki {

/**
 * \brief Synchronization class between a task and interrupt.
 * This class provide the synchronization between a task and interrupt.
 *
 * Synchronization mean, task waits for a interrupt by calling
 * InterruptSynchronizer::WaitForInterruptFromTask() and during the wait, task yields the cpu to
 * other task. So, CPU can do other job during a task is waiting for interrupt. Interrupt will
 * allow task run again by InterruptSynchronizer::ReleasetaskFromISR() member function.
 * \ingroup MURASAKI_SYNC_GROUP
 *
 */
class Synchronizer
{
 public:
    /**
     * \brief Constructor. Asignning semaphore internally.
     */
    Synchronizer();
    /**
     * \brief Destructor. Deleting semaphore internally.
     */
    virtual ~Synchronizer();
    /**
     * \brief Let the task wait for an interrupt.
     * \param timeout_ms Timeout by millisecond. The default value let the task wait for interrupt forever.
     * \return True if interrupt came before timeout. False if timeout happen.
     * \details
     * This member function have to be called from the task context. Otherwise, the behavior is
     * not predictable.
     */
    bool Wait(WaitMilliSeconds timeout_ms = kwmsIndefinitely);
    /**
     * \brief Release the task.
     * \details
     * Release the task waiting. This member function must be called from both task and the interrupt context.
     */
    void Release();
 protected:
    SemaphoreHandle_t const semaphore_;
};

} /* namespace murasaki */

#endif /* SYNCHRONIZER_HPP_ */
