#ifndef JOYSTICKCLASS
#define JOYSTICKCLASS


#include <SDL.h>
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

