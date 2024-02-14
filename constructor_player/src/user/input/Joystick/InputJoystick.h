#ifndef JOYSTICKCLASS
#define JOYSTICKCLASS

#ifdef WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

//класс описания...
#include "../../JoyDataClass.h"


//класс обработчиков устройства
class InputJoystickClass
{
public:
	InputJoystickClass();

	void update();
	
	//возврат параметров джойстиков
	JoysticksData GetJoysticksData();
	

private:
	int Jmax;
	std::vector <SDL_Joystick*> joyN;
	
	JoysticksData Joysticks;
	
};
	
#endif

