/*
 * callbackrepositorysingleton.cpp
 *
 *  Created on: 2020/04/11
 *      Author: takemasa
 */

#include "callbackrepositorysingleton.hpp"
#include "murasaki_assert.hpp"

namespace murasaki {

CallbackRepositorySingleton::CallbackRepositorySingleton()
        :
        length_(0)
{

    for (int i = 0; i < NUM_OF_EXTI_OBJECTS; i++)
        objects_[i] = nullptr;
}

CallbackRepositorySingleton* CallbackRepositorySingleton::GetInstance()
{
    // The singleton instance is created when this member function is called first time.
    static CallbackRepositorySingleton repository;

    return &repository;
}

void CallbackRepositorySingleton::AddPeripheralObject(murasaki::PeripheralStrategy *peripheral_object) {
    // Check the number of the objects
    MURASAKI_ASSERT(length_ < NUM_OF_EXTI_OBJECTS)

    objects_[length_] = peripheral_object;
    length_++;

    if (length_ >= NUM_OF_CALLBACK_OBJECTS)
        length_ = NUM_OF_CALLBACK_OBJECTS - 1;
}

murasaki::PeripheralStrategy* CallbackRepositorySingleton::GetPeripheralObject(void *peripheral_handle) {
    // Check the number of the objects
    MURASAKI_ASSERT(length_ > 0)

    // Search all stored objects. Are there any one match with given peripheral?
    for (unsigned int i = 0; i < length_; i++)
        if (objects_[i]->Match(peripheral_handle))
            return objects_[i];
    // if not matched, that is error.
    MURASAKI_ASSERT(false)

    return nullptr;

}

} /* namespace murasaki */

