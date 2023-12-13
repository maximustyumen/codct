#ifndef JOYDATACLASS
#define JOYDATACLASS

#include <stdlib.h>
#include <vector> 
#include <iostream>

class JoysticksData
{
public:
	//класс описания джойстика
	class OneJoystickClass
	{
	public:
		//информация о джойстике
		std::string name;
		int NumAxes;
		int NumButtons;
		int NumHats;
		//значения...
		std::vector <bool> Buttons;
		std::vector <std::string> Hats;
		std::vector <int> Axes;
	};


	int JoysticksNum;
	std::vector <OneJoystickClass> Joysticks;

        bool getButtonState(int JoystickNum, int ButtonNum);
        std::string getHatState(int JoystickNum, int HatNum);
        int getAxesState(int JoystickNum, int AxesNum);
};



#endif
	


