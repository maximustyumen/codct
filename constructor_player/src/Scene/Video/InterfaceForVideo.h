#ifndef INTERFACEFORVIDEO
#define INTERFACEFORVIDEO

#include <iostream>
#include "./BaseVideoObject.h"
#include "../Visitors/FindNodeByName.h"

//Обработка критических ошибок
#include "../../error/errorclass.h"

class InterfaceForVideo
{	
public:
	//конструктор
	InterfaceForVideo(osg::ref_ptr<osg::Group> _root);
	~InterfaceForVideo();
	//сброс
	bool Reset();
	//прикрепляет к объекту видеофайл (QuickTime MOV)
	bool CreateVideoPlane(std::string, std::string);

	//1. Играть
	void Play(std::string name);
	//2. Пауза
	void Pause(std::string name);
	//3. Длинна файла в секундах
	double getLength(std::string name);
	//4. Перемотать на секунду ...
	void setReferenceTime(std::string name, double time);
	//5. Позиция в секундах..
	double getReferenceTime(std::string name) ;
	//6. Установить громкость
	void setVolume(std::string name, float volume);
	//7. Узнать громкость
	float getVolume(std::string name);
	//8. Режим повтора
	void setLoopingMode(std::string name, bool loop);

private:
	//указатель на главную группу сцены
	osg::ref_ptr<osg::Group> root;
	//сервисная функция
	osg::ref_ptr<BaseVideoObject> FindVideoObject(std::string name);
	
};

#endif
