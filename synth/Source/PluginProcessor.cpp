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
	addParameter(ampAttack = new juce::AudioParameterFloat(
				"ampAttack", "Amplitude Attack",
				juce::NormalisableRange<float>(ENV_SEC_MIN, ENV_SEC_MAX), ENV_SEC_MIN));
	addParameter(ampRelease = new juce::AudioParameterFloat(
				"ampRelease", "Amplitude Release",
				juce::NormalisableRange<float>(ENV_SEC_MIN, ENV_SEC_MAX), ENV_SEC_MIN));
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
	voice.setEnvParams(*ampAttack, *ampRelease);
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

	for (const auto midi : midiMessages) { // deal with incoming MIDI
		auto message = midi.getMessage();
		if (message.isNoteOn()) {
			voice.noteOn(message.getNoteNumber(), message.getVelocity());
		} else {
			voice.noteOff(message.getNoteNumber());
		}
	}

	voice.setEnvParams(*ampAttack, *ampRelease); // update internal envelope parameters

	float temp;
	for (int sample = 0; sample < totalBufSamples; ++sample) { // process audio
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
	juce::XmlElement* stateInfo(new juce::XmlElement("synth"));
	stateInfo->setAttribute("ampAttack", (double)*ampAttack);
	stateInfo->setAttribute("ampRelease", (double)*ampRelease);
	copyXmlToBinary(*stateInfo, destData);
}

void SynthAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	std::unique_ptr<juce::XmlElement> stateInfo(getXmlFromBinary(data, sizeInBytes));
	if (stateInfo.get() != nullptr) {
		if (stateInfo->hasTagName("synth")) {
			*ampAttack = (float)stateInfo->getDoubleAttribute("ampAttack", ENV_SEC_MIN);
			*ampRelease = (float)stateInfo->getDoubleAttribute("ampRelease", ENV_SEC_MIN);
		}
	}
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new SynthAudioProcessor();
}
