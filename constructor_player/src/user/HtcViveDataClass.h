#ifndef HTCVIVECLASS
#define HTCVIVECLASS

#include <iostream>
#include <string>

class HtcViveData
{
public:
	HtcViveData()
	{
		presend=false;


	};

	bool presend;

	//голова
	osg::Quat HeadOrientationQuat;
	osg::Vec3 HeadPosition;
	osg::Vec3 HeadOrientationEuler;

	//рука правая
	osg::Quat RightHandOrientationQuat;
	osg::Vec3 RightHandPosition;
	osg::Vec3 RightHandOrientationEuler;

	bool RightHand_EButton_ApplicationMenu;
	bool RightHand_EButton_SteamVR_Trigger;
	bool RightHand_EButton_Grip;
	bool RightHand_EButton_SteamVR_Touchpad;
	float RightHand_TrackPad_x;
	float RightHand_TrackPad_y;
	float RightHand_Trigger_x;
	float RightHand_Trigger_y;
	float RightHand_Joystick_x;
	float RightHand_Joystick_y;

	//рука левая
	osg::Quat LeftHandOrientationQuat;
	osg::Vec3 LeftHandPosition;
	osg::Vec3 LeftHandOrientationEuler;

	bool LeftHand_EButton_ApplicationMenu;
	bool LeftHand_EButton_SteamVR_Trigger;
	bool LeftHand_EButton_Grip;
	bool LeftHand_EButton_SteamVR_Touchpad;
	float LeftHand_TrackPad_x;
	float LeftHand_TrackPad_y;
	float LeftHand_Trigger_x;
	float LeftHand_Trigger_y;
	float LeftHand_Joystick_x;
	float LeftHand_Joystick_y;

private:

};

#endif


