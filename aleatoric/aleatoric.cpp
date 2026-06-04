/* This code is licensed under the GNU GPL v3 or later.
 * To read the license, please see the `license.txt` file in the directory above or go to:
 * https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 * Copyright 2026 Tristan Bay
 */

#include "aleatoric.h"
#include <iostream>
#include <chrono>
#include <random>
#include <limits>
#include <cmath>
#include <vector>
#include <sndfile.h>
#include <portaudio.h>

Song::Song() // public constructor
{
	unsigned rseed = std::chrono::system_clock::now().time_since_epoch().count();
	std::mt19937 rng(rseed);

	edo = edo_choices[rng() % EDO_COUNT]; // choose edo
	std::cout << "EDO: " << edo << "\n";

	key = rng() % edo;
	std::cout << "Key: " << key << " edostep(s) above A=" << A_NATURAL << "\n";
	key_hz = A_NATURAL * pow(2, (float)key / edo);

	tempo = ((float)rng() / std::numeric_limits<unsigned>::max() // choose tempo
			* (BPM_MAX - BPM_MIN)) + BPM_MIN; // bpm
	std::cout << "BPM: " << tempo << "\n";
	tempo = (float)SAMPLE_RATE * 60 / tempo; // convert to samples per beat
	
	generate_scales(); // generate note frequencies and indices for the scales to be used
	std::cout << "\n";
	for (int i = 0; i < BASE_SCALE_NOTES; ++i) {
		std::cout << "Base scale note " << i << ": degree " << base_scale_indices[i]
			<< " of full scale, " << full_scale[base_scale_indices[i]] << " samples\n";
	}
	std::cout << "\n";
	for (int i = 0; i < FULL_SCALE_NOTES; ++i)
		std::cout << "Full scale note " << i << ": " << full_scale[i] << " samples\n";
	std::cout << "\n";

	choose_song_structure(rng); // choose progression and line pattern to use
	for (int i = 0; i < UNIQUE_LINES; ++i) {
		std::cout << "Progression " << i << ": ";
		for (int j = 0; j < CHORD_COUNT; ++j) {
			std::cout << progressions[chosen_progs[i]][j];
			if (j < CHORD_COUNT - 1)
				std::cout << ", ";
		}
		std::cout << "\n";
	}
	std::cout << "\nLine pattern number: " << chosen_line_pattern << "\n\n";

	write_notes(rng); // write all the notes to the 2D melodies array
	
	for (int i = 0; i < UNIQUE_LINES; ++i) {
		std::cout << "Line " << i << ":\n";
		for (int j = 0; j < CHORD_COUNT * BEAT_COUNT * NOTES_PER_BEAT; ++j)
			std::cout << "\tNote " << j << ": " << melodies[i][j] << " samples\n";
		std::cout << "\n";
	}

	audio_len = ceil(tempo * BEAT_COUNT * CHORD_COUNT * LINE_COUNT);
	audio = new short[audio_len]; // allocate audio array
	audio[audio_len - 1] = 0; // just in case there's a rounding error or something
	write_audio(); // write data to audio array
}

// --- PRIVATE CONSTRUCTOR HELPERS ---

void Song::generate_scales() // use chain of fifths to generate scales
{
	int degree_gen_steps = round(log(GEN_INTERVAL) / log(EQU_INTERVAL) * edo);

	int index_gen_steps = round(log(GEN_INTERVAL) / log(EQU_INTERVAL) * FULL_SCALE_NOTES);
	int index = FULL_SCALE_OFFSET;
	int degree = 0;
	for (int i = 0; i != FULL_SCALE_OFFSET; // set edo degree to correct value
			i = (i + FULL_SCALE_NOTES - index_gen_steps) % FULL_SCALE_NOTES) {
		degree = (degree + edo - degree_gen_steps) % edo;
	}
	do { // fill in full scale notes
		full_scale[index] = (float)SAMPLE_RATE
			/ (key_hz * pow(EQU_INTERVAL, (float)degree / edo));
		index = (index + index_gen_steps) % FULL_SCALE_NOTES;
		degree = (degree + degree_gen_steps) % edo;
	} while (index != FULL_SCALE_OFFSET);

	degree_gen_steps = index_gen_steps; // becomes degree steps of full scale
	index_gen_steps = round(log(GEN_INTERVAL) / log(EQU_INTERVAL) * BASE_SCALE_NOTES);
	index = BASE_SCALE_OFFSET;
	degree = 0;
	for (int i = 0; i != BASE_SCALE_OFFSET; // set full scale degree to correct value
			i = (i + BASE_SCALE_NOTES - index_gen_steps) % BASE_SCALE_NOTES) {
		degree = (degree + FULL_SCALE_NOTES - degree_gen_steps) % FULL_SCALE_NOTES;
	}
	do { // fill in base scale note indices
		base_scale_indices[index] = degree;
		index = (index + index_gen_steps) % BASE_SCALE_NOTES;
		degree = (degree + degree_gen_steps) % FULL_SCALE_NOTES;
	} while (index != BASE_SCALE_OFFSET);
}

void Song::choose_song_structure(std::mt19937& rng) // random chord progs and line pattern
{
	std::vector<int> progs_to_choose_from;
	for (int i = 0; i < PROG_COUNT; ++i)
		progs_to_choose_from.emplace_back(i);
	int prog_choice;
	for (int i = 0; i < UNIQUE_LINES; ++i) {
		prog_choice = rng() % (PROG_COUNT - i);
		chosen_progs[i] = progs_to_choose_from[prog_choice];
		progs_to_choose_from.erase(progs_to_choose_from.begin() + prog_choice);
	}
	chosen_line_pattern = rng() % LINE_PTN_COUNT;
}

void Song::write_notes(std::mt19937& rng)
{
	for (int i = 0; i < UNIQUE_LINES; ++i) {
		for (int j = 0; j < CHORD_COUNT; ++j) {
			for (int k = 0; k < BEAT_COUNT * NOTES_PER_BEAT; ++k) {
				if (rng() % 100 < CHORD_PERCENT) { // if chord note
					melodies[i][j * BEAT_COUNT * NOTES_PER_BEAT + k] =
						chord_note(progressions[chosen_progs[i]][j], rng);
				} else { // if base scale note
					melodies[i][j * BEAT_COUNT * NOTES_PER_BEAT + k] = base_scale_note(rng);
				}
			}
		}
	}
}

float Song::chord_note(int chord_index, std::mt19937& rng)
{
	int chord_note = rng() % CHORD_NOTES;
	int out = abs(chord_index); // chord index is in degrees of full scale, not base scale
	switch (chord_note) {
		case 0:
			break;
		case 1:
			out += chord_index < 0 ? TOP_CHORD_NOTE - MID_CHORD_NOTE : MID_CHORD_NOTE;
			break;
		default:
			out += TOP_CHORD_NOTE;
			break;
	}
	return full_scale[out % FULL_SCALE_NOTES];
}

float Song::base_scale_note(std::mt19937& rng)
{
	int scale_note = rng() % BASE_SCALE_NOTES;
	return full_scale[base_scale_indices[scale_note]];
}

void Song::write_audio()
{
	double note_len = tempo / NOTES_PER_BEAT; // length of note in samples
	double note_period = 0; // the wave period of current note; corresponds to frequency

	double note_prog = 0; // how far through the note in samples
	double wave_prog = 0; // how far through the wave out of 1

	int line_prog = 0; // the current note in line
	int which_line = 0; // current line in song
	for (int i = 0; i < audio_len; ++i) {
		note_period = melodies[line_patterns[chosen_line_pattern][which_line]][line_prog];
		audio[i] = saw_wave_note(wave_prog, note_prog, note_len);
		note_prog += 1.0;
		if (note_prog >= note_len) { // wrap note progress & advance to next note if needed
			++line_prog;
			if (line_prog >= NOTES_PER_BEAT * BEAT_COUNT * CHORD_COUNT) { // to next line
				++which_line;
				line_prog = 0;
			}
			note_prog -= note_len;
		}
		wave_prog += 1.0 / note_period;
		if (wave_prog >= 1)
			wave_prog -= 1.0;
	}
}

short Song::saw_wave_note(double wave_prog, double note_prog, double note_len)
{
	double base_amp = wave_prog < 0.5 ? wave_prog : wave_prog - 1.0;
	double env = 1.0 - (note_prog / note_len);
	return (short)round(base_amp * env * (std::numeric_limits<unsigned short>::max() - 1));
}

// --- END OF PRIVATE CONSTRUCTOR HELPERS ---

Song::~Song() // public destructor
{
	delete[] audio; // deallocate audio array
}

void Song::write_to_wav() // some of this function was taken from the clipped assignment
{
	SF_INFO wave_info = {
		audio_len,
		SAMPLE_RATE,
		CHANNELS,
		SF_FORMAT_WAV | SF_FORMAT_PCM_16,
		0,
		0
	};

	SNDFILE* wave = sf_open(FILENAME, SFM_WRITE, &wave_info);

	sf_write_short(wave, audio, audio_len);

	sf_close(wave);
}

void Song::play() // some of this function was taken from the clipped assignment
{
	PaStreamParameters out_params;
	PaStream* out_audio;
	PaError out_error = Pa_Initialize();
	
	if (out_error != paNoError) {
		std::cout << "Playback initialization error\n";
		Pa_Terminate();
		return;
	}

	WaveData play_this(audio_len, audio);
	out_params.device = Pa_GetDefaultOutputDevice();
	if (out_params.device == paNoDevice) {
		std::cout << "No output device detected\n";
		Pa_Terminate();
		return;
	}

	out_params.channelCount = CHANNELS;
	out_params.sampleFormat = paInt16;
	out_params.suggestedLatency =
		Pa_GetDeviceInfo(out_params.device)->defaultLowOutputLatency;
	out_params.hostApiSpecificStreamInfo = nullptr;

	out_error =
		Pa_OpenStream(&out_audio, nullptr, &out_params, SAMPLE_RATE, OUTBUF_SIZE, paClipOff,
		play_callback, &play_this);
	if (out_error != paNoError) {
		std::cout << "Error opening audio stream\n";
		Pa_Terminate();
		return;
	}

	if (out_audio) {
		out_error = Pa_StartStream(out_audio);
		if (out_error != paNoError) {
			std::cout << "Error starting audio playback\n";
			Pa_Terminate();
			return;
		}

		while ((out_error = Pa_IsStreamActive(out_audio)) == 1)
			Pa_Sleep(125);
		if (out_error != paNoError) {
			std::cout << "Error playing audio\n";
			Pa_Terminate();
			return;
		}

		out_error = Pa_CloseStream(out_audio);
		if (out_error != paNoError) {
			std::cout << "Error closing audio stream\n";
			Pa_Terminate();
			return;
		}
	}

	Pa_Terminate();
}

int Song::play_callback(const void* in_buf, void* out_buf, unsigned long buf_len,
		const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags flags,
		void* user_data) // some of this function was taken from the clipped assignment
{
	WaveData* w_data = (WaveData*)user_data;
	int finished, extra;
	short* write = (short*)out_buf;
	unsigned remaining = w_data->get_data_size() - w_data->get_current_index();
	
	(void)in_buf; // not using these, so this is to prevent unused variable warnings
	(void)time_info;
	(void)flags;

	if (remaining < buf_len) {
		extra = buf_len - remaining;
		for (unsigned i = 0; i < remaining; ++i)
			*(write++) = w_data->get_and_advance_sample();
		for (int i = 0; i < extra; ++i)
			*(write++) = 0;
		finished = paComplete;
	} else {
		for (unsigned i = 0; i < buf_len; ++i)
			*(write++) = w_data->get_and_advance_sample();
		finished = paContinue;
	}
	return finished;
}

WaveData::WaveData(unsigned data_len, short* data):
		current(0), data_size(data_len), audio(data)
{
}

WaveData::~WaveData()
{
	audio = nullptr;
}

short WaveData::get_and_advance_sample()
{
	short out = audio[current];
	++current;
	return out;
}

unsigned WaveData::get_current_index()
{
	return current;
}

unsigned WaveData::get_data_size()
{
	return data_size;
}

int main()
{
	Song song;
	song.write_to_wav();
	song.play();
	return 0;
}
