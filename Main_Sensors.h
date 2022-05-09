#pragma once
#include <JuceHeader.h>
#include "IMU_Sensor.h"
using namespace juce;

class Main_Sensors
{
public:

	// Common UI Elements
	std::vector <Label*> headers;
	std::vector <String> header_Names;
	std::vector <float> header_ColWidths;
	float row_Ht = 0.1;
	float row_Spc = 0.05;

	bool is_SensorsInitialized = false;
	bool is_SensorRecording = false;
	bool is_SensorRecording_z1 = false;

	// Sensor Objects
	std::vector <IMU_Sensor> sensors;
	bool toLoad_Video = false;
	bool toPlay_Video = false;
	bool toPause_Video = false;
	bool toStop_Video = false;
	bool toUnload_Video = false;

	// WAV WRITING
	std::unique_ptr<juce::FileOutputStream> outStream;
	std::unique_ptr<juce::AudioFormatWriter> writer;
	int fs_AUDIO = 48000;

	// Sensor Logging
	UI_ControlGroup* sensorLogs;
	bool isLog_Playback = false;
	SensorLog_Handling* logHandle;
	float fracPos_sensorLogs[4] = { 0.01,0.9,0.98,0.09 };
	FILE* mpLog;
	FILE* offsetsLog;

	Main_Sensors()
	{
		sensorLogs = new UI_ControlGroup("Sensor Logs", Colours::navy);
		logHandle = new SensorLog_Handling;

		initializeSensors();

		init_HeaderNames_Widths();
		init_Header_Labels();

		initializeLoggingBehavior();
	}

	~Main_Sensors()
	{

	}

	void init_HeaderNames_Widths()
	{
		String* headerName = new String("Body Part");
		header_Names.push_back(*headerName);
		header_ColWidths.push_back(0.1);

		*headerName = "IP Address";
		header_Names.push_back(*headerName);
		header_ColWidths.push_back(0.1);

		*headerName = "Port";
		header_Names.push_back(*headerName);
		header_ColWidths.push_back(0.05);

		*headerName = "Status";
		header_Names.push_back(*headerName);
		header_ColWidths.push_back(0.08);

		*headerName = "Battery";
		header_Names.push_back(*headerName);
		header_ColWidths.push_back(0.08);

		*headerName = "Msg %";
		header_Names.push_back(*headerName);
		header_ColWidths.push_back(0.05);

		*headerName = "Placement";
		header_Names.push_back(*headerName);
		header_ColWidths.push_back(0.1);

		*headerName = "Pol";
		header_Names.push_back(*headerName);
		header_ColWidths.push_back(0.07);

		*headerName = "Bias Calibrate";
		header_Names.push_back(*headerName);
		header_ColWidths.push_back(0.1);

		*headerName = "Pitch/Roll Ranges";
		header_Names.push_back(*headerName);
		header_ColWidths.push_back(0.2);

		*headerName = "Beta";
		header_Names.push_back(*headerName);
		header_ColWidths.push_back(0.05);
	}

	void init_Header_Labels()
	{
		for (int i = 0; i < header_Names.size(); i++)
		{
			Label* headerTemp = new Label;
			headers.push_back(headerTemp);
			headers[i]->setJustificationType(Justification::centred);
			headers[i]->setFont(Font(22, Font::bold));
			headers[i]->setColour(headers[i]->textColourId, Colours::yellow);
			headers[i]->setText(header_Names[i], dontSendNotification);
		}
	}

	void setVisible(bool on)
	{
		for (int i = 0; i < header_Names.size(); i++)
			headers[i]->setVisible(on);

		for (int i = 0; i < sensors.size(); i++)
			sensors[i].setVisible(on);

		if (on)
		{
			sensorLogs->setVisible(on);
			logHandle->setVisible(on);
		}
	}

	void updateElements()
	{
		if (is_SensorsInitialized)
		{
			for (IMU_Sensor& iter : sensors)
			{
				iter.update_UI_Elements();
			}
		}
	}

	void setLayout(int wd, int ht)
	{
		float cumSum_Width = 0;
		for (int i = 0; i < header_Names.size(); i++)
		{
			headers[i]->setBounds(cumSum_Width*wd, 30, header_ColWidths[i]*wd , 30);
			cumSum_Width += header_ColWidths[i];
		}

		float cumSum_Height = 0;

		for (int j = 0; j < sensors.size(); j++)
		{
			cumSum_Width = 0;
			cumSum_Height += row_Ht + row_Spc;

			sensors[j].LAB_sensor_Name->setBounds(0, cumSum_Height * ht, header_ColWidths[0] * wd,row_Ht*ht);
			cumSum_Width += header_ColWidths[0];

			sensors[j].SET_IP_Address->setBounds(cumSum_Width*wd, cumSum_Height * ht, header_ColWidths[1] * wd, row_Ht * ht);
			cumSum_Width += header_ColWidths[1];

			sensors[j].SET_Port->setBounds(cumSum_Width * wd, cumSum_Height * ht, header_ColWidths[2] * wd, row_Ht * ht);
			cumSum_Width += header_ColWidths[2];

			sensors[j].LAB_sensor_Status->setBounds(cumSum_Width * wd, cumSum_Height * ht, header_ColWidths[3] * wd, row_Ht * ht);
			cumSum_Width += header_ColWidths[3];

			sensors[j].LAB_sensor_BatteryPct->setBounds(cumSum_Width * wd, cumSum_Height * ht, header_ColWidths[4] * wd, row_Ht * ht);
			cumSum_Width += header_ColWidths[4];

			sensors[j].LAB_sensor_MessagePct->setBounds(cumSum_Width * wd, cumSum_Height * ht, header_ColWidths[5] * wd, row_Ht * ht);
			cumSum_Width += header_ColWidths[5];

			sensors[j].SET_frontSide->setBounds(cumSum_Width * wd, cumSum_Height * ht, header_ColWidths[6] * wd, row_Ht * ht);
			cumSum_Width += header_ColWidths[6];

			sensors[j].SET_polarity->setBounds(cumSum_Width * wd + (header_ColWidths[7] * wd) / 2 - 8, cumSum_Height * ht, header_ColWidths[7] * wd, row_Ht * ht);
			cumSum_Width += header_ColWidths[7];

			sensors[j].CAL_Bias->setBounds(cumSum_Width * wd, cumSum_Height * ht, header_ColWidths[8] * wd, row_Ht * ht);
			cumSum_Width += header_ColWidths[8];

			sensors[j].SET_angRange_Pitch->setBounds(cumSum_Width * wd, cumSum_Height * ht, header_ColWidths[9] * wd, row_Ht/2 * ht);
			sensors[j].SET_angRange_Roll->setBounds(cumSum_Width * wd, cumSum_Height * ht + row_Ht/2*ht, header_ColWidths[9] * wd, row_Ht / 2 * ht);
			cumSum_Width += header_ColWidths[9];

			sensors[j].SET_Madgwick_Beta->setBounds(cumSum_Width * wd, cumSum_Height * ht, header_ColWidths[10] * wd, row_Ht * ht);
			cumSum_Width += header_ColWidths[10];
		}

		sensorLogs->getBoundingRec(wd, ht, fracPos_sensorLogs[0], fracPos_sensorLogs[1], fracPos_sensorLogs[2], fracPos_sensorLogs[3]);

		// TEST DYNAMIC BOUND SETTING
		sensorLogs->memberComponent_setBounds(logHandle->record, 0.02, 0.3, 0.15, 0.6);
		sensorLogs->memberComponent_setBounds(logHandle->load, 0.3, 0.3, 0.09, 0.6);
		sensorLogs->memberComponent_setBounds(logHandle->smpl_Idx_Strt, 0.3, 0.3, 0.09, 0.25);
		sensorLogs->memberComponent_setBounds(logHandle->smpl_Idx_End, 0.3, 0.6, 0.09, 0.25);
		sensorLogs->memberComponent_setBounds(logHandle->unload, 0.4, 0.3, 0.09, 0.6);
		sensorLogs->memberComponent_setBounds(logHandle->playPause, 0.5, 0.3, 0.09, 0.6);
		sensorLogs->memberComponent_setBounds(logHandle->rwd, 0.6, 0.3, 0.04, 0.6);
		sensorLogs->memberComponent_setBounds(logHandle->fwd, 0.65, 0.3, 0.04, 0.6);
		sensorLogs->memberComponent_setBounds(&logHandle->progBar, 0.7, 0.3, 0.23, 0.6);
	}

	void initializeSensors()
	{
		IMU_Sensor* sensorPtr;
		String* stringPtr;

		sensorPtr = new IMU_Sensor("Trunk",1,1,true,-45,45,-30,30);
		sensors.push_back(*sensorPtr);
		stringPtr = new String("\\Raw IMU Data - " + sensorPtr->location + ".csv");
		logHandle->addExtension(stringPtr);

		sensorPtr = new IMU_Sensor("Thigh",2,2,true,-60,60,-30,30);
		sensors.push_back(*sensorPtr);
		stringPtr = new String("\\Raw IMU Data - " + sensorPtr->location + ".csv");
		logHandle->addExtension(stringPtr);

		sensorPtr = new IMU_Sensor("Shank",3,2,true,-60,60,-30,30);
		sensors.push_back(*sensorPtr);
		stringPtr = new String("\\Raw IMU Data - " + sensorPtr->location + ".csv");
		logHandle->addExtension(stringPtr);

		// Do the same for video file
		stringPtr = new String("\\Motion Video.mov");
		logHandle->addExtension(stringPtr);

		is_SensorsInitialized = true;
	}

	void initializeLoggingBehavior()
	{
		// RECORD BUTTON
		logHandle->record->setColour(logHandle->record->buttonColourId, Colours::red);
		logHandle->record->setButtonText("Record Session");
		logHandle->record->onClick = [this]
		{
			if (!is_SensorRecording)
			{
				// GENERATE PATH AND DIRECTORY FOR LOG STORAGE
				String rec_LogPath = File::getSpecialLocation(File::currentApplicationFile).getFullPathName();
				rec_LogPath = rec_LogPath.upToLastOccurrenceOf("\\", true, false);
				rec_LogPath += "MP Log - " + getCurrentTime() + "\\";
				CreateDirectory(rec_LogPath.toStdString().c_str(), NULL);

				for (IMU_Sensor& iter : sensors)
				{
					if (iter.oscObj->isOnline)					//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ADD WHEN TESTING COMPLETE
					iter.rec_InitFile(rec_LogPath);
				}

				String path_mpLog = rec_LogPath + "Movement Feature Log.csv";
				mpLog = fopen(path_mpLog.toStdString().c_str(), "w");

				write_offsetsLog(rec_LogPath);

				// CREATE WAV FILE FOR WRITING
				String wavPath_Rec_FULL = rec_LogPath + "Audio Output.wav";
				File wavFile(wavPath_Rec_FULL);
				outStream = wavFile.createOutputStream();
				juce::WavAudioFormat format;
				writer.reset(format.createWriterFor(outStream.get(), fs_AUDIO, 2, 32, {}, 0));

				is_SensorRecording = true;
				logHandle->record->setColour(logHandle->record->buttonColourId, Colours::blue);
				logHandle->record->setButtonText("Stop Recording");
			}

			else
			{
				is_SensorRecording = false;
				outStream.release();
				logHandle->record->setColour(logHandle->record->buttonColourId, Colours::red);
				logHandle->record->setButtonText("Record Sensors");
			}
		};

		// LOAD
		logHandle->load->setColour(logHandle->load->buttonColourId, Colours::green);
		logHandle->load->setButtonText("Load Log");
		logHandle->load->onClick = [this]
		{
			// CONFIGURE BEHAVIOR !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			String filePath = "";
			String extension = ".csv";

			FileChooser logChooser("Please select a valid log folder.", File::getSpecialLocation(File::currentApplicationFile), extension);
			if (logChooser.browseForDirectory())
			{
				File logFile(logChooser.getResult());
				filePath = logFile.getFullPathName();

				if (logHandle->loadSensorLogs_FromDir(filePath))
				{
					logHandle->mode = 2;
					logHandle->setVisible(true);
					toLoad_Video = true;
				}
				load_sensorOffsets(logHandle->offsetsVector);
			}
		};

		// UNLOAD
		logHandle->unload->setColour(logHandle->unload->buttonColourId, Colours::red);
		logHandle->unload->setButtonText("Unload Log");
		logHandle->unload->onClick = [this]
		{
			toUnload_Video = true;
			logHandle->flushAllVectors();
			logHandle->mode = 1;
			logHandle->setVisible(true);
		};

		// PLAY / PAUSE
		logHandle->playPause->setColour(logHandle->playPause->buttonColourId, Colours::blue);
		logHandle->playPause->setButtonText("Play Log");
		logHandle->playPause->onClick = [this]
		{
			// CONFIGURE BEHAVIOR !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			if (logHandle->mode == 2)
			{
				toPlay_Video = true;
				logHandle->mode = 3;
				logHandle->playPause->setColour(logHandle->playPause->buttonColourId, Colours::red);
				logHandle->playPause->setButtonText("Pause Log");
				logHandle->setVisible(true);
			}

			else if (logHandle->mode == 3)
			{
				toPause_Video = true;
				logHandle->mode = 2;
				logHandle->playPause->setColour(logHandle->playPause->buttonColourId, Colours::blue);
				logHandle->playPause->setButtonText("Play Log");
				logHandle->setVisible(true);
			}
		};

		// RWD
		logHandle->rwd->setColour(logHandle->rwd->buttonColourId, Colours::orange);
		logHandle->rwd->setButtonText("RWD");
		logHandle->rwd->onClick = [this]
		{
			// CONFIGURE BEHAVIOR !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		};

		// FWD
		logHandle->fwd->setColour(logHandle->fwd->buttonColourId, Colours::orange);
		logHandle->fwd->setButtonText("FWD");
		logHandle->fwd->onClick = [this]
		{
			// CONFIGURE BEHAVIOR !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		};

		// PROGBAR
		logHandle->progBar.setColour(logHandle->progBar.foregroundColourId, Colours::yellow);
		logHandle->progBar.setColour(logHandle->progBar.backgroundColourId, Colours::blue);
		logHandle->progBar.setPercentageDisplay(false);
	}

	void write_offsetsLog(String rec_LogPath)
	{
		String path_offsetsLog = rec_LogPath + "Sensor Offsets Log.csv";
		offsetsLog = fopen(path_offsetsLog.toStdString().c_str(), "w");

		std::string format = "%s,\n";

		String presentLine = "";
		for (IMU_Sensor& iter : sensors)
		{
			presentLine = "";
			presentLine = iter.location + "," + String(iter.SET_angRange_Pitch->offset) + "," +
				String(iter.SET_angRange_Roll->offset);

			fprintf(offsetsLog, format.c_str(), presentLine);
		}

		fclose(offsetsLog);
	}

	void load_sensorOffsets(std::vector<String> offsetStrings)
	{
		String lineRem = "";
		String sensorLocation = "";
		float offset_Pitch;
		float offset_Roll;

		for (String& iter : offsetStrings)
		{
			sensorLocation = iter.upToFirstOccurrenceOf(",", false, true);
			lineRem = iter.fromFirstOccurrenceOf(",", false, true);
			offset_Pitch = lineRem.upToFirstOccurrenceOf(",", false, true).getDoubleValue();
			lineRem = lineRem.fromFirstOccurrenceOf(",", false, true);
			offset_Roll = lineRem.upToFirstOccurrenceOf(",", false, true).getDoubleValue();

			for (IMU_Sensor& sens : sensors)
			{
				if (sensorLocation == sens.location)
				{
					sens.SET_angRange_Pitch->range->setValue(offset_Pitch);
					sens.SET_angRange_Roll->range->setValue(offset_Roll);
				}
			}
		}
	}

	void callback()
	{
		if (is_SensorsInitialized)
		{
			bool recStopped = is_SensorRecording_z1 && (!is_SensorRecording);
			isLog_Playback = logHandle->mode == logHandle->FILE_LOADED;
			String logRow = "";

			for (IMU_Sensor& iter : sensors)
			{
				if (isLog_Playback)
				{
					/*switch (logHandle->numSmpl_NOW)
					{
					case 50:		iter.madgwickAHRS->beta = 0.6; break;
					case 500:		iter.madgwickAHRS->beta = 0.6; break;
					}*/

					logRow = logHandle->streamSensorLog_Line(iter.location);
				}

				iter.callback(is_SensorRecording, recStopped, isLog_Playback,logRow);
			}

			if (recStopped)
				fclose(mpLog);

			if (isLog_Playback)
			{
				logHandle->incrementLogIdx();
			}
			is_SensorRecording_z1 = is_SensorRecording;
		}
	}

	void updateSensorStatus()
	{
		if (is_SensorsInitialized)
		{
			for (IMU_Sensor& iter : sensors)
				iter.oscObj->isSensorActive();
		}
	}

	String getCurrentTime()
	{
		time_t rawtime;
		struct tm* timeinfo;
		char buffer[80];

		time(&rawtime);
		timeinfo = localtime(&rawtime);

		strftime(buffer, sizeof(buffer), "%d-%m-%Y %H-%M-%S", timeinfo);
		std::string str(buffer);

		return String(str);
	}
};
