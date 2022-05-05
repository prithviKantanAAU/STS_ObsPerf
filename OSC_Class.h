#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "BiQuad.h"
#include "medianFilter.h"
using namespace juce;

class OSCReceiverUDP_Sensor : public Component,
	private OSCReceiver,
	private OSCReceiver::ListenerWithOSCAddress<OSCReceiver::MessageLoopCallback>
{
public:
	bool connectionStatus = false;
	bool isOnline = false;
	int portNumber = 0;
	int portNumber_Remote = 0;
	String ipAddress = "";

	bool isCalibrating = false;
	bool isCalibrated = false;

	// DATA BUFFERS
	float oscDataArray[10] = { 0.0 };
	short messageSize = 10;
	float acc[3] = { 0.0 }; float acc_Buf[3] = { 0.0 };
	float gyr[3] = { 0.0 }; float gyr_Buf[3] = { 0.0 };
	float mag[3] = { 0.0 }; float mag_Buf[3] = { 0.0 };

	// BIAS BUFFERS
	double bias_mean_acc[3] = { 0.0 };
	double bias_mean_gyr[3] = { 0.0 };
	double rest_mean_acc[3] = { 0.0, 0.0, -1.0 };
	double rest_mean_gyr[3] = { 0.0, 0.0, 0.0 };
	double maxs_mag[3] = { 0.0, 0.0, 0.0 };
	double mins_mag[3] = { 0.0, 0.0, 0.0 };
	float biasComp_avgInterval_sec = 10;
	float biasComp_avgInterval_smpl = biasComp_avgInterval_sec * fs;
	int biasComp_elapsedInterval_smpl = 0;
	bool isBiasComp_ON = false;
	bool isBiasComp_DONE = false;
	
	// BATTERY LEVEL
	float level_Battery = 0.0;
	
	// MESSAGE COUNT FOR ONLINE STATUS
	int messageCount_Recvd = 0;
	int messageCount_Recvd_z1 = 0;
	short messageCount_Recvd_smpl_z0 = 0;
	short messageCount_Recvd_smpl_z1 = 0;
	float isMessageRecvd_smpl_z0 = 0;

	// FILTERING
	float fs = 100;
	BiQuad LPF_Interp[9];
	MedianFilter medianPre[9];
	
	// CONNECTION HEALTH CHECKING
	short oscSampleReceived_BUFFER[200] = { 0 };
	int oscSampleReceived_WriteIdx = 0;
	float oscSampleReceived_Percent = 0;

	OSCSender connectionVerify;

	OSCReceiverUDP_Sensor()
	{
		for (int i = 0; i < 9; i++)
		{
			medianPre[i].filterLength = 3;
			LPF_Interp[i].calculateLPFCoeffs(49, 0.7, 100);
		}
	}

	~OSCReceiverUDP_Sensor()
	{
		disconnect();
	}

	void setSampleRate(float value)
	{
		fs = value;
	}

	void setupPortAndListener(int localPort, int remotePort, String address)
	{
		portNumber = localPort;
		portNumber_Remote = remotePort;
		connectAndCheckStatus();
		addListener(this, address);
	}
	
	void connectAndCheckStatus()
	{
		connectionStatus = (connect(portNumber)) ? true : false;
	}

	void updateBuffers()
	{
		for (int i = 0; i < 3; i++)
		{
			acc[i] = medianPre[i].doFiltering(acc[i]);
			gyr[i] = medianPre[i + 3].doFiltering(gyr[i]);
			mag[i] = medianPre[i + 6].doFiltering(mag[i]);

			acc_Buf[i] = LPF_Interp[i].doBiQuad(acc[i],0.0);
			gyr_Buf[i] = LPF_Interp[i + 3].doBiQuad(gyr[i],0.0);
			mag_Buf[i] = LPF_Interp[i + 6].doBiQuad(mag[i], 0.0);
		}
		updateBias(acc_Buf, gyr_Buf);
		compensateBias(acc_Buf, gyr_Buf, mag_Buf);

		messageCount_Recvd_smpl_z0 = messageCount_Recvd;
		isMessageRecvd_smpl_z0 = (messageCount_Recvd_smpl_z0 > messageCount_Recvd_smpl_z1) ? 1.0 : 0.0;
		messageCount_Recvd_smpl_z1 = messageCount_Recvd_smpl_z0;

		// Calculate Short Term (2 Sec) Average Packet Received Percent
		oscSampleReceived_BUFFER[oscSampleReceived_WriteIdx] = (short)isMessageRecvd_smpl_z0;
		oscSampleReceived_WriteIdx = (oscSampleReceived_WriteIdx + 1) % 200;
		oscSampleReceived_Percent = 0;
		for (int i = 0; i < 200; i++)
			oscSampleReceived_Percent += oscSampleReceived_BUFFER[i];
		oscSampleReceived_Percent /= 2.0;
	}

	void oscMessageReceived(const OSCMessage& message) override
	{
		messageCount_Recvd++;
		if (message.size() == messageSize)
		{
			for (int i = 0; i < message.size(); i++)
			{
				if (message[i].isFloat32())
				{
					oscDataArray[i] = jlimit(-10000.0f, 10000.0f, message[i].getFloat32());
					switch (i)
					{
					case 0:
						acc[0] = jlimit(-10000.0f, 10000.0f, message[i].getFloat32());
						break;
					case 1:
						acc[1] = jlimit(-10000.0f, 10000.0f, message[i].getFloat32());
						break;
					case 2:
						acc[2] = jlimit(-10000.0f, 10000.0f, message[i].getFloat32());
						break;
					case 3:
						gyr[0] = jlimit(-10000.0f, 10000.0f, message[i].getFloat32());
						break;
					case 4:
						gyr[1] = jlimit(-10000.0f, 10000.0f, message[i].getFloat32());
						break;
					case 5:
						gyr[2] = jlimit(-10000.0f, 10000.0f, message[i].getFloat32());
						break;
					case 6:
						level_Battery = jlimit(-10000.0f, 10000.0f, message[i].getFloat32());
						break;
					case 7:
						mag[0] = jlimit(-10000.0f, 10000.0f, message[i].getFloat32());
						break;
					case 8:
						mag[1] = jlimit(-10000.0f, 10000.0f, message[i].getFloat32());
						break;
					case 9:
						mag[2] = jlimit(-10000.0f, 10000.0f, message[i].getFloat32());
						break;
					default:
						break;
					}
				}
			}
		}
	}

	void isSensorActive()
	{
		if (messageCount_Recvd > messageCount_Recvd_z1)
		{
			messageCount_Recvd_z1 = messageCount_Recvd;
			isOnline = true;
		}
		else isOnline = false;
	};

	double updateMean(double oldMean, int count, float newVal)
	{
		return (oldMean*count + newVal) / (count + 1);
	};

	void updateBias(float *accBuf, float *gyrBuf)
	{
		if (isBiasComp_ON)
		{
			if (biasComp_elapsedInterval_smpl < biasComp_avgInterval_smpl)		//ONGOING
			{
				for (int i = 0; i < 3; i++)
				{
					bias_mean_acc[i] = updateMean(bias_mean_acc[i], biasComp_elapsedInterval_smpl, accBuf[i]);
					bias_mean_gyr[i] = updateMean(bias_mean_gyr[i], biasComp_elapsedInterval_smpl, gyrBuf[i]);
				}
				biasComp_elapsedInterval_smpl++;
			}
			else
			{
				for (int i = 0; i < 3; i++)
					bias_mean_acc[i] -= -rest_mean_acc[i];

				isBiasComp_DONE = true;
				isBiasComp_ON = false;
				biasComp_elapsedInterval_smpl = 0;
			}
		}
	};

	void compensateBias(float *accBuf, float *gyrBuf, float *magBuf)
	{
		for (int i = 0; i < 3; i++)
		{
			accBuf[i] -= bias_mean_acc[i];
			gyrBuf[i] -= bias_mean_gyr[i];
		}
	}

	String getLine_RawLog()
	{
		return String(acc_Buf[0]) + "," + String(acc_Buf[1]) + "," + String(acc_Buf[2]) + "," +
			String(gyr_Buf[0]) + "," + String(gyr_Buf[1]) + "," + String(gyr_Buf[2]) + "," +
			String(mag_Buf[0]) + "," + String(mag_Buf[1]) + "," + String(mag_Buf[2]) + "," + (isMessageRecvd_smpl_z0 ? "1" :  "0");
	}

	void get_IMUValues_from_LogRow(String logRow)
	{
		double val = 0;
		for (int i = 0; i < 9; i++)
		{
			val = logRow.upToFirstOccurrenceOf(",", false, true).getDoubleValue();
			logRow = logRow.fromFirstOccurrenceOf(",", false, true);
			if (i < 3)
				acc[i % 3] = val;
			if (i >= 3 && i < 6)
				gyr[i % 3] = val;
			if (i >= 6 && i < 9)
				mag[i % 3] = val;
		}
	}

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OSCReceiverUDP_Sensor)
};


