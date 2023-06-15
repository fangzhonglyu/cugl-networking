/*
 * mp3stream:  An simple mp3 streaming API
 * Copyright (C) 2022 Walker M. White
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
#define MINIMP3_IMPLEMENTATION
#include "minimp3.h"
#include "minimp3_ex.h"
#include "mp3stream.h"
#include <SDL.h>

/** Internal, opaque type for decoding MP3s */
struct MP3Stream {
	SDL_RWops* source;
	mp3dec_ex_t context;
	mp3dec_io_t stream;
};

/**
 * The wrapper to read from the IO stream
 *
 * @param buf		The buffer to hold the data read
 * @param size		The number of bytes to read
 * @param user_data	The data stream to read from
 *
 * @return the number of bytes read
 */
size_t read_stream(void *buf, size_t size, void *user_data) {
	SDL_RWops *stream = (SDL_RWops*)user_data;
	return SDL_RWread(stream,buf,1,size);
}

/**
 * The wrapper to seek the IO stream
 *
 * @param position	The byte offset to seek
 * @param user_data	The data stream to read from
 *
 * @return 0 if the seek is successful
 */
int seek_stream(uint64_t position, void *user_data) {
	SDL_RWops *stream = (SDL_RWops*)user_data;
	return SDL_RWseek(stream,position,RW_SEEK_SET) < 0;
}

/**
 * Allocates a new MP3 stream for the given file
 *
 * The file must be in the current working directory, or else this will return NULL.
 * It the responsibility of the user to free this stream when done.
 *
 * @param file	The name of the MP3 file
 *
 * @return a new MP3 stream for the given file
 */
MP3Stream* MP3Stream_Alloc(const char* file) {
	SDL_RWops *source = SDL_RWFromFile(file, "rb");
	MP3Stream* result = (MP3Stream*)malloc(sizeof(MP3Stream));
	
	result->source = source;
	result->stream.read_data = source;
	result->stream.seek_data = source;	
	result->stream.read = read_stream;
	result->stream.seek = seek_stream;

	int error = mp3dec_ex_open_cb(&(result->context), &(result->stream), MP3D_SEEK_TO_SAMPLE);
	if (error) {
		SDL_SetError("Could not open '%s': Code %d\n", file,error);
		SDL_RWclose(source);
		free(result);
        return NULL;
	}
	
	return result;
}

/**
 * Frees the given MP3 stream
 *
 * It the responsibility of the user to free any allocated stream when done.
 *
 * @param stream	The MP3 stream to free
 *
 * @return 0 if successful
 */
Sint32 MP3Stream_Free(MP3Stream* stream) {
	if (stream == NULL) {
		SDL_SetError("Attempt to access a NULL MP3 stream");
        return -1;
	} else if (stream->source == NULL) {
		SDL_SetError("MP3 stream has been corrupted");
        return -1;
	}
	SDL_RWclose(stream->source);
	stream->source = NULL;
	free(stream);
	return 0;
}

/**
 * Returns 1 if this MP3 stream is stereo
 *
 * MP3 files can only either be mono or stereo.
 *
 * @param stream	The MP3 stream
 *
 * @return 1 if stereo, 0 if mono
 */
Uint32 MP3Stream_IsStereo(MP3Stream* stream) {
	if (stream == NULL) {
		return 0;
	}
	return stream->context.info.channels == 2;
}

/**
 * Returns the sample rate for this MP3 stream
 *
 * @param stream	The MP3 stream
 *
 * @return the sample rate for this MP3 stream
 */
Uint32 MP3Stream_GetFrequency(MP3Stream* stream) {
	if (stream == NULL) {
		return 0;
	}
	return stream->context.info.hz;	
}

/**
 * Returns the page size for this MP3 stream
 *
 * The page size is measured in audio frames, not samples. An audio frame is a 
 * collection of simultaneous samples for all of the different channels.
 *
 * @param stream	The MP3 stream
 *
 * @return the page size for this MP3 stream
 */
Uint32 MP3Stream_GetPageSize(MP3Stream* stream) {
	if (stream == NULL) {
		return 0;
	}
	return MINIMP3_MAX_SAMPLES_PER_FRAME/stream->context.info.channels;	
}

/**
 * Returns the last page for this MP3 stream
 *
 * @param stream	The MP3 stream
 *
 * @return the last page for this MP3 stream
 */
Uint32 MP3Stream_GetLastPage(MP3Stream* stream) {
	if (stream == NULL) {
		return 0;
	}
	return (int)(stream->context.samples/MINIMP3_MAX_SAMPLES_PER_FRAME);	
}

/**
 * Returns the size of this MP3 stream in audio frames
 *
 * An audio frame (not to be confused with a page) is a collection of simultaneous 
 * samples for all of the different channels. 
 *
 * @param stream	The MP3 stream
 *
 * @return the size of this MP3 stream in audio frames
 */
Uint32 MP3Stream_GetLength(MP3Stream* stream) {
	if (stream == NULL) {
		return 0;
	}
	return (int)stream->context.samples/stream->context.info.channels;
}

/**
 * Sets the current page for the MP3 stream
 *
 * Pages are more course grained than samples. We only allow seeking at the page
 * level. If the page is out of bounds, we will set to the last page.
 *
 * @param stream	The MP3 stream
 * @param page		The page to seek
 *
 * @return the new current page
 */
Uint32 MP3Stream_SetPage(MP3Stream* stream, Uint32 page) {
	if (stream == NULL) {
		return 0;
	}
	return mp3dec_ex_seek(&(stream->context),page*MINIMP3_MAX_SAMPLES_PER_FRAME);
}

/**
 * Reads in a page of MP3 data.
 *
 * This function reads in the current stream page into the buffer. The data written 
 * into the buffer is linear PCM data with interleaved channels. If the stream is
 * at the end, nothing will be written.
 *
 * The size of a page is given by {@link MP3Stream_GetPageSize}. This buffer should
 * large enough to hold this data. As the page size is in audio frames, that means
 * the buffer should be pagesize * # of channels * sizeof(Sint16) bytes.
 *
 * @param stream	The MP3 stream
 * @param buffer	The buffer to hold the PCM data
 *
 * @return the number of audio frames (samples/channels) read
 */
Sint64 MP3Stream_ReadPage(MP3Stream* stream, Sint16 *buffer) {
	if (stream == NULL) {
		return 0;
	}
    mp3dec_frame_info_t frame_info;
    memset(&frame_info, 0, sizeof(frame_info));
	mp3d_sample_t *buf_frame = NULL;
	size_t read_samples = mp3dec_ex_read_frame(&(stream->context), &buf_frame, &frame_info, stream->context.samples);
	if (!read_samples) {
		SDL_SetError("Unable to read frame");
        return -1;
	}

	memcpy(buffer, buf_frame, read_samples * sizeof(mp3d_sample_t));
	return (Sint64)read_samples;
}

/**
 * Reads in the entire MP3 stream into the buffer.
 *
 * The data written into the buffer is linear PCM data with interleaved channels. If the
 * stream is not at the initial page, it will rewind before writing the data. It will
 * restore the stream to the initial page when done.
 *
 * The buffer needs to be large enough to hold the entire MP3 stream. As the length is
 * measured in audio frames, this means that the buffer should be 
 * length * # of channels * sizeof(Sint16) bytes.
 *
 * @param stream	The MP3 stream
 * @param buffer	The buffer to hold the PCM data
 *
 * @return the number of audio frames (samples/channels) read
 */
Sint64 MP3Stream_Read(MP3Stream* stream, Sint16 *buffer) {
	return mp3dec_ex_read(&(stream->context),buffer,stream->context.samples);
}	
