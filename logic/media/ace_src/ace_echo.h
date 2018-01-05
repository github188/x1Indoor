#ifndef ACE_ECHO_H
#define ACE_ECHO_H
/** @defgroup ACEEchoState ACEEchoState: Acoustic echo canceller
 *  This is the acoustic echo canceller module.
 *  @{
 */
#include "ace_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Obtain frame size used by the AEC */
#define ACE_ECHO_GET_FRAME_SIZE 3

/** Set sampling rate */
#define ACE_ECHO_SET_SAMPLING_RATE 24
/** Get sampling rate */
#define ACE_ECHO_GET_SAMPLING_RATE 25

/** Internal echo canceller state. Should never be accessed directly. */
struct ACEEchoState_;

/** @class ACEEchoState
 * This holds the state of the echo canceller. You need one per channel. 
*/

/** Internal echo canceller state. Should never be accessed directly. */
typedef struct ACEEchoState_ ACEEchoState;

/** Creates a new echo canceller state
 * @param frame_size Number of samples to process at one time (should correspond to 10-20 ms)
 * @param filter_length Number of samples of echo to cancel (should generally correspond to 100-500 ms)
 * @return Newly-created echo canceller state
 */
ACEEchoState *ace_echo_state_init(int frame_size, int filter_length);

/** Destroys an echo canceller state 
 * @param st Echo canceller state
*/
void ace_echo_state_destroy(ACEEchoState *st);

/** Performs echo cancellation a frame, based on the audio sent to the speaker (no delay is added
 * to playback in this form)
 *
 * @param st Echo canceller state
 * @param rec Signal from the microphone (near end + far end echo)
 * @param play Signal played to the speaker (received from far end)
 * @param out Returns near-end signal with echo removed
 */
void ace_echo_cancellation(ACEEchoState *st, const ace_int16_t *rec, const ace_int16_t *play, ace_int16_t *out);

/** Performs echo cancellation a frame (deprecated) */
void ace_echo_cancel(ACEEchoState *st, const ace_int16_t *rec, const ace_int16_t *play, ace_int16_t *out, ace_int32_t *Yout);

/** Perform echo cancellation using internal playback buffer, which is delayed by two frames
 * to account for the delay introduced by most soundcards (but it could be off!)
 * @param st Echo canceller state
 * @param rec Signal from the microphone (near end + far end echo)
 * @param out Returns near-end signal with echo removed
*/
void ace_echo_capture(ACEEchoState *st, const ace_int16_t *rec, ace_int16_t *out);

/** Let the echo canceller know that a frame was just queued to the soundcard
 * @param st Echo canceller state
 * @param play Signal played to the speaker (received from far end)
*/
void ace_echo_playback(ACEEchoState *st, const ace_int16_t *play);

/** Reset the echo canceller to its original state 
 * @param st Echo canceller state
 */
void ace_echo_state_reset(ACEEchoState *st);

/** Used like the ioctl function to control the echo canceller parameters
 *
 * @param st Echo canceller state
 * @param request ioctl-type request (one of the ACE_ECHO_* macros)
 * @param ptr Data exchanged to-from function
 * @return 0 if no error, -1 if request in unknown
 */
int ace_echo_ctl(ACEEchoState *st, int request, void *ptr);


void ace_echo_cancellation1(ACEEchoState *st, const ace_int16_t *in, const ace_int16_t *far_end, ace_int16_t *out);

void ace_echo_cancellation2(ACEEchoState *st, const ace_int16_t *in, const ace_int16_t *far_end, ace_int16_t *out);

#ifdef __cplusplus
}
#endif


/** @}*/
#endif
