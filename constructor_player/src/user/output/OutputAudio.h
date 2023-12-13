#include <stdlib.h>
#include <stdio.h> 
#include <sstream>
#include <iostream>

#include <math.h>


	#include <AL/al.h>
	#include <AL/alc.h>

#include <AL/alut.h>

#include <osg/Timer>

#include "../../Configure/Configure.h"

//Обработка критических ошибок
#include "../../error/errorclass.h"


class OutputAudioClass
{
public:
	OutputAudioClass(Configure* configuration);
	~OutputAudioClass();
	void Update(float posx,float posy, float posz, float Yaw, float Pitch, float Roll);

private:
	ALCdevice* pDevice;
	ALCcontext * pContext;

	ALboolean CheckALCError();
	ALboolean CheckALError();

	//позиция и ориентация слушателя
	float x,y,z, lookupdown, lookleftright;

	bool SOUND_OFF;

};

