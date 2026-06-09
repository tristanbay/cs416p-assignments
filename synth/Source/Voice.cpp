#include "Voice.h"
#include <vector>
#include <cmath>

// -- Voice functions --

Voice::Voice(): note(MIDI_REF_NOTE), vel(DEFAULT_VEL), freq(MIDI_REF_FREQ), phase(0.0)
{ // stack and ampEnv are automatically constructed
}

void Voice::noteOn(int num, int vl)
{
	if (num != note) {
		note = num;
		freq = noteFreq();
	}
	vel = vl;
	stack.noteOn(num);
	isOn = 1;
}

void Voice::noteOff(int num)
{
	int newNote = stack.noteOff(num);
	if (num == newNote)
		isOn = 0;
	note = newNote;
	freq = noteFreq();
}
	
void Voice::setSampleRate(double rate)
{
	sampleRate = (float)rate;
}

float Voice::audioOut()
{
	float out = sawWave() * vel / MAX_VEL * isOn;
	if (isOn != 0) {
		phase += 1.0 / (sampleRate / freq);
		if (phase >= 1.0)
			phase -= 1.0;
	}
	return out;
}

float Voice::noteFreq() // private
{
	return MIDI_REF_FREQ * pow(2, (double)(note - MIDI_REF_NOTE) / EDO);
}

float Voice::sawWave()
{
	return (phase - 0.5) * 2;
}

// -- MonoStack functions --

MonoStack::MonoStack(): stackData({})
{
}

void MonoStack::noteOn(int num)
{
	stackData.push_back(num);
}

int MonoStack::noteOff(int num)
{
	int temp = findInData(num);
	stackData.erase(stackData.begin() + temp);
	if (stackData.empty())
		return num;
	return stackData.back();
}

int MonoStack::findInData(int num) // private
{
	int out = -1;
	for (int i = 0; (i < stackData.size()) && (out == -1); ++i)
		if (stackData[i] == num)
			out = i;
	return out;
}

// -- Envelope functions --

Envelope::Envelope(): stage(0)
{
}
