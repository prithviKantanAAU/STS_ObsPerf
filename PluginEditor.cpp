#include "PluginProcessor.h"
#include "PluginEditor.h"

STS_ObsPerfAudioProcessorEditor::STS_ObsPerfAudioProcessorEditor (STS_ObsPerfAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize(win_Wd, win_Ht);

    tabs = std::make_unique<TabbedComponent>(TabbedButtonBar::TabsAtTop);
    addAndMakeVisible(*tabs);
    tabs->addTab("Sensors", Colours::black, Component::getParentComponent(), 1);
    tabs->addTab("Movement Mapping", Colours::black, Component::getParentComponent(), 1);
    tabs->setBounds(0, 0, getWidth(), getHeight());

    addAllElements_SENS();
    addAllElements_MOV();
    startTimerHz(ui_RefreshFreq);
}

STS_ObsPerfAudioProcessorEditor::~STS_ObsPerfAudioProcessorEditor()
{
}

void STS_ObsPerfAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (Colours::black);
    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
}

void STS_ObsPerfAudioProcessorEditor::resized()
{
    ptr_SENS->setLayout(win_Wd, win_Ht);
    ptr_MOV->setLayout(win_Wd, win_Ht);
}

void STS_ObsPerfAudioProcessorEditor::timerCallback()
{
    check_TabChange();
    update_TabElements();

    if (ptr_SENS->logHandle->rwd->isDown())
        ptr_SENS->logHandle->numSmpl_NOW = max(0, (ptr_SENS->logHandle->numSmpl_NOW - 20));

    if (ptr_SENS->logHandle->fwd->isDown())
        ptr_SENS->logHandle->numSmpl_NOW = (ptr_SENS->logHandle->numSmpl_NOW + 20) % ptr_SENS->logHandle->numSmpl_TOT;
}

void STS_ObsPerfAudioProcessorEditor::check_TabChange()
{
    tab_NOW = tabs->getCurrentTabIndex();
    if (tab_NOW != tab_Z1)
    {
        switch (tab_NOW)
        {
        case 0:
            ptr_SENS->setVisible(true);
            ptr_MOV->setVisible(false);
            break;
        case 1:
            ptr_SENS->setVisible(false);
            ptr_MOV->setVisible(true);
            break;
        }
    }
    tab_Z1 = tab_NOW;
}

void STS_ObsPerfAudioProcessorEditor::update_TabElements()
{
    switch (tab_NOW)
    {
    case 0:
        ptr_SENS->updateElements();
        break;
    case 1:
        ptr_MOV->updateElements(ptr_SENS);
        break;
    }
}

void STS_ObsPerfAudioProcessorEditor::addAllElements_SENS()
{
    // Add Sens Page Headers
    for (int i = 0; i < ptr_SENS->headers.size(); i++)
        addAndMakeVisible(ptr_SENS->headers[i]);

    // Add Individual Sensor Elements
    for (int i = 0; i < ptr_SENS->sensors.size(); i++)
    {
        addAndMakeVisible(ptr_SENS->sensors[i].LAB_sensor_Name);
        addAndMakeVisible(ptr_SENS->sensors[i].SET_IP_Address);
        addAndMakeVisible(ptr_SENS->sensors[i].SET_Port);
        addAndMakeVisible(ptr_SENS->sensors[i].LAB_sensor_Status);
        addAndMakeVisible(ptr_SENS->sensors[i].LAB_sensor_BatteryPct);
        addAndMakeVisible(ptr_SENS->sensors[i].LAB_sensor_MessagePct);
        addAndMakeVisible(ptr_SENS->sensors[i].SET_frontSide);
        addAndMakeVisible(ptr_SENS->sensors[i].SET_polarity);
        addAndMakeVisible(ptr_SENS->sensors[i].CAL_Bias);
        addAndMakeVisible(ptr_SENS->sensors[i].SET_Madgwick_Beta);
        addAndMakeVisible(ptr_SENS->sensors[i].SET_angRange_Pitch->range);
        addAndMakeVisible(ptr_SENS->sensors[i].SET_angRange_Pitch->bounds[0]);
        addAndMakeVisible(ptr_SENS->sensors[i].SET_angRange_Pitch->bounds[1]);
        addAndMakeVisible(ptr_SENS->sensors[i].SET_angRange_Pitch->indicator);
        addAndMakeVisible(ptr_SENS->sensors[i].SET_angRange_Pitch->indicator_ZERO);
        addAndMakeVisible(ptr_SENS->sensors[i].SET_angRange_Roll->range);
        addAndMakeVisible(ptr_SENS->sensors[i].SET_angRange_Roll->bounds[0]);
        addAndMakeVisible(ptr_SENS->sensors[i].SET_angRange_Roll->bounds[1]);
        addAndMakeVisible(ptr_SENS->sensors[i].SET_angRange_Roll->indicator);
        addAndMakeVisible(ptr_SENS->sensors[i].SET_angRange_Roll->indicator_ZERO);
    }

    add_UIGroup(ptr_SENS->sensorLogs);

    // SENSOR LOG HANDLING
    addAndMakeVisible(ptr_SENS->logHandle->record);
    addAndMakeVisible(ptr_SENS->logHandle->playPause);
    addAndMakeVisible(ptr_SENS->logHandle->rwd);
    addAndMakeVisible(ptr_SENS->logHandle->fwd);
    addAndMakeVisible(ptr_SENS->logHandle->load);
    addAndMakeVisible(ptr_SENS->logHandle->unload);
    addAndMakeVisible(ptr_SENS->logHandle->progBar);
    addAndMakeVisible(ptr_SENS->logHandle->smpl_Idx_Strt);
    addAndMakeVisible(ptr_SENS->logHandle->smpl_Idx_End);
}

void STS_ObsPerfAudioProcessorEditor::addAllElements_MOV()
{
    // ADD ALL UI GROUPS
    add_UIGroup(ptr_MOV->CoM_Vis);
    add_UIGroup(ptr_MOV->EnergyVis);
    add_UIGroup(ptr_MOV->MapControl);
    add_UIGroup(ptr_MOV->VideoControl);
    add_UIGroup(ptr_MOV->MiscControl);
    add_UIGroup(ptr_MOV->MasterControl);

    // VIDEO PLAYER
    addAndMakeVisible(ptr_MOV->movVideoPlayer);

    // ENERGY METER
    addAndMakeVisible(ptr_MOV->energyMeter->meter_Empty);
    addAndMakeVisible(ptr_MOV->energyMeter->meter_Full);
    addAndMakeVisible(ptr_MOV->masterLevel->meter_Empty);
    addAndMakeVisible(ptr_MOV->masterLevel->meter_Full);

    for (MappingSlider& iter : ptr_MOV->mappingSliders)
    {
        addAndMakeVisible(iter.sliderControl);
        addAndMakeVisible(iter.nameLabel);
    }
}
