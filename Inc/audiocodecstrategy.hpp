/**
 * @file audiocodecstrategy.hpp
 *
 * @date 2018/05/11
 * @author: Seiichi "Suikan" Horie
 */

#ifndef AUDIO_CODEC_STRATEGY_HPP_
#define AUDIO_CODEC_STRATEGY_HPP_

#include <murasaki_defs.hpp>

namespace murasaki {

/**
 * \ingroup MURASAKI_ABSTRACT_GROUP
 * \brief abstract audio codec controller.
 * \details
 *   This class is template for all codec classes
 */
class AudioCodecStrategy
{
 public:
    /**
     * \brief constructor.
     * \param fs Sampling frequency.
     * \details
     *   initialize the internal variables.
     */
    AudioCodecStrategy(unsigned int fs)
            : fs_(fs) {
    }
    ;
    virtual ~AudioCodecStrategy() {
    }
    ;

    /**
     * \brief Actual initializer.
     * \details
     *   Initialize the codec itself and start the conversion process.
     *   and configure for given parameter.
     *
     *   Finally, set the input gain to 0dB.
     */
    virtual void Start(void)=0;

    /**
     * @brief Set channel gain
     * @param channel
     * @param left_gain
     * @param right_gain
     */
    virtual void SetGain(murasaki::CodecChannel channel, float left_gain, float right_gain) = 0;

    /**
     * @brief Mute the specific channel.
     * @param channel Channel to mute on / off
     * @param mute On if true, off if false.
     */
    virtual void Mute(murasaki::CodecChannel channel, bool mute = true) = 0;
    /**
     *
     * \brief send one command to CODEC
     * \param command command data array.
     * \details
     */
    virtual void SendCommand(const uint8_t command[], int size) = 0;

 protected:
    /**
     * @brief Sampling Frequency [Hz]
     */
    unsigned int fs_;
};

} /* namespace murasaki */

#endif /* THIRD_PARTY_ABSTRACTAUDIOCODEC_HPP_ */
