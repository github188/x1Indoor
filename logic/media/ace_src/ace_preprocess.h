#ifndef ACE_PREPROCESS_H
#define ACE_PREPROCESS_H
/** @defgroup ACEPreprocessState ACEPreprocessState: The ACE preprocessor
 *  This is the ACE preprocessor. The preprocess can do noise suppression, 
 * residual echo suppression (after using the echo canceller), automatic
 * gain control (AGC) and voice activity detection (VAD).
 *  @{
 */

#include "ace_types.h"

#ifdef __cplusplus
extern "C" {
#endif
   
/** State of the preprocessor (one per channel). Should never be accessed directly. */
struct ACEPreprocessState_;

/** State of the preprocessor (one per channel). Should never be accessed directly. */
typedef struct ACEPreprocessState_ ACEPreprocessState;


/** Creates a new preprocessing state. You MUST create one state per channel processed.
 * @param frame_size Number of samples to process at one time (should correspond to 10-20 ms). Must be
 * the same value as that used for the echo canceller for residual echo cancellation to work.
 * @param sampling_rate Sampling rate used for the input.
 * @return Newly created preprocessor state
*/
ACEPreprocessState *ace_preprocess_state_init(int frame_size, int noiselevel, int echolevel);

/** Destroys a preprocessor state 
 * @param st Preprocessor state to destroy
*/
void ace_preprocess_state_destroy(ACEPreprocessState *st);

/** Preprocess a frame 
 * @param st Preprocessor state
 * @param x Audio sample vector (in and out). Must be same size as specified in ace_preprocess_state_init().
 * @return Bool value for voice activity (1 for speech, 0 for noise/silence), ONLY if VAD turned on.
*/
int ace_preprocess_run(ACEPreprocessState *st, ace_int16_t *x);

/** Preprocess a frame (deprecated, use ace_preprocess_run() instead)*/
int ace_preprocess(ACEPreprocessState *st, ace_int16_t *x, ace_int32_t *echo);

/** Update preprocessor state, but do not compute the output
 * @param st Preprocessor state
 * @param x Audio sample vector (in only). Must be same size as specified in ace_preprocess_state_init().
*/
void ace_preprocess_estimate_update(ACEPreprocessState *st, ace_int16_t *x);

/** Used like the ioctl function to control the preprocessor parameters 
 * @param st Preprocessor state
 * @param request ioctl-type request (one of the ACE_PREPROCESS_* macros)
 * @param ptr Data exchanged to-from function
 * @return 0 if no error, -1 if request in unknown
*/
int ace_preprocess_ctl(ACEPreprocessState *st, int request, void *ptr);



/** Set preprocessor denoiser state */
#define ACE_PREPROCESS_SET_DENOISE 0
/** Get preprocessor denoiser state */
#define ACE_PREPROCESS_GET_DENOISE 1

/** Set preprocessor Automatic Gain Control state */
#define ACE_PREPROCESS_SET_AGC 2
/** Get preprocessor Automatic Gain Control state */
#define ACE_PREPROCESS_GET_AGC 3

/** Set preprocessor Voice Activity Detection state */
#define ACE_PREPROCESS_SET_VAD 4
/** Get preprocessor Voice Activity Detection state */
#define ACE_PREPROCESS_GET_VAD 5

/** Set preprocessor Automatic Gain Control level */
#define ACE_PREPROCESS_SET_AGC_LEVEL 6
/** Get preprocessor Automatic Gain Control level */
#define ACE_PREPROCESS_GET_AGC_LEVEL 7

/** Set preprocessor dereverb state */
#define ACE_PREPROCESS_SET_DEREVERB 8
/** Get preprocessor dereverb state */
#define ACE_PREPROCESS_GET_DEREVERB 9

/** Set preprocessor dereverb level */
#define ACE_PREPROCESS_SET_DEREVERB_LEVEL 10
/** Get preprocessor dereverb level */
#define ACE_PREPROCESS_GET_DEREVERB_LEVEL 11

/** Set preprocessor dereverb decay */
#define ACE_PREPROCESS_SET_DEREVERB_DECAY 12
/** Get preprocessor dereverb decay */
#define ACE_PREPROCESS_GET_DEREVERB_DECAY 13

/** Set probability required for the VAD to go from silence to voice */
#define ACE_PREPROCESS_SET_PROB_START 14
/** Get probability required for the VAD to go from silence to voice */
#define ACE_PREPROCESS_GET_PROB_START 15

/** Set probability required for the VAD to stay in the voice state (integer percent) */
#define ACE_PREPROCESS_SET_PROB_CONTINUE 16
/** Get probability required for the VAD to stay in the voice state (integer percent) */
#define ACE_PREPROCESS_GET_PROB_CONTINUE 17

/** Set maximum attenuation of the noise in dB (negative number) */
#define ACE_PREPROCESS_SET_NOISE_SUPPRESS 18
/** Get maximum attenuation of the noise in dB (negative number) */
#define ACE_PREPROCESS_GET_NOISE_SUPPRESS 19

/** Set maximum attenuation of the residual echo in dB (negative number) */
#define ACE_PREPROCESS_SET_ECHO_SUPPRESS 20
/** Get maximum attenuation of the residual echo in dB (negative number) */
#define ACE_PREPROCESS_GET_ECHO_SUPPRESS 21

/** Set maximum attenuation of the residual echo in dB when near end is active (negative number) */
#define ACE_PREPROCESS_SET_ECHO_SUPPRESS_ACTIVE 22
/** Get maximum attenuation of the residual echo in dB when near end is active (negative number) */
#define ACE_PREPROCESS_GET_ECHO_SUPPRESS_ACTIVE 23

/** Set the corresponding echo canceller state so that residual echo suppression can be performed (NULL for no residual echo suppression) */
#define ACE_PREPROCESS_SET_ECHO_STATE 24
/** Get the corresponding echo canceller state */
#define ACE_PREPROCESS_GET_ECHO_STATE 25

/** Set maximal gain increase in dB/second (int32) */
#define ACE_PREPROCESS_SET_AGC_INCREMENT 26

/** Get maximal gain increase in dB/second (int32) */
#define ACE_PREPROCESS_GET_AGC_INCREMENT 27

/** Set maximal gain decrease in dB/second (int32) */
#define ACE_PREPROCESS_SET_AGC_DECREMENT 28

/** Get maximal gain decrease in dB/second (int32) */
#define ACE_PREPROCESS_GET_AGC_DECREMENT 29

/** Set maximal gain in dB (int32) */
#define ACE_PREPROCESS_SET_AGC_MAX_GAIN 30

/** Get maximal gain in dB (int32) */
#define ACE_PREPROCESS_GET_AGC_MAX_GAIN 31

/*  Can't set loudness */
/** Get loudness */
#define ACE_PREPROCESS_GET_AGC_LOUDNESS 33

#ifdef __cplusplus
}
#endif

/** @}*/
#endif
