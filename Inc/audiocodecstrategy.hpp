/**
 * @file audiocodecstrategy.hpp
 *
 * @date 2018/05/11
 * @author: Seiichi "Suikan" Horie
 */

#ifndef THIRD_PARTY_ABSTRACTAUDIOCODEC_HPP_
#define THIRD_PARTY_ABSTRACTAUDIOCODEC_HPP_

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
	AudioCodecStrategy( unsigned int fs ):fs_(fs){};
	virtual ~AudioCodecStrategy(){};

		/**
		* \brief Actual initializer.
		* \details
		*   Initialize the codec itself and start the conversion process.
		*   and configure for given parameter.
		*
		*   Finally, set the input gain to 0dB.
		*/
	virtual void start(void)=0;

		/**
		* \brief Set the line input gain and enable the relevant mixer.
		* \param left_gain Gain by dB. The gain value outside of the acceptable range will be saturated.
		* \param right_gain Gain by dB. The gain value outside of the acceptable range will be saturated.
		* \param mute set true to mute
		*/
	virtual void set_line_input_gain(float left_gain, float right_gain, bool mute=false){};
		/**
		* \brief Set the aux input gain and enable the relevant mixer.
		* \param left_gain Gain by dB. The gain value outside of the acceptable range will be saturated.
		* \param right_gain Gain by dB. The gain value outside of the acceptable range will be saturated.
		* \param mute set true to mute
		*/
	virtual void set_aux_input_gain(float left_gain, float right_gain, bool mute=false){};
		/**
		* \brief Set the mic input gain and enable the relevant mixer.
		* \param left_gain Gain by dB. The gain value outside of the acceptable range will be saturated.
		* \param right_gain Gain by dB. The gain value outside of the acceptable range will be saturated.
		* \param mute set true to mute
		*/
	virtual void set_mic_input_gain(float left_gain, float right_gain, bool mute=false){};
		/**
		* \brief Set the line output gain and enable the relevant mixer.
		* \param left_gain Gain by dB. The gain value outside of the acceptable range will be saturated.
		* \param right_gain Gain by dB. The gain value outside of the acceptable range will be saturated.
		* \param mute set true to mute
		*/
	virtual void set_line_output_gain(float left_gain, float right_gain, bool mute=false){};
		/**
		* \brief Set the headphone output gain and enable the relevant mixer.
		* \param left_gain Gain by dB. The gain value outside of the acceptable range will be saturated.
		* \param right_gain Gain by dB. The gain value outside of the acceptable range will be saturated.
		* \param mute set true to mute
		*/
	virtual void set_hp_output_gain(float left_gain, float right_gain, bool mute=false){};
protected:
	unsigned int fs_;
};


} /* namespace murasaki */

#endif /* THIRD_PARTY_ABSTRACTAUDIOCODEC_HPP_ */
