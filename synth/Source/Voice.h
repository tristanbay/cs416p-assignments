#pragma once

#include <JuceHeader.h>
#include <vector>

#define MIDI_NOTES 128
#define MAX_VEL 127

#define EDO 12
#define MIDI_REF_NOTE 69
#define MIDI_REF_FREQ 440

#define ENV_SEC_MIN 0.005
#define ENV_SEC_MAX 10.0

struct MonoStack // implements last-pressed note priority for note number (but not velocity)
{
	MonoStack();

	void noteOn(int); // add new note number to stack
	int noteOff(int); // remove lifted note number from stack and return last data element

	private:
		int findInData(int); // sequential search, returns 1st inst of arg val in data or -1

		std::vector<int> stackData; // MIDI note number data of the note stack
};

enum EnvState { // the different envelope states
	OFF,
	ATTACK,
	HOLD,
	RELEASE
};

struct Envelope // implements envelope modulation
{
	Envelope();

	void setSampleRate(float);
	void setParams(float, float);

	void resetState(EnvState, float); // sets start to current value
	bool isOn(); // is the envelope not in the OFF state
	float process();
	private:
		EnvState state; // determines what time variable to use, behavior, etc.
		float attack; // attack time
		float release; // release time
		
		float start; // starting position of state
		float end; // ending position of state
		float lastVal; // last-processed envelope value
		float prog; // progress through state from 0 to 1

		float sampleRate; // copy of sample rate for the envelope object to refer to
};

struct Voice // implements the audio generation for the synth
{
	Voice();

	void noteOn(int, int); // stuff to do when note is pressed
	void noteOff(int); // stuff to do when note is released
	
	void setSampleRate(double);
	void setEnvParams(float, float);
	float audioOut(); // function that actually does the audio processing
	private:
		float noteFreq(); // calculate the frequency of the note based on the note number
		float sawWave(); // calculate saw wave audio sample based on voice phase

		MonoStack stack;
		Envelope ampEnv;
		int note; // current MIDI note of voice
		float freq; // current frequency of voice, calculated from "note"
		float phase; // current phase of wave, 0 to 1
		float sampleRate; // sample rate of the synth
};
