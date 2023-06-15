/*
 * SDL_codec:  An audio codec library for use with SDL
 * Copyright (C) 2022 Walker M. White, Sam Lantinga
 *
 * This is a simple library to lad different types of audio files as PCM data.
 * We choose this over SDL_mixer because it gives us finer grain control over 
 * our audio layer (which we need for effects).
 * 
 * SDL License:
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/* 
 * This is a WAV file loading framework. 
 * 
 * It is heavily based on an old version of SDL_wave.c by Sam Lantinga. It
 * has been refactored to support WAV streaming.
 */
// TODO: Update with a more recent SDL_wave.c
#include <SDL_codec.h>
#include "CODEC_internals.h"
#include <assert.h>
#include <string.h>

/** Macro for error checking */
#define CHECK_SOURCE(source, retval) \
    if (source == NULL) { \
        CODEC_SetError("Attempt to access a NULL codec source"); \
        return retval; \
    } else if (source->decoder == NULL) { \
        CODEC_SetError("Codec source has invalid state"); \
        return retval; \
    }

#pragma mark CONSTANTS

/*******************************************/
/* Define values for Microsoft WAVE format */
/*******************************************/
#define RIFF            0x46464952      /* "RIFF" */
#define WAVE            0x45564157      /* "WAVE" */
#define FACT            0x74636166      /* "fact" */
#define LIST            0x5453494c      /* "LIST" */
#define BEXT            0x74786562      /* "bext" */
#define JUNK            0x4B4E554A      /* "JUNK" */
#define FMT             0x20746D66      /* "fmt " */
#define DATA            0x61746164      /* "data" */
#define PCM_CODE        0x0001
#define MS_ADPCM_CODE   0x0002
#define IEEE_FLOAT_CODE 0x0003
#define IMA_ADPCM_CODE  0x0011
#define MP3_CODE        0x0055
#define WAVE_MONO       1
#define WAVE_STEREO     2
#define PAGE_SIZE       4096

#pragma mark ADCPM Proxy Decoder
/**
 * This struct contains the RIFF information at the start of a WAVE file
 *
 * Normally, this is stored in the first three chunks at the start of a
 * WAVE file.
 */
typedef struct {
    /** The encoding type (PCM, IEEE, MS ADPCM, IMA ADPCM) */
    Uint16 encoding;
    /** The number of channels  (1 = mono, 2 = stereo) */
    Uint16 channels;
    /** The sample rate (11025, 22050, or 44100 Hz) */
    Uint32 frequency;
    /** The average bytes per second */
    Uint32 byterate;
    /** The number of bytes per sample block */
    Uint16 blockalign;
    /** One of 8, 12, 16, or 4 for ADPCM */
    Uint16 bitspersample;
} WaveFMT;


/**
 * This type represents an internal decoder for ADPCM encoded WAV files.
 *
 * This decoder is a proxy decoder for ADPCM files, which are compressed.
 * This is the abstract base class for either MS or IMA decoding.  This 
 * class is for internal use and should never be instantiated by the user.
 */
typedef struct {
    /** The RIFF header */
    WaveFMT wavefmt;
    /** The buffer size for AD PCM decoding */
    Uint8*  blkbuffer;
    /** The internal buffer for AD PCM decoding */
    Uint16  blocksize;
    /** The internal decoder state */
    void* internal;
} CODEC_ADPCM;

/**
 * This struct represents the decoding state for MS ADPCM decoding
 *
 * This data is stored as a struct to simplify stereo decoding.
 */
typedef struct {
    Uint8 hPredictor;
    Uint16 iDelta;
    Sint16 iSamp1;
    Sint16 iSamp2;
} MS_state;

/**
 * This type represents an internal decoder for MS ADPCM decoding.
 */
typedef struct {
    /** The number of decoding coefficients */
    Uint16   numCoef;
    /** The decoding coefficients */
    Sint16   coeff[7][2];
    /** The decoding state (for mono or stereo decoding) */
    MS_state state[2];
} MS_decoder;

/**
 * This struct represents the decoding state for IMA ADPCM decoding
 *
 * This data is stored as a struct to simplify stereo decoding.
 */
typedef struct {
    Sint32 sample;
    Sint8 index;
} IMA_state;

/**
 * This type represents an internal decoder for IMA ADPCM decoding.
 */
typedef struct {
    /** The decoding state (for mono or stereo decoding) */
    IMA_state state[2];
} IMA_decoder;

CODEC_ADPCM* CODEC_Alloc_MS_ADPCM(WaveFMT * format) {
	CODEC_ADPCM* result = (CODEC_ADPCM*)SDL_malloc(sizeof(CODEC_ADPCM));
	result->wavefmt.encoding = SDL_SwapLE16(format->encoding);
    result->wavefmt.channels = SDL_SwapLE16(format->channels);
    result->wavefmt.frequency = SDL_SwapLE32(format->frequency);
    result->wavefmt.byterate = SDL_SwapLE32(format->byterate);
    result->wavefmt.blockalign = SDL_SwapLE16(format->blockalign);
    result->wavefmt.bitspersample = SDL_SwapLE16(format->bitspersample);
    result->blkbuffer = (Uint8*)SDL_malloc(result->wavefmt.blockalign);
    result->blocksize = 0;
    result->internal = (MS_decoder*)SDL_malloc(sizeof(MS_decoder));
    
    MS_decoder* dec = result->internal;
    Uint8 *rogue_feel = (Uint8 *) format + sizeof(*format);
    if (sizeof(*format) == 16) {
        rogue_feel += sizeof(Uint16);
    }
    result->blocksize = ((rogue_feel[1] << 8) | rogue_feel[0]);
    rogue_feel += sizeof(Uint16);
    
    dec->numCoef = ((rogue_feel[1] << 8) | rogue_feel[0]);
    rogue_feel += sizeof(Uint16);
    if (dec->numCoef != 7) {
        SDL_SetError("Unknown set of MS_ADPCM coefficients");
        return NULL;
    }
    
    for (int i = 0; i < dec->numCoef; ++i) {
        dec->coeff[i][0] = ((rogue_feel[1] << 8) | rogue_feel[0]);
        rogue_feel += sizeof(Uint16);
        dec->coeff[i][1] = ((rogue_feel[1] << 8) | rogue_feel[0]);
        rogue_feel += sizeof(Uint16);
    }
    
    return result;
}

CODEC_ADPCM* CODEC_Alloc_IMA_ADPCM(WaveFMT * format) {
	CODEC_ADPCM* result = (CODEC_ADPCM*)SDL_malloc(sizeof(CODEC_ADPCM));
	result->wavefmt.encoding = SDL_SwapLE16(format->encoding);
    result->wavefmt.channels = SDL_SwapLE16(format->channels);
    result->wavefmt.frequency = SDL_SwapLE32(format->frequency);
    result->wavefmt.byterate = SDL_SwapLE32(format->byterate);
    result->wavefmt.blockalign = SDL_SwapLE16(format->blockalign);
    result->wavefmt.bitspersample = SDL_SwapLE16(format->bitspersample);
    result->blkbuffer = (Uint8*)SDL_malloc(result->wavefmt.blockalign);
    result->blocksize = 0;
    result->internal = (IMA_decoder*)SDL_malloc(sizeof(IMA_decoder));
    return result;
}

void CODEC_Free_ADPCM(CODEC_ADPCM* decoder) {
	SDL_free(decoder->internal);
	decoder->internal = NULL;
	SDL_free(decoder);
	decoder = NULL;
}

/**
 * Returns the number of frames to decompress from the given number of bytes
 *
 * ADPCM decoding expands the number of frames that a group of bytes can
 * produce.  This method allows the main decoder to align with the proxy.
 *
 * @param bytes The number of bytes to convert
 *
 * @return the number of frames to decompress from the given number of bytes
 */
Uint64 CODEC_ADPCM_GetFrames(CODEC_ADPCM* decoder,Uint64 bytes) {
	return (decoder->blocksize*bytes)/decoder->wavefmt.blockalign;
}

/**
 * Returns a single sample extracted from the encoded data
 *
 * @param state     The decoder state
 * @param nybble    The byte to convert
 * @param coeff     The decoder coefficients
 *
 * @return a single sample extracted from the encoded data
 */
Sint32 MS_State_Nibble(MS_state *state, Uint8 nybble, Sint16 * coeff) {
    const Sint32 max_audioval = ((1 << (16 - 1)) - 1);
    const Sint32 min_audioval = -(1 << (16 - 1));
    const Sint32 adaptive[] = {
        230, 230, 230, 230, 307, 409, 512, 614,
        768, 614, 512, 409, 307, 230, 230, 230
    };
    Sint32 new_sample, delta;
    
    new_sample = ((state->iSamp1 * coeff[0]) +
                  (state->iSamp2 * coeff[1])) / 256;
    if (nybble & 0x08) {
        new_sample += state->iDelta * (nybble - 0x10);
    } else {
        new_sample += state->iDelta * nybble;
    }
    if (new_sample < min_audioval) {
        new_sample = min_audioval;
    } else if (new_sample > max_audioval) {
        new_sample = max_audioval;
    }
    delta = ((Sint32) state->iDelta * adaptive[nybble]) / 256;
    if (delta < 16) {
        delta = 16;
    }
    state->iDelta = (Uint16) delta;
    state->iSamp2 = state->iSamp1;
    state->iSamp1 = (Sint16) new_sample;
    return (new_sample);
}

/**
 * Reads a single page from the given file.
 *
 * The buffer should be able to store block size * channels * 2 bytes of
 * data (the latter 2 representing sizeof(Sint16) ).  If the read fails,
 * this method returns -1.
 *
 * @param source    The source file
 * @param buffer    The buffer to store the decoded data
 *
 * @return the number of bytes read (or -1 on error)
 */
Sint32 MS_Decoder_Read(CODEC_ADPCM* decoder, SDL_RWops* source, Uint8* buffer) {
    // Read in a single block align
	if (decoder == NULL) {
        return -1;
	} else if (!SDL_RWread(source, decoder->blkbuffer, decoder->wavefmt.blockalign, 1)) {
        return -1;
    }
    
    MS_decoder* internal = (MS_decoder*)(decoder->internal);
    MS_state *state[2];
    Uint8 *encoded, *decoded;
    Sint32 samplesleft;
    Sint32 output_len;
    Sint32 new_sample;
    Sint16 *coeff[2];
    Sint8 nybble;
    Uint8 stereo;
    
    // Adjustable pointers to input and output
    output_len = decoder->blocksize * decoder->wavefmt.channels * sizeof(Sint16);
    encoded = decoder->blkbuffer;
    decoded = buffer;
    
    // Handle mono or stereo
    stereo = (decoder->wavefmt.channels == 2);
    state[0] = &(internal->state[0]);
    state[1] = &(internal->state[stereo]);
    
    // Grab the initial information for this block
    state[0]->hPredictor = *encoded++;
    if (stereo) {
        state[1]->hPredictor = *encoded++;
    }
    state[0]->iDelta = ((encoded[1] << 8) | encoded[0]);
    encoded += sizeof(Sint16);
    if (stereo) {
        state[1]->iDelta = ((encoded[1] << 8) | encoded[0]);
        encoded += sizeof(Sint16);
    }
    state[0]->iSamp1 = ((encoded[1] << 8) | encoded[0]);
    encoded += sizeof(Sint16);
    if (stereo) {
        state[1]->iSamp1 = ((encoded[1] << 8) | encoded[0]);
        encoded += sizeof(Sint16);
    }
    state[0]->iSamp2 = ((encoded[1] << 8) | encoded[0]);
    encoded += sizeof(Sint16);
    if (stereo) {
        state[1]->iSamp2 = ((encoded[1] << 8) | encoded[0]);
        encoded += sizeof(Sint16);
    }
    coeff[0] = internal->coeff[state[0]->hPredictor];
    coeff[1] = internal->coeff[state[1]->hPredictor];
        
    // Store the two initial samples we start with
    decoded[0] = state[0]->iSamp2 & 0xFF;
    decoded[1] = state[0]->iSamp2 >> 8;
    decoded += 2;
    if (stereo) {
        decoded[0] = state[1]->iSamp2 & 0xFF;
        decoded[1] = state[1]->iSamp2 >> 8;
        decoded += 2;
    }
    decoded[0] = state[0]->iSamp1 & 0xFF;
    decoded[1] = state[0]->iSamp1 >> 8;
    decoded += 2;
    if (stereo) {
        decoded[0] = state[1]->iSamp1 & 0xFF;
        decoded[1] = state[1]->iSamp1 >> 8;
        decoded += 2;
    }
        
    // Decode and store the other samples in this block
    samplesleft = (decoder->blocksize - 2) * decoder->wavefmt.channels;
    while (samplesleft > 0) {
        nybble = (*encoded) >> 4;
        new_sample = MS_State_Nibble(state[0], nybble, coeff[0]);
        decoded[0] = new_sample & 0xFF;
        new_sample >>= 8;
        decoded[1] = new_sample & 0xFF;
        decoded += 2;
            
        nybble = (*encoded) & 0x0F;
        new_sample = MS_State_Nibble(state[1], nybble, coeff[1]);
        decoded[0] = new_sample & 0xFF;
        new_sample >>= 8;
        decoded[1] = new_sample & 0xFF;
        decoded += 2;
            
        ++encoded;
        samplesleft -= 2;
    }
    
    return output_len;
}


/**
 * Returns a single sample extracted from the encoded data
 *
 * @param state     The decoder state
 * @param nybble    The byte to convert
 *
 * @return a single sample extracted from the encoded data
 */
Sint32 IMA_State_Nibble(IMA_state* state, Uint8 nybble) {
    const Sint32 max_audioval = ((1 << (16 - 1)) - 1);
    const Sint32 min_audioval = -(1 << (16 - 1));
    const int index_table[16] = {
        -1, -1, -1, -1,
        2, 4, 6, 8,
        -1, -1, -1, -1,
        2, 4, 6, 8
    };
    const Sint32 step_table[89] = {
        7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 19, 21, 23, 25, 28, 31,
        34, 37, 41, 45, 50, 55, 60, 66, 73, 80, 88, 97, 107, 118, 130,
        143, 157, 173, 190, 209, 230, 253, 279, 307, 337, 371, 408,
        449, 494, 544, 598, 658, 724, 796, 876, 963, 1060, 1166, 1282,
        1411, 1552, 1707, 1878, 2066, 2272, 2499, 2749, 3024, 3327,
        3660, 4026, 4428, 4871, 5358, 5894, 6484, 7132, 7845, 8630,
        9493, 10442, 11487, 12635, 13899, 15289, 16818, 18500, 20350,
        22385, 24623, 27086, 29794, 32767
    };
    Sint32 delta, step;
    
    // Compute difference and new sample value
    if (state->index > 88) {
        state->index = 88;
    } else if (state->index < 0) {
        state->index = 0;
    }
    
    // Explicit cast to avoid gcc warning about using 'char' as array index
    step = step_table[(int)state->index];
    delta = step >> 3;
    if (nybble & 0x04)
        delta += step;
    if (nybble & 0x02)
        delta += (step >> 1);
    if (nybble & 0x01)
        delta += (step >> 2);
    if (nybble & 0x08)
        delta = -delta;
    state->sample += delta;
    
    // Update index value
    state->index += index_table[nybble];
    
    // Clamp output sample
    if (state->sample > max_audioval) {
        state->sample = max_audioval;
    } else if (state->sample < min_audioval) {
        state->sample = min_audioval;
    }
    return (state->sample);
}

/**
 * Fills the array with a single data block for the given channel
 *
 * This method reads 8 samples at a time into the decoded array.
 *
 * @param decoded       The place to store the decoded data
 * @param encoded       The encoded data to read
 * @param channel       The channel to decode
 * @param numchannels   The total number of channels
 * @param state         The decoder state
 */
void IMA_Decoder_Fill(Uint8 * decoded, Uint8 * encoded, int channel, 
					  int numchannels, IMA_state *state) {
    Sint8 nybble;
    Sint32 new_sample;
    
    decoded += (channel * 2);
    for (int i = 0; i < 4; ++i) {
        nybble = (*encoded) & 0x0F;
        new_sample = IMA_State_Nibble(state, nybble);
        decoded[0] = new_sample & 0xFF;
        new_sample >>= 8;
        decoded[1] = new_sample & 0xFF;
        decoded += 2 * numchannels;
        
        nybble = (*encoded) >> 4;
        new_sample = IMA_State_Nibble(state, nybble);
        decoded[0] = new_sample & 0xFF;
        new_sample >>= 8;
        decoded[1] = new_sample & 0xFF;
        decoded += 2 * numchannels;
        
        ++encoded;
    }
}

/**
 * Reads a single page from the given file.
 *
 * The buffer should be able to store block size * channels * 2 bytes of
 * data (the latter 2 representing sizeof(Sint16) ). If the read fails,
 * this method returns -1.
 *
 * @param source    The source file
 * @param buffer    The buffer to store the decoded data
 *
 * @return the number of bytes read (or -1 on error)
 */
Sint32 IMA_Decoder_Read(CODEC_ADPCM* decoder, SDL_RWops* source, Uint8* buffer) {
    IMA_state *state;
    Uint8 *encoded, *decoded;
    Sint32 output_len, samplesleft;
    
    // Check to make sure we have enough variables in the state array
    // Then read in a single block align
	if (decoder == NULL) {
        return -1;
    }
    
    IMA_decoder* internal = (IMA_decoder*)(decoder->internal);
    state = internal->state;

    if (decoder->wavefmt.channels > SDL_arraysize(internal->state)) {
        SDL_SetError("IMA ADPCM decoder can only handle %u channels",
                     (unsigned int)SDL_arraysize(internal->state));
        return -1;
    } else if (!SDL_RWread(source, decoder->blkbuffer, decoder->wavefmt.blockalign, 1)) {
        return -1;
    }
    
    
    // Allocate the proper sized output buffer
    output_len = decoder->blocksize * decoder->wavefmt.channels * sizeof(Sint16);
    encoded = decoder->blkbuffer;
    decoded = buffer;
    
    // Grab the initial information for this block
    for (Uint32 c = 0; c < decoder->wavefmt.channels; ++c) {
        // Fill the state information for this block
        state[c].sample = ((encoded[1] << 8) | encoded[0]);
        encoded += 2;
        if (state[c].sample & 0x8000) {
            state[c].sample -= 0x10000;
        }
        state[c].index = *encoded++;
        // Reserved byte in buffer header, should be 0
        if (*encoded++ != 0) {
            // Uh oh, corrupt data?  Buggy code?
            assert(0);
        }
            
        // Store the initial sample we start with
        decoded[0] = (Uint8) (state[c].sample & 0xFF);
        decoded[1] = (Uint8) (state[c].sample >> 8);
        decoded += 2;
    }
        
    // Decode and store the other samples in this block
    samplesleft = (decoder->blocksize - 1) * decoder->wavefmt.channels;
    while (samplesleft > 0) {
        for (Uint32 c = 0; c < decoder->wavefmt.channels; ++c) {
            IMA_Decoder_Fill(decoded, encoded, c, decoder->wavefmt.channels, &state[c]);
            encoded += 4;
            samplesleft -= 8;
        }
        decoded += (decoder->wavefmt.channels * 8 * 2);
    }

    return output_len;
}


#pragma mark -
#pragma mark Chunk Management

/**
 * This struct represents a general data chunk
 */
typedef struct Chunk {
    Uint32 magic;
    Uint32 length;
    Uint8 *data;
} WaveChunk;

/**
 * Reads a single chunk of data from the given file.
 *
 * If the read is successful, this method returns the number of bytes read.
 * If it is not successful, it will return a negative number with the 
 * appropriate error code.
 *
 * @param src   The source file
 * @param chunk The struct to store the data read
 *
 * @return the number of bytes read (or an error code on faulure)
 */
static int readChunk(SDL_RWops * src, WaveChunk * chunk) {
    chunk->magic = SDL_ReadLE32(src);
    chunk->length = SDL_ReadLE32(src);
    chunk->data = (Uint8 *) SDL_malloc(chunk->length);
    if (chunk->data == NULL) {
        return SDL_OutOfMemory();
    }
    if (SDL_RWread(src, chunk->data, chunk->length, 1) != 1) {
        SDL_free(chunk->data);
        chunk->data = NULL;
        return SDL_Error(SDL_EFREAD);
    }
    return (chunk->length);
}

#pragma mark -
#pragma mark WAV Decoder

/**
 * This represents the supported WAV encoding types
 *
 * More obscure coding types (e.g. DTS WAV) may or may not be supported.
 */
typedef enum {
	/** Raw PCM data in 16bit samples (the most common format) */
	WAV_PCM_DATA   = 0,
	/** Raw PCM data with 32bit float samples */
	WAV_IEEE_FLOAT = 1,
	/** MS encoded ADPCM data */
	WAV_MS_ADPCM   = 2,
	/** IMA encoded ADPCM data */
	WAV_IMA_ADPCM  = 3,
	/** MP3 data encoded in a WAV file */
	WAV_MP3_DATA   = 4,
	/** Unsupported WAV encoding */
	WAV_UNKNOWN    = 5
} WaveType;

#pragma mark -
#pragma mark CODEC Functions

/**
 * The internal structure for decoding 
 */
typedef struct {
    /** The file stream for the audio */
    SDL_RWops* stream;
    /** The buffer for reading pages */
    Uint8* chunker;
    /** The encoding type */
    WaveType datatype;
    
	/** The size of a decoder chunk */
    Uint32 pagesize;
    
    /** The current page in the stream */
    Uint64 currpage;
    /** The previous page in the stream */
    Uint64 lastpage;

    /** The number of bits per sample represented as a flag mask */
    Uint32 sampbits;
    /** The number of bytes per sample */
    Uint32 sampsize;
    /** The start of the audio stream, just after the header */
    Sint64 datamark;
    
    /** An optional proxy for decoding ACPCM data */
    CODEC_ADPCM* adpcm;
} CODEC_WAV;

/**
 * Reads a single page of audio data into the buffer
 *
 * This function reads in the current stream page into the buffer. The data written 
 * into the buffer is linear PCM data with interleaved channels. If the stream is
 * at the end, nothing will be written.
 *
 * The size of a page is given by {@link CODEC_PageSize}. This buffer should
 * large enough to hold this data. As the page size is in audio frames, that means
 * the buffer should be pagesize * # of channels * sizeof(float) bytes.
 *
 * This function is a helper for {@link CODEC_Read} and {@link CODEC_Fill}.
 *
 * @param source    The audio source
 * @param decoder   The WAV decoder
 * @param buffer    The buffer to hold the PCM data
 *
 * @return the number of audio frames (samples/channels) read
 */
Sint32 wav_read_page(CODEC_Source* source, CODEC_WAV* decoder, float* buffer) {
    // Read into the local chunk
    Uint32 avail = decoder->pagesize*source->channels*decoder->sampsize;
    if (decoder->currpage == decoder->lastpage) {
        avail = (source->frames % decoder->pagesize)*source->channels*decoder->sampsize;
    } else if (decoder->currpage > decoder->lastpage) {
        avail = 0;
    }
    
    if (avail == 0) {
        return 0;
    } else {
    	switch (decoder->datatype) {
    	case WAV_MS_ADPCM:
			if (!MS_Decoder_Read((CODEC_ADPCM*)(decoder->adpcm), decoder->stream, decoder->chunker)) {
            	return 0;
        	}
			break;
    	case WAV_IMA_ADPCM:
			if (!IMA_Decoder_Read((CODEC_ADPCM*)(decoder->adpcm), decoder->stream, decoder->chunker)) {
            	return 0;
        	}
			break;
    	default:
			if (!SDL_RWread(decoder->stream, decoder->chunker, avail, 1)) {
            	return 0;
        	}
        	break;
    	}
    }
    
    // Now convert
    Uint32 temp = avail/decoder->sampsize;
    float* output = buffer;
    switch (decoder->sampbits) {
        case AUDIO_S16:
        {
            Sint16* input = (Sint16*)decoder->chunker;
            double factor = 1.0/(1 << 16);
            while(temp--) {
                *output = *input*factor;
                output++;
                input++;
            }
        }
            break;
        case AUDIO_S8:
        {
            Sint8* input = (Sint8*)decoder->chunker;
            double factor = 1.0/(1 << 8);
            while(temp--) {
                *output = *input*(factor);
                output++;
                input++;
            }
        }
            break;
        case AUDIO_S32:
        {
            Sint32* input = (Sint32*)decoder->chunker;
            double factor = 1.0/(((Uint64)1) << 32);
            while(temp--) {
                *output = *input*(factor);
                output++;
                input++;
            }
        }
            break;
        case AUDIO_F32:
        {
            float* input = (float*)decoder->chunker;
            while(temp--) {
                *output = *input;
                output++;
                input++;
            }
        }
            break;
            
        default:
            break;
    }
    
    decoder->currpage++;
    return avail/(source->channels*decoder->sampsize);
}

/** 
 * Creates a new CODEC_Source from an WAV file
 * 
 * This function will return NULL if the file cannot be located or is not an
 * proper WAV file. The file will not be read into memory, but is instead
 * available for streaming.
 *
 * Note that WAV is a container type in addition to a codec. Not all WAV files
 * are supported.
 *
 * Is the responsibility of the caller of this function to close the CODEC_Source
 * (with {@link CODEC_Close}) when done.
 *
 * @param filename    The name of the file to load
 *
 * @return a new CODEC_Source from an WAV file
 */
CODEC_Source* CODEC_OpenWAV(const char* filename) {
	WaveChunk chunk;
    CODEC_WAV* decoder = NULL;
    
    int was_error = 0;
    int is_adpcm = 0;
    int lenread;
    
    Uint8 channels = 0;
    Uint32 rate = 0;
    Uint64 frames = 0;
    
    // WAV magic header
    Uint32 RIFFchunk;
    Uint32 WAVEmagic;
    Uint32 wavelen = 0;
    
    // FMT chunk
    WaveFMT *format = NULL;
    
    // For seeking through chunk blocks
    Uint8* temp = NULL;
    Uint32 mark = JUNK;
    
    SDL_zero(chunk);
    
    SDL_RWops *stream = SDL_RWFromFile(filename,"r");
	was_error = 0;
    if (stream == NULL) {
        SDL_SetError("'%s' not found",filename);
        was_error = 1;
        goto done;
    }
    
    // Check the magic header
    RIFFchunk = SDL_ReadLE32(stream);
    wavelen   = SDL_ReadLE32(stream);
    if (wavelen == WAVE) {
        // The RIFFchunk has already been read
        WAVEmagic = wavelen;
        wavelen = RIFFchunk;
        RIFFchunk = RIFF;
    } else {
        WAVEmagic = SDL_ReadLE32(stream);
    }
    
    if ((RIFFchunk != RIFF) || (WAVEmagic != WAVE)) {
        SDL_SetError("'%s' has unrecognized file type (not WAVE)", filename);
        was_error = 1;
        goto done;
    }
    
    // Read the audio data format chunk
    chunk.data = NULL;
    do {
        SDL_free(chunk.data);
        chunk.data = NULL;
        lenread = readChunk(stream, &chunk);
        if (lenread < 0) {
            was_error = 1;
            goto done;
        }
    } while ((chunk.magic == FACT) || (chunk.magic == LIST) || (chunk.magic == BEXT) || (chunk.magic == JUNK));
    
    // Decode the audio data format
    format = (WaveFMT *) chunk.data;
    if (chunk.magic != FMT) {
        SDL_SetError("Complex WAVE files not supported");
        was_error = 1;
        goto done;
    } else if (format == NULL) {
        SDL_SetError("WAVE file format not recognized");
        was_error = 1;
        goto done;
    }
    
    decoder = (CODEC_WAV*)SDL_malloc(sizeof(CODEC_WAV));
    decoder->adpcm = NULL;
    switch (SDL_SwapLE16(format->encoding)) {
        case PCM_CODE:
            // We can understand this
            decoder->datatype = WAV_PCM_DATA;
            break;
        case IEEE_FLOAT_CODE:
            // We can understand this
            decoder->datatype = WAV_IEEE_FLOAT;
            break;
        case MS_ADPCM_CODE:
            // Try to understand this
            decoder->datatype = WAV_MS_ADPCM;
            decoder->adpcm = CODEC_Alloc_MS_ADPCM(format);
            is_adpcm = 1;
            if (decoder->adpcm == NULL) {
                was_error = 1;
                goto done;
            }
            break;
        case IMA_ADPCM_CODE:
            // Try to understand this
            decoder->datatype = WAV_IMA_ADPCM;
            decoder->adpcm = CODEC_Alloc_IMA_ADPCM(format);
            is_adpcm = 1;
            if (decoder->adpcm == NULL) {
                was_error = 1;
                goto done;
            }
            break;
        case MP3_CODE:
            SDL_SetError("MPEG Layer 3 data is not supported in WAVE files");
            was_error = 1;
            goto done;
            
        default:
            decoder->datatype = WAV_UNKNOWN;
            SDL_SetError("Unknown WAVE data format: 0x%.4x",
                         SDL_SwapLE16(format->encoding));
            was_error = 1;
            goto done;
    }
    
    rate = SDL_SwapLE32(format->frequency);
    decoder->sampbits = AUDIO_S16;
    channels = (Uint8) SDL_SwapLE16(format->channels);
    
    decoder->sampsize = sizeof(Sint16);
    if (decoder->datatype == WAV_IEEE_FLOAT) {
        if ((SDL_SwapLE16(format->bitspersample)) != 32) {
            was_error = 1;
        } else {
            decoder->sampbits = AUDIO_F32;
            decoder->sampsize = sizeof(float);
        }
    } else {
        switch (SDL_SwapLE16(format->bitspersample)) {
            case 4:
                if (is_adpcm) {
                    decoder->sampbits = AUDIO_S16;
                } else {
                    was_error = 1;
                }
                break;
            case 8:
                decoder->sampbits = AUDIO_U8;
                decoder->sampsize = sizeof(Uint8);
                break;
            case 16:
                decoder->sampbits = AUDIO_S16;
                break;
            case 32:
                decoder->sampbits = AUDIO_S32;
                decoder->sampsize = sizeof(Sint32);
                break;
            default:
                was_error = 1;
                break;
        }
    }
    
    if (was_error) {
        SDL_SetError("Unknown %d-bit PCM data format",
                     SDL_SwapLE16(format->bitspersample));
        goto done;
    }
    
    // Seek ahead to the beginning of data
    do {
        mark = SDL_ReadLE32(stream);
        frames = SDL_ReadLE32(stream);
        if (mark != DATA) {
            temp = (Uint8 *) SDL_malloc((size_t)frames);
            if (SDL_RWread(stream, temp, (size_t)frames, 1) != 1) {
                SDL_SetError("Data chunk is corrupted");
                was_error = 1;
                mark = DATA;
            }
            SDL_free(temp);
        }
    } while (mark != DATA);
    
    if (is_adpcm) {
        decoder->pagesize = decoder->adpcm->blocksize;
        decoder->lastpage = (Uint32)(frames/decoder->pagesize);
        frames  = (decoder->pagesize*frames)/decoder->adpcm->wavefmt.blockalign;
    } else {
        // Good default buffer size
        frames  /= (decoder->sampsize*channels);
        decoder->pagesize = PAGE_SIZE/(decoder->sampsize*channels);
        decoder->lastpage = (Uint32)(frames/decoder->pagesize);
    }
    
    decoder->currpage = 0;
    decoder->datamark = SDL_RWtell(stream);

done:
    SDL_free(format);
    if (was_error && stream != NULL) {
        SDL_RWclose(stream);
        stream = NULL;
        if (decoder->adpcm != NULL) {
        	SDL_free(decoder->adpcm);
        	decoder->adpcm = NULL;
        }
        SDL_free(decoder);
        decoder = NULL;
        return NULL;
    }
    
    
    if (!was_error) {
    	Uint32 capacity = decoder->pagesize*channels*decoder->sampsize;
        decoder->chunker = (Uint8 *)SDL_malloc(capacity);
        memset(decoder->chunker,0,capacity);
        decoder->stream = stream;
        
        CODEC_Source* result = (CODEC_Source*)SDL_malloc(sizeof(CODEC_Source));
		result->type = CODEC_TYPE_WAV;
        result->channels = channels;
        result->rate = rate;
        result->frames = frames;
        result->decoder = decoder;
        return result;
    }
    
    return NULL;
}

/**
 * The WAV specific implementation of {@link CODEC_Close}.
 *
 * @param source    The source to close
 * 
 * @return 0 if the source was successfully closed, -1 otherwise.
 */
Sint32 CODEC_WAV_Close(CODEC_Source* source) {
	CHECK_SOURCE(source,-1)
    
    CODEC_WAV* decoder = (CODEC_WAV*)(source->decoder);
    if (decoder->adpcm != NULL) {
		SDL_free(decoder->adpcm);
		decoder->adpcm = NULL;
	}
    if (decoder->stream != NULL) {
        SDL_RWclose(decoder->stream);
		decoder->stream = NULL;
	}
	SDL_free(decoder);
	source->decoder = NULL;
	SDL_free(source);
	return 0;
}

/**
 * The WAV specific implementation of {@link CODEC_Seek}.
 *
 * @param source    The audio source
 * @param page        The page to seek
 *
 * @return the page acquired, or -1 if there is an error
 */
Sint32 CODEC_WAV_Seek(CODEC_Source* source, Uint32 page) {
	CHECK_SOURCE(source,-1)
    
    CODEC_WAV* decoder = (CODEC_WAV*)(source->decoder);
    Uint32 chunk  = decoder->pagesize*source->channels*decoder->sampsize;
    Uint64 offset = page*chunk;
    if (page >= decoder->lastpage) {
        offset = source->frames*source->channels*decoder->sampsize;
    }
    
    SDL_RWseek(decoder->stream, decoder->datamark+offset, RW_SEEK_SET);
    decoder->currpage = offset/chunk;
	return (int)decoder->currpage;
}

/**
 * The WAV specific implementation of {@link CODEC_PageSize}.
 *
 * @param source    The audio source
 *
 * @return the number of audio frames in a page, or -1 if there is an error
 */
Sint32 CODEC_WAV_PageSize(CODEC_Source* source) {
    CHECK_SOURCE(source,-1)
    return ((CODEC_WAV*)(source->decoder))->pagesize;
}

/**
 * The WAV specific implementation of {@link CODEC_LastPage}.
 *
 * @param source    The audio source
 *
 * @return the index of the last page in the stream, or -1 if there is an error
 */
Sint32 CODEC_WAV_LastPage(CODEC_Source* source) {
    CHECK_SOURCE(source,-1)
    return (Sint32)((CODEC_WAV*)(source->decoder))->lastpage;
}

/**
 * The WAV specific implementation of {@link CODEC_Tell}.
 *
 * @param source    The audio source
 *
 * @return the index of the current page in the stream, or -1 if there is an error
 */
Sint32 CODEC_WAV_Tell(CODEC_Source* source) {
    CHECK_SOURCE(source,-1)
    return (Sint32)((CODEC_WAV*)(source->decoder))->currpage;
}

/**
 * The WAV specific implementation of {@link CODEC_EOF}.
 *
 * @param source    The audio source
 *
 * @return 1 if the audio source is at the end of the stream; 0 otherwise
 */
Uint32 CODEC_WAV_EOF(CODEC_Source* source) {
    CHECK_SOURCE(source,0)
    CODEC_WAV* decoder = (CODEC_WAV*)(source->decoder);    
    return (decoder->currpage == decoder->lastpage);
}

/**
 * The WAV specific implementation of {@link CODEC_Read}.
 *
 * @param source    The audio source
 * @param buffer    The buffer to hold the PCM data
 *
 * @return the number of audio frames (samples/channels) read
 */
Sint32 CODEC_WAV_Read(CODEC_Source* source, float* buffer) {
	CHECK_SOURCE(source,-1)
	
	CODEC_WAV* decoder = (CODEC_WAV*)(source->decoder);
	return wav_read_page(source, decoder, buffer);
}

/**
 * The WAV specific implementation of {@link CODEC_Fill}.
 *
 * @param source    The audio source
 * @param buffer    The buffer to hold the PCM data
 *
 * @return the number of audio frames (samples/channels) read
 */
Sint64 CODEC_WAV_Fill(CODEC_Source* source, float* buffer) {
    CHECK_SOURCE(source,-1)
    
    CODEC_WAV* decoder = (CODEC_WAV*)(source->decoder);
    Uint32 currpage = (Uint32)decoder->currpage;
    if (currpage != 0) {
        CODEC_WAV_Seek(source,0);
    }
	
    Sint32 amt  = 0;
	Sint64 read = 0;
	Sint64 offset = 0;
	while (decoder->currpage < decoder->lastpage) {
        amt = wav_read_page(source,decoder,buffer+offset);
		read += amt;
		offset += amt*source->channels;
	}
    if (decoder->currpage == decoder->lastpage) {
        amt = wav_read_page(source,decoder,buffer+offset);
        read += amt;
    }

    if (currpage != 0) {
        CODEC_WAV_Seek(source,currpage);
    }
    return read;
}
