#ifndef BASETEXTOBJECTCLASS
#define BASETEXTOBJECTCLASS

#include <osgText/Font>
#include <osgText/Text>
#include <osg/Geode>
#include <osg/MatrixTransform>
#include <iostream>

/*
class MyTextObject: public osgText::Text
{
	public:
		// we need to set up the bounding box of the data too, so that the scene graph knows where this
        // objects is, for both positioning the camera at start up, and most importantly for culling.
        virtual osg::BoundingSphere computeBound() const
		{
			osg::BoundingSphere bbox;
			//
			bbox.expandBy(osg::Vec3(0,0,0));
			bbox.expandBy(osg::Vec3(100,100,100));
			//
			return bbox;
		}

		 virtual osg::BoundingSphere getBound() const
		{
			osg::BoundingSphere bbox;
			//
			bbox.expandBy(osg::Vec3(0,0,0));
			bbox.expandBy(osg::Vec3(100,100,100));
			//
			return bbox;
		}
		 
};
*/

class BaseTextObject: public osg::MatrixTransform
{
public:
	//конструктор
	BaseTextObject(std::string Name, std::string value);
	//деструктор
	~BaseTextObject();

	//1. изменение значения текстового поля
        bool SetText(std::string Value, bool UTF_Encoding);
	//2. изменение значения текстового поля
	bool SetTextColor(float r, float g, float b, float a);
	//3. изменение размера букв
	bool SetTextSize(float size);
	//4. устанавливает шрифт (arial,gost ...)
	bool SetTextFont(std::string FontName);
	//5. задание равнения текста 0-слева 1-справа 2-по центру
	bool SetTextAlignment(int align);
	//6. устанавливает ориентацию текста  0-Горизонтально 1-вертикально
	bool SetTextLayout(int layout);
	//7. Выравнивание по осям (HUD и все такое)
	//XY_PLANE 	-0	
	//XZ_PLANE 	-1	
	//YZ_PLANE 	-2	
	//SCREEN 	-3
	//USER_DEFINED_ROTATION -4
	bool setAxisAlignment(int axis);

	
	

private:
        osg::ref_ptr<osgText::Text> text;
};


#endif
