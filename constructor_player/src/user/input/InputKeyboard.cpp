/***************************************************************************
InputKeyboard.cpp  -  обработчики клавиатуры
-------------------
begin                : 10 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/
// 
// обработчики ввода с клавиатуры
// 

#include "InputKeyboard.h"

//#define JUMP_version true


//режим управоения или просмотра 1 - просмотр 2- управление 3- перемещение
void KeyboardEventHandler::setMode(bool head, bool commands, bool move)
{
	//флаг разрешения режима просмотра (изменения направления взгляда)
	viewModeEnable = head;
	//флаг разрешения режима управления
	commandModeEnable = commands;
	//флаг разрешения обработки перемещения пользователя
	moveModeEnable = move;
}

//блокировка выхода (например F10)
void KeyboardEventHandler::BlockExit(bool block)
{
	_BlockExit = block;
}


//возврат состояния "ног"
FootData KeyboardEventHandler::GetFootData()
{
	FootData data;
	data.SetMode (FootData::VECTORS);

        //режим бега
        int k=1;
        if (keyRUN==true)
        {
            k=2;
        }

	if (moveModeEnable==true)
	{
		//dx
		if ((keyStrafeLEFT==true)&&(keyStrafeRIGHT==false)) data.SetVectorX (1.0f);
		if ((keyStrafeRIGHT==true)&&(keyStrafeLEFT==false)) data.SetVectorX (-1.0f);
		//dy
                if ((keyFORWARD==true)&&(keyBACK==false)) data.SetVectorY (1.0f*k);
		if ((keyFORWARD==false)&&(keyBACK==true)) data.SetVectorY (-1.0f);
		//dz
		if ((keyDOWN==true)&&(keyJUMP==false)) data.SetVectorZ (-1.0f);
		if ((keyDOWN==false)&&(keyJUMP==true)) data.SetVectorZ (1.0f);
		//zoom
		if (keyZOOM==true) data.SetZoom (true);
	}
	return data;
}

//сброс состояния всех клавиш
void KeyboardEventHandler::ReleaseAllKeys()
{
	keyStrafeLEFT=false;
	keyStrafeRIGHT=false;
	keyFORWARD=false;
	keyBACK=false;
	keyDOWN=false;
	keyJUMP=false;
        keyRUN = false;
	//keyZOOM=false;
}

//обработчик
bool KeyboardEventHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us)
{
    switch(ea.getEventType())
    {		
		//нажатие клавиши
		case(osgGA::GUIEventAdapter::KEYDOWN ):
        {
			//
                        int _key = tranlator->GetConvertKey(ea.getKey());
                        int temp=ea.getKey();

                        //1
                        if ((ea.getModKeyMask() ==  osgGA::GUIEventAdapter::MODKEY_SHIFT) || (ea.getModKeyMask() ==  1))
                        {
                            keyRUN = true;
                        }
                        else
                        {
                            keyRUN = false;
                        }

                        //printf("Key 1 = %d \n",_key);
                        //if ((_key<0xff00)&&(_key!=0)&&(_key!=0x20)) _key&=0xFFDF;
                        //std::cout << "key = " << _key << " normal = " << temp << " mask = " << ea.getModKeyMask() << std::endl;
                        //printf("Key = %d (%d) [%d] mod = %d\n",_key,KEYS.keyForvard,temp,ea.getModKeyMask());
                        //std::cout << "KEY=" << _key << std::endl;
                        //printf("Key mod = %d\n", ea.getModKeyMask());

			//Движение вперед
                        if (KEYS.keyForvard == _key)
			{
				keyFORWARD=true;
				keyBACK=false;
			}
			//Движение назад
                        if (KEYS.keyBack==_key)
			{
				keyBACK=true;
				keyFORWARD=false;
			}
			//стрейф влево
                        if (KEYS.keySLeft==_key)
			{
				keyStrafeLEFT=true;
				keyStrafeRIGHT=false;
			}
			//стрейф вправо
                        if (KEYS.keySRight==_key)
			{
				keyStrafeRIGHT=true;
				keyStrafeLEFT=false;
			}
			//присесть
			if (KEYS.keyPrisest==_key)
			{
				ReleaseAllKeys();
				keyDOWN=true;
			}
			//вытянуть шею или прыгнуть
			if (KEYS.keyJump==_key)
			{
				#ifdef JUMP_version
				ReleaseAllKeys();
				keyJUMP=true;
				#endif
			}
			//нагнутся над объектом (ZOOM)
                        if (KEYS.keyZoom==_key)
			{
				//ReleaseAllKeys();
				keyZOOM=true;
			}

                        if (65479  == _key) //osgGA::GUIEventAdapter::KEY_F10
			{
				if (_BlockExit==false)
				{
					//ExitProgramm("Exit OK\n");
					std::cout << "F10 - Exit process .." << std::endl;

					dynamic_cast<osgViewer::Viewer*>(&us)->setDone(true);
				}
			}
			//попытка взять/положить/использовать предмет
			if ((osgGA::GUIEventAdapter::KEY_F2 == _key)||(KEYS.keyUse  == _key))
			{
				//UserUSEActionFunc();
			}
			//смена языка
                        if (osgGA::GUIEventAdapter::KEY_F5 == _key)
			{
				//ChangeLanguageActionFunc();
			}
			//возврат?
			return true;
        }

		//отпускание клавиши
		case(osgGA::GUIEventAdapter::KEYUP):
        {
			//switch(ea.getKey())
			//{
                        int _key = tranlator->GetConvertKey(ea.getKey());
                        //if ((_key<0xff00)&&(_key!=0)&&(_key!=0x20)) _key&=0xFFDF;
			//std::cout << "new key = " << _key << std::endl;
			//Движение вперед
                        if (KEYS.keyForvard == _key)
			{
				keyFORWARD=false;
			}
			//Движение назад
                        if (KEYS.keyBack==_key) //osgGA::GUIEventAdapter::KEY_Down:
			{
				keyBACK=false;
			}
			//стрейф влево
                        if (KEYS.keySLeft==_key)
			{
				keyStrafeLEFT=false;
			}
			//стрейф ВПРАВО
                        if (KEYS.keySRight==_key)
			{
				keyStrafeRIGHT=false;
			}
			//присесть
			if (KEYS.keyPrisest==_key)
			{
				keyDOWN=false;
			}
			//вытянуть шею или прыгнуть
			if (KEYS.keyJump==_key)
			{
				keyJUMP=false;
			}
			//нагнутся над объектом (ZOOM)
                        if (KEYS.keyZoom==_key)
			{
				keyZOOM=false;
			}
	        return true;
        }
	    default:
        return false;
    }
}

//void KeyboardEventHandler::accept(osgGA::GUIEventHandlerVisitor& v)
//{
//        v.visit(*this);
//}
     
KeyboardEventHandler::KeyboardEventHandler()
{
    tranlator = new KeyTranslation();

    ReleaseAllKeys();
    keyZOOM=false;
}

