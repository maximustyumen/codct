/***************************************************************************
InputJoystick.cpp  -  стандартный интерфейс для джойстика
-------------------
begin                : 5 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

#include "./InputJoystick.h"
#include <iostream>

InputJoystickClass::InputJoystickClass()
{
	std::cout << "\tInputJoystickClass constructor ... " <<std::endl;
	
	#ifndef WIN32
	return;
	#endif
	
	//информация о версии
	//SDL_version v;
	//SDL_GetVersion(&v);
	//printf("\tSDL version: %u.%u.%u\n", v.major, v.minor, v.patch);

	// Initialize the joystick subsystem
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);

	
	// Check for joysticks
	Jmax = SDL_NumJoysticks();
	std::cout << "\tFound " << Jmax << " joystick(s)" << std::endl;

	//ЗАПОЛНЯЕМ ПОЛЕ - КОЛИЧЕСТВО ДЖОЙСТИКОВ
	Joysticks.JoysticksNum = Jmax;
		
	for (int i=0;i<Jmax;i++)
	{
		joyN.push_back (SDL_JoystickOpen(i));
		//Создаем описание-структуру для 1 джойстика
		JoysticksData::OneJoystickClass tempJoy;
		//Заполняем структуру для 1 джойстика
		if (joyN.at(i))
		{
			std::cout << "\tOpened Joystick " << i << std::endl;
			printf("\tName: %s\n", SDL_JoystickName(joyN.at(i)));
			//Название
			tempJoy.name = SDL_JoystickName(joyN.at(i));

			//Количество осей вращения
			int NumAxes = SDL_JoystickNumAxes(joyN.at(i));
			printf("\tNumber of Axes: %d\n", NumAxes);
			tempJoy.NumAxes = NumAxes;
			//инициализируем массив
			for (int n=0; n < NumAxes; n++) tempJoy.Axes.push_back(0);

			//Количество кнопок
			int NumButtons = SDL_JoystickNumButtons(joyN.at(i));
			printf("\tNumber of Buttons: %d\n", NumButtons);
			tempJoy.NumButtons = NumButtons;
			//инициализируем массив
			for (int n=0; n < NumButtons; n++) tempJoy.Buttons.push_back(false);

			printf("\tNumber of Balls: %d\n", SDL_JoystickNumBalls(joyN.at(i)));

			//Количество "шляп"
			int NumHats = SDL_JoystickNumHats(joyN.at(i));
			printf("\tNumber of Hats: %d\n", NumHats );
			tempJoy.NumHats = NumHats;
			for (int n=0; n < NumHats; n++) tempJoy.Hats.push_back("NOT STATE");

		}
		else
		{
			std::cout <<  "\tCouldn't open Joystick" << std::endl;
		}
		//Добавляем джойстик ..
		Joysticks.Joysticks.push_back (tempJoy);
	}


}

void InputJoystickClass::update()
{
	#ifndef WIN32
	return;
	#endif
	
	//есть ли джойстики
	if (Joysticks.JoysticksNum<=0) return;

	// Update and check joystick state
    SDL_JoystickUpdate();

	//заполняем данные по всем джойстикам
	for (int i=0; i < Joysticks.JoysticksNum ; i++)
	{
		//клавиши
		for (int n =0; n <Joysticks.Joysticks.at(i).NumButtons ;n++)
		{
			if ( SDL_JoystickGetButton( joyN.at(i), n ) )
			{
				//std::cout << "Button " << n << " press" << std::endl;
				Joysticks.Joysticks.at(i).Buttons.at(n) = true;
			}
			else
			{
				Joysticks.Joysticks.at(i).Buttons.at(n) = false;
			}
		}

		//Оси
		for (int n =0; n <Joysticks.Joysticks.at(i).NumAxes ;n++)
		{
			Sint16 value =  SDL_JoystickGetAxis( joyN.at(i), n );
			//std::cout << "Axes " << n << "=" << value <<  std::endl;
			Joysticks.Joysticks.at(i).Axes.at(n) = value;
		}

		//Шляпы
		for (int n =0; n <Joysticks.Joysticks.at(i).NumHats ;n++)
		{
			Uint8 HatState =  SDL_JoystickGetHat( joyN.at(i), n );
			switch ( HatState )
			{
			 case SDL_HAT_CENTERED:
				//std::cout << "Hat " << i << "=" << "SDL_HAT_CENTERED" <<  std::endl;
				Joysticks.Joysticks.at(i).Hats.at(n) = "CENTERED";
				break;
			 case SDL_HAT_UP:
				//std::cout << "Hat " << i << "=" << "SDL_HAT_UP" <<  std::endl;
				Joysticks.Joysticks.at(i).Hats.at(n) = "UP";
				break;
			 case SDL_HAT_RIGHT:
				//std::cout << "Hat " << i << "=" << "SDL_HAT_RIGHT" <<  std::endl;
				Joysticks.Joysticks.at(i).Hats.at(n) = "RIGHT";
				break;
			 case SDL_HAT_DOWN:
				///std::cout << "Hat " << i << "=" << "SDL_HAT_DOWN" <<  std::endl;
				Joysticks.Joysticks.at(i).Hats.at(n) = "DOWN";
				break;
			 case SDL_HAT_LEFT:
                                //std::cout << "Hat " << i << "=" << "SDL_HAT_LEFT" <<  std::endl;
				Joysticks.Joysticks.at(i).Hats.at(n) = "LEFT";
				break;
			 case SDL_HAT_RIGHTUP:
				//std::cout << "Hat " << i << "=" << "SDL_HAT_RIGHTUP" <<  std::endl;
				Joysticks.Joysticks.at(i).Hats.at(n) = "RIGHTUP";
				break;
			 case SDL_HAT_RIGHTDOWN:
				//std::cout << "Hat " << i << "=" << "SDL_HAT_RIGHTDOWN" <<  std::endl;
				Joysticks.Joysticks.at(i).Hats.at(n) = "RIGHTDOWN";
				break;
			 case SDL_HAT_LEFTUP:
				//std::cout << "Hat " << i << "=" << "SDL_HAT_LEFTUP" <<  std::endl;
				Joysticks.Joysticks.at(i).Hats.at(n) = "LEFTUP";
				break;
			 case SDL_HAT_LEFTDOWN:
				//std::cout << "Hat " << i << "=" << "SDL_HAT_LEFTDOWN" <<  std::endl;
				Joysticks.Joysticks.at(i).Hats.at(n) = "LEFTDOWN";
				break;
			 default:
				//std::cout << "Hat " << i << "=" << "NOT STATE" <<  std::endl;
				Joysticks.Joysticks.at(i).Hats.at(n) = "NOT STATE";
			}
		}
		//
	}
}


//возврат параметров джойстиков
JoysticksData InputJoystickClass::GetJoysticksData()
{
	return Joysticks;
}


