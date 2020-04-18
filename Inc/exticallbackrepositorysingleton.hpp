/**
 * @file exticallbackrepositorysingleton.hpp
 *
 * @date 2020/04/18
 * @author takemasa
 */

#ifndef EXTICALLBACKREPOSITORYSINGLETON_HPP_
#define EXTICALLBACKREPOSITORYSINGLETON_HPP_

#include "murasaki_config.hpp"
#include "interruptstrategy.hpp"

namespace murasaki {

/**
 * @brief Repository of the EXTI handling objects.
 * @ingroup MURASAKI_HELPER_GROUP
 * @details
 * A Singleton design repository. This is Murasaki internal only. Do not use in the application.
 *
 * This class object stores the list of the EXTI callback responding objects.
 * The registration is done in the client classe's construction phase. So, it is invisible from the
 * application programmer.
 *
 * For each EXTI, the callback routine calls the @ref GetExtiObject() with given
 * peripheral handler. The GetExtiHandler() routine search for the list whether
 * responding object exists or not. If exists, return that object.
 *
 * Only EXTI is supported by this class.
 *
 * This class is not thread safe.
 */
class ExtiCallbackRepositorySingleton {
 public:
    /**
     * @brief Return a singleton instance.
     * @return Instance of this class.
     * For each time of calling, this function returns identical pointer.
     */
    static ExtiCallbackRepositorySingleton* GetInstance();

    /**
     * @brief Add the given object to the internal list of the EXTI
     * @param peripheral_object
     * @details
     * Store the given object to the internal list. This member function is called from
     * only inside @ref InterruptStrategy or its sub classes.
     */
    void AddExtiObject(murasaki::InterruptStrategy *exti_object);

    /**
     * @brief Search an EXTI responding object.
     * @param peripheral_handle
     * @return Matched object pointer.
     * @details
     * If the object is not found, Assertion failed.
     */
    murasaki::InterruptStrategy* GetExtiObject(unsigned int line);

 private:
    /**
     * @brief Constructor
     */
    ExtiCallbackRepositorySingleton();

    // Current length of list.
    unsigned int length_;
    // List of the objects.
    murasaki::InterruptStrategy *objects_[NUM_OF_EXTI_OBJECTS];

};

} /* namespace murasaki */

#endif /* EXTICALLBACKREPOSITORYSINGLETON_HPP_ */
