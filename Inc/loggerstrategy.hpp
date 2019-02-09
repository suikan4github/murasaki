/**
 * \file loggerstrategy.hpp
 *
 * \date 2018/01/20
 * \author: takemasa
 * \brief Simplified logging function.
 */

#ifndef LOGGERSTRATEGY_HPP_
#define LOGGERSTRATEGY_HPP_

#include <peripheralstrategy.hpp>
#include <stdint.h>
namespace murasaki {

/**
 * \ingroup MURASAKI_ABSTRACT_GROUP
 * \{
 */
/**
 * \brief Abstract class for logging.
 * \details
 * A generic class to serve a logging function. This class is designed to pass to the
 * \ref murasaki::Debugger.
 *
 * As a service class to Debug. This class's two member functions ( putMessage() and getCharacter() )
 * have to be able to run in the task context. Both member functions also have to be the
 * blocking function.
 */
class LoggerStrategy
{
 public:
    /**
     * \brief Detructor.
     * \details
     * Do nothing here. Declared to enforce the derived class's constructor as "virtual".
     *
     */
    virtual ~LoggerStrategy()
    {
    }
    ;
    /**
     * \brief Message output member function.
     * \param message Non null terminated character array. This data is stored or output to the logger.
     * \param size Byte length of the message parameter of the putMessage member function.
     * \details
     * This function is considered as blcoking. That mean, it will not wayt until
     * data is stored to the storage or output.
     */
    virtual void putMessage(char message[], unsigned int size) = 0;
    /**
     * \brief Character input member function.
     * \return A character from input is returned.
     * \details
     * This function is considered as blocking. That mean, the function will wait for
     * any user input forever.
     */
    virtual char getCharacter() = 0;
    /**
     * \brief Start post mortem process
     * \param debugger_fifo Pointer to the DebuggerFifo class object. This is declared as void to avoid the include confusion.
     * This member function read the data in given FIFO, and then do the auto history.
     *
     * By default this is not implemented. But in case user implments a method, it should call the
     * Debugger::SetPostMortem() internaly.
     */
    virtual void DoPostMortem( void * debugger_fifo){while(true);}
 protected:
    /**
     * \brief This special method helps derived loggers. The loggers can access the raw device, in case of the
     * post mortem processing.
     */
    static void * GetPeripheralHandle(murasaki::PeripheralStrategy * peripheral);
};
/**
 * \}
 */

} /* namespace murasaki */

inline void* murasaki::LoggerStrategy::GetPeripheralHandle(
		murasaki::PeripheralStrategy* peripheral)
{
	return(peripheral->GetPeripheralHandle());
}

#endif /* LOGGERSTRATEGY_HPP_ */
