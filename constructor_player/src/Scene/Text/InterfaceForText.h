#ifndef INTERFACEFORTEXT
#define INTERFACEFORTEXT

#include <iostream>
#include <osgViewer/Viewer>
#include <osg/LightModel>
//#include <osg/CameraNode>
#include <osg/CoordinateSystemNode>
#include <osg/PositionAttitudeTransform>
#include <osg/MatrixTransform>
#include <osgText/Font>
#include <osgText/Text>
#include "../Visitors/FindNodeByName.h"
#include "./BaseTextObject.h"

//Обработка критических ошибок
#include "../../error/errorclass.h"

class InterfaceForText
{	
public:
	//конструктор
	InterfaceForText(osg::ref_ptr<osg::Group> _root);
	//
	~InterfaceForText();
	//сброс
	bool Reset();

	//создание нового текстового объекта
	bool CreateText(std::string Name, std::string value);
	//изменение значения текстового поля
        bool SetText(std::string Name,std::string Value, bool UTF_Encoding);
	//изменение значения текстового поля
	bool SetTextColor(std::string Name, float r, float g, float b, float a);
	//изменение размера букв
	bool SetTextSize(std::string Name, float size);
	//устанавливает шрифт (arial,gost ...)
	bool SetTextFont(std::string Name, std::string FontName);
	//задание равнения текста 0-слева 1-справа 2-по центру
	bool SetTextAlignment(std::string Name, int align);
	//устанавливает ориентацию текста  0-Горизонтально 1-вертикально
	bool SetTextLayout(std::string Name, int layout);
	//Выравнивание по осям (HUD и все такое)
	//XY_PLANE 	-0	
	//XZ_PLANE 	-1	
	//YZ_PLANE 	-2	
	//SCREEN 	-3
	//USER_DEFINED_ROTATION -4
	bool SetAxisAlignment(std::string Name, int axis);
	
private:
	//указатель на главную группу сцены
	osg::ref_ptr<osg::Group> root;
	//
	//созданные обертки для текстовых источников (для возможности работать с текстом командами типа RSetPosition..)
	//std::vector <osg::ref_ptr <osg::MatrixTransform> > textsMatrix;
	//и сами текстовые элементы
	//std::map <std::string, osgText::Text*> texts;

	//сервисная функция
        osg::ref_ptr<BaseTextObject> FindTextObject(std::string name)  throw (ErrorClass);

        //
        bool  _CreateText(std::string Name, std::string value)  throw (ErrorClass);
};

#endif
