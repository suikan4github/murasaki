/**
 * \file allocators.cpp
 *
 * \date 2018/05/02
 * \author Seiichi "Suikan" Horie
 * \brief Alternative memory allocators.
 * \details
 * These definitions allows to used the FreeRTOS's heap instead of the system heap.
 *
 * The system heap by the standard library doesn't check the limit of the heap cerefly.
 * As a result, it is not clear how to detect the over commiting memory.
 *
 * FreeRTOS hepa is considered safer than system heap. Then, the new and the delete
 * operators are overloaded to use the pvPortMalloc().
 */

#include <cstddef>
#include <FreeRTOS.h>

/**
 * @ingroup MURASAKI_HELPER_GROUP
 * @brief Allocate a memory piece with given size.
 * @param size Size of the memory to allocate [byte]
 * @return Allocated memory in FreeRTOS heap. Null mean fail to allocate.
 */
void* operator new(std::size_t size)
{
	return pvPortMalloc(size);
}

/**
 * @ingroup MURASAKI_HELPER_GROUP
 * @brief Allocate a memory piece with given size.
 * @param size Size of the memory to allocate [byte]
 * @return Allocated memory in FreeRTOS heap. Null mean fail to allocate.
 */
void* operator new[](std::size_t size) {
	return pvPortMalloc(size);
}

/**
 * @ingroup MURASAKI_HELPER_GROUP
 * @brief Deallocate the given memory
 * @param ptr Pointer to the memory to deallocate
 * @return Allocated memory in FreeRTOS heap. Null mean fail to allocate.
 */
void operator delete(void* ptr) {
	vPortFree(ptr);
}

/**
 * @ingroup MURASAKI_HELPER_GROUP
 * @brief Deallocate the given memory
 * @param ptr Pointer to the memory to deallocate
 * @return Allocated memory in FreeRTOS heap. Null mean fail to allocate.
 */
void operator delete[](void* ptr) {
	vPortFree(ptr);
}


