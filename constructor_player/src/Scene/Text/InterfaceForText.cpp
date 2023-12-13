/***************************************************************************
InterfaceForText.cpp  -  интерфейс для работы с текстом (2D)
-------------------
begin                : 25 марта 2023
modify               : 25 апреля 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/


#include "./InterfaceForText.h"


//конструктор
InterfaceForText::InterfaceForText(osg::ref_ptr<osg::Group> _root)
{
	root = _root;
}
//
InterfaceForText::~InterfaceForText()
{
	std::cout << "Destructor InterfaceForText."  << std::endl;
	Reset();
}

//сброс 
bool InterfaceForText::Reset()
{
	return true;
}

//создание нового текстового объекта
bool InterfaceForText::CreateText(std::string Name, std::string value)
{
    try
    {
        _CreateText(Name, value);
    }
    catch (ErrorClass error)
    {
        error.print();
    }
    return true;
}


bool  InterfaceForText::_CreateText(std::string Name, std::string value) throw (ErrorClass)
{
    //проверка на совпадение имен с уже имеющимися
    osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(Name);
    root->accept(*fnnv.get());

    if (fnnv->_foundNodes.empty()!=true)
    {
        throw (ErrorClass (__FILE__, __LINE__ , "InterfaceForText::CreateText()", "Add new Text: Name already use: " + Name, 3));
    }

    //добавляем в сцену (добавлять в root не нужно.. оно само))
    osg::ref_ptr<BaseTextObject> newText = new BaseTextObject(Name, value);
    root->addChild(newText);
    return true;
}

//изменение значения текстового поля
bool InterfaceForText::SetText(std::string Name,std::string Value, bool UTF_Encoding)
{
    try
    {
        FindTextObject(Name)->SetText (Value, UTF_Encoding);
        return true;
    }
    catch (ErrorClass error)
    {
        error.print();
        return false;
    }

}

//изменение значения цвета
bool InterfaceForText::SetTextColor(std::string Name, float r, float g, float b, float a)
{
    try
    {
	FindTextObject(Name)->SetTextColor (r,g,b,a);
	return true;
    }
    catch (ErrorClass error)
    {
        error.print();
        return false;
    }
}

//изменение размера букв
bool InterfaceForText::SetTextSize(std::string Name, float size)
{
    try
    {
	FindTextObject(Name)->SetTextSize(size);
	return true;
    }
    catch (ErrorClass error)
    {
        error.print();
        return false;
    }
}

//устанавливает шрифт (arial,gost ...)
bool InterfaceForText::SetTextFont(std::string Name, std::string FontName)
{
    try
    {
	FindTextObject(Name)->SetTextFont(FontName);
	return true;
    }
    catch (ErrorClass error)
    {
        error.print();
        return false;
    }
}

//задание равнения текста 0-слева 1-справа 2-по центру
bool InterfaceForText::SetTextAlignment(std::string Name, int align)
{
    try
    {
	FindTextObject(Name)->SetTextAlignment(align);
	return true;
    }
    catch (ErrorClass error)
    {
        error.print();
        return false;
    }
}

//устанавливает ориентацию текста  0-Горизонтально 1-вертикально
bool InterfaceForText::SetTextLayout(std::string Name, int layout)
{
    try
    {
	FindTextObject(Name)->SetTextLayout(layout);
	return true;
    }
    catch (ErrorClass error)
    {
        error.print();
        return false;
    }
}


//Выравнивание по осям (HUD и все такое)
//XY_PLANE 	-0	
//XZ_PLANE 	-1	
//YZ_PLANE 	-2	
//SCREEN 	-3
//USER_DEFINED_ROTATION -4
bool InterfaceForText::SetAxisAlignment(std::string Name, int axis)
{
	try
    {
		FindTextObject(Name)->setAxisAlignment(axis);
		return true;
    }
    catch (ErrorClass error)
    {
        error.print();
        return false;
    }
}




//--------------------------------------------------------------------------------------------------------
//сервисная функция
osg::ref_ptr<BaseTextObject> InterfaceForText::FindTextObject(std::string name) throw (ErrorClass)
{

    //ищем в графе сцены
    osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(name);
    root->accept(*fnnv.get());

    //try
    //{
	//
	if (!fnnv->_foundNodes.empty())
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << name << std::endl;
		//попытка преобразовать в BaseSoundObject
		if ( BaseTextObject* _foundTextObject = dynamic_cast<BaseTextObject*>( fnnv->_foundNodes.front().get() ) )
		{
			osg::ref_ptr<BaseTextObject> foundTextObject =  _foundTextObject;
			return foundTextObject.get();
		}
                throw (ErrorClass (__FILE__, __LINE__ , "InterfaceForText::FindTextObject()", "Object - NOT text! " + name, 3));
	}
	else
	{
            throw (ErrorClass (__FILE__, __LINE__ , "InterfaceForText::FindTextObject()", "Find objects in Root - FALSE! " + name, 3));
	}
    //}
   // catch (ErrorClass error)
   // {
   //     error.print();
   // }

}
//--------------------------------------------------------------------------------------------------------
