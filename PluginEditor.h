#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Main_Sensors.h"
#include "Main_MovementAnalysis.h"
#include "IMU_Sensor.h"
#include "UtilityClasses.h"
using namespace juce;

class STS_ObsPerfAudioProcessorEditor  : public juce::AudioProcessorEditor, public Timer
{
public:
    STS_ObsPerfAudioProcessorEditor (STS_ObsPerfAudioProcessor&);
    ~STS_ObsPerfAudioProcessorEditor() override;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    STS_ObsPerfAudioProcessor& audioProcessor;

    std::unique_ptr<TabbedComponent> tabs;
    void addAllElements_SENS();
    void addAllElements_MOV();
    void check_TabChange();
    void update_TabElements();
    short tab_NOW = 0, tab_Z1 = -1;

    // Shortcut Pointers
    Main_Sensors* ptr_SENS = &audioProcessor.main_Sens;
    Main_MovementAnalysis* ptr_MOV = &audioProcessor.main_Mov;

    // Interface Specifics, Helper Variables
    int win_Wd = 1400;
    int win_Ht = 700;
    Justification mid = Justification::centred;

    // Timer
    int ui_RefreshFreq = 40;
    void timerCallback();

    // Helper Functions
    void add_UIGroup(UI_ControlGroup* grp)
    {
        addAndMakeVisible(grp->border_L);
        addAndMakeVisible(grp->border_R);
        addAndMakeVisible(grp->border_D);
        addAndMakeVisible(grp->border_UL);
        addAndMakeVisible(grp->border_UR);
        addAndMakeVisible(grp->groupName);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (STS_ObsPerfAudioProcessorEditor)
};
