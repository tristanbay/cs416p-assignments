// Copyright (c) 2026 me (Tristan); see license.txt for license info

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sndfile.h>

#define PI 3.14159265358979323846
#define RATE 48000
#define AUDIONAME "message.wav"
#define BITLEN 160
#define MARKF 2225
#define SPACEF 2025


char correlate(short* input)
{
	double mark_i = 0, mark_q = 0, space_i = 0, space_q = 0, mark_angle, space_angle;
	for (int n = 0; n < BITLEN; ++n) {
		mark_angle = 2.0 * PI * MARKF * n / (double)RATE;
		space_angle = 2.0 * PI * SPACEF * n / (double)RATE;
		mark_i += input[n] * cos(mark_angle);
		space_i += input[n] * cos(space_angle);
		mark_q += input[n] * sin(mark_angle);
		space_q += input[n] * sin(space_angle);
	}
	double mark_pwr = (mark_i * mark_i) + (mark_q * mark_q);
	double space_pwr = (space_i * space_i) + (space_q * space_q);
	return mark_pwr > space_pwr ? (char)1 : (char)0;
}

char decode(short* input)
{
	char output = (char)0;
	for (int i = 1; i < 9; ++i)
		output += correlate(&input[BITLEN * i]) << (i - 1);
	return output;
}

int main()
{
	SF_INFO audio_info;
	SNDFILE* audio_in = sf_open(AUDIONAME, SFM_READ, &audio_info);
	short* audio_data = (short*)malloc(sizeof(short) * audio_info.frames);
	int text_size = audio_info.frames / BITLEN / 10;
	char* text_data = (char*)malloc(text_size + 1);

	sf_read_short(audio_in, audio_data, audio_info.frames);

	for (int i = 0; i < text_size; ++i)
		text_data[i] = decode(&audio_data[i * BITLEN * 10]);

	text_data[text_size] = '\0';
	printf("%s", text_data);

	free(audio_data);
	free(text_data);
	sf_close(audio_in);
	return EXIT_SUCCESS;
}
