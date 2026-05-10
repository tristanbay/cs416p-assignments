// Assignment for CS 416P at Portland State University, April/May 2026

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <math.h>
#include <sndfile.h>
#include <portaudio.h>

#define PI 3.14159265358979323846
#define RATE 48000
#define SECONDS 1
#define CHANNELS 1
#define FREQ 440
#define OUTBUF_SIZE 2048
#define MODENAME_MAX 8
#define USAGE_MSG "Usage: \"./clipped <mode (sine or clipped)>\"\n"

typedef struct wave_data_s
{
	uint32_t current, amt;
	int16_t* data;
} wave_data_t;

int32_t play_callback(const void* in_buf, void* out_buf, uint64_t buf_len,
		const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags flags,
		void* user_data)
{
	wave_data_t* w_data = (wave_data_t*)user_data;
	int32_t finished, extra;
	int16_t* read = &(w_data->data[w_data->current]);
	int16_t* write = (int16_t*)out_buf;
	int32_t remaining = w_data->amt - w_data->current;

	(void)in_buf;
	(void)time_info;
	(void)flags;

	if (remaining < buf_len) {
		extra = buf_len - remaining;
		for (int32_t i = 0; i < remaining; ++i)
			*(write++) = *(read++);
		for (int32_t i = 0; i < extra; ++i)
			*(write++) = 0;
		w_data->current += remaining;
		finished = paComplete;
	} else {
		for (int32_t i = 0; i < buf_len; ++i)
			*(write++) = *(read++);
		w_data->current += buf_len;
		finished = paContinue;
	}
	return finished;
}

int32_t main(int32_t argc, char** argv)
{
	uint32_t data_amt = RATE * SECONDS;
	int16_t wave_data[data_amt];
	bool clipped_mode = false;

	if (argc > 2) {
		printf(USAGE_MSG);
		return EXIT_FAILURE;
	}
	if (argc == 2) {
		if (!strncmp(argv[1], "clipped", MODENAME_MAX)) {
			clipped_mode = true;
		} else if (strncmp(argv[1], "sine", MODENAME_MAX)) {
			printf(USAGE_MSG);
			return EXIT_FAILURE;
		}
	}

	// code is more repetitive here for faster runtime (cache/memory access efficiency)
	if (clipped_mode) {
		for (uint32_t i = 0; i < data_amt; ++i)
			wave_data[i] = (int16_t)round(sin(i*2*PI*FREQ/(double)RATE)*SHRT_MAX/(double)2);
		for (uint32_t i = 0; i < data_amt; ++i) {
			if (wave_data[i] > SHRT_MAX / 4) {
				wave_data[i] = SHRT_MAX / 4;
			} else if (wave_data[i] < -SHRT_MAX / 4) {
				wave_data[i] = -SHRT_MAX / 4;
			}
		}
	} else {
		for (uint32_t i = 0; i < data_amt; ++i)
			wave_data[i] = (int16_t)round(sin(i*2*PI*FREQ/(double)RATE)*SHRT_MAX/(double)4);
	}
	
	SF_INFO wave_info = {
		(int32_t)data_amt,
		RATE,
		CHANNELS,
		SF_FORMAT_WAV | SF_FORMAT_PCM_16,
		0,
		0
	};

	SNDFILE* wave = sf_open(clipped_mode ? "clipped.wav" : "sine.wav", SFM_WRITE,
			&wave_info);

	sf_write_short(wave, wave_data, data_amt);
	
	sf_close(wave);

	PaStreamParameters out_params;
	PaStream* stream;
	PaError err = Pa_Initialize();
	if (err != paNoError) {
		printf("PortAudio error\n");
		Pa_Terminate();
		return EXIT_FAILURE;
	}

	wave_data_t play_this = { 0, data_amt, &(wave_data[0]) };

	out_params.device = Pa_GetDefaultOutputDevice();
	if (out_params.device == paNoDevice) {
		printf("No output device detected\n");
		Pa_Terminate();
		return EXIT_FAILURE;
	}
	out_params.channelCount = CHANNELS;
	out_params.sampleFormat = paInt16;
	out_params.suggestedLatency =
		Pa_GetDeviceInfo(out_params.device)->defaultLowOutputLatency;
	out_params.hostApiSpecificStreamInfo = NULL;

	err = Pa_OpenStream(&stream, NULL, &out_params, RATE, OUTBUF_SIZE, paClipOff, play_callback,
			&play_this);
	if (err != paNoError) {
		printf("Error opening stream\n");
		Pa_Terminate();
		return EXIT_FAILURE;
	}

	if (stream) {
		err = Pa_StartStream(stream);
		if (err != paNoError) {
			printf("Error starting stream\n");
			Pa_Terminate();
			return EXIT_FAILURE;
		}

		while ((err = Pa_IsStreamActive(stream)) == 1)
			Pa_Sleep(125);
		if (err < 0) {
			printf("Error playing stream\n");
			Pa_Terminate();
			return EXIT_FAILURE;
		}

		err = Pa_CloseStream(stream);
		if (err != paNoError) {
			printf("Error closing stream\n");
			Pa_Terminate();
			return EXIT_FAILURE;
		}
	}

	Pa_Terminate();
	return EXIT_SUCCESS;
}
