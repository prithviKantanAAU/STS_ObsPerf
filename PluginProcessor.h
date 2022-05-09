#pragma once
#include <JuceHeader.h>
#include "Main_Sensors.h"
#include "energyTest.h"
#include "Main_MovementAnalysis.h"

using namespace juce;
class dsp;
class mapUI;

class STS_ObsPerfAudioProcessor  : public juce::AudioProcessor, public HighResolutionTimer
{
public:
    STS_ObsPerfAudioProcessor();
    ~STS_ObsPerfAudioProcessor() override;
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

	bool isReady = false;
	bool isDSP_ON = true;
	float musicLevel_dB = 0;

	Main_Sensors main_Sens;
	Main_MovementAnalysis main_Mov;

	void startMusicDSP()
	{
		fDSP = new mydsp();
		fDSP->init(getSampleRate());
		fUI = new MapUI();
		fDSP->buildUserInterface(fUI);
		outputs = new float* [2];
		for (int channel = 0; channel < 2; ++channel) {
			outputs[channel] = new float[getBlockSize()];
		}
		isDSP_ON = true;
		isReady = true;
	}

	void stopMusicDSP()
	{
		if (isDSP_ON)
		{
			isReady = false;
			isDSP_ON = false;
			delete fDSP;
			delete fUI;
			for (int channel = 0; channel < 2; ++channel) {
				delete[] outputs[channel];
			}
			delete[] outputs;
		}
	}

	void set_masterGain(float faderVal)
	{
		//main_Aud.mixerSettings.masterGain = faderVal;
		//fUI->setParamValue(main_Aud.faustStrings.masterGain.toStdString().c_str(), faderVal);
	}

	void mapToFAUST()
	{
		fUI->setParamValue("SUBPARAM_blowPressure", *main_Mov.mappingSliders.at(0).mapVal);
		fUI->setParamValue("SUBPARAM_freq_Hz", *main_Mov.mappingSliders.at(1).mapVal);
		fUI->setParamValue("SUBPARAM_subAmp", *main_Mov.mappingSliders.at(2).mapVal);
		fUI->setParamValue("SUBPARAM_drive", *main_Mov.mappingSliders.at(3).mapVal);

		main_Mov.energyMeter->processInput(abs(sin(cyclesDone * 0.004)));
		fUI->setParamValue("energy", abs(sin(cyclesDone*0.004)));		
		//fUI->setParamValue("energy", main_Mov.energyCompute.CoM_Speed);		
		fUI->setParamValue("MasterGain", main_Mov.masterGain_dB);
	}

private:

	MapUI* fUI;
	dsp* fDSP;
	float** outputs;
	int timer_Interval_MS = 10;
	void hiResTimerCallback();
	double timeDone = 0;
	long cyclesDone = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (STS_ObsPerfAudioProcessor)
};
