#include "Voice.h"

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

Envelope::Envelope(): state(OFF), attack(ENV_SEC_MIN), release(ENV_SEC_MIN),
		start(0), end(0), prog(0)
{
}

void Envelope::setSampleRate(float rate)
{
	sampleRate = rate;
}

void Envelope::setParams(float att, float rel)
{
	attack = att;
	release = rel;
}

void Envelope::resetState(EnvState st, float newEnd)
{
	state = st;
	prog = 0;
	start = lastVal; // this freezes the value of the envelope for 1 sample
	end = newEnd;
}

bool Envelope::isOn()
{
	return state != OFF;
}

float Envelope::process()
{
	lastVal = (end - start) * prog + start;
	switch (state) {
		case ATTACK:
			prog += 1.0 / (attack * sampleRate);
			if (prog >= 1.0) {
				prog = 1.0;
				state = HOLD;
				start = end;
			}
			break;
		case RELEASE:
			prog += 1.0 / (release * sampleRate);
			if (prog >= 1.0) {
				prog = 1.0;
				state = OFF;
				start = 0;
				end = 0;
			}
			break;
		default:
			break;
	}
	return lastVal;
}

// -- Voice functions --

Voice::Voice(): note(MIDI_REF_NOTE), freq(MIDI_REF_FREQ), phase(0.0)
{ // stack and ampEnv are automatically constructed
}

void Voice::noteOn(int num, int vel)
{
	if (num != note) {
		note = num;
		freq = noteFreq();
	}
	stack.noteOn(num);
	ampEnv.resetState(ATTACK, (float)vel / MAX_VEL);
}

void Voice::noteOff(int num)
{
	int newNote = stack.noteOff(num);
	if (num == newNote)
		ampEnv.resetState(RELEASE, 0);	
	note = newNote;
	freq = noteFreq();
}
	
void Voice::setSampleRate(double rate)
{
	sampleRate = rate;
	ampEnv.setSampleRate(rate);
}

void Voice::setEnvParams(float att, float rel)
{
	ampEnv.setParams(att, rel);
}

float Voice::audioOut()
{
	float out = sawWave() * ampEnv.process();
	if (ampEnv.isOn()) {
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
