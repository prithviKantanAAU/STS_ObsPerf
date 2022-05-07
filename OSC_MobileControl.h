#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
using namespace juce;

class OSC_MobileControl : public Component,
	private OSCReceiver,
	private OSCReceiver::ListenerWithOSCAddress<OSCReceiver::MessageLoopCallback>
{
public:

	int portNumber = 0;
	int portNumber_Remote = 0;
	String ipAddress = "";
	bool connectionStatus = false;

	// DATA BUFFERS
	float oscDataArray[4] = { 0.0 };
	short messageSize = 1;

	bool message_toHandle[4] = { false, false, false, false };

	OSC_MobileControl()
	{
		setupPortAndListener(9995, 9999, "/oscControl/slider");
	}

	~OSC_MobileControl()
	{
		disconnect();
	}

	void setupPortAndListener(int localPort, int remotePort, String address)
	{
		portNumber = localPort;
		portNumber_Remote = remotePort;
		connectAndCheckStatus();
		addListener(this, address + "1");
		addListener(this, address + "2");
		addListener(this, address + "3");
		addListener(this, address + "4");
	}

	void connectAndCheckStatus()
	{
		connectionStatus = (connect(portNumber)) ? true : false;
	}

	void oscMessageReceived(const OSCMessage& message) override
	{
		if (message.size() == messageSize)
		{
			for (int i = 0; i < message.size(); i++)
			{
				if (message[i].isFloat32())
				{
					for (int j = 0; j < 4; j++)
					{
						if (message.getAddressPattern() == ("/oscControl/slider" + String(j + 1)))
						{
							message_toHandle[j] = true;
							oscDataArray[j] = message[i].getFloat32();
						}
					}
				}
			}
		}
	}
};