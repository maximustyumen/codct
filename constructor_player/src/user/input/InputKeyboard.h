#ifndef KEYBOARDCLASS
#define KEYBOARDCLASS

#include <osgGA/GUIEventHandler>
#include <osgViewer/Viewer>
#include <osgDB/Registry>
#include <iostream>
#include "./keytranslation.h"

#include "../FootDataClass.h"

//клавиши
class KeyTable
{
public:
	int     keyUp;
    int     keyDown;
    int     keyLeft;
    int     keyRight;
    int     keyForvard;
	int     keyForvard2;
    int     keyBack;
	int     keyBack2;
    int     keySLeft;
	int     keySLeft2;
    int     keySRight;
	int     keySRight2;
    int     keyJump;
    int		keyPrisest;
    int		keyUse;
    int		keyUse2;
	int		keyZoom;
	int		keyZoom2;



	//конструктор
	KeyTable()
	{
		keyUp=38;
		keyDown=40;
		keyLeft=37;
		keyRight=39;
		keyForvard=87;
		keyForvard2=65494;
		keyBack=83;
		keyBack2=65499;
		keySLeft=68;
		keySLeft2=65474;
		keySRight=65;
		keySRight2=65492;
		keyJump=65505;
		keyPrisest=65507;
		keyUse=69;
		keyUse2=13;	
		keyZoom=90;
		keyZoom2=65503;
	}
};

class KeyboardEventHandler : public osgGA::GUIEventHandler
{
public:
	virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter&);
	//virtual void accept(osgGA::GUIEventHandlerVisitor& v);
	KeyboardEventHandler();
	
	//возврат состояния "ног" и зума
	FootData GetFootData();

	//режим управоения или просмотра 1 - просмотр 2- управление 3- перемещение
	void setMode(bool, bool, bool);

	//блокировка выхода (например F10)
	void BlockExit(bool block);


private:
        //транслятор кодов клавиш
        KeyTranslation *tranlator;
	//состояние клавиш
	bool keyFORWARD,keyBACK,keyStrafeLEFT, keyStrafeRIGHT;
	bool keyHeadLEFT,keyHeadRIGHT,keyHeadUP,keyHeadDOWN;
	//присел ли пользователь
	bool keyDOWN;
	bool keyJUMP;
	//нагнулся ли пользователь (зум)
	bool keyZOOM;
        //режим бега
        bool keyRUN;
	//kay table
	KeyTable KEYS;
	
	//флаг разрешения режима просмотра (изменения направления взгляда)
	bool viewModeEnable;
	//флаг разрешения режима управления
	bool commandModeEnable;
	//флаг разрешения обработки перемещения пользователя с помощью прокрутки мыши
	bool moveModeEnable;
	//сброс состояния всех клавиш
	void ReleaseAllKeys();
	//
	bool _BlockExit;
};


#endif


