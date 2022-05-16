	#pragma once
#include <JuceHeader.h>
#include "OSC_Class.h"
#include "quaternionFilters.h"
#include "UtilityClasses.h"
using namespace juce;

#define M_PI           3.14159265358979323846  /* pi */
#define RAD_TO_DEG		180 / M_PI
#define DEG_TO_RAD		M_PI / 180.0

class IMU_Sensor : ComboBox::Listener
{
public:

	long callbackCount = 0;

	// LOCATION, POLARITY
	String location = "";
	bool* isPol_POS = new bool(true);
	short* isFront_Side = new short(1);									// 1 = FRONT, 2 = SIDE

	// ORIENTATION ANGLE VALUES
	float angRange_Pitch[2] = { -90, 90 }, angRange_Roll[2] = { -90, 90 };
	float* ang_Pitch = new float(0); 
	float* ang_Roll = new float(0);
	float* ang_Yaw = new float(0);

	// MADGWICK FILTER
	QuaternionFilter* madgwickAHRS;

	// OSC RECEIVER OBJECT
	OSCReceiverUDP_Sensor* oscObj;

	// UI ELEMENTS - Connection
	Label* LAB_sensor_Name;
	TextEditor* SET_IP_Address;
	ComboBox* SET_Port;
	Label* LAB_sensor_Status;
	Label* LAB_sensor_BatteryPct;
	Label* LAB_sensor_MessagePct;
	
	// UI ELEMENTS - Sensor Behavior
	ComboBox* SET_frontSide;
	ToggleButton* SET_polarity;
	
	// CALIBRATION
	TextButton* CAL_Bias;
	float* Cal_Bias_TimeSec = new float(0);

	MovementRangeSlider_wOFFSET* SET_angRange_Pitch;
	MovementRangeSlider_wOFFSET* SET_angRange_Roll;
	Slider* SET_Madgwick_Beta;

	// LOGGING
	FILE* rawLog;
	std::string rawLog_Format = "%s,\n";

	IMU_Sensor(String loc, short portId, short frontSide, bool isPol_POSITIVE, 
		float pitch_MIN, float pitch_MAX, float roll_MIN, float roll_MAX)
	{
		oscObj = new OSCReceiverUDP_Sensor;
		madgwickAHRS = new QuaternionFilter;

		location = loc;
		UI_Init_LookFeel();
		UI_Init_Behavior();

		SET_Port->setSelectedId(portId);
		SET_frontSide->setSelectedId(frontSide);
		SET_angRange_Pitch->range->setMinAndMaxValues(pitch_MIN, pitch_MAX, sendNotificationSync);
		SET_angRange_Roll->range->setMinAndMaxValues(roll_MIN, roll_MAX, sendNotificationSync);
		SET_polarity->setToggleState(isPol_POSITIVE,sendNotificationSync);
	}

	~IMU_Sensor()
	{

	}

	void ipVerify_AssignedSensor()
	{
		bool toSend = false; callbackCount++; if (callbackCount % 10 == 0) toSend = true;
		if (SET_Port->getSelectedId() != 6 && !oscObj->isOnline && toSend)		// ONLY IF ASSIGNED
			{
				if (oscObj->connectionVerify.connect(oscObj->ipAddress, oscObj->portNumber_Remote))
					{
						OSCMessage message("/CONNECT");
						message.addString("Please Connect");
						oscObj->connectionVerify.send(message);
					}
			}
	}

	void callback(bool isRecording_MOV, bool isRecording_STOPPED, bool isLog_Playback, String logRow)
	{
		// CHECK IF TRYING TO CONNECT
		ipVerify_AssignedSensor();
		
		 if (oscObj->isOnline)													//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ADD WHEN TESTING DONE
		{
			 // CHECK IF CALIBRATING
			 if (oscObj->isCalibrating)
			 {
				 *Cal_Bias_TimeSec += 0.01;
				 if (*Cal_Bias_TimeSec >= 10)
				 {
					 oscObj->isCalibrating = false;
					 oscObj->isCalibrated = true;
					 *Cal_Bias_TimeSec = 0;
				 }
			 }

			// HANDLE WRITING TO RESPECTIVE RAW LOG FILE
			if (isRecording_MOV) rec_WriteLine();

			if (isRecording_STOPPED) 
				fclose(rawLog);
		}
		 
		 // IF OFFLINE, CHECK IF LOGGING IS ENABLED
		 else if (isLog_Playback)
		 {
			// INJECT RESPECTIVE STRING INTO OSC
			 oscObj->get_IMUValues_from_LogRow(logRow);
		 }

		 // UPDATE OSC BUFFERS
		 oscObj->updateBuffers();

		 *ang_Pitch = 0;
		 *ang_Roll = 0;

		 // ANGULAR ORIENTATION CALCULATION
		 if (oscObj->isOnline || isLog_Playback)
		 getOrientation_Quaternion(oscObj->acc_Buf, oscObj->gyr_Buf, oscObj->mag_Buf, madgwickAHRS,
			 (*isFront_Side == 1) ? ang_Pitch : ang_Roll,
			 (*isFront_Side == 1) ? ang_Roll : ang_Pitch,
			 ang_Yaw);

		 *ang_Pitch += SET_angRange_Pitch->offset;
		 *ang_Roll += SET_angRange_Roll->offset;

		 applyOrientationBounds();
	}

	void getOrientation_Quaternion(float* accBuf, float* gyrBuf,
		float* magBuf, QuaternionFilter* qFilt, float* pitch, float* roll, float* yaw)
	{
		qFilt->MadgwickAHRSupdateIMU(gyrBuf[0] * DEG_TO_RAD, gyrBuf[1] * DEG_TO_RAD, gyrBuf[2] * DEG_TO_RAD,
			accBuf[0], accBuf[1], accBuf[2]);

		*yaw = atan2(2.0f * (*(qFilt->getQ() + 1) * *(qFilt->getQ() + 2)
			+ *qFilt->getQ() * *(qFilt->getQ() + 3)), *qFilt->getQ() *
			*qFilt->getQ() + *(qFilt->getQ() + 1) * *(qFilt->getQ() + 1)
			- *(qFilt->getQ() + 2) * *(qFilt->getQ() + 2) - *(qFilt->getQ() + 3)
			* *(qFilt->getQ() + 3));

		*roll = -asin(2.0f * (*(qFilt->getQ() + 1) * *(qFilt->getQ() + 3)
			- *qFilt->getQ() * *(qFilt->getQ() + 2)));

		*pitch = atan2(2.0f * (*qFilt->getQ() * *(qFilt->getQ() + 1) +
			*(qFilt->getQ() + 2) * *(qFilt->getQ() + 3)), *qFilt->getQ() *
			*qFilt->getQ() - *(qFilt->getQ() + 1) * *(qFilt->getQ() + 1)
			- *(qFilt->getQ() + 2) * *(qFilt->getQ() + 2) + *(qFilt->getQ() + 3)
			* *(qFilt->getQ() + 3));

		*roll *= RAD_TO_DEG;	*yaw *= RAD_TO_DEG;		*yaw -= 8.5;	*pitch *= RAD_TO_DEG;	*pitch -= 90;
		*roll = isnan(*roll) ? 0 : *roll;
		*pitch = isnan(*pitch) ? 0 : *pitch;

		*yaw = isnan(*yaw) ? 0 : *yaw;

		*roll = *isPol_POS ? *roll : -1 * *roll;
		*pitch = *isPol_POS ? *pitch : -1 * *pitch;
	}

	void applyOrientationBounds()
	{
		*ang_Roll = jlimit(SET_angRange_Roll->VAL_bounds[0], SET_angRange_Roll->VAL_bounds[1], *ang_Roll);
		*ang_Pitch = jlimit(SET_angRange_Pitch->VAL_bounds[0], SET_angRange_Pitch->VAL_bounds[1], *ang_Pitch);
	}

	void UI_Init_LookFeel()
	{
		LAB_sensor_Name = new Label;
		SET_IP_Address = new TextEditor;
		SET_Port = new ComboBox;
		LAB_sensor_Status = new Label;
		LAB_sensor_BatteryPct = new Label;
		LAB_sensor_MessagePct = new Label;
		
		SET_frontSide = new ComboBox;
		SET_polarity = new ToggleButton;
		CAL_Bias = new TextButton;
		SET_angRange_Pitch = new MovementRangeSlider_wOFFSET(-90,90);
		SET_angRange_Roll = new MovementRangeSlider_wOFFSET(-90,90);
		SET_Madgwick_Beta = new Slider;

		// Set Label Text
		update_UI_Elements();

		// Set Label Justifications
		LAB_sensor_Name->setJustificationType(Justification::centred);
		LAB_sensor_Name->setFont(Font(22, Font::bold));
		LAB_sensor_Status->setJustificationType(Justification::centred);
		LAB_sensor_Status->setFont(Font(22, Font::bold));
		LAB_sensor_MessagePct->setJustificationType(Justification::centred);
		LAB_sensor_MessagePct->setFont(Font(22, Font::bold));
		LAB_sensor_BatteryPct->setJustificationType(Justification::centred);
		LAB_sensor_BatteryPct->setFont(Font(22, Font::bold));
		SET_IP_Address->setJustification(Justification::centred);
		SET_IP_Address->setFont(Font(16, Font::bold));

		SET_polarity->setToggleState(isPol_POS, sendNotificationSync);
		
		SET_frontSide->addListener(this);
		SET_frontSide->addItem("Front", 1);
		SET_frontSide->addItem("Side", 2);
		SET_frontSide->setSelectedId(1);

		SET_Port->addListener(this);
		SET_Port->addItem("9999", 1);
		SET_Port->addItem("9998", 2);
		SET_Port->addItem("9997", 3);
		SET_Port->addItem("9996", 4);
		SET_Port->addItem("9995", 5);
		SET_Port->addItem("Unassigned", 6);

		CAL_Bias->setButtonText("Calibrate");
		CAL_Bias->setColour(CAL_Bias->buttonColourId, Colours::red);

		SET_angRange_Pitch->range->setMinValue(angRange_Pitch[0]);
		SET_angRange_Pitch->range->setMaxValue(angRange_Pitch[1]);

		SET_angRange_Roll->range->setMinValue(angRange_Roll[0]);
		SET_angRange_Roll->range->setMaxValue(angRange_Roll[1]);

		SET_Madgwick_Beta->setColour(SET_Madgwick_Beta->trackColourId, Colours::yellow);
		SET_Madgwick_Beta->setColour(SET_Madgwick_Beta->backgroundColourId, Colours::blue);
		SET_Madgwick_Beta->setRange(0.033, 0.6);
		SET_Madgwick_Beta->setValue(0.6);
		SET_Madgwick_Beta->setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
	}

	void UI_Init_Behavior()
	{
		SET_polarity->onClick = [this]
		{
			*isPol_POS = SET_polarity->getToggleState();
		};

		CAL_Bias->onClick = [this]
		{
			if (oscObj->isOnline)
			{
				oscObj->isCalibrated = false;
				oscObj->isCalibrating = true;
			}
		};

		SET_IP_Address->onTextChange = [this]
		{
			oscObj->ipAddress = SET_IP_Address->getText();
		};

		SET_Madgwick_Beta->onValueChange = [this]
		{
			madgwickAHRS->beta = SET_Madgwick_Beta->getValue();
		};
	}

	void comboBoxChanged(juce::ComboBox* box)
	{
		if (box == SET_frontSide)
		{
			*isFront_Side = box->getSelectedId();
		}

		if (box == SET_Port)
		{
			oscObj->setupPortAndListener(10000 - box->getSelectedId(), 8889 - box->getSelectedId(),
				"/S" + String(box->getSelectedId()));
		}
	}

	void update_UI_Elements()
	{
		LAB_sensor_Name->setText(location, dontSendNotification);
		LAB_sensor_Status->setText(oscObj->isOnline ? "ON" : "OFF", dontSendNotification);
		LAB_sensor_Status->setColour(LAB_sensor_Status->backgroundColourId, oscObj->isOnline ? Colours::green : Colours::red);
		LAB_sensor_BatteryPct->setText(String(oscObj->level_Battery), dontSendNotification);
		LAB_sensor_MessagePct->setText(String(oscObj->oscSampleReceived_Percent, 2), dontSendNotification);
		LAB_sensor_MessagePct->setColour(LAB_sensor_MessagePct->backgroundColourId,
			(oscObj->oscSampleReceived_Percent > 50) ? Colours::green : Colours::red);

		// HANDLE CALIBRATION BUTTON
		if (!oscObj->isCalibrating && !oscObj->isCalibrated)
		{
			CAL_Bias->setColour(CAL_Bias->buttonColourId, Colours::red);
			CAL_Bias->setButtonText("Calibrate");
		}
		if (oscObj->isCalibrating)
		{
			CAL_Bias->setColour(CAL_Bias->buttonColourId, Colours::blue);
			CAL_Bias->setButtonText("Calibrating");
		}
		if (oscObj->isCalibrated)
		{
			CAL_Bias->setColour(CAL_Bias->buttonColourId, Colours::green);
			CAL_Bias->setButtonText("Recalibrate");
		}

		SET_angRange_Pitch->updateIndicator(*ang_Pitch);
		SET_angRange_Roll->updateIndicator(*ang_Roll);
	}

	void setVisible(bool on)
	{
		LAB_sensor_Name->setVisible(on);
		SET_IP_Address->setVisible(on);
		SET_Port->setVisible(on);
		LAB_sensor_Status->setVisible(on);
		LAB_sensor_BatteryPct->setVisible(on);
		LAB_sensor_MessagePct->setVisible(on);

		SET_frontSide->setVisible(on);
		SET_polarity->setVisible(on);
		CAL_Bias->setVisible(on);
		SET_angRange_Pitch->setVisible(on);
		SET_angRange_Roll->setVisible(on);
		SET_Madgwick_Beta->setVisible(on);
	}

	float getAngVel_AP()
	{
		switch (*isFront_Side)
		{
		case 1:
			return abs(oscObj->gyr_Buf[0]);
			break;
		case 2:
			return abs(oscObj->gyr_Buf[1]);
			break;
		}
	}

	void rec_InitFile(String path)
	{
		path = path + "Raw IMU Data - " + location + ".csv";
		rawLog = fopen(path.toStdString().c_str(), "w");
	}

	void rec_WriteLine()
	{
		String rawLog_PresentLine = oscObj->getLine_RawLog();
		fprintf(rawLog, rawLog_Format.c_str(), rawLog_PresentLine);
	}
};
