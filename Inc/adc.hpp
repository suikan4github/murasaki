/**
 * @file adc.hpp
 * @brief AD Coverter class.
 * @date Feb 22, 2020
 * @author Seiichi Horie
 */

#ifndef ADC_HPP_
#define ADC_HPP_

#include "synchronizer.hpp"
#include "criticalsection.hpp"
#include "adcstrategy.hpp"

namespace murasaki {

#ifdef HAL_ADC_MODULE_ENABLED

/**
 * @brief STM32 dedicated ADC class.
 * @details
 * This is a single conversion ADC class. That mean, Only one specified channel will be converted at onces and then, return.
 * Optionally, The sampling clock duration can be set for each channels.
 *
 * ### Configuration
 * To configure an analog input, select an ADC to use, and check the ADC input line.
 * Note that even checking multiple lines, you can convert only one line at once.
 *
 * @image html "adc-config-1.png"
 * @image latex "adc-config-1.png" width=3.42in
 *
 * Then, enable the NVIC interrupt line for the ADC.
 *
 * @image html "adc-config-2.png"
 * @image latex "adc-config-2.png" width=4.09in
 *
 *
 * ### Creating a peripheral object
 *
 * To create an ADC object, use new operator to the murasaki::Adc class.
 * The parameter is the handle of that ADC.
 * The handle will be generated by CubeIDE around the top of the main.c.
 *
 * @code
 *   murasaki::platform.adc = new murasaki::Adc(&hadc1);
 *   MURASAKI_ASSERT(nullptr != murasaki::platform.adc)
 * @endcode
 *
 * ### Interrupt Handling
 *
 * The interrupt handler is the HAL_ADC_ConvCpltCallback() and the HAL_ADC_ErrorCallback().
 * These are regulated by HAL manual. If you define these functions, HAL will call that
 * callback whenever the relevant interrupt happens.
 *
 * Inside HAL_ADC_ConvCpltCallback(), you must call the Adc::ConversionCompleteCallback() member function.
 * The parameter should be the hadc parameter of the Interrupt callback.
 * The Adc::ConversionCompleteCallback() will return with if the given hadc is the handle of that object.
 * If the Adc::ConversionCompleteCallback() return true, you can return from the handler.
 *
 * The HAL_ADC_ErrorCallback() is similar but you must call Adc::HandleError().
 * @code
 * void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
 *     if (murasaki::platform.adc->ConversionCompleteCallback(hadc))
 *         return;
 * }
 *
 * void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc) {
 *     if (murasaki::platform.adc->HandleError(hadc))
 *         return;
 * }
 *
 * @endcode
 *
 * ### ADC data reading
 * To read the data, use Adc::Convert() member function.
 * @code
 * murasaki::platform.adc->Convert(ADC_CHANNEL_TEMPSENSOR, &value);
 * @endcode
 *
 * \ingroup MURASAKI_GROUP
 */
class Adc : public AdcStrategy {
 public:
    /**
     * @brief Constructor
     * @param peripheral handle of the ADC device in STM32.
     */
    Adc(ADC_HandleTypeDef *peripheral);
    /**
     * @brief destructor
     */
    virtual ~Adc();
    /**
     * @brief Set the sample clocks for specified  channel.
     * @param channel Specify the ADC channel.Use ADC_CHANNEL_* value.
     * @param clocks How many clocks is applied to sample. Use ADC_SAMPLETIME_*CYCLES value
     * @details
     * Set the dedicated sample clock to the specific ADC channel. The higher impedance signal source
     * may need longer clocks.
     *
     * The channel parameter is restricted as ADC_CHANNEL_* value. Otherwise, HAL may
     * fail the assertion inside Convert() member function.
     *
     * The clocks parameter is restricted as ADC_SAMPLETIME_*CYCLES. Otherwise, HAL may
     * fail the assertion inside Convert() member function.
     *
     * Up to 32 channels data can be stored.
     * If more than 32 channels are stored, assertion failed.
     */
    virtual void SetSampleClock(unsigned int channel, unsigned int clocks);
    /**
     * @brief Convert the analog input through the given channel by STM32 internal ADC
     * @param channel Specify the ADC channel.Use ADC_CHANNEL_* value.
     * @param value Pointer to the variable to receive the data. Data is normalized by range [-1,1)
     * @param size Must be 1. Otherwise, assertion failed.
     * @return Status. murasaki::kasOk is return if success.
     * @details
     * Convert the analog singal through the given channel to digital.
     *
     * The processing is synchronous and blocking. During certain channel is converted, other channels are kept waited.
     *
     */
    virtual murasaki::AdcStatus Convert(const unsigned int channel, float *value, unsigned int size = 1);

    /**
     * \brief Callback function for the interrupt handler.
     * \param ptr Pointer for generic use. Usually, points a struct of a peripheral control
     * \return true: ptr matches with peripheral and handle the call back. false : doesn't match.
     * \details
     * A call back to notify the end of conversion. The definition of calling timing is
     * depend on the implementation. This function must be called from HAL_ADC_ConvCpltCallback().
     * @code
     * void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
     * {
     *     if (murasaki::platform.adc->ConversionCompleteCallback(hadc))
     *         return;
     * }
     * @endcode
     */
    virtual bool ConversionCompleteCallback(void *ptr);
    /**
     * @brief Handling error report of device.
     * @param ptr Pointer for generic use. Usually, points a struct of a device control
     * @return true if ptr matches with device and handle the error. false if ptr doesn't match
     * A member function to detect error.
     *
     * This must be called from HAL_ADC_ErrorCallback()
     *
     * @code
     * void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
     * {
     *     if (murasaki::platform.adc->ConversionCompleteCallback(hadc))
     *         return;
     * }
     * @endcode
     *
     *
     */
    virtual bool HandleError(void *ptr);

 protected:
    /**
     * @brief pass the raw peripheral handler
     * @return pointer to the raw peripheral handler hidden in a class.
     */
    virtual void* GetPeripheralHandle();
    ADC_HandleTypeDef *const peripheral_;

 private:
    Synchronizer *const sync_;
    CriticalSection *const critical_section_;
    murasaki::AdcStatus status_;
    const unsigned int channel_array_len_;
    unsigned int *const channels_;
    unsigned int *const clocks_;
    unsigned int data_count_;

    void InstallSampleClocks(unsigned int channel, unsigned int clocks);
    unsigned int GetSampleClocks(unsigned int channel);
};
// ADCStrategy

#endif // HAL_ADC_MODULE_ENABLED

}
/* namespace murasaki */

#endif /* ADC_HPP_ */
