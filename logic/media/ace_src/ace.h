#ifndef ACE_H
#define ACE_H
/** @defgroup Codec ACE encoder and decoder
 *  This is the ACE codec itself.
 *  @{
 */

#include "ace_bits.h"
#include "ace_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Values allowed for *ctl() requests */

/** Set enhancement on/off (decoder only) */
#define ACE_SET_ENH 0
/** Get enhancement state (decoder only) */
#define ACE_GET_ENH 1

/*Would be ACE_SET_FRAME_SIZE, but it's (currently) invalid*/
/** Obtain frame size used by encoder/decoder */
#define ACE_GET_FRAME_SIZE 3

/** Set quality value */
#define ACE_SET_QUALITY 4
/** Get current quality setting */
/* #define ACE_GET_QUALITY 5 -- Doesn't make much sense, does it? */

/** Set sub-mode to use */
#define ACE_SET_MODE 6
/** Get current sub-mode in use */
#define ACE_GET_MODE 7

/** Set low-band sub-mode to use (wideband only)*/
#define ACE_SET_LOW_MODE 8
/** Get current low-band mode in use (wideband only)*/
#define ACE_GET_LOW_MODE 9

/** Set high-band sub-mode to use (wideband only)*/
#define ACE_SET_HIGH_MODE 10
/** Get current high-band mode in use (wideband only)*/
#define ACE_GET_HIGH_MODE 11

/** Set VBR on (1) or off (0) */
#define ACE_SET_VBR 12
/** Get VBR status (1 for on, 0 for off) */
#define ACE_GET_VBR 13

/** Set quality value for VBR encoding (0-10) */
#define ACE_SET_VBR_QUALITY 14
/** Get current quality value for VBR encoding (0-10) */
#define ACE_GET_VBR_QUALITY 15

/** Set complexity of the encoder (0-10) */
#define ACE_SET_COMPLEXITY 16
/** Get current complexity of the encoder (0-10) */
#define ACE_GET_COMPLEXITY 17

/** Set bit-rate used by the encoder (or lower) */
#define ACE_SET_BITRATE 18
/** Get current bit-rate used by the encoder or decoder */
#define ACE_GET_BITRATE 19

/** Define a handler function for in-band ACE request*/
#define ACE_SET_HANDLER 20

/** Define a handler function for in-band user-defined request*/
#define ACE_SET_USER_HANDLER 22

/** Set sampling rate used in bit-rate computation */
#define ACE_SET_SAMPLING_RATE 24
/** Get sampling rate used in bit-rate computation */
#define ACE_GET_SAMPLING_RATE 25

/** Reset the encoder/decoder memories to zero*/
#define ACE_RESET_STATE 26

/** Get VBR info (mostly used internally) */
#define ACE_GET_RELATIVE_QUALITY 29

/** Set VAD status (1 for on, 0 for off) */
#define ACE_SET_VAD 30

/** Get VAD status (1 for on, 0 for off) */
#define ACE_GET_VAD 31

/** Set Average Bit-Rate (ABR) to n bits per seconds */
#define ACE_SET_ABR 32
/** Get Average Bit-Rate (ABR) setting (in bps) */
#define ACE_GET_ABR 33

/** Set DTX status (1 for on, 0 for off) */
#define ACE_SET_DTX 34
/** Get DTX status (1 for on, 0 for off) */
#define ACE_GET_DTX 35

/** Set submode encoding in each frame (1 for yes, 0 for no, setting to no breaks the standard) */
#define ACE_SET_SUBMODE_ENCODING 36
/** Get submode encoding in each frame */
#define ACE_GET_SUBMODE_ENCODING 37

/*#define ACE_SET_LOOKAHEAD 38*/
/** Returns the lookahead used by ACE */
#define ACE_GET_LOOKAHEAD 39

/** Sets tuning for packet-loss concealment (expected loss rate) */
#define ACE_SET_PLC_TUNING 40
/** Gets tuning for PLC */
#define ACE_GET_PLC_TUNING 41

/** Sets the max bit-rate allowed in VBR mode */
#define ACE_SET_VBR_MAX_BITRATE 42
/** Gets the max bit-rate allowed in VBR mode */
#define ACE_GET_VBR_MAX_BITRATE 43

/** Turn on/off input/output high-pass filtering */
#define ACE_SET_HIGHPASS 44
/** Get status of input/output high-pass filtering */
#define ACE_GET_HIGHPASS 45

/** Get "activity level" of the last decoded frame, i.e.
    how much damage we cause if we remove the frame */
#define ACE_GET_ACTIVITY 47


/* Preserving compatibility:*/
/** Equivalent to ACE_SET_ENH */
#define ACE_SET_PF 0
/** Equivalent to ACE_GET_ENH */
#define ACE_GET_PF 1

/* Values allowed for mode queries */
/** Query the frame size of a mode */
#define ACE_MODE_FRAME_SIZE 0

/** Query the size of an encoded frame for a particular sub-mode */
#define ACE_SUBMODE_BITS_PER_FRAME 1

/** Get major ACE version */
#define ACE_LIB_GET_MAJOR_VERSION 1
/** Get minor ACE version */
#define ACE_LIB_GET_MINOR_VERSION 3
/** Get micro ACE version */
#define ACE_LIB_GET_MICRO_VERSION 5
/** Get extra ACE version */
#define ACE_LIB_GET_EXTRA_VERSION 7
/** Get ACE version string */
#define ACE_LIB_GET_VERSION_STRING 9

/*#define ACE_LIB_SET_ALLOC_FUNC 10
#define ACE_LIB_GET_ALLOC_FUNC 11
#define ACE_LIB_SET_FREE_FUNC 12
#define ACE_LIB_GET_FREE_FUNC 13

#define ACE_LIB_SET_WARNING_FUNC 14
#define ACE_LIB_GET_WARNING_FUNC 15
#define ACE_LIB_SET_ERROR_FUNC 16
#define ACE_LIB_GET_ERROR_FUNC 17
*/

/** Number of defined modes in ACE */
#define ACE_NB_MODES 3

/** modeID for the defined narrowband mode */
#define ACE_MODEID_NB 0

/** modeID for the defined wideband mode */
#define ACE_MODEID_WB 1

/** modeID for the defined ultra-wideband mode */
#define ACE_MODEID_UWB 2

struct ACEMode;


/* Prototypes for mode function pointers */

/** Encoder state initialization function */
typedef void *(*encoder_init_func)(const struct ACEMode *mode);

/** Encoder state destruction function */
typedef void (*encoder_destroy_func)(void *st);

/** Main encoding function */
typedef int (*encode_func)(void *state, void *in, ACEBits *bits);

/** Function for controlling the encoder options */
typedef int (*encoder_ctl_func)(void *state, int request, void *ptr);

/** Decoder state initialization function */
typedef void *(*decoder_init_func)(const struct ACEMode *mode);

/** Decoder state destruction function */
typedef void (*decoder_destroy_func)(void *st);

/** Main decoding function */
typedef int  (*decode_func)(void *state, ACEBits *bits, void *out);

/** Function for controlling the decoder options */
typedef int (*decoder_ctl_func)(void *state, int request, void *ptr);


/** Query function for a mode */
typedef int (*mode_query_func)(const void *mode, int request, void *ptr);

/** Struct defining a ACE mode */ 
typedef struct ACEMode {
   /** Pointer to the low-level mode data */
   const void *mode;

   /** Pointer to the mode query function */
   mode_query_func query;
   
   /** The name of the mode (you should not rely on this to identify the mode)*/
   const char *modeName;

   /**ID of the mode*/
   int modeID;

   /**Version number of the bitstream (incremented every time we break
    bitstream compatibility*/
   int bitstream_version;

   /** Pointer to encoder initialization function */
   encoder_init_func enc_init;

   /** Pointer to encoder destruction function */
   encoder_destroy_func enc_destroy;

   /** Pointer to frame encoding function */
   encode_func enc;

   /** Pointer to decoder initialization function */
   decoder_init_func dec_init;

   /** Pointer to decoder destruction function */
   decoder_destroy_func dec_destroy;

   /** Pointer to frame decoding function */
   decode_func dec;

   /** ioctl-like requests for encoder */
   encoder_ctl_func enc_ctl;

   /** ioctl-like requests for decoder */
   decoder_ctl_func dec_ctl;

} ACEMode;

/**
 * Returns a handle to a newly created ACE encoder state structure. For now, 
 * the "mode" argument can be &nb_mode or &wb_mode . In the future, more modes 
 * may be added. Note that for now if you have more than one channels to 
 * encode, you need one state per channel.
 *
 * @param mode The mode to use (either ace_nb_mode or ace_wb.mode) 
 * @return A newly created encoder state or NULL if state allocation fails
 */
void *ace_encoder_init(const ACEMode *mode);

/** Frees all resources associated to an existing ACE encoder state. 
 * @param state Encoder state to be destroyed */
void ace_encoder_destroy(void *state);

/** Uses an existing encoder state to encode one frame of speech pointed to by
    "in". The encoded bit-stream is saved in "bits".
 @param state Encoder state
 @param in Frame that will be encoded with a +-2^15 range. This data MAY be 
        overwritten by the encoder and should be considered uninitialised 
        after the call.
 @param bits Bit-stream where the data will be written
 @return 0 if frame needs not be transmitted (DTX only), 1 otherwise
 */
int ace_encode(void *state, float *in, ACEBits *bits);

/** Uses an existing encoder state to encode one frame of speech pointed to by
    "in". The encoded bit-stream is saved in "bits".
 @param state Encoder state
 @param in Frame that will be encoded with a +-2^15 range
 @param bits Bit-stream where the data will be written
 @return 0 if frame needs not be transmitted (DTX only), 1 otherwise
 */
int ace_encode_int(void *state, ace_int16_t *in, ACEBits *bits);

/** Used like the ioctl function to control the encoder parameters
 *
 * @param state Encoder state
 * @param request ioctl-type request (one of the ACE_* macros)
 * @param ptr Data exchanged to-from function
 * @return 0 if no error, -1 if request in unknown, -2 for invalid parameter
 */
int ace_encoder_ctl(void *state, int request, void *ptr);


/** Returns a handle to a newly created decoder state structure. For now, 
 * the mode argument can be &nb_mode or &wb_mode . In the future, more modes
 * may be added.  Note that for now if you have more than one channels to
 * decode, you need one state per channel.
 *
 * @param mode ACE mode (one of ace_nb_mode or ace_wb_mode)
 * @return A newly created decoder state or NULL if state allocation fails
 */ 
void *ace_decoder_init(const ACEMode *mode);

/** Frees all resources associated to an existing decoder state.
 *
 * @param state State to be destroyed
 */
void ace_decoder_destroy(void *state);

/** Uses an existing decoder state to decode one frame of speech from
 * bit-stream bits. The output speech is saved written to out.
 *
 * @param state Decoder state
 * @param bits Bit-stream from which to decode the frame (NULL if the packet was lost)
 * @param out Where to write the decoded frame
 * @return return status (0 for no error, -1 for end of stream, -2 corrupt stream)
 */
int ace_decode(void *state, ACEBits *bits, float *out);

/** Uses an existing decoder state to decode one frame of speech from
 * bit-stream bits. The output speech is saved written to out.
 *
 * @param state Decoder state
 * @param bits Bit-stream from which to decode the frame (NULL if the packet was lost)
 * @param out Where to write the decoded frame
 * @return return status (0 for no error, -1 for end of stream, -2 corrupt stream)
 */
int ace_decode_int(void *state, ACEBits *bits, ace_int16_t *out);

/** Used like the ioctl function to control the encoder parameters
 *
 * @param state Decoder state
 * @param request ioctl-type request (one of the ACE_* macros)
 * @param ptr Data exchanged to-from function
 * @return 0 if no error, -1 if request in unknown, -2 for invalid parameter
 */
int ace_decoder_ctl(void *state, int request, void *ptr);


/** Query function for mode information
 *
 * @param mode ACE mode
 * @param request ioctl-type request (one of the ACE_* macros)
 * @param ptr Data exchanged to-from function
 * @return 0 if no error, -1 if request in unknown, -2 for invalid parameter
 */
int ace_mode_query(const ACEMode *mode, int request, void *ptr);

/** Functions for controlling the behavior of libace
 * @param request ioctl-type request (one of the ACE_LIB_* macros)
 * @param ptr Data exchanged to-from function
 * @return 0 if no error, -1 if request in unknown, -2 for invalid parameter
 */
int ace_lib_ctl(int request, void *ptr);

/** Default narrowband mode */
extern const ACEMode ace_nb_mode;

/** Default wideband mode */
extern const ACEMode ace_wb_mode;

/** Default "ultra-wideband" mode */
extern const ACEMode ace_uwb_mode;

/** List of all modes available */
extern const ACEMode * const ace_mode_list[ACE_NB_MODES];

/** Obtain one of the modes available */
const ACEMode * ace_lib_get_mode (int mode);

#ifndef WIN32
/* We actually override the function in the narrowband case so that we can avoid linking in the wideband stuff */
#define ace_lib_get_mode(mode) ((mode)==ACE_MODEID_NB ? &ace_nb_mode : ace_lib_get_mode (mode))
#endif

#ifdef __cplusplus
}
#endif

/** @}*/
#endif
