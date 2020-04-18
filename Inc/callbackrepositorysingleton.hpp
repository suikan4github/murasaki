/**
 * @file callbackrepositorysingleton.hpp
 *
 * @date 2020/04/11
 * @author takemasa
 */

#ifndef CALLBACKREPOSITORYSINGLETON_HPP_
#define CALLBACKREPOSITORYSINGLETON_HPP_

#include "murasaki_config.hpp"
#include "peripheralstrategy.hpp"

namespace murasaki {

/**
 * @brief Repository of the interrupt handling objects.
 * @ingroup MURASAKI_HELPER_GROUP
 * @details
 * A Singleton design repository. This is Murasaki internal only. Do not use in the application.
 *
 * This class object stores the list of the interrupt callback responding objects.
 * The registration is done in the  client classe's construction phase. So, it is invisible from the
 * application programmer.
 *
 * For each interrupt, the callback routines call the @ref GetPeripheralObject() with given
 * peripheral handler. The GetPeripheralHandler() routine search for the list whether
 * responding object exists or not. If exist, return that object.
 *
 * The EXTI is not supported by this class.
 *
 * This class is not thread safe.
 */
class CallbackRepositorySingleton {
 public:
    /**
     * @brief Return a singleton instance.
     * @return Instance of this class.
     * For each time of calling, this function returns identical pointer.
     */
    static CallbackRepositorySingleton* GetInstance();

    /**
     * @brief Add the given object to the internal list of the peripheral
     * @param peripheral_object
     * @details
     * Store the given object to the internal list. This member function is called from
     * only inside @ref PeripheralStrategy or its sub classes.
     */
    void AddPeripheralObject(murasaki::PeripheralStrategy *peripheral_object);

    /**
     * @brief Search an interrupt responding object.
     * @param peripheral_handle
     * @return Matched object pointer.
     * @details
     * If the object is not found, Assertion failed.
     */
    murasaki::PeripheralStrategy* GetPeripheralObject(void *peripheral_handle);

 private:
    /**
     * @brief Constructor
     */
    CallbackRepositorySingleton();

    // Current length of list.
    unsigned int length_;
    // List of the objects.
    murasaki::PeripheralStrategy *objects_[NUM_OF_CALLBACK_OBJECTS];

};

} /* namespace murasaki */

#endif /* CALLBACKREPOSITORYSINGLETON_HPP_ */
