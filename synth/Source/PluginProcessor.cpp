#include "PluginProcessor.h"

SynthAudioProcessor::SynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
		: AudioProcessor(BusesProperties()
 #if ! JucePlugin_IsMidiEffect
  #if ! JucePlugin_IsSynth
		.withInput("Input",  juce::AudioChannelSet::stereo(), true)
  #endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
 #endif
		)
#endif
{
}

SynthAudioProcessor::~SynthAudioProcessor()
{
}

const juce::String SynthAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool SynthAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool SynthAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool SynthAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double SynthAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int SynthAudioProcessor::getNumPrograms()
{
	return 1;
}

int SynthAudioProcessor::getCurrentProgram()
{
	return 0;
}

void SynthAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String SynthAudioProcessor::getProgramName(int index)
{
	return {};
}

void SynthAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

void SynthAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	voice.setSampleRate(sampleRate);
}

void SynthAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SynthAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
 #if JucePlugin_IsMidiEffect
	juce::ignoreUnused(layouts);
	return true;
 #else
	// mono and stereo layouts supported only
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
			&& layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
		return false;
  #if ! JucePlugin_IsSynth
	// input and output layout match?
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
  #endif
	return true;
 #endif
}
#endif

void SynthAudioProcessor::processBlock(
		juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	juce::ScopedNoDenormals noDenormals;
	auto totalNumInputChannels  = getTotalNumInputChannels();
	auto totalBufSamples = buffer.getNumSamples();

	for (const auto midi : midiMessages) {
		auto message = midi.getMessage();
		if (message.isNoteOn()) {
			voice.noteOn(message.getNoteNumber(), message.getVelocity());
		} else {
			voice.noteOff(message.getNoteNumber());
		}
	}

	float temp;
	for (int sample = 0; sample < totalBufSamples; ++sample) {
		temp = voice.audioOut();
		for (int channel = 0; channel < totalNumInputChannels; ++channel)
			buffer.setSample(channel, sample, temp);
	}
}

juce::AudioProcessorEditor* SynthAudioProcessor::createEditor()
{
	return new juce::GenericAudioProcessorEditor(*this);
}

bool SynthAudioProcessor::hasEditor() const
{
	return true;
}

void SynthAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
}

void SynthAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new SynthAudioProcessor();
}
