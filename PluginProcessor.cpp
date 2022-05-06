#include "PluginProcessor.h"
#include "PluginEditor.h"

STS_ObsPerfAudioProcessor::STS_ObsPerfAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    startTimer(timer_Interval_MS);
}

STS_ObsPerfAudioProcessor::~STS_ObsPerfAudioProcessor()
{
}

void STS_ObsPerfAudioProcessor::hiResTimerCallback()
{
    // RUN CALLBACKS
    main_Sens.callback();
    main_Mov.callback(&main_Sens, main_Sens.is_SensorRecording, main_Sens.mpLog);

    // MAP TO FAUST
    if (isReady) mapToFAUST();

    // UPDATE TIME AND CHECK SENSOR STATUS
    timeDone += (double)timer_Interval_MS / 1000;
    cyclesDone++;
    if (cyclesDone % 200 == 0)
        main_Sens.updateSensorStatus();
}

const juce::String STS_ObsPerfAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool STS_ObsPerfAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool STS_ObsPerfAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool STS_ObsPerfAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double STS_ObsPerfAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int STS_ObsPerfAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int STS_ObsPerfAudioProcessor::getCurrentProgram()
{
    return 0;
}

void STS_ObsPerfAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String STS_ObsPerfAudioProcessor::getProgramName (int index)
{
    return {};
}

void STS_ObsPerfAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void STS_ObsPerfAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    fDSP = new mydsp();
    fDSP->init(sampleRate);
    fUI = new MapUI();
    fDSP->buildUserInterface(fUI);
    outputs = new float* [2];
    for (int channel = 0; channel < 2; ++channel) {
        outputs[channel] = new float[samplesPerBlock];
    }
    main_Sens.fs_AUDIO = getSampleRate();
    isReady = true;
}

void STS_ObsPerfAudioProcessor::releaseResources()
{
    if (isDSP_ON)
        stopMusicDSP();
}


#ifndef JucePlugin_PreferredChannelConfigurations
bool STS_ObsPerfAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void STS_ObsPerfAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    if (isReady)
    {
        fDSP->compute(buffer.getNumSamples(), NULL, outputs);                           // GET MUSIC FROM FAUST

        for (int channel = 0; channel < totalNumOutputChannels; ++channel) {
            for (int i = 0; i < buffer.getNumSamples(); i++) {

                *buffer.getWritePointer(channel, i) = outputs[channel][i]; //+ outputVoice; // SUM DATA
            }

            // GET DB LEVEL FOR METERING
            musicLevel_dB = fmax(-40, Decibels::gainToDecibels(buffer.getMagnitude(0, 0, buffer.getNumSamples())));
            main_Mov.masterLevel->processInput(musicLevel_dB);
        }

        if (main_Sens.is_SensorRecording)
        {
            main_Sens.writer->flush();
            main_Sens.writer->writeFromAudioSampleBuffer(buffer, 0, buffer.getNumSamples());
        }
    }
}

bool STS_ObsPerfAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* STS_ObsPerfAudioProcessor::createEditor()
{
    return new STS_ObsPerfAudioProcessorEditor (*this);
}

void STS_ObsPerfAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
}

void STS_ObsPerfAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new STS_ObsPerfAudioProcessor();
}
