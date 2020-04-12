/**
 * @file murasaki_callback.cpp
 *
 * @date 2020/04/12
 * @author Seiichi "Suikan" Horie
 * @brief A glue file between the callback and HAL/RTOS.
 */

// Include the definition created by CubeIDE.
#include <murasaki_platform.hpp>
#include "callbackrepositorysingleton.hpp"

// Include the murasaki class library.
#include "murasaki.hpp"

/* ------------------------- UART ---------------------------------- */

/**
 * @brief Essential to sync up with UART.
 * @ingroup MURASAKI_PLATFORM_GROUP
 * @param huart
 * @details
 * This is called from inside of HAL when an UART transmission done interrupt is accepted.
 *
 * STM32Cube HAL has same name function internally.
 * That function is invoked whenever an relevant interrupt happens.
 * In the other hand, that function is declared as weak bound.
 * As a result, this function overrides the default TX interrupt call back.
 *
 * In this call back, the uart device handle have to be passed to the
 * murasaki::Uart::TransmissionCompleteCallback() function.
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
                             {
    // Obtain the responding object.
    murasaki::PeripheralStrategy *peripheral = murasaki::CallbackRepositorySingleton::GetInstance()->GetPeripheralObject(huart);
    // Convert it to the appropriate type.
    murasaki::UartStrategy *uart = reinterpret_cast<murasaki::UartStrategy*>(peripheral);
    // Handle the callback by the object.
    uart->TransmitCompleteCallback(huart);
}

/**
 * @brief Essential to sync up with UART.
 * @ingroup MURASAKI_PLATFORM_GROUP
 * @param huart
 * @details
 * This is called from inside of HAL when an UART receive done interrupt is accepted.
 *
 * STM32Cube HAL has same name function internally.
 * That function is invoked whenever an relevant interrupt happens.
 * In the other hand, that function is declared as weak bound.
 * As a result, this function overrides the default RX interrupt call back.
 *
 * In this call back, the uart device handle have to be passed to the
 * murasaki::Uart::ReceiveCompleteCallback() function.
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
                             {
    // Obtain the responding object.
    murasaki::PeripheralStrategy *peripheral = murasaki::CallbackRepositorySingleton::GetInstance()->GetPeripheralObject(huart);
    // Convert it to the appropriate type.
    murasaki::UartStrategy *uart = reinterpret_cast<murasaki::UartStrategy*>(peripheral);
    // Handle the callback by the object.
    uart->ReceiveCompleteCallback(huart);
}

/**
 * @brief Optional error handling of UART
 * @ingroup MURASAKI_PLATFORM_GROUP
 * @param huart
 * @details
 * This is called from inside of HAL when an UART error interrupt is accepted.
 *
 * STM32Cube HAL has same name function internally.
 * That function is invoked whenever an relevant interrupt happens.
 * In the other hand, that function is declared as weak bound.
 * As a result, this function overrides the default error interrupt call back.
 *
 * In this call back, the uart device handle have to be passed to the
 * murasaki::Uart::HandleError() function.
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    // Obtain the responding object.
    murasaki::PeripheralStrategy *peripheral = murasaki::CallbackRepositorySingleton::GetInstance()->GetPeripheralObject(huart);
    // Convert it to the appropriate type.
    murasaki::UartStrategy *uart = reinterpret_cast<murasaki::UartStrategy*>(peripheral);
    // Handle the callback by the object.
    uart->HandleError(huart);

}

/* -------------------------- SPI ---------------------------------- */

#ifdef HAL_SPI_MODULE_ENABLED

/**
 * @brief Essential to sync up with SPI.
 * @ingroup MURASAKI_PLATFORM_GROUP
 * @param hspi
 * @details
 * This is called from inside of HAL when an SPI transfer done interrupt is accepted.
 *
 * STM32Cube HAL has same name function internally.
 * That function is invoked whenever an relevant interrupt happens.
 * In the other hand, that function is declared as weak bound.
 * As a result, this function overrides the default TX/RX interrupt call back.
 *
 * In this call back, the SPI device handle have to be passed to the
 * murasaki::Spi::TransmitAndReceiveCompleteCallback () function.
 */
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
    // Obtain the responding object.
    murasaki::PeripheralStrategy *peripheral = murasaki::CallbackRepositorySingleton::GetInstance()->GetPeripheralObject(hspi);
    // Convert it to the appropriate type.
    murasaki::SpiStrategy *spi = reinterpret_cast<murasaki::SpiStrategy*>(peripheral);
    // Handle the callback by the object.
    spi->TransmitAndReceiveCompleteCallback(hspi);
}

/**
 * @brief Optional error handling of SPI
 * @ingroup MURASAKI_PLATFORM_GROUP
 * @param hspi
 * @details
 * This is called from inside of HAL when an SPI error interrupt is accepted.
 *
 * STM32Cube HAL has same name function internally.
 * That function is invoked whenever an relevant interrupt happens.
 * In the other hand, that function is declared as weak bound.
 * As a result, this function overrides the default error interrupt call back.
 *
 * In this call back, the uart device handle have to be passed to the
 * murasaki::Uart::HandleError() function.
 */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi) {
    // Obtain the responding object.
    murasaki::PeripheralStrategy *peripheral = murasaki::CallbackRepositorySingleton::GetInstance()->GetPeripheralObject(hspi);
    // Convert it to the appropriate type.
    murasaki::SpiStrategy *spi = reinterpret_cast<murasaki::SpiStrategy*>(peripheral);
    // Handle the callback by the object.
    spi->HandleError(hspi);
}

#endif

/* -------------------------- I2C ---------------------------------- */

#ifdef HAL_I2C_MODULE_ENABLED

/**
 * @brief Essential to sync up with I2C.
 * @ingroup MURASAKI_PLATFORM_GROUP
 * @param hi2c
 * @details
 * This is called from inside of HAL when an I2C transmission done interrupt is accepted.
 *
 * STM32Cube HAL has same name function internally.
 * That function is invoked whenever an relevant interrupt happens.
 * In the other hand, that function is declared as weak bound.
 * As a result, this function overrides the default TX interrupt call back.
 *
 * In this call back, the uart device handle have to be passed to the
 * murasaki::I2c::TransmitCompleteCallback() function.
 */
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
                                  {
    // Obtain the responding object.
    murasaki::PeripheralStrategy *peripheral = murasaki::CallbackRepositorySingleton::GetInstance()->GetPeripheralObject(hi2c);
    // Convert it to the appropriate type.
    murasaki::I2cStrategy *i2c = reinterpret_cast<murasaki::I2cStrategy*>(peripheral);
    // Handle the callback by the object.
    i2c->TransmitCompleteCallback(hi2c);

}

/**
 * @brief Essential to sync up with I2C.
 * @param hi2c
 * @details
 * This is called from inside of HAL when an I2C receive done interrupt is accepted.
 *
 * STM32Cube HAL has same name function internally.
 * That function is invoked whenever an relevant interrupt happens.
 * In the other hand, that function is declared as weak bound.
 * As a result, this function overrides the default RX interrupt call back.
 *
 * In this call back, the uart device handle have to be passed to the
 * murasaki::Uart::ReceiveCompleteCallback() function.
 */
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c) {
    // Obtain the responding object.
    murasaki::PeripheralStrategy *peripheral = murasaki::CallbackRepositorySingleton::GetInstance()->GetPeripheralObject(hi2c);
    // Convert it to the appropriate type.
    murasaki::I2cStrategy *i2c = reinterpret_cast<murasaki::I2cStrategy*>(peripheral);
    // Handle the callback by the object.
    i2c->ReceiveCompleteCallback(hi2c);
}
/**
 * @brief Essential to sync up with I2C.
 * @ingroup MURASAKI_PLATFORM_GROUP
 * @param hi2c
 * @details
 * This is called from inside of HAL when an I2C transmission done interrupt is accepted.
 *
 * STM32Cube HAL has same name function internally.
 * That function is invoked whenever an relevant interrupt happens.
 * In the other hand, that function is declared as weak bound.
 * As a result, this function overrides the default TX interrupt call back.
 *
 * In this call back, the I2C slave device handle have to be passed to the
 * murasaki::I2cSlave::TransmitCompleteCallback() function.
 */
void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c)
                                 {
    // Obtain the responding object.
    murasaki::PeripheralStrategy *peripheral = murasaki::CallbackRepositorySingleton::GetInstance()->GetPeripheralObject(hi2c);
    // Convert it to the appropriate type.
    murasaki::I2cStrategy *i2c = reinterpret_cast<murasaki::I2cStrategy*>(peripheral);
    // Handle the callback by the object.
    i2c->TransmitCompleteCallback(hi2c);
}

/**
 * @brief Essential to sync up with I2C.
 * @param hi2c
 * @details
 * This is called from inside of HAL when an I2C receive done interrupt is accepted.
 *
 * STM32Cube HAL has same name function internally.
 * That function is invoked whenever an relevant interrupt happens.
 * In the other hand, that function is declared as weak bound.
 * As a result, this function overrides the default RX interrupt call back.
 *
 * In this call back, the I2C slave device handle have to be passed to the
 * murasaki::I2cSlave::ReceiveCompleteCallback() function.
 */
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c) {
    // Obtain the responding object.
    murasaki::PeripheralStrategy *peripheral = murasaki::CallbackRepositorySingleton::GetInstance()->GetPeripheralObject(hi2c);
    // Convert it to the appropriate type.
    murasaki::I2cStrategy *i2c = reinterpret_cast<murasaki::I2cStrategy*>(peripheral);
    // Handle the callback by the object.
    i2c->ReceiveCompleteCallback(hi2c);
}

/**
 * @brief Optional error handling of I2C
 * @ingroup MURASAKI_PLATFORM_GROUP
 * @param hi2c
 * @details
 * This is called from inside of HAL when an I2C error interrupt is accepted.
 *
 * STM32Cube HAL has same name function internally.
 * That function is invoked whenever an relevant interrupt happens.
 * In the other hand, that function is declared as weak bound.
 * As a result, this function overrides the default error interrupt call back.
 *
 * In this call back, the uart device handle have to be passed to the
 * murasaki::I2c::HandleError() function.
 */
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c) {
    // Obtain the responding object.
    murasaki::PeripheralStrategy *peripheral = murasaki::CallbackRepositorySingleton::GetInstance()->GetPeripheralObject(hi2c);
    // Convert it to the appropriate type.
    murasaki::I2cStrategy *i2c = reinterpret_cast<murasaki::I2cStrategy*>(peripheral);
    // Handle the callback by the object.
    i2c->HandleError(hi2c);

}

#endif
/* ------------------ ADC -------------------------- */

#ifdef HAL_ADC_MODULE_ENABLED

/**
 * @brief Optional interrupt handler for ADC.
 * @param hadc Pointer to the Adc control structure.
 * @details
 * This is called from inside of HAL when an ADC conversion is done.
 *
 * STM32Cube HAL has same name function internally.
 * That function is invoked whenever an relevant interrupt happens.
 * In the other hand, that function is declared as weak bound.
 * As a result, this function overrides the default error interrupt call back.
 *
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
                              {

    // Obtain the responding object.
    murasaki::PeripheralStrategy *peripheral = murasaki::CallbackRepositorySingleton::GetInstance()->GetPeripheralObject(hadc);
    // Convert it to the appropriate type.
    murasaki::AdcStrategy *adc = reinterpret_cast<murasaki::AdcStrategy*>(peripheral);
    // Handle the callback by the object.
    adc->ConversionCompleteCallback(hadc);
}

/**
 * @brief Optional interrupt handler for ADC.
 * @param hadc Pointer to the Adc control structure.
 * @details
 * This is called from inside of HAL when an ADC conversion failed by Error.
 *
 *
 */
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
                           {
    // Obtain the responding object.
    murasaki::PeripheralStrategy *peripheral = murasaki::CallbackRepositorySingleton::GetInstance()->GetPeripheralObject(hadc);
    // Convert it to the appropriate type.
    murasaki::AdcStrategy *adc = reinterpret_cast<murasaki::AdcStrategy*>(peripheral);
    // Handle the callback by the object.
    adc->HandleError(hadc);
}

#endif

/* ------------------ SAI  -------------------------- */
#ifdef HAL_SAI_MODULE_ENABLED
/**
 * @brief Optional SAI interrupt handler at buffer transfer halfway.
 * @ingroup MURASAKI_PLATFORM_GROUP
 * @param hsai Handler of the SAI device.
 * @details
 * Invoked after SAI RX DMA complete interrupt is at halfway.
 * This interrupt have to be forwarded to the  murasaki::DuplexAudio::ReceiveCallback().
 * The second parameter of the ReceiveCallback() have to be 0 which mean the halfway interrupt.
 */
void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai) {
    // Obtain the responding object.
    murasaki::PeripheralStrategy *peripheral = murasaki::CallbackRepositorySingleton::GetInstance()->GetPeripheralObject(hsai);
    // Convert it to the appropriate type.
    murasaki::DuplexAudio *audio = reinterpret_cast<murasaki::DuplexAudio*>(peripheral);
    // Handle the callback by the object.
    audio->DmaCallback(hsai, 0);

}

/**
 * @brief Optional SAI interrupt handler at buffer transfer complete.
 * @ingroup MURASAKI_PLATFORM_GROUP
 * @param hsai Handler of the SAI device.
 * @details
 * Invoked after SAI RX DMA complete interrupt is at halfway.
 * This interrupt have to be forwarded to the  murasaki::DuplexAudio::ReceiveCallback().
 * The second parameter of the ReceiveCallback() have to be 1 which mean the complete interrupt.
 */
void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai) {
    // Obtain the responding object.
    murasaki::PeripheralStrategy *peripheral = murasaki::CallbackRepositorySingleton::GetInstance()->GetPeripheralObject(hsai);
    // Convert it to the appropriate type.
    murasaki::DuplexAudio *audio = reinterpret_cast<murasaki::DuplexAudio*>(peripheral);
    // Handle the callback by the object.
    audio->DmaCallback(hsai, 1);
}

/**
 * @brief Optional SAI error interrupt handler.
 * @ingroup MURASAKI_PLATFORM_GROUP
 * @param hsai Handler of the SAI device.
 * @details
 * The error have to be forwarded to murasaki::DuplexAudio::HandleError().
 * Note that DuplexAudio::HandleError() trigger a hard fault.
 * So, never return.
 */

void HAL_SAI_ErrorCallback(SAI_HandleTypeDef *hsai) {
    // Obtain the responding object.
    murasaki::PeripheralStrategy *peripheral = murasaki::CallbackRepositorySingleton::GetInstance()->GetPeripheralObject(hsai);
    // Convert it to the appropriate type.
    murasaki::DuplexAudio *audio = reinterpret_cast<murasaki::DuplexAudio*>(peripheral);
    // Handle the callback by the object.
    audio->HandleError(hsai);
}

#endif

/* ------------------ I2S  -------------------------- */
#ifdef HAL_I2S_MODULE_ENABLED
/**
 * @brief Optional I2S interrupt handler at buffer transfer halfway.
 * @ingroup MURASAKI_PLATFORM_GROUP
 * @param hsai Handler of the I2S device.
 * @details
 * Invoked after I2S RX DMA complete interrupt is at halfway.
 * This interrupt have to be forwarded to the  murasaki::DuplexAudio::ReceiveCallback().
 * The second parameter of the ReceiveCallback() have to be 0 which mean the halfway interrupt.
 */
void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s) {
    // Obtain the responding object.
    murasaki::PeripheralStrategy *peripheral = murasaki::CallbackRepositorySingleton::GetInstance()->GetPeripheralObject(hi2s);
    // Convert it to the appropriate type.
    murasaki::DuplexAudio *audio = reinterpret_cast<murasaki::DuplexAudio*>(peripheral);
    // Handle the callback by the object.
    audio->DmaCallback(hi2s, 0);
}

/**
 * @brief Optional I2S interrupt handler at buffer transfer complete.
 * @ingroup MURASAKI_PLATFORM_GROUP
 * @param hsai Handler of the I2S device.
 * @details
 * Invoked after I2S RX DMA complete interrupt is at halfway.
 * This interrupt have to be forwarded to the  murasaki::DuplexAudio::ReceiveCallback().
 * The second parameter of the ReceiveCallback() have to be 1 which mean the complete interrupt.
 */
void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s) {
    // Obtain the responding object.
    murasaki::PeripheralStrategy *peripheral = murasaki::CallbackRepositorySingleton::GetInstance()->GetPeripheralObject(hi2s);
    // Convert it to the appropriate type.
    murasaki::DuplexAudio *audio = reinterpret_cast<murasaki::DuplexAudio*>(peripheral);
    // Handle the callback by the object.
    audio->DmaCallback(hi2s, 1);
}

/**
 * @brief Optional I2S error interrupt handler.
 * @ingroup MURASAKI_PLATFORM_GROUP
 * @param hsai Handler of the I2S device.
 * @details
 * The error have to be forwarded to murasaki::DuplexAudio::HandleError().
 * Note that DuplexAudio::HandleError() trigger a hard fault.
 * So, never return.
 */

void HAL_I2S_ErrorCallback(I2S_HandleTypeDef *hi2s) {
    // Obtain the responding object.
    murasaki::PeripheralStrategy *peripheral = murasaki::CallbackRepositorySingleton::GetInstance()->GetPeripheralObject(hi2s);
    // Convert it to the appropriate type.
    murasaki::DuplexAudio *audio = reinterpret_cast<murasaki::DuplexAudio*>(peripheral);
    // Handle the callback by the object.
    audio->HandleError(hi2s);
}
#endif

/* ------------------ ASSERTION AND ERROR -------------------------- */

/**
 * @brief Hook for the assert_failure() in main.c
 * @ingroup MURASAKI_PLATFORM_GROUP
 * @param file Name of the source file where assertion happen
 * @param line Number of the line where assertion happen
 * @details
 * This routine provides a custom hook for the assertion inside STM32Cube HAL.
 * All assertion raised in HAL will be redirected here.
 *
 * @code
 * void assert_failed(uint8_t* file, uint32_t line)
 * {
 *     CustomAssertFailed(file, line);
 * }
 * @endcode
 * By default, this routine output a message with location informaiton
 * to the debugger console.
 */
extern "C" void CustomAssertFailed(uint8_t *file, uint32_t line)
                                   {
    murasaki::debugger->Printf("Wrong parameters value: file %s on line %d\n",
                               file,
                               line);
    // To stop the execusion, raise assert.
    MURASAKI_ASSERT(false);
}

/*
 * CustmDefaultHanlder :
 *
 * An entry of the exception. Especialy for the Hard Fault exception.
 * In this function, the Stack pointer just before exception is retrieved
 * and pass as the first parameter of the PrintFaultResult().
 *
 * Note : To print the correct information, this function have to be
 * Jumped in from the exception entry without any data push to the stack.
 * To avoid the pushing extra data to stack or making stack frame,
 * Compile the program without debug information and with certain
 * optimization leve, when you investigate the Hard Fault.
 */
__asm volatile (
        ".global CustomDefaultHandler \n"
        "CustomDefaultHandler: \n"
        " movs r0,#4       \n"
        " movs r1, lr      \n"
        " tst r0, r1       \n"
        " beq _MSP         \n"
        " mrs r0, psp      \n"
        " b _HALT          \n"
        "_MSP:               \n"
        " mrs r0, msp      \n"
        "_HALT:              \n"
        " ldr r1,[r0,#20]  \n"
        " b PrintFaultResult \n"
        " bkpt #0          \n"
);

/**
 * @brief Printing out the context information.
 * @param stack_pointer retrieved stack pointer before interrupt / exception.
 * @details
 * Do not call from application. This is murasaki_internal_only.
 *
 */
extern "C" void PrintFaultResult(unsigned int *stack_pointer) {

    murasaki::debugger->Printf("\nSpurious exception or hardfault occured.  \n");
    murasaki::debugger->Printf("Stacked R0  : 0x%08X \n", stack_pointer[0]);
    murasaki::debugger->Printf("Stacked R1  : 0x%08X \n", stack_pointer[1]);
    murasaki::debugger->Printf("Stacked R2  : 0x%08X \n", stack_pointer[2]);
    murasaki::debugger->Printf("Stacked R3  : 0x%08X \n", stack_pointer[3]);
    murasaki::debugger->Printf("Stacked R12 : 0x%08X \n", stack_pointer[4]);
    murasaki::debugger->Printf("Stacked LR  : 0x%08X \n", stack_pointer[5]);
    murasaki::debugger->Printf("Stacked PC  : 0x%08X \n", stack_pointer[6]);
    murasaki::debugger->Printf("Stacked PSR : 0x%08X \n", stack_pointer[7]);

    murasaki::debugger->Printf("       CFSR : 0x%08X \n", *(unsigned int*) 0xE000ED28);
    murasaki::debugger->Printf("       HFSR : 0x%08X \n", *(unsigned int*) 0xE000ED2C);
    murasaki::debugger->Printf("       DFSR : 0x%08X \n", *(unsigned int*) 0xE000ED30);
    murasaki::debugger->Printf("       AFSR : 0x%08X \n", *(unsigned int*) 0xE000ED3C);

    murasaki::debugger->Printf("       MMAR : 0x%08X \n", *(unsigned int*) 0xE000ED34);
    murasaki::debugger->Printf("       BFAR : 0x%08X \n", *(unsigned int*) 0xE000ED38);

    murasaki::debugger->Printf("(Note : To avoid the stacking by C compiler, use release build to investigate the fault. ) \n");

    murasaki::debugger->DoPostMortem();
}

/**
 * @brief StackOverflow hook for FreeRTOS
 * @param xTask Task ID which causes stack overflow.
 * @param pcTaskName Name of the task which cuases stack overflow.
 * @fn vApplicationStackOverflowHook
 * @details
 * This function will be called from FreeRTOS when some task causes overflow.
 * See TaskStrategy::getStackMinHeadroom() for details.
 */
extern "C" void vApplicationStackOverflowHook(TaskHandle_t xTask,
                                              signed char *pcTaskName) {
    murasaki::debugger->Printf("Stack overflow at task : %s \n", pcTaskName);
    MURASAKI_ASSERT(false);
}
