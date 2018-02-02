#pragma once
#include <Windows.h>
#include <Xinput.h>
#include <memory>

namespace Input
{
	struct Stick
	{
		float xAxis;
		float yAxis;
	};

	enum Button
	{
		A = 0x0, B = 0x1, X = 0x2, Y = 0x3,
		RB = 0x4, LB = 0x5, R3 = 0x6, L3 = 0x7,
		Start = 0x8, Select = 0x9,
		DPadLeft = 0xA, DPadRight = 0xB, DPadUp = 0xC, DPadDown = 0xD
	};

	class XBoxInput
	{
	public:
		// These two constructors are basically just to set the vibration to zero
		XBoxInput();
		~XBoxInput();

		// Call this function ONCE in an update
		bool DownloadPackets(unsigned int _numControllers = 1);

		// Call this function to get the sticks. The function returns false if there is no controller for this index
		bool GetSticks(int _index, Stick & _leftStick, Stick & _rightStick);
		// This function returns false if there is no controller plugged in to this index
		bool GetConnected(int _index);
		// Call this function to get the Button. The function returns false if there is no controller, OR if the button isn't pressed
		bool GetButton(int _index, Button _buttons);
		// Call this function to get the triggers on the controller. It returns false if there is no controller connected
		bool GetTriggers(int _index, float &_leftTrigger, float &_rightTrigger);
		// Call this function to set the vibration. It returns false if there is no controller connected
		bool SetVibration(int _index, float _leftPower, float _rightPower);

	private:
		XINPUT_STATE m_state;

		bool m_buttonStates[4][14];
		bool m_connected[4];

		std::pair<float, float> m_triggers[4];

		Stick m_leftSticks[4];
		Stick m_rightSticks[4];

		int m_packetNumber[4];
	};
}