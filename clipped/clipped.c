// Assignment for CS 416P at Portland State University, April 2026

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <math.h>
#include <sndfile.h>

#define PI 3.14159265358979323846
#define RATE 48000
#define SECONDS 1
#define CHANNELS 1
#define FREQ 440
#define MODENAME_MAX 8
#define USAGE_MSG "Usage: \"./clipped <mode (sine or clipped)>\"\n"

int main(int32_t argc, char** argv)
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
	return EXIT_SUCCESS;
}
