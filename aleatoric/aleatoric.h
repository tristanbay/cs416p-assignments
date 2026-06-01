/* This code is licensed under the GNU GPL v3 or later.
 * To read the license, please see the `license.txt` file in the directory above or go to:
 * https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 * Copyright 2026 Tristan Bay
 */

#include <random>

#define SAMPLE_RATE 48000

#define A_NATURAL 220

#define BPM_MIN 80
#define BPM_MAX 160

#define GEN_INTERVAL 1.5
#define EQU_INTERVAL 2.0

#define BASE_SCALE_NOTES 7
#define BASE_SCALE_OFFSET 3

#define FULL_SCALE_NOTES 12
#define FULL_SCALE_OFFSET 1

#define CHORD_NOTES 3

#define MID_CHORD_NOTE 4
#define TOP_CHORD_NOTE 7

#define NOTES_PER_BEAT 2
#define BEAT_COUNT 4
#define CHORD_COUNT 4
#define UNIQUE_LINES 4
#define LINE_COUNT 6

#define EDO_COUNT 4

#define PROG_COUNT 10
#define LINE_PTN_COUNT 3

#define CHORD_PERCENT 80

#define CHANNELS 1
#define OUTBUF_SIZE 2048

struct Song
{
	Song();
	~Song();
	void write_to_wav();
	void play();
	private:
		void generate_scales();
		void choose_song_structure(std::mt19937&);
		void write_notes(std::mt19937&);
		float chord_note(int, std::mt19937&);
		float base_scale_note(std::mt19937&);

		void write_audio(unsigned);
		short saw_wave_note(double, double, double);

		int edo; // the number of equal divisions per octave of the song (the tuning)
		int edo_choices[EDO_COUNT] = { 17, 26, 27, 31 }; // possible edos song could be in

		int key; // key of the song in number of edosteps from A natural
		float key_hz; // key of the song in hz

		float tempo; // tempo of song in samples per beat
		float full_scale[FULL_SCALE_NOTES]; // base = major scale, full = for chords
		int base_scale_indices[BASE_SCALE_NOTES]; // full scale note indices of base scale
		int progressions[PROG_COUNT][CHORD_COUNT] = { // possible chord progs
			{ 0, 3, -1, 4 }, // positive number is major, negative number is minor
			{ 0, -5, -1, 4 }, // absolute value of number is scale degree of root - 1
			{ 0, -2, 3, -3 }, // minor tonic chord can be represented as -7
			{ 0, 4, -1, 4 },
			{ 0, -5, 3, 4 },
			{ 3, 4, -5, 3 },
			{ 0, 4, -5, 0 },
			{ 0, 3, -3, 0 },
			{ 3, 4, 0, 0 },
			{ -5, 3, 0, 4 }
		};
		int chosen_progs[UNIQUE_LINES];
		int line_patterns[LINE_PTN_COUNT][LINE_COUNT] = { // possible line seqs for song
			{ 0, 0, 1, 1, 2, 2 },
			{ 0, 1, 0, 1, 2, 3 },
			{ 0, 1, 2, 3, 3, 3 }
		};
		int chosen_line_pattern;
		// 2D array to store the melodies to (potentially) be used
		float melodies[UNIQUE_LINES][CHORD_COUNT * BEAT_COUNT * NOTES_PER_BEAT];
		short* audio; // pointer to dynamically-allocated audio data for song
};
