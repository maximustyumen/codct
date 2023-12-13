/***************************************************************************
JoyDataClass.cpp  -  стандартный интерфейс для описания состояния "джойстика"
-------------------
begin                : 7 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

#include "JoyDataClass.h"

bool JoysticksData::getButtonState(int JoystickNum, int ButtonNum)
{
	//Проверка
	if (JoysticksNum < (JoystickNum+1)) 
	{
		std::cout << "Joysticks " << JoystickNum << " not exist!" << std::endl;
		return false;
	}
	if (Joysticks.at(JoystickNum).NumButtons < (ButtonNum+1)) 
	{
		std::cout << "Button " << ButtonNum << " not exist!" << std::endl;
		return false;
	}


	return Joysticks.at(JoystickNum).Buttons.at(ButtonNum);
}


std::string JoysticksData::getHatState(int JoystickNum, int HatNum)
{
	//Проверка
	if (JoysticksNum < (JoystickNum+1)) 
	{
		std::cout << "Joysticks " << JoystickNum << " not exist!" << std::endl;
		return "";
	}
	if (Joysticks.at(JoystickNum).NumHats  < (HatNum+1)) 
	{
		std::cout << "Hat " << HatNum << " not exist!" << std::endl;
		return "";
	}

	return Joysticks.at(JoystickNum).Hats.at(HatNum);
}



int JoysticksData::getAxesState(int JoystickNum, int AxesNum)
{
	if (JoysticksNum < (JoystickNum+1)) 
	{
		std::cout << "Joysticks " << JoystickNum << " not exist!" << std::endl;
		return 0;
	}
	if (Joysticks.at(JoystickNum).NumAxes   < (AxesNum+1)) 
	{
		std::cout << "Axes " << AxesNum << " not exist!" << std::endl;
		return 0;
	}
	//пока без проверки диапазонов 
	return Joysticks.at(JoystickNum).Axes.at(AxesNum);
}


