#pragma once
#include <JuceHeader.h>
#include "UtilityClasses.h"
#include "Main_Sensors.h"
#include "OSC_MobileControl.h"
using namespace juce;

class Main_MovementAnalysis
{
public:

	short MP_num = 0;
	std::vector <MovementFeature> movementFeatures;
	std::string mpLog_Format = "%s,\n";
	unsigned long lines_written = 0;
	OSC_MobileControl oscControl;
	EnergyCompute energyCompute;
	CoM_Info_Display* comDisplay;

	// Common UI Elements
	UI_ControlGroup* CoM_Vis;
	UI_ControlGroup* EnergyVis;
	UI_ControlGroup* MapControl;
	UI_ControlGroup* MasterControl;
	UI_ControlGroup* VideoControl;
	UI_ControlGroup* MiscControl;

	// Mapping Sliders
	std::vector<MappingSlider> mappingSliders;

	// Master Fader
	float masterGain_dB = 0;
	Slider* masterGain;

	// Meters
	Norm_Denorm_Metering* energyMeter;
	Norm_Denorm_Metering* masterLevel;	

	short sensorIdx_per_bodyPart[10] = {0,0,0,0,0,0,0,0,0,0}; 
	// 0 = TRUNK // 1 = THIGH // 2 = SHANK

	bool is_First_Callback_Complete = false;
	void initialize_sensorIdx_bodyPart(Main_Sensors *sensObj)
	{
		for (int i = 0; i < sensObj->sensors.size(); i++)
		{
			if (sensObj->sensors[i].location == "Trunk")
				sensorIdx_per_bodyPart[0] = i;

			if (sensObj->sensors[i].location == "Thigh")
				sensorIdx_per_bodyPart[1] = i;

			if (sensObj->sensors[i].location == "Shank")
				sensorIdx_per_bodyPart[3] = i;
		}
	}

	VideoComponent* movVideoPlayer;

	Main_MovementAnalysis()
	{	
		// CREATE USER CONTROL GROUPS
		CoM_Vis = new UI_ControlGroup("CoM", Colours::blue);
		EnergyVis = new UI_ControlGroup("Energy Meter", Colours::blue);
		MapControl = new UI_ControlGroup("Sound Mapping", Colours::red);
		MasterControl = new UI_ControlGroup("Master Volume", Colours::green);
		VideoControl = new UI_ControlGroup("Video", Colours::green);
		MiscControl = new UI_ControlGroup("Movement", Colours::green);
		comDisplay = new CoM_Info_Display(CoM_Vis);

		// CREATE MAPPING SLIDERS
		MappingSlider* sliderPtr = new MappingSlider("Control 1", 0, 0.5);
		mappingSliders.push_back(*sliderPtr);
		sliderPtr = new MappingSlider("Control 2", 0, 1200);
		mappingSliders.push_back(*sliderPtr);
		sliderPtr = new MappingSlider("Control 3", 0, 2);
		mappingSliders.push_back(*sliderPtr);
		sliderPtr = new MappingSlider("Control 4", 0, 0.5);
		mappingSliders.push_back(*sliderPtr);

		// Master Gain
		masterGain = new Slider;
		masterGain->setRange(-40, 0);
		masterGain->setColour(masterGain->backgroundColourId, Colours::blue);
		masterGain->setColour(masterGain->trackColourId, Colours::yellow);
		masterGain->setTextBoxStyle(Slider::NoTextBox,true,0,0);
		masterGain->onValueChange = [this]
		{
			masterGain_dB = masterGain->getValue();
		};

		// CREATE VIDEO COMPONENT
		movVideoPlayer = new VideoComponent(true);
		movVideoPlayer->setAudioVolume(0.0);

		// METERS
		energyMeter = new Norm_Denorm_Metering("Energy", 1, 0, 1);
		masterLevel = new Norm_Denorm_Metering("Master Level", 1, -100, 0);

		// INITIALIZE MOVEMENT FEATURES
		MovementFeature* featurePtr;

		// CONTINUOUS VARIABLES
		featurePtr = new MovementFeature("Ang_AP_Shank", -90, 90, 0, false);
		movementFeatures.push_back(*featurePtr);

		featurePtr = new MovementFeature("Ang_AP_Thigh", -90, 90, 0, false);
		movementFeatures.push_back(*featurePtr);

		featurePtr = new MovementFeature("Ang_AP_Trunk", -90, 90, 0, false);
		movementFeatures.push_back(*featurePtr);

		featurePtr = new MovementFeature("CoM_Coord_H", -1, 1, 0, false);
		movementFeatures.push_back(*featurePtr);

		featurePtr = new MovementFeature("CoM_Coord_V", 0, 1, 0, false);
		movementFeatures.push_back(*featurePtr);

		featurePtr = new MovementFeature("CoM_Speed", 0, 15, 0, false);
		movementFeatures.push_back(*featurePtr);

	}

	~Main_MovementAnalysis()
	{

	}

	void callback(Main_Sensors *sensObj, bool isSensor_Recording, FILE* mpLogFile)
	{
		calcMovParams(sensObj);
		updateVisualizerData(sensObj);
		handleLogging(isSensor_Recording,mpLogFile);
	}

	void handleExternalController()
	{
		for (int i = 0; i < 4; i++)
		{
			if (oscControl.message_toHandle[i])
			{
				mappingSliders.at(i).sliderControl->setValue(oscControl.oscDataArray[i]);
				oscControl.message_toHandle[i] = false;
			}
		}
	}

	void updateVisualizerData(Main_Sensors* sensObj)
	{		
		
	}

	void updateVisualizers(Main_Sensors* sensObj)
	{
		// CHECK IF ONLINE OR LOG PLAYING
		
	}

	void setVisible(bool on)
	{
		// UI GROUPS
		CoM_Vis->setVisible(on);
		EnergyVis->setVisible(on);
		MapControl->setVisible(on);
		MasterControl->setVisible(on);
		VideoControl->setVisible(on);
		MiscControl->setVisible(on);

		// INDIVIDUAL MEMBER COMPONENTS
		movVideoPlayer->setVisible(on);

		// METERS
		energyMeter->setVisible(on);
		masterLevel->setVisible(on);
		masterGain->setVisible(on);

		// CoM
		comDisplay->CoM_H->setVisible(on);
		comDisplay->CoM_V->setVisible(on);
		comDisplay->CoM_Speed->setVisible(on);

		for (MappingSlider& iter : mappingSliders)			iter.setVisible(on);
;	}

	void handleVideoPlayback(Main_Sensors* sensObj)
	{
		if (sensObj->toLoad_Video)
		{
			auto result = movVideoPlayer->load(File(sensObj->logHandle->videoURL));
			sensObj->toLoad_Video = false;
		}

		if (sensObj->toPlay_Video)
		{
			if (movVideoPlayer->isVideoOpen())
			{
				movVideoPlayer->play();
				sensObj->toPlay_Video = false;
			}
		}

		if (sensObj->toPause_Video)
		{
			if (movVideoPlayer->isVideoOpen())
			{
				movVideoPlayer->stop();
				sensObj->toPause_Video = false;
			}
		}

		if (sensObj->toStop_Video)
		{
			if (movVideoPlayer->isVideoOpen())
			{
				movVideoPlayer->stop();
				sensObj->toStop_Video = false;
			}
		}


		if (sensObj->toUnload_Video)
		{
			if (movVideoPlayer->isVideoOpen())
			{
				movVideoPlayer->closeVideo();
				sensObj->toUnload_Video = false;
			}
		}

		if (!movVideoPlayer->isPlaying() && (sensObj->logHandle->mode == sensObj->logHandle->FILE_LOADED))
		{
			sensObj->logHandle->playPause->triggerClick();
			sensObj->logHandle->numSmpl_NOW = 0;			
		}
	}

	void updateElements(Main_Sensors* sensObj)
	{
		handleExternalController();
		handleVideoPlayback(sensObj);

		// PUT FIRST TIME FUNCTIONS HERE
		if (!is_First_Callback_Complete)
		{
			initialize_sensorIdx_bodyPart(sensObj);
			is_First_Callback_Complete = true;
		}

		//energyMeter->processInput(energyCompute.CoM_Speed);
		masterLevel->updateMeter();
		energyMeter->updateMeter();
		comDisplay->update(energyCompute.CoM_Coord_H, energyCompute.CoM_Coord_V, energyCompute.CoM_Speed);
		//updateVisualizers(sensObj);
	}

	void setLayout(int wd, int ht)
	{
		CoM_Vis->getBoundingRec(wd, ht, 0.03, 0.08, 0.19, 0.3);
		EnergyVis->getBoundingRec(wd, ht, 0.25, 0.08, 0.3, 0.1);
		MiscControl->getBoundingRec(wd, ht, 0.25, 0.21, 0.3, 0.17);
		MapControl->getBoundingRec(wd, ht, 0.03, 0.42, 0.52, 0.4);
		VideoControl->getBoundingRec(wd, ht, 0.6, 0.08, 0.33, 0.6);
		MasterControl->getBoundingRec(wd, ht, 0.6, 0.73, 0.33, 0.09);

		VideoControl->memberComponent_setBounds(movVideoPlayer, 0.05, 0.05, 0.9, 0.9);

		energyMeter->setLayout(EnergyVis, 0, 0.25, 1, 0.9);
		masterLevel->setLayout(MasterControl, 0.5, 0.25, 0.45, 0.9);
		MasterControl->memberComponent_setBounds(masterGain, 0.1, 0.25, 0.35, 0.5);

		CoM_Vis->memberComponent_setBounds(comDisplay->CoM_H, 0.05, 0.0, 0.9, 0.33);
		CoM_Vis->memberComponent_setBounds(comDisplay->CoM_V, 0.05, 0.33, 0.9, 0.33);
		CoM_Vis->memberComponent_setBounds(comDisplay->CoM_Speed, 0.05, 0.66, 0.9, 0.33);

		int numMappingSliders = mappingSliders.size();
		float gap_Sliders_Horiz = 0.9 / (float)numMappingSliders;
		int i = 0;
		for (MappingSlider& iter : mappingSliders)
		{
			iter.setBounds(MapControl, 0.05 + i * gap_Sliders_Horiz, 0.1, gap_Sliders_Horiz, 0.9);			i++;
		}
	}

	void setValue_MovFeature(String featureName, double val)
	{
		for (MovementFeature& iter : movementFeatures)
		{
			if (iter.mpName == featureName)
				iter.storeValue(val);
		}
	}

	double getValue_MovFeature(String featureName)
	{
		for (MovementFeature& iter : movementFeatures)
		{
			if (iter.mpName == featureName)
				return iter.val;
		}
	}

	bool getStatus(Main_Sensors* sensObj, String loc)
	{
		for (IMU_Sensor& iter : sensObj->sensors)
		{
			if (iter.location == loc)
			{
				return iter.oscObj->isOnline;
			}
		}

		return false;
	}

	void calcMovParams(Main_Sensors* sensObj)
	{
		float pitch_Trunk = *sensObj->sensors.at(0).ang_Pitch;
		float pitch_Thigh = *sensObj->sensors.at(1).ang_Pitch;
		float pitch_Shank = *sensObj->sensors.at(2).ang_Pitch;

		energyCompute.calcCoMCoordinates(pitch_Trunk, pitch_Thigh, pitch_Shank);

		setValue_MovFeature("CoM_Coord_H", energyCompute.CoM_Coord_H);
		setValue_MovFeature("CoM_Coord_V", energyCompute.CoM_Coord_V);
		setValue_MovFeature("CoM_Speed", energyCompute.CoM_Speed);
	}

	void setAngle(Main_Sensors* sensObj, String loc, short axis, float value)
	{
		for (IMU_Sensor& iter : sensObj->sensors)
		{
			if (iter.location == loc)
			{
				switch (axis)
				{
				case 1:
					iter.ang_Pitch = &value;
					break;
				case 2:
					iter.ang_Roll = &value;
					break;
				default:
					break;
				}
			}
		}
	}

	float getAngle(Main_Sensors *sensObj, String loc, short axis) // 1 = pitch, 2 = roll
	{
		float *returnVal = 0;

		for (IMU_Sensor& iter : sensObj->sensors)
		{
			if (iter.location == loc)
			{
				switch (axis)
				{
				case 1:
					returnVal = iter.ang_Pitch;
					return *returnVal;
					break;
				case 2:
					returnVal = iter.ang_Roll;
					return *returnVal;
					break;
				default:
					return 0;
					break;
				}
			}
		}
	}

	void handleLogging(bool isSensor_Recording, FILE* mpLogFile)
	{
		String line = "";

		if (isSensor_Recording)
		{
			if (lines_written == 0)
			{
				for (MovementFeature& iter : movementFeatures)
					line += iter.mpName + ",";
			}

			else
			{
				for (MovementFeature& iter : movementFeatures)
					line += String(iter.val, 3) + ",";
			}

			// ADD INFO ABOUT SLIDER VALUES!

			fprintf(mpLogFile, mpLog_Format.c_str(), line);
			lines_written++;
		}

		else lines_written = 0;
	}
};


