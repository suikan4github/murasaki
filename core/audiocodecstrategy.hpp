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
 * \brief abstract audio CODEC controller.
 * \details
 *   This class is template for all CODEC classes
 */
class AudioCodecStrategy {
 public:
  /**
   * \brief constructor.
   * \param fs Sampling frequency[Hz].
   * \details
   *   initialize the internal variables.
   */
  AudioCodecStrategy(unsigned int fs) : fs_(fs){};
  virtual ~AudioCodecStrategy(){};

  /**
   * \brief Actual initializer.
   * \details
   *   Initialize the CODEC and start the conversion process.
   */
  virtual void Start(void) = 0;

  /**
   * @brief Set channel gain
   * @param channel
   * @param left_gain [dB]
   * @param right_gain [dB]
   */
  virtual void SetGain(murasaki::CodecChannel channel, float left_gain,
                       float right_gain) = 0;

  /**
   * @brief Mute the specific channel.
   * @param channel Channel to mute on / off
   * @param mute On if true, off if false.
   */
  virtual void Mute(murasaki::CodecChannel channel, bool mute = true) = 0;

 protected:
  /**
   * @brief Sampling Frequency [Hz]
   */
  unsigned int fs_;
};

} /* namespace murasaki */

#endif /* THIRD_PARTY_ABSTRACTAUDIOCODEC_HPP_ */
