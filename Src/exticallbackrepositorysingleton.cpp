/*
 * exticallbackrepositorysingleton.cpp
 *
 *  Created on: 2020/04/18
 *      Author: takemasa
 */

#include "exticallbackrepositorysingleton.hpp"
#include "murasaki_assert.hpp"

namespace murasaki {

ExtiCallbackRepositorySingleton::ExtiCallbackRepositorySingleton()
        :
        length_(0)
{

    for (int i = 0; i < NUM_OF_EXTI_OBJECTS; i++)
        objects_[i] = nullptr;
}

ExtiCallbackRepositorySingleton* ExtiCallbackRepositorySingleton::GetInstance()
{
    // The singleton instance is created when this member function is called first time.
    static ExtiCallbackRepositorySingleton repository;

    return &repository;
}

void ExtiCallbackRepositorySingleton::AddExtiObject(murasaki::InterruptStrategy *exti_object) {
    // Check the number of the objects
    MURASAKI_ASSERT(length_ < NUM_OF_EXTI_OBJECTS)

    objects_[length_] = exti_object;
    length_++;
    if (length_ >= NUM_OF_EXTI_OBJECTS)
        length_ = NUM_OF_EXTI_OBJECTS - 1;
}

murasaki::InterruptStrategy* ExtiCallbackRepositorySingleton::GetExtiObject(unsigned int line) {
    // Check the number of the objects
    MURASAKI_ASSERT(length_ > 0)

    // Search all stored objects. Are there any one match with given exti?
    for (unsigned int i = 0; i < length_; i++)
        if (objects_[i]->Match(line))
            return objects_[i];
    // if not matched, that is error.
    MURASAKI_ASSERT(false)

    return nullptr;

}

} /* namespace murasaki */

