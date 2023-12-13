/***************************************************************************
BaseTextObject.cpp  -  описание класса текстового объекта
-------------------
begin                : 25 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

#include "./BaseTextObject.h"





BaseTextObject::BaseTextObject(std::string Name, std::string value)
{
	//добавляем в текстовый контейнер
	text = new osgText::Text();

//	osg::ref_ptr<osgText::Font> ff = new osgText::Font;
//	ff->setMinFilterHint(osg::Texture::LINEAR); //NEAREST
//	ff->setMagFilterHint(osg::Texture::LINEAR); 
//	text->setFont(ff);



//	text->setUseVertexBufferObjects(true);
	// reproduce outline bounding box compute problem with backdrop on.
//	text->setBackdropType(osgText::Text::OUTLINE);
//	text->setDrawMode(osgText::Text::TEXT | osgText::Text::BOUNDINGBOX);
	text->setCharacterSize(20);
	text->setText(value);
	//создаем обертку (для возможности работать с текстом командами типа RSetPosition..)
	osg::ref_ptr<osg::Geode> tempGeode = new osg::Geode();

	//Это для 3D HUD MENU
	//osg::ref_ptr<osg::StateSet> stateset = tempGeode->getOrCreateStateSet();
	osg::ref_ptr<osg::StateSet> stateset = this->getOrCreateStateSet();
	//stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	stateset->setRenderBinDetails( 150, "RenderBin", osg::StateSet::OVERRIDE_RENDERBIN_DETAILS);
	
	//stateset->setMode(GL_BLEND,osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
    //stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    //stateset->setRenderBinDetails(1001, "DepthSortedBin");
	//

	tempGeode->addDrawable (text);
	//
	addChild(tempGeode);
	setName(Name);
	//
	std::string timesFont("fonts/arial.ttf");
	text->setCharacterSize(20.0f);
	text->setFont(timesFont);
	text->setColor(osg::Vec4(0.0f,0.0f,0.0f,1.0f));


	text->setFontResolution(18,18);

	

        SetText(value, false);
}


//деструктор
BaseTextObject::~BaseTextObject()
{
        //text= NULL;
	std::cout << "Destructor BaseTextObject: " << getName() <<  std::endl;
	//otherMatrix
}

//1. изменение значения текстового поля
bool BaseTextObject::SetText(std::string Value, bool UTF_Encoding)
{
    if (UTF_Encoding == true)
    {
        osgText::String textstr;
        textstr.set(Value, osgText::String::ENCODING_UTF8);
	text->setText(textstr);
	//textstr.set(Value, osgText::String::ENCODING_UTF16);
	//osgText::String temp = Value;
	//return item.createUTF8EncodedString();
    }
    else
    {
        text->setText(Value);
    }
    return true;
}

//2. изменение значения цвета текстового поля
bool BaseTextObject::SetTextColor(float r, float g, float b, float a)
{
	text->setColor(osg::Vec4(r,g,b,a));
	return true;
}

//3. изменение размера букв
bool BaseTextObject::SetTextSize(float size)
{
	text->setCharacterSize (size);
	return true;
}

//4. устанавливает шрифт (arial,gost ...)
bool BaseTextObject::SetTextFont(std::string FontName)
{
	osg::ref_ptr<osgText::Font> font = osgText::readFontFile(FontName);
	if (!font.valid())
	{
		std::cout << "Font not found: " << FontName << std::endl;
		return(0);
	}
	text->setFont (font);
	return true;
}

//5. задание равнения текста 0-слева 1-справа 2-по центру
bool BaseTextObject::SetTextAlignment(int align)
{
	if (align==0)
		text->setAlignment(osgText::Text::LEFT_BASE_LINE);
	else if (align==1) text->setAlignment(osgText::Text::RIGHT_BASE_LINE);
	else if (align==2) text->setAlignment(osgText::Text::CENTER_BASE_LINE);
	return true;
}

//6. устанавливает ориентацию текста  0-Горизонтально 1-вертикально 2-справо налево (по-арабски)
bool BaseTextObject::SetTextLayout(int layout)
{
	if (layout==0)
		text->setLayout(osgText::Text::LEFT_TO_RIGHT);
	else if (layout==1) text->setLayout(osgText::Text::VERTICAL);
        else if (layout==2)
        {
            text->setLayout(osgText::Text::RIGHT_TO_LEFT);
            text->setAlignment(osgText::Text::RIGHT_BASE_LINE);
        }
	return true;
}


//7. Выравнивание по осям (HUD и все такое)
//XY_PLANE 	-0	
//XZ_PLANE 	-1	
//YZ_PLANE 	-2	
//SCREEN 	-3
//USER_DEFINED_ROTATION -4

bool BaseTextObject::setAxisAlignment(int axis)
{
	switch ( axis ) 
	{
	case 0:
	  text->setAxisAlignment( osgText::TextBase::XY_PLANE );
	  break;
	case 1:
	  text->setAxisAlignment( osgText::TextBase::XZ_PLANE );
	  break;
	case 2:
	  text->setAxisAlignment( osgText::TextBase::YZ_PLANE );
	  break;
	case 3:
	  text->setAxisAlignment( osgText::TextBase::SCREEN );
	  break;
	case 4:
	  text->setAxisAlignment( osgText::TextBase::USER_DEFINED_ROTATION );
	  break;
	default:
	  break;
	}
	return true;
}



