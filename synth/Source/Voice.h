#pragma once

#include <JuceHeader.h>
#include <vector>

#define MIDI_NOTES 128
#define MAX_VEL 127

#define DEFAULT_VEL 64

#define EDO 12
#define MIDI_REF_NOTE 69
#define MIDI_REF_FREQ 440

struct MonoStack // implements last-pressed note priority for note number (but not velocity)
{
	MonoStack();

	void noteOn(int); // add new note number to stack
	int noteOff(int); // remove lifted note number from stack and return last data element

	private:
		int findInData(int); // sequential search, returns 1st inst of arg val in data or -1

		std::vector<int> stackData; // MIDI note number data of the note stack
};

struct Envelope // implements envelope modulation
{
	Envelope();

	private:
		int stage;
};

struct Voice
{
	Voice();

	void noteOn(int, int); // stuff to do when note is pressed
	void noteOff(int); // stuff to do when note is released
	
	void setSampleRate(double);
	float audioOut(); // function that actually does the audio processing
	private:
		float noteFreq(); // calculate the frequency of the note based on the note number
		float sawWave(); // calculate saw wave audio sample based on voice phase

		MonoStack stack;
		int isOn; // 0 if off, 1 if on
		int note; // current MIDI note of voice
		int vel; // current velocity of voice
		float freq; // current frequency of voice, calculated from "note"
		float phase; // current phase of wave, 0 to 1
		float sampleRate; // sample rate of the synth
};
