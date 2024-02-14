/***************************************************************************
GraphicSceneClass.cpp  - класс описания 3d объектов сцены (геометрия)
-------------------
begin                : 21 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

#include "GraphicSceneClass.h"
#include <osgAnimation/MorphGeometry>
#include <osgWidget/Browser>

//углы
#include "./Angle.h"

GraphicSceneClass::GraphicSceneClass(osg::ref_ptr<osg::Group> mainRoot)
{
	std::cout << "GraphicSceneClass constructor ..." << std::endl;

        //osgDB::Registry::instance()->setBuildKdTreesHint(osgDB::ReaderWriter::Options::BUILD_KDTREES);

	root = mainRoot; 
}

GraphicSceneClass::~GraphicSceneClass()
{
	std::cout << "Destructor GraphicSceneClass."  << std::endl;
}

//загрузка файла и добавление в сцену
bool GraphicSceneClass::LoadGraphicFile(std::string filename)
{
	// load the scene.
	if (filename=="") return false;

	//char ppp[1024];
	//OemToChar(filename.c_str(),ppp);

	osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile(filename); //readNodeFiles
	#ifdef SHADOW
	/*
	if (filename.find("room.osg")!=std::string::npos)
	{
		loadedModel->setNodeMask(ReceivesShadowTraversalMask);
	}
	else
	{
		loadedModel->setNodeMask(CastsShadowTraversalMask ); //| ReceivesShadowTraversalMask
	}
	*/
	#endif
	
	if (!loadedModel.valid())
	{
		std::cout << filename <<": No data loaded." << std::endl;
		return false;
	}

        //QT!!!
        #ifdef QTBUG
            osg::ref_ptr<FindAndUnrefTextureVisitor> fnnv = new FindAndUnrefTextureVisitor();
            loadedModel->accept(*fnnv.get());
        #endif
        //

	if(m_observer) m_observer->on_graphic_scene_class_file_loaded(loadedModel.get());

	// add loaded model to root scene
	root->addChild (loadedModel);
	return true;
}

//удаление и выгрузка из сцены всех загруженных файлов
bool GraphicSceneClass::Reset()
{
	return true;
}


bool GraphicSceneClass::ChangeShaderUniform(std::string groupname ,std::string uniformname,float value)
{
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(groupname);
    root->accept(*fnnv.get());
	if (!fnnv->_foundNodes.empty())
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << groupname << std::endl;
		
		osg::ref_ptr<ChangeShaderUniformVisitor> visitor = new  ChangeShaderUniformVisitor(uniformname,value);
		fnnv->_foundNodes.front().get()->accept(*visitor.get());
	}
	else
	{
		std::cout << "Find objects in Root - FALSE! " << groupname << std::endl;
		return false;
	}
	return true;
}

//Функция для перемещения объекта (3d)
bool GraphicSceneClass::RSetPosition(std::string ObjectName, float dx, float dy, float dz)
{
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(ObjectName);
    root->accept(*fnnv.get());
	if (!fnnv->_foundNodes.empty())
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
		fnnv->_foundNodes.front().get()->setMatrix(fnnv->_foundNodes.front().get()->getMatrix() * osg::Matrix::translate(dx,dy,dz));
	}
	else
	{
		std::cout << "Find objects in Root - FALSE! " << ObjectName << std::endl;
		//delete fnnv;
		return false;
	}
	//delete fnnv;
	return true;
}

//Функция совмещения точки привязки одного объекта с другим
bool GraphicSceneClass::RSetAt(std::string ObjectName , std::string ObjectName2)
{
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(ObjectName);
    root->accept(*fnnv.get());
	osg::ref_ptr<FindNamedNodeVisitor> fnnv2 = new FindNamedNodeVisitor(ObjectName2);
    root->accept(*fnnv2.get());

	if ((!fnnv->_foundNodes.empty())&&(!fnnv2->_foundNodes.empty()))
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
		if (fnnv2->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName2 << std::endl;
		osg::Matrix matrix2 = fnnv2->_foundNodes.front().get()->getMatrix();
		
		osg::Vec3d scale2 = matrix2.getScale();
		osg::Matrix matrix = fnnv->_foundNodes.front().get()->getMatrix();
		osg::Vec3d scale = matrix.getScale();

		osg::Vec3d scale3 = osg::Vec3d( scale.x() / scale2.x() , scale.y() / scale2.y(), scale.z() / scale2.z() );
		matrix2.preMult(osg::Matrix::scale(scale3));
		fnnv->_foundNodes.front().get()->setMatrix(matrix2);
		//delete fnnv;
		//delete fnnv2;
		return true;
	}
	else
	{
		std::cout << "Find objects in Root - FALSE! " << ObjectName << " or " << ObjectName2 << std::endl;
		//delete fnnv;
		//delete fnnv2;
		return false;
	}
}

//Функция совмещения точки привязки одного объекта с другим
bool GraphicSceneClass::RResetObject(std::string ObjectName,bool position,bool rotate,bool scale)
{
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(ObjectName);
    root->accept(*fnnv.get());
	if (!fnnv->_foundNodes.empty())
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
		//osg::Matrix mat;
		osg::Matrix mat = osg::Matrix::identity();
		//fnnv->_foundNodes.front().get()->setMatrix
		
		if (!position) mat.preMult (osg::Matrix::translate(fnnv->_foundNodes.front().get()->getMatrix().getTrans()));
		if (!rotate) mat.preMult (osg::Matrix::rotate(fnnv->_foundNodes.front().get()->getMatrix().getRotate()));
		if (!scale) mat.preMult (osg::Matrix::scale(fnnv->_foundNodes.front().get()->getMatrix().getScale()));

		fnnv->_foundNodes.front().get()->setMatrix (mat);
	}
	else
	{
		std::cout << "Find objects in Root - FALSE! " << ObjectName << std::endl;
		//delete fnnv;
		return false;
	}
	//delete fnnv;
	return true;
}

//альтернативное вращение 3D объекта
bool GraphicSceneClass::RSetRotate(std::string ObjectName, float angle1, float angle2, float angle3)
{
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(ObjectName);
        root->accept(*fnnv.get());
	if (!fnnv->_foundNodes.empty())
	{
                //std::cout << "CASHE size=" << FindNamedNodeVisitor::getCacheSize() <<  std::endl;
                //FindNameCache[ObjectName]._foundNodes = fnnv->_foundNodes;
                //FindNameCache[ObjectName].accumulated = fnnv->accumulated;


		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
		osg::Matrix matrix = fnnv->_foundNodes.front().get()->getMatrix();
		if (angle1!=0)	matrix.preMult(osg::Matrix::rotate(angle1,osg::Vec3d(1,0,0)));
		if (angle2!=0)	matrix.preMult(osg::Matrix::rotate(angle2,osg::Vec3d(0,1,0)));
		if (angle3!=0)	matrix.preMult(osg::Matrix::rotate(angle3,osg::Vec3d(0,0,1)));
		fnnv->_foundNodes.front().get()->setMatrix(matrix);
	}
	else
	{
		std::cout << "Find objects in Root - FALSE! " << ObjectName << std::endl;
		//delete fnnv;
		return false;
	}
	//delete fnnv;
	return true;
}

//альтернативное cложное вращение объекта
bool GraphicSceneClass::RSetRotate2(std::string ObjectName,float angle1, float x1, float y1, float z1, float angle2, float x2, float y2, float z2,float angle3, float x3, float y3, float z3)
{
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(ObjectName);
    root->accept(*fnnv.get());
	if (!fnnv->_foundNodes.empty())
	{
		//std::cout << "Find object in Root - OK!" << std::endl;
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
		osg::Matrix matrix = fnnv->_foundNodes.front().get()->getMatrix();
		matrix.preMult(osg::Matrix::rotate(angle1,osg::Vec3d(x1,y1,z1),angle2,osg::Vec3d(x2,y2,z2),angle3,osg::Vec3d(x3,y3,z3)));
		fnnv->_foundNodes.front().get()->setMatrix(matrix);
	}
	else
	{
		std::cout << "Find objects in Root - FALSE! " << ObjectName << std::endl;
	}
	//delete fnnv;
	return true;
}


//альтернативное перемещение относительно другого объекта по абсолютным координатам
bool GraphicSceneClass::ASetAt(std::string ObjectName, std::string ObjectName2)
{
        osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(ObjectName);
    root->accept(*fnnv.get());
        osg::ref_ptr<FindNamedNodeVisitor> fnnv2 = new FindNamedNodeVisitor(ObjectName2, true);
    root->accept(*fnnv2.get());
	if ((!fnnv->_foundNodes.empty())&&(!fnnv2->_foundNodes.empty()))
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
		if (fnnv2->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName2 << std::endl;
		osg::Matrix matrix2 = fnnv2->accumulated;
		osg::Vec3d scale2 = matrix2.getScale();
		osg::Matrix matrix = fnnv->_foundNodes.front().get()->getMatrix();
		osg::Vec3d scale = matrix.getScale();
		osg::Vec3d scale3 = osg::Vec3d( scale.x() / scale2.x() , scale.y() / scale2.y(), scale.z() / scale2.z() );
		matrix2.preMult(osg::Matrix::scale(scale3));
		fnnv->_foundNodes.front().get()->setMatrix(matrix2);
	}
	else
	{
		std::cout << "Find objects in Root - FALSE! " << ObjectName << " or " << ObjectName2 << std::endl;
		//delete fnnv;
		//delete fnnv2;
		return false;
	}
	//delete fnnv;
	//delete fnnv2;
	return true;
}

//альтернативное задание видимости объекта
bool GraphicSceneClass::RSetVisible (std::string ObjectName,bool visible)
{
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(ObjectName);
    root->accept(*fnnv.get());
	if (!fnnv->_foundNodes.empty())
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
		if (visible==false)
		{
			fnnv->_foundNodes.front().get()->setNodeMask(0x000000);
		}
		else 
		{
			fnnv->_foundNodes.front().get()->setNodeMask(0xffffff);
		}
	}
	else
	{
		std::cout << "Find objects in Root - FALSE! " << ObjectName << std::endl;
		//delete fnnv;
		return false;
	}
	//delete fnnv;
	return true;
}

//альтернативное масштабирование объекта
bool GraphicSceneClass::RSetScale(std::string ObjectName, float dx, float dy, float dz)
{
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(ObjectName);
    root->accept(*fnnv.get());
	if (!fnnv->_foundNodes.empty())
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
		osg::Matrix matrix = fnnv->_foundNodes.front().get()->getMatrix();
		matrix.preMult(osg::Matrix::scale(dx,dy,dz));
		fnnv->_foundNodes.front().get()->setMatrix(matrix);
	}
	else
	{
		std::cout << "Find objects in Root - FALSE! " << ObjectName << std::endl;
		//delete fnnv;
		return false;
	}
	//delete fnnv;
	return true;
}

//альтернативная передача x,y,z объекта
bool GraphicSceneClass::RGetPosition(std::string ObjectName, float &x, float& y, float& z)
{
        osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(ObjectName, false);
    root->accept(*fnnv.get());
	if (!fnnv->_foundNodes.empty())
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
		//std::cout << "Find object in Root - OK!" << std::endl;
		osg::Vec3f xyz =  fnnv->_foundNodes.front().get()->getMatrix().getTrans();
		x=xyz.x();
		y=xyz.y();
		z=xyz.z();
	}
	else
	{
		std::cout << "Find objects in Root - FALSE! " << ObjectName << std::endl;
		x=0;
		y=0;
		z=0;
		//delete fnnv;
		return false;
	}
	//delete fnnv;
	return true;
}

//альтернативная передача x,y,z объекта - абсолютные координаты
bool GraphicSceneClass::AGetPosition(std::string ObjectName, float &x, float& y, float& z)
{
    osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(ObjectName, true);
    root->accept(*fnnv.get());
	if (!fnnv->_foundNodes.empty())
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
		osg::Vec3f xyz =  fnnv->accumulated.getTrans();
		x=xyz.x();
		y=xyz.y();
		z=xyz.z();
	}
	else
	{
		std::cout << "Find objects in Root - FALSE! " << ObjectName << std::endl;
		x=0;
		y=0;
		z=0;
		//delete fnnv;
		return false;
	}
	//delete fnnv;
	return true;
}

//альтернативное получение масштаба объекта
bool GraphicSceneClass::RGetScale(std::string ObjectName, float &x, float& y, float& z)
{
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(ObjectName);
    root->accept(*fnnv.get());
	if (!fnnv->_foundNodes.empty())
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
		osg::Vec3f xyz =  fnnv->_foundNodes.front().get()->getMatrix().getScale();
		x=xyz.x();
		y=xyz.y();
		z=xyz.z();
	}
	else
	{
		std::cout << "Find objects in Root - FALSE! " << ObjectName << std::endl;
		x=0;
		y=0;
		z=0;
		//delete fnnv;
		return false;
	}
	//delete fnnv;
	return true;
}

//альтернативное получение квантериона вращения объекта
bool GraphicSceneClass::RGetRotate(std::string ObjectName, float &x, float& y, float& z, float &w)
{
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(ObjectName);
    root->accept(*fnnv.get());
	if (!fnnv->_foundNodes.empty())
	{
            if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
            osg::Quat rotate = fnnv->_foundNodes.front().get()->getMatrix().getRotate();

            /*
            {
                double heading, attitude,  bank;
                getEulerFromQuat (rotate, heading, attitude,  bank);

                x= heading;
                y= attitude;
                z= bank;
                w=1;
            }
            */


            x=rotate.x();
            y=rotate.y();
            z=rotate.z();
            w=rotate.w();
	}
	else
	{
            std::cout << "Find objects in Root - FALSE! " << ObjectName << std::endl;
            x=0;
            y=0;
            z=0;
            w=0;
            //delete fnnv;
            return false;
	}
	//delete fnnv;
	return true;
}

//-------------------------------рисование-------------------------------------
//альтернативное рисование на текстуре объекта
bool GraphicSceneClass::RTextureDraw(std::string ObjectName, int x, int y, float r,float g,float b,float a,bool flush,unsigned int UnitNumber)
{
	//Находим объект
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(ObjectName);
    root->accept(*fnnv.get());
	//Если такой объект есть, ищем текстуру
	if (!fnnv->_foundNodes.empty())
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
		osg::ref_ptr<FindTextureVisitor> zzz = new FindTextureVisitor(UnitNumber);
		fnnv->_foundNodes.front().get()->accept (*zzz.get());
		//Если текстура есть
		if (zzz->image.get()) 
		{
			//std::cout << "texture find";
			//std::cout << zzz->image->s() << std::endl; //ширина
			//std::cout << zzz->image->t() << std::endl; //высота
			// std::cout << zzz->image->r() << std::endl; //глубина

			int format = zzz->image->getPixelSizeInBits();
			//std::cout <<"getPixelSizeInBits : "<< format <<std::endl;
            int width = zzz->image->s();
			int heught = zzz->image->t();
			//
			GLubyte *ptr;
			int inc=0;
			ptr = zzz->image->data();
			if (x>=width) x= 0;
			if (y>=heught) y= 0;
			if (x<0) x= 0;
			if (y<0) y= 0;
			inc=(y*width + x)*format/8; //смещение по RGBARGBARGBA... или 4
			*(ptr+inc+0) = (GLubyte)(r*255.0f);
			*(ptr+inc+1) = (GLubyte)(g*255.0f);
			*(ptr+inc+2) = (GLubyte)(b*255.0f);
			//если RGBA
                        //std::cout << "rgba=" << format/8 << std::endl;
                        //std::cout << "rgba2=" << format << std::endl;
            if (format==32)
			{
				*(ptr+inc+3) = (GLubyte)(a*255.0f);
			}
			//Отправляем текстуру в видеопамять)
			if (flush)
			{
                                //osg::Image* dest =dynamic_cast<osg::Image*> (zzz->image->clone(osg::CopyOp::DEEP_COPY_ALL));
                                //zzz->texture->setImage(dest);//zzz->image
                            
                            zzz->image->dirty();
                            //zzz->texture->dirtyTextureObject();

                            //Добавил
                                /*
                                zzz->stateset->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
                                zzz->stateset->setRenderBinDetails( 2, "RenderBin" );
                                zzz->stateset->setMode( GL_CULL_FACE, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );
                                zzz->stateset->setMode(GL_BLEND, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
                                zzz->stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
                                zzz->texture->dirtyTextureObject();

                                zzz->material->setAlpha(osg::Material::FRONT_AND_BACK, 0.8);
                                zzz->stateset->setAttributeAndModes(zzz->material,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
                                osg::BlendFunc* bf = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA );
                                zzz->stateset->setAttributeAndModes(bf);
	
				// dirty the image (increments the modified count) so that any textures
				// using the image can be informed that they need to update.
                                */
				



				//zzz->texture->setDataVariance(osg::Object::DYNAMIC); 
                                //zzz->stateset->setTextureAttributeAndModes(UnitNumber, zzz->texture, osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
                                //zzz->drawable->setStateSet(zzz->stateset);
			}
		}
		else
		{
			std::cout << "texture NOT find";
			//delete zzz;
			//delete fnnv;
			return false;
		}
		//Чистим мусор
		//delete zzz;
	}
	else
	{
		std::cout << "Find objects in Root - FALSE! " << ObjectName << std::endl;
		//delete fnnv;
		return false;
	}
	//Чистим мусор
	//delete fnnv;
	return true;
}

//альтернативная замена текстуры  объекта
bool GraphicSceneClass::RChangeTexture(std::string ObjectName, std::string filename,unsigned int UnitNumber)
{
	//Находим объект
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(ObjectName);
    root->accept(*fnnv.get());
	//Если такой объект есть, ищем текстуру
	if (!fnnv->_foundNodes.empty())
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
		osg::ref_ptr<FindTextureVisitor> zzz = new FindTextureVisitor(UnitNumber);
		fnnv->_foundNodes.front().get()->accept (*zzz.get());
		//Если текстура есть
		if (zzz->texture.get()) 
		{
			osg::ref_ptr<osg::Image> image = osgDB::readImageFile(filename);
			/*
			osg::ImageStream* imagestream = dynamic_cast<osg::ImageStream*>(image);
			if (imagestream) 
			{
				std::cout << "VIDEO!!!!!" << std::endl;
				imagestream->play();
			}
			*/

			//Отправляем текстуру в видеопамять)
			if (image.get())
			{
				zzz->texture->setImage(image.get());
				// dirty the image (increments the modified count) so that any textures
				// using the image can be informed that they need to update.
				image->dirty();
			}
			else
			{
				std::cout << "NOT find image file for texture";
				//delete zzz;
				//delete fnnv;
				return false;
			}
		}
		else
		{
			std::cout << "texture NOT find";
		}
		//Чистим мусор
		//delete zzz;
	}
	else
	{
		std::cout << "Find objects in Root - FALSE! " << ObjectName << std::endl;
		//delete fnnv;
		return false;
	}
	//Чистим мусор
	//delete fnnv;
	return true;
}


bool GraphicSceneClass::RChangeTexture( const std::string& str_object_name, std::stringstream& ss,
                                                                           const std::string& str_extension, unsigned int UnitNumber )
{
        FindNamedNodeVisitor fnnv( str_object_name );
        root->accept( fnnv );

        if( !fnnv._foundNodes.empty() )
        {
                if( fnnv._foundNodes.size() > 1 ) std::cout << "Warning: Duplicate names - " << str_object_name << std::endl;

                FindTextureVisitor ftv( UnitNumber );
                fnnv._foundNodes.front().get()->accept( ftv );
                if( ftv.texture.get() )
                {
                        // find plugin
                        osgDB::ReaderWriter* plugin = osgDB::Registry::instance()->getReaderWriterForExtension( str_extension );

                        // read image
                        osgDB::ReaderWriter::ReadResult result = plugin->readImage( ss );

                        osg::Image* p_image = result.getImage();
                        ftv.texture->setImage( p_image );

                        // dirty the image (increments the modified count) so that any textures
                        // using the image can be informed that they need to update.
                        p_image->dirty();
                }
        }
        else
        {
                std::cout << "Error to find 3d-object in root: " << str_object_name << std::endl;
                return false;
        }

        return true;
}


//альтернативная Установка цвета объекта
bool GraphicSceneClass::RSetColor(std::string ObjectName, float r,float g ,float b ,float a)
{
	//Находим объект
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(ObjectName);
    root->accept(*fnnv.get());
	//Если такой объект есть, ищем текстуру и материал
	if (!fnnv->_foundNodes.empty())
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
		osg::ref_ptr<FindTextureVisitor> zzz = new FindTextureVisitor();
		fnnv->_foundNodes.front().get()->accept (*zzz.get());
		//Если материал есть
		if (zzz->material.get()) 
		{
			//Если надо .. включаем смешивание
			if (a<1)
			{
				zzz->stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
				zzz->stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
			}
			zzz->material->setDiffuse( osg::Material::FRONT_AND_BACK, osg::Vec4( r, g, b, a ) );
			zzz->material->setAmbient( osg::Material::FRONT_AND_BACK, osg::Vec4( r, g, b, a ) );
		}
		else
		{
			std::cout << "material NOT find";
			//delete zzz;
			//delete fnnv;
			return false;
		}
		//А если есть и текстура ...... (альфа канал)
		//Если текстура есть
		if (zzz->image.get()) 
		{
                        //std::cout << "RSetColor..." << "found texture" << std::endl;
			int format = zzz->image->getPixelSizeInBits();
                        //std::cout << " format = " << format << std::endl;
			if ((format)<=24)
			{
                                //std::cout << "RSetColor..." << "no alpha channel" << std::endl;
			}
			else
			{
                                //std::cout << "RSetColor..." << "found alpha channel" << std::endl;
				//
				unsigned int width = zzz->image->s(); //ширина
				unsigned int heught = zzz->image->t();//высота
				//
				GLubyte *ptr;
				ptr = zzz->image->data();
				int inc;

				for (unsigned int x=0;x<width-1;x++)
				{
					for(unsigned int y=0;y<heught-1;y++)
					{
						inc=(y*width + x)*format/8; //смещение по RGBARGBARGBA... или 4
						//*(ptr+inc+0) = (GLubyte)(r*255.0f);  //R
						//*(ptr+inc+1) = (GLubyte)(g*255.0f);  //G
						//*(ptr+inc+2) = (GLubyte)(b*255.0f);  //B
						*(ptr+inc+3) = (GLubyte)(a*255.0f);	   //A
					}
				}
				//Отправляем текстуру в видеопамять)
				zzz->image->dirty();
			}
		}
		//Чистим мусор
		//delete zzz;
	}
	else
	{
		std::cout << "Find objects in Root - FALSE! " << ObjectName << std::endl;
		//delete fnnv;
		return false;
	}
	//Чистим мусор
	//delete fnnv;
	return true;
}

bool GraphicSceneClass::RConnectObjectTo(std::string ObjectName, std::string ObjectName2)
{
	if (ObjectName==ObjectName2) 
	{
		std::cout << "RConnectObjectTo: " << ObjectName << "=="  << ObjectName2 << std::endl;
		return false;
	}

        try
        {
            //запретить переностить HUD, курсор, 0 плоскость куда либо
            if ((ObjectName=="__NullPlane__") || (ObjectName=="__CURSOR__") || (ObjectName=="HUD") || (ObjectName=="__CURSORFORHUD__AIR") || (ObjectName=="__CURSOR_CAMERA__") || (ObjectName=="CURSOR_AIR") )
            {
                throw (ErrorClass (__FILE__, __LINE__ , "GraphicSceneClass::RConnectObjectTo", "RConnectObjectTo: " + ObjectName + " is system object!", 3));
            }
            //запретить переностить В курсор и 0 плоскость что либо
            if ((ObjectName2=="__NullPlane__") || (ObjectName2=="__CURSOR__"))
            {
                throw (ErrorClass (__FILE__, __LINE__ , "GraphicSceneClass::RConnectObjectTo", "RConnectObjectTo: " + ObjectName2 + " is system object!", 3));
            }
        }
        catch (ErrorClass error)
        {
            error.print();
        }


	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(ObjectName);
        root->accept(*fnnv.get());
	//подчиняется ..
	osg::ref_ptr<FindNamedNodeVisitor> fnnv2 = new FindNamedNodeVisitor(ObjectName2);
        root->accept(*fnnv2.get());
	if ((!fnnv->_foundNodes.empty())&&(!fnnv2->_foundNodes.empty()))
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
		if (fnnv2->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName2 << std::endl;
		osg::Node::ParentList parents = fnnv->_foundNodes.front().get()->getParents();
		for(osg::Node::ParentList::iterator pitr=parents.begin();
			pitr!=parents.end();
		    ++pitr)
                {
                        (*pitr)->removeChild(fnnv->_foundNodes.front().get());
                }
		fnnv2->_foundNodes.front().get()->addChild(fnnv->_foundNodes.front().get());
		//ДОБАВИТЬ
		fnnv->_foundNodes.front().get()->setMatrix(osg::Matrix::translate(0.0f,0.0f,0.0f)*
                                     osg::Matrix::scale(1.0f,1.0f,1.0f)*
                                     osg::Matrix::rotate(0.0f,0.0f,0.0f,1.0f));
		/*
		//Если это проекция на экран
		if (ObjectName2=="HUD")
		{
			std::cout << "add 0xF0000000..." << std::endl;
			fnnv->_foundNodes.front().get()->setNodeMask (0xF0000000);
		}
		*/
		
	}
	else
	{
		std::cout << "Find objects in Root - FALSE! " << ObjectName << " or " << ObjectName2 << std::endl;
		//delete fnnv;
		//delete fnnv2;
		return false;
	}
	//delete fnnv;
	//delete fnnv2;
	return true;
}
//-----------------------------------------------------------------------------
//Тест на пересечение 2 объектов в относительных координатах
bool GraphicSceneClass::RTestIntersects(std::string ObjectName, std::string ObjectName2, float koeff)
{
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(ObjectName);
    root->accept(*fnnv.get());
	osg::ref_ptr<FindNamedNodeVisitor> fnnv2 = new FindNamedNodeVisitor(ObjectName2);
    root->accept(*fnnv2.get());
	bool test=false;
	if ((!fnnv->_foundNodes.empty())&&(!fnnv2->_foundNodes.empty()))
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
		if (fnnv2->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName2 << std::endl;
		// get the bounds of the model.    
		osg::ComputeBoundsVisitor cbbv1;
		fnnv->_foundNodes.front().get()->accept(cbbv1);
		osg::BoundingBox bb1 = cbbv1.getBoundingBox();
		// get the bounds of the model.    
		osg::ComputeBoundsVisitor cbbv2;
		fnnv2->_foundNodes.front().get()->accept(cbbv2);
		osg::BoundingBox bb2 = cbbv2.getBoundingBox();
		if(bb1.valid()&&bb2.valid())
		{
			test = bb1.intersects(bb2);
		}
		else
		{
			std::cout << "get Bounding box - FALSE!" << std::endl;
			test=false;
			return test;
		}
	}
	else
	{
		std::cout << "Find objects in Root - FALSE! " << ObjectName << " or " << ObjectName2 << std::endl;
		test=false;
		return test;
	}
	return test;
}

//-----------------------------------------------------------------------------
//Тест на пересечение 2 объектов в относительных координатах
bool GraphicSceneClass::RTestIntersects2(std::string ObjectName, std::string ObjectName2, float koeff)
{
   return false;
}

//Тест на пересечение линией
bool GraphicSceneClass::RTestLineIntersects(float x1,float y1,float z1,float x2,float y2,float z2, std::string proxyObjectName, std::string& _FindObjectName , float& _gx, float& _gy, float& _gz, float& _lx, float& _ly, float& _lz)
{
	if (proxyObjectName=="") proxyObjectName = "Не может быть никогда!";
	osgUtil::IntersectVisitor isectVisitor;
	osg::ref_ptr<osg::LineSegment> lineIsect[1];
	//создание сегмента линии
	lineIsect[0] = new osg::LineSegment(osg::Vec3f(x1, y1, z1) ,osg::Vec3f(x2, y2, z2) );
	//включение сегмента в IntersectVisitor
	isectVisitor.addLineSegment(lineIsect[0].get());
	// проверка на пересечение со всеми объектами сцены
	root->accept(isectVisitor);
	//получение объектов, пересеченных сегментом линии с индексом 0
	osgUtil::IntersectVisitor::HitList hitList = isectVisitor.getHitList(lineIsect[0].get());
	//перебор объектов ( начиная с самого ближнего по пересечению и  заканчивая самим дальним)
	std::string nameObject;
	std::string parentObject;
	for(osgUtil::IntersectVisitor::HitList::iterator hitr=hitList.begin();	hitr!=hitList.end(); ++hitr)
	{
		parentObject = "";
		nameObject="";
		//определяем имя
		if (!hitr->getNodePath().empty() && !(hitr->getNodePath().back()->getName().empty()))
		{
			//запоминаем имя объекта, на который прицелен курсор
			nameObject = hitr->getNodePath().back()->getName();
		}
		else if (hitr->getDrawable())
		{
			//отображаем предка _geode
			//запоминаем имя объекта, на который прицелен курсор
			nameObject = hitr->getNodePath().back()->getParent(0)->getName();
			parentObject = hitr->getNodePath().back()->getParent(0)->getParent(0)->getName();
		}
		if ((nameObject=="")&&(parentObject!=""))
		{
			nameObject=parentObject;
		}
		//Курсор является "прозрачным"
		if ((nameObject=="Sphere01") || (nameObject == proxyObjectName )) continue;
		//Передача 
		osg::Vec3d GlobalIntersectPosition = hitr->getWorldIntersectPoint();
		osg::Vec3d LocalIntersectPosition = hitr->getLocalIntersectPoint();
		//да. пересекает
		//название
		_FindObjectName = nameObject;
		//Глобальные коорд пересечения
		_gx = GlobalIntersectPosition.x();
		_gy = GlobalIntersectPosition.y();
		_gz = GlobalIntersectPosition.z();
		//Локальные коорд пересечения
		_lx = LocalIntersectPosition.x();
		_ly = LocalIntersectPosition.y();
		_lz = LocalIntersectPosition.z();
		return true;
	}
	//если ничего нет...
	//название
	_FindObjectName = "";
	//Глобальные коорд пересечения
	_gx = 0;
	_gy = 0;
	_gz = 0;
	//Локальные коорд пересечения
	_lx = 0;
	_ly = 0;
	_lz = 0;
	return false;	
}

//возвращает предка (точнее ближайшего предка имеющего имя)
std::string  GraphicSceneClass::GetParent (std::string ObjectName)
{
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(ObjectName);
    root->accept(*fnnv.get());
	if (!fnnv->_foundNodes.empty())
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
	}
	else
	{
		std::cout << "Find objects in Root - FALSE! " << ObjectName << std::endl;
		//delete fnnv;
		return "";
	}
	osg::Node::ParentList list =  fnnv->_foundNodes.front().get()->getParents ();
	std::string ParentName;
	while (!list.empty())
	{
		std::string parentname = list.at(0)->getName();
		if (parentname!="")
		{
			ParentName = parentname;
			break;
		}
		//
		list = list.at(0)->getParents();
	}
	//delete fnnv;
	return ParentName;
}


//---------------------Shaders-------------------------------------------------

//-----------------------------------------------------------------------------
//установка паузы анимации, снятие паузы
bool GraphicSceneClass::SetPauseAnimation(std::string ObjectName, bool pause, bool allChildren=false)
{
	//Находим объект
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(ObjectName);
    root->accept(*fnnv.get());
	//Если такой объект есть, ищем анимацию
	if (!fnnv->_foundNodes.empty())
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
		osg::ref_ptr<FindAnimationVisitor> zzz = new FindAnimationVisitor();
		fnnv->_foundNodes.front().get()->accept (*zzz.get());
		//анимация есть?
		if (zzz->_foundNodes.empty())
		{
			std::cout << "animation NOT found";
			//delete zzz;
			//delete fnnv;
			return false;
		}
		
		unsigned int max;
		if (allChildren==true) 
		{
			//если применить команду ко всем анимациям детей объекта.....
			max = zzz->_foundNodes.size();
		}
		else
		{
			//или только собственно к своей анимации
			max=1;
		}
		//применяем
		for (unsigned int i=0; i<max;i++)
		{
			zzz->_foundNodes.at(i)->setPause (pause);
		}
		//Чистим мусор
		//delete zzz;
	}
	else
	{
		std::cout << "Find objects in Root - FALSE! " << ObjectName << std::endl;
		//delete fnnv;
		return false;
	}
	//Чистим мусор
	//delete fnnv;
	return true;
}

//Переход на нужное время анимации
bool GraphicSceneClass::GotoAnimation(std::string ObjectName, float new_t, bool allChildren=false)
{
	//Находим объект
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(ObjectName);
    root->accept(*fnnv.get());
	//Если такой объект есть, ищем анимацию
	if (!fnnv->_foundNodes.empty())
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
		osg::ref_ptr<FindAnimationVisitor> zzz = new FindAnimationVisitor();
		fnnv->_foundNodes.front().get()->accept (*zzz.get());
		//Если анимация есть
		//анимация есть?
		if (zzz->_foundNodes.empty())
		{
			std::cout << "animation NOT found";
			//delete zzz;
			//delete fnnv;
			return false;
		}

		unsigned int max;
		if (allChildren==true) 
		{
			//если применить команду ко всем анимациям детей объекта.....
			max = zzz->_foundNodes.size();
		}
		else
		{
			//или только собственно к своей анимации
			max=1;
		}

		//применяем или к одному или ко всем анимациям
		for (unsigned int i=0; i<max;i++)
		{
			osg::AnimationPath::ControlPoint controlPoint;
			bool test =zzz->_foundNodes.at(i)->getAnimationPath()->getInterpolatedControlPoint (new_t, controlPoint);
			if (test)
			{
				osg::Matrix matrix ;
				controlPoint.getMatrix(matrix);
				//fnnv->_foundNodes.at(i).get()->setMatrix(matrix);
				zzz->_foundTransforms.at(i).get()->asMatrixTransform()->setMatrix(matrix);
			}
		}
		//Чистим мусор
		//delete zzz;
	}
	else
	{
		std::cout << "Find objects in Root - FALSE! " << ObjectName << std::endl;
		//delete fnnv;
		return false;
	}
	//Чистим мусор
	//delete fnnv;
	return true;
}

//Установка опорного смещения анимации
bool GraphicSceneClass::SetPivotPointAnimation(std::string ObjectName, float new_x,float new_y,float new_z)
{
	//Находим объект
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(ObjectName);
    root->accept(*fnnv.get());
	//Если такой объект есть, ищем анимацию
	if (!fnnv->_foundNodes.empty())
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
		osg::ref_ptr<FindAnimationVisitor> zzz = new FindAnimationVisitor();
		fnnv->_foundNodes.front().get()->accept (*zzz.get());
		//Если анимация есть
		//анимация есть?
		if (zzz->_foundNodes.empty())
		{
			std::cout << "animation NOT found";
			//delete zzz;
			//delete fnnv;
			return false;
		}
		//применяем
		zzz->_foundNodes.front()->setPivotPoint(osg::Vec3d(new_x,new_y,new_z));
		//Чистим мусор
		//delete zzz;
	}
	else
	{
		std::cout << "Find objects in Root - FALSE! " << ObjectName << std::endl;
		//delete fnnv;
		return false;
	}
	//Чистим мусор
	//delete fnnv;
	return true;
}


//-----------------------------------------morph--------------------------------------------------------------
// поиск matrix transform
osg::MatrixTransform* GraphicSceneClass::find_matrix_transform( const std::string& str_obj_name )
{
        FindNamedNodeVisitor fnnv( str_obj_name );
        root->accept( fnnv );
        if( fnnv._foundNodes.empty() )
        {
                std::cout << "Find objects in Root - FALSE! " << str_obj_name << std::endl;
                return NULL;
        }

        if( fnnv._foundNodes.size() > 1 )
        {
                std::cout << "Warning: Duplicate names - " << str_obj_name << std::endl;
        }

        return fnnv._foundNodes.front().get();
}

// поиск шейпа
osg::Geometry* getShape( osg::MatrixTransform* node )
{
        FindGeometryVisitor finder;
        node->accept( finder );
        if( !finder.geom )
        {
            std::cout << "Morph shape: " << node->getName() << std::endl;
        }
        return finder.geom.get();
}

// создание morph-анимации
void GraphicSceneClass::CreateMorph( const std::string& str_morph, const std::string& str_default_state,
                                                                         const std::vector<std::string>& v_str_target_states )
{
        osg::MatrixTransform* default_state_node = find_matrix_transform( str_default_state );
        if( default_state_node )
        {
            // отсоединяем default_state_node от родительских узлов
            osg::Node::ParentList parents = default_state_node->getParents();

            //std::cout << "Parents size: " << parents.size() << std::endl;

            for( osg::Node::ParentList::iterator pitr=parents.begin(); pitr!=parents.end(); ++pitr )
            {
                    (*pitr)->removeChild( default_state_node );
            }

            // создаем morph-геометрию
            osgAnimation::MorphGeometry* morph = new osgAnimation::MorphGeometry( *getShape( default_state_node ) );

            // создаём новый узел
            osg::MatrixTransform* group_morph = new osg::MatrixTransform;
            group_morph->setName( str_morph );
            group_morph->setMatrix( default_state_node->getMatrix() );

            osg::Geode* geode_morph = new osg::Geode;
            //geode_morph->setName( "geo test" );
            geode_morph->addDrawable( morph );
            group_morph->addChild( geode_morph );


            // прицепляем к ним geode
            for( osg::Node::ParentList::iterator pitr=parents.begin(); pitr!=parents.end(); ++pitr )
            {
                    (*pitr)->addChild( group_morph );
            }

            // и default_state_node добавляем к geode
            group_morph->addChild( default_state_node );
            default_state_node->setNodeMask( 0x000000 );


            for( std::vector<std::string>::const_iterator itera = v_str_target_states.begin(),
                    itera_end = v_str_target_states.end();  itera != itera_end; ++itera )
            {
                    // отсоединяем от родительских узлов
                    osg::MatrixTransform* node = find_matrix_transform( *itera );
                    parents = node->getParents();
                    for( osg::Node::ParentList::iterator pitr=parents.begin(); pitr!=parents.end(); ++pitr )
                    {
                            (*pitr)->removeChild( node );
                    }

                    // добавляем к morph'y
                    morph->addMorphTarget( getShape( node ), 0.f );

                    // и node добавляем к geode
                    group_morph->addChild( node );
                    node->setNodeMask( 0x000000 );
            }
        }
}

// visitor, обнаруживающий osgAnimation::MorphGeometry
struct FindMorphGeometryVisitor: public osg::NodeVisitor
{
        osgAnimation::MorphGeometry* _morph_geom;
        FindMorphGeometryVisitor(): osg::NodeVisitor( osg::NodeVisitor::TRAVERSE_ALL_CHILDREN ), _morph_geom( NULL )
        {
                setTraversalMask( 0xffffffff );
                setNodeMaskOverride( 0xffffffff );
        }

        virtual void apply( osg::MatrixTransform& node )
        {
                node.traverse( *this );
        }

        virtual void apply( osg::Geode& node )
        {
                if( NULL == _morph_geom )
                {
                        for( unsigned int i = 0; i < node.getNumDrawables(); i++ )
                        {
                                osgAnimation::MorphGeometry* geom = dynamic_cast<osgAnimation::MorphGeometry*>( node.getDrawable(i) );
                                if( geom )
                                {
                                        _morph_geom = geom;
                                        return;
                                }
                        }
                }
        }
};

// переход на нужное время morph-анимации
void GraphicSceneClass::GotoMorphState( const std::string& str_morph, const std::vector<float>& v_weights )
{
        osg::MatrixTransform* morph_node = find_matrix_transform( str_morph );
        if( morph_node )
        {
            // ищем morph geometry
            FindMorphGeometryVisitor fmgv;
            morph_node->accept( fmgv );

            if( fmgv._morph_geom )
            {
                    // обновляем веса
                    for( size_t i=0, end_i=v_weights.size(); i<end_i; i++ )
                    {
                            fmgv._morph_geom->setWeight( i, v_weights[i] );
                    }

                    for( size_t i=v_weights.size(), end_i=fmgv._morph_geom->getMorphTargetList().size(); i<end_i; i++ )
                    {
                            fmgv._morph_geom->setWeight( i, 0.f );
                    }
            }
            else
            {
                    std::cout << "Error to find geometry morph object: " << str_morph << std::endl;
            }
        }
        else
        {
            std::cout << "Error to find morph object: " << str_morph << std::endl;
        }
}

//------------------------------------------------------------------------------------------------------------

//Клонирование объекта
bool GraphicSceneClass::CloneObject(std::string OriginalObjectName, std::string CloneObjectName, std::string postfix)
{
	//Находим объект
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(OriginalObjectName);
    root->accept(*fnnv.get());
	//такой объект есть?....
	if (fnnv->_foundNodes.empty()==true)
	{
		std::cout << "Find objects in Root - FALSE! " << OriginalObjectName << std::endl;
		//delete fnnv;
		return false;
	}
	osg::Node* mycopy = dynamic_cast<osg::Node*>(fnnv->_foundNodes.front()->clone(osg::CopyOp::DEEP_COPY_NODES | osg::CopyOp::DEEP_COPY_STATESETS | osg::CopyOp::DEEP_COPY_TEXTURES | osg::CopyOp::DEEP_COPY_IMAGES | osg::CopyOp::DEEP_COPY_STATESETS | osg::CopyOp::DEEP_COPY_STATEATTRIBUTES | osg::CopyOp::DEEP_COPY_DRAWABLES | osg::CopyOp::DEEP_COPY_OBJECTS));
    //osg::Node* mycopy = dynamic_cast<osg::Node*>(fnnv->_foundNodes.front()->clone(osg::CopyOp::DEEP_COPY_ALL));
	// note, we need the dyanmic_cast because MS Visual Studio can't handle covarient
    // return types, so that clone has return just Object*.  bahh hum bug
	//osg::ref_ptr<osg::Node>
   //osg::Node* mycopy = dynamic_cast<osg::Node*>(fnnv->_foundNodes.front()->clone(MyCopyOp(osg::CopyOp::DEEP_COPY_ALL)));
    // do a graph preserving deep copy.
    //std::cout << "Doing a graph preserving deep copy of scene graph nodes"<<std::endl;
    //osg::ref_ptr<osg::Node> mycopy = dynamic_cast<osg::Node*>(fnnv->_foundNodes.front()->clone(GraphCopyOp(osg::CopyOp::DEEP_COPY_NODES)));
	//osg::ref_ptr<osg::Node> mycopy = dynamic_cast<osg::Node*>(fnnv->_foundNodes.front()->clone(GraphCopyOp(osg::CopyOp::DEEP_COPY_NODES | osg::CopyOp::DEEP_COPY_STATESETS | osg::CopyOp::DEEP_COPY_TEXTURES | osg::CopyOp::DEEP_COPY_IMAGES )));



	osg::ref_ptr<RenameNodeVisitor> rennv = new RenameNodeVisitor(postfix);
	mycopy->accept(*rennv.get());
	mycopy->setName (CloneObjectName);
	fnnv->_foundNodes.front()->getParent(0)->addChild (mycopy); //.get()
	//std::cout << "new name " << CloneObjectName << std::endl;
	//delete fnnv;
	//delete rennv;
	return true;
}

//Удаление объекта из сцены
bool GraphicSceneClass::UnloadObject(std::string ObjectName)
{
    try
    {
        std::cout << "Unload Object: " << ObjectName << std::endl;
        if ((ObjectName=="__NullPlane__")||(ObjectName=="__CURSOR__")|| (ObjectName=="HUD") || (ObjectName=="__CURSORFORHUD__AIR") || (ObjectName=="__CURSOR_CAMERA__") || (ObjectName=="CURSOR_AIR") )
	{
            throw (ErrorClass (__FILE__, __LINE__ , "GraphicSceneClass::UnloadObject", "False unload Object: " + ObjectName + " is system object!", 3));
	}

	//Находим объект
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(ObjectName);
        root->accept(*fnnv.get());
	//такой объект есть?....
	if (fnnv->_foundNodes.empty()==true)
	{
            throw (ErrorClass (__FILE__, __LINE__ , "GraphicSceneClass::UnloadObject", "Find objects in Root - FALSE! " + ObjectName, 3));
	}

	osg::Node::ParentList parents = fnnv->_foundNodes.front().get()->getParents();
	for(osg::Node::ParentList::iterator pitr=parents.begin();
		pitr!=parents.end();
	    ++pitr)
        {
            bool ok = (*pitr)->removeChild(fnnv->_foundNodes.front().get());
            if (ok==true)
            {
                //std::cout << "Unload Object: " << ObjectName << " from " << (*pitr)->getName() << " successful" <<std::endl;
            }
        }
        FindNamedNodeVisitor::CheckCache();
        return true;
    }
    catch (ErrorClass error)
    {
        error.print();
		return false;
    }
}


//----------------------------------ЭКСПЕРИМЕНТАЛЬНЫЕ ФУНКЦИИ-------------------------------
//-------------------------------Вершины---------------------------------------
bool GraphicSceneClass::RSetVertexPosition(std::string ObjectName, unsigned int i, float x, float y, float z)
{
	//Находим объект
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(ObjectName);
    root->accept(*fnnv);
	//Если такой объект есть, ищем анимацию
	if (!fnnv->_foundNodes.empty())
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
		osg::ref_ptr<FindGeometryVisitor> zzz = new FindGeometryVisitor();
		fnnv->_foundNodes.front().get()->accept (*zzz);
		//Если геометрия найдена
		if (!zzz->geom)
		{
			std::cout << "geometry NOT found";
			//delete zzz;
			//delete fnnv;
			return false;
		}
                //Двигаем вертексы

                //
                osg::ref_ptr<osg::Vec3Array> vertices = dynamic_cast<osg::Vec3Array*> (zzz->geom->getVertexArray()) ;
                if (vertices)
                {
                    vertices->at(i).x() += x;
                    vertices->at(i).y() += y;
                    vertices->at(i).z() += z;
                }
                else
                {
                    std::cout << "getVertexArray false" << std::endl;
                }
		//Чистим мусор
		//delete zzz;
	}
	else
	{
		std::cout << "Find objects in Root - FALSE! " << ObjectName << std::endl;
		//delete fnnv;
		return false;
	}
	//Чистим мусор
	//delete fnnv;
	return true;
}


bool GraphicSceneClass::RSetTexturePosition(std::string ObjectName, float dx, float dy, unsigned int UnitNumber)
{
	//Находим объект
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(ObjectName);
    root->accept(*fnnv.get());
	//Если такой объект есть, ищем анимацию
	if (!fnnv->_foundNodes.empty())
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
		osg::ref_ptr<FindGeometryVisitor> zzz = new FindGeometryVisitor();
		fnnv->_foundNodes.front().get()->accept (*zzz.get());
		//Если геометрия найдена
		if (!zzz->geom.get())
		{
			std::cout << "geometry NOT found";
			//delete zzz;
			//delete fnnv;
			return false;
		}
		//Двигаем текстурные координыты
		osg::ref_ptr<osg::Vec2Array> vertices2 = dynamic_cast<osg::Vec2Array*> (zzz->geom->getTexCoordArray(UnitNumber)) ;
		for (unsigned int u =0 ; u < vertices2->size(); u++)
		{
			vertices2->at(u).x() += dx;
			vertices2->at(u).y() += dy;
		}
		//Чистим мусор
		//delete zzz;
	}
	else
	{
		std::cout << "Find objects in Root - FALSE! " << ObjectName << std::endl;
		//delete fnnv;
		return false;
	}
	//Чистим мусор
	//delete fnnv;
	return true;
}

bool GraphicSceneClass::RSetTexturePointPosition(std::string ObjectName, unsigned int i, float dx, float dy)
{
	//Находим объект
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(ObjectName);
        root->accept(*fnnv.get());
	//Если такой объект есть, ищем анимацию
	if (!fnnv->_foundNodes.empty())
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
		osg::ref_ptr<FindGeometryVisitor> zzz = new FindGeometryVisitor();
		fnnv->_foundNodes.front().get()->accept (*zzz.get());
		//Если геометрия найдена
		if (!zzz->geom.get())
		{
			std::cout << "geometry NOT found";
			//delete zzz;
			//delete fnnv;
			return false;
		}
		//Двигаем текстурные координыты
		osg::ref_ptr<osg::Vec2Array> vertices2 = dynamic_cast<osg::Vec2Array*> (zzz->geom->getTexCoordArray(0)) ;
		vertices2->at(i).x() += dx;
		vertices2->at(i).y() += dy;
		//Чистим мусор
		//delete zzz;
	}
	else
	{
		std::cout << "Find objects in Root - FALSE! " << ObjectName << std::endl;
		//delete fnnv;
		return false;
	}
	//Чистим мусор
	//delete fnnv;
	return true;
}

//------------------------------------------------------------------------------------------------


//"Cursor" объекта для конструктора
bool GraphicSceneClass::SelectObject(std::string ObjectName, std::string method)
{
    //method
    //="BoundingBox" по умолчанию
    //="WireFrame" - представить модель "проволокой"
    //="Alpha" - прозрачность
    //="None" - только нарислвать оси и pivot

    //добавить проверку, вдруг курсор уже есть


    //1. Находим объект
    osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(ObjectName);
    root->accept(*fnnv.get());
    if (!fnnv->_foundNodes.empty())
    {
        if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;

        //обертка курсора
        osg::ref_ptr<osg::MatrixTransform> CursorMatrix = new osg::MatrixTransform;
        //геометрия курсора
        osg::ref_ptr<osg::Geode>  CursorNode = new osg::Geode;
        CursorMatrix->addChild(CursorNode);

        CursorMatrix->setName (ObjectName + "_cursor");

        CcalculateBoundingBox bbox;
        fnnv->_foundNodes.front()->accept( bbox  );
        osg::BoundingBox box = bbox.getBoundBox();

        if (method == "BoundingBox")
        {
            //geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f,0.0f,0.0f),bs.radius(),bs.radius(),bs.radius())));
            //osg::ref_ptr<osg::ShapeDrawable> box1Drawable = new osg::ShapeDrawable(new osg::Box(box.center(),box.xMax()-box.xMin(),box.yMax()-box.yMin(),box.zMax()-box.zMin()));

            // create LINES X
            {
                    // create Geometry object to store all the vertices and lines primtive.
                    osg::ref_ptr<osg::Geometry> linesGeom = new osg::Geometry();
                    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(25);

                    (*vertices)[0].set(box.corner(0)); //-+box.center()
                    (*vertices)[1].set(box.corner(1));
                    (*vertices)[2].set(box.corner(1));
                    (*vertices)[3].set(box.corner(3));
                    (*vertices)[4].set(box.corner(3));
                    (*vertices)[5].set(box.corner(2));
                    (*vertices)[6].set(box.corner(2));
                    (*vertices)[7].set(box.corner(0));

                    (*vertices)[8].set(box.corner(0));
                    (*vertices)[9].set(box.corner(4));
                    (*vertices)[10].set(box.corner(1));
                    (*vertices)[11].set(box.corner(5));
                    (*vertices)[12].set(box.corner(2));
                    (*vertices)[13].set(box.corner(6));
                    (*vertices)[14].set(box.corner(3));
                    (*vertices)[15].set(box.corner(7));

                    (*vertices)[16].set(box.corner(4));
                    (*vertices)[17].set(box.corner(5));
                    (*vertices)[18].set(box.corner(5));
                    (*vertices)[19].set(box.corner(7));
                    (*vertices)[20].set(box.corner(7));
                    (*vertices)[21].set(box.corner(6));
                    (*vertices)[22].set(box.corner(6));
                    (*vertices)[23].set(box.corner(4));

                    linesGeom->setVertexArray(vertices);
                    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
                    colors->push_back(osg::Vec4(1.0f,1.0f,0.0f,1.0f));
                    linesGeom->setColorArray(colors);
                    linesGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
                    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
                    normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));
                    linesGeom->setNormalArray(normals);
                    linesGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);
                    linesGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,24));
                    CursorNode->addDrawable(linesGeom);
            }

            //CursorNode->addDrawable(box1Drawable.get());
            //
            //
            osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
            osg::ref_ptr<osg::PolygonMode> polymode = new osg::PolygonMode;
            polymode->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);
            stateset->setAttributeAndModes(polymode,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
            osg::ref_ptr<osg::LineWidth> linewidth = new osg::LineWidth();
            linewidth->setWidth(2.0f);
            stateset->setAttributeAndModes(linewidth,osg::StateAttribute::ON);
            stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
            //применяем к жертве)
            CursorNode->setStateSet(stateset);
        }
        if (method == "WireFrame")
        {
            osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
            osg::ref_ptr<osg::PolygonMode> polymode = new osg::PolygonMode;
            polymode->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);
            stateset->setAttributeAndModes(polymode,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
            osg::ref_ptr<osg::LineWidth> linewidth = new osg::LineWidth();
            linewidth->setWidth(4.0f);
            stateset->setAttributeAndModes(linewidth,osg::StateAttribute::ON);
            stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
            //применяем к жертве)
            fnnv->_foundNodes.front().get()->setStateSet(stateset);
        }
        if (method == "None")
        {
        }
        if (method == "Alpha")
        {
        }

        //Рисуем оси в любом случае
        // create LINES X
        {
                // create Geometry object to store all the vetices and lines primtive.
                osg::ref_ptr<osg::Geometry> linesGeom = new osg::Geometry();
                osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(6);
                (*vertices)[0].set(0, 0,0);
                (*vertices)[1].set(0, 0, box.xMax()-box.xMin() );
                linesGeom->setVertexArray(vertices);
                osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
                colors->push_back(osg::Vec4(1.0f,0.0f,0.0f,1.0f));
                linesGeom->setColorArray(colors);
                linesGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
                osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
                normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));
                linesGeom->setNormalArray(normals);
                linesGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);
                linesGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,2));
                CursorNode->addDrawable(linesGeom);
        }
        // create LINES Y
        {
                // create Geometry object to store all the vetices and lines primtive.
                osg::ref_ptr<osg::Geometry> linesGeom = new osg::Geometry();
                osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(6);
                (*vertices)[0].set(0, 0,0);
                (*vertices)[1].set(0,  box.yMax()-box.yMin(),0);
                linesGeom->setVertexArray(vertices);
                osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
                colors->push_back(osg::Vec4(0.0f,1.0f,0.0f,1.0f));
                linesGeom->setColorArray(colors);
                linesGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
                osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
                normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));
                linesGeom->setNormalArray(normals);
                linesGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);
                linesGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,2));
                CursorNode->addDrawable(linesGeom);
        }
        // create LINES Z
        {
                // create Geometry object to store all the vetices and lines primtive.
                osg::ref_ptr<osg::Geometry> linesGeom = new osg::Geometry();
                osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(6);
                (*vertices)[0].set(0, 0,0);
                (*vertices)[1].set( box.zMax()-box.zMin(),0, 0);
                linesGeom->setVertexArray(vertices);
                osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
                colors->push_back(osg::Vec4(0.0f,0.0f,1.0f,1.0f));
                linesGeom->setColorArray(colors);
                linesGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
                osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
                normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));
                linesGeom->setNormalArray(normals);
                linesGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);
                linesGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,2));
                CursorNode->addDrawable(linesGeom);
        }

        //компас
        {
                osg::ref_ptr<osg::Geometry> linesGeom = new osg::Geometry();
                osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(6);
                (*vertices)[0].set(0, 0,0);
                (*vertices)[1].set(0, 0, 10000 );
                linesGeom->setVertexArray(vertices);
                osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
                colors->push_back(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
                linesGeom->setColorArray(colors);
                linesGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
                osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
                normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));
                linesGeom->setNormalArray(normals);
                linesGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);
                linesGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,2));

                osg::ref_ptr<osg::Geode> tempGeode = new osg::Geode();
                //
                osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
                osg::ref_ptr<osg::LineWidth> linewidth = new osg::LineWidth();
                linewidth->setWidth(1.0f);
                stateset->setAttributeAndModes(linewidth,osg::StateAttribute::ON);
                stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
                //применяем к жертве)
                tempGeode->setStateSet(stateset);
                //
                tempGeode->addDrawable(linesGeom);


                osg::ref_ptr<osg::AutoTransform> at = new osg::AutoTransform;
                at->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN); //osg::AutoTransform::ROTATE_TO_SCREEN ROTATE_TO_CAMERA
                at->addChild   (tempGeode);
                CursorMatrix->addChild(at);
        }

        //Прикрепляем курсор;
        fnnv->_foundNodes.front().get()->addChild(CursorMatrix);
    }
    else
    {
        std::cout << "Find objects in Root - FALSE! " << ObjectName << std::endl;
        return false;
    }

    return true;

    //fnnv->_foundNodes.front().get()->addChild(m_MatrixTransform);
    //osg::Vec3f scale = fnnv->_foundNodes.front().get()->getMatrix().getScale();
    //m_MatrixTransform->setMatrix(osg::Matrix::translate(0.0f,0.0f,0.0f)* osg::Matrix::scale(1.0f/scale.x(),1.0f/scale.y(),1.0f/scale.z() )* osg::Matrix::rotate(0.0f,0.0f,0.0f,1.0f));
}



// Получить BoundingBox объекта ObjectName. global - вернуть в мировых координатах.
float* GraphicSceneClass::GetBoundingBox(std::string ObjectName, bool global)
{
	float *result = new float[9]; //!!!

	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(ObjectName);    
	if (!fnnv->_foundNodes.empty())
	{
		root->accept(*fnnv.get());
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
		// get the bounds of the model.    
		osg::ComputeBoundsVisitor cbbv1;

		fnnv->_foundNodes.front().get()->accept(cbbv1); // accept(cbbv1);		
		
		osg::BoundingBox bb1 = cbbv1.getBoundingBox();

		if(bb1.valid())
		{
			if (global)
			{				
				osg::MatrixList worldMatrices = fnnv->_foundNodes.front().get()->getParent(0)->getWorldMatrices();
				osg::Matrix totmat = osg::Matrix::identity();

				for(osg::MatrixList::iterator itr = worldMatrices.begin(); itr != worldMatrices.end(); ++itr)
				{
					osg::Matrix& matrix = *itr;
					totmat *= matrix;
				}
				
				osg::Vec3d scl = totmat.getScale();
				osg::Box *unitCube = new osg::Box(bb1.center(), (bb1.xMax() - bb1.xMin())*scl.x(), (bb1.yMax() - bb1.yMin())*scl.y(), (bb1.zMax() - bb1.zMin())*scl.z());
				osg::ShapeDrawable* unitCubeDrawable = new osg::ShapeDrawable(unitCube);
				osg::Geode *geo = new osg::Geode();
				geo->addDrawable(unitCubeDrawable);

				osg::MatrixTransform *mt = new osg::MatrixTransform();
				mt->setMatrix(totmat);
				mt->addChild(geo);

				unitCube->setRotation(totmat.getRotate());
				unitCube->setCenter(bb1.center()*totmat);

				bb1 = geo->getBoundingBox();
			}
			result[0] = bb1.xMin();
			result[1] = bb1.yMin();
			result[2] = bb1.zMin();
			result[3] = bb1.xMax();
			result[4] = bb1.yMax();
			result[5] = bb1.zMax();
			result[6] = bb1.center()[0];
			result[7] = bb1.center()[1];
			result[8] = bb1.center()[2];
		}
		else
		{
			std::cout << "get Bounding box - FALSE!" << std::endl;
		}
	}
	return result;
}

//--------------------------------------------------------------------------------

bool GraphicSceneClass::AddVolumeTexture(std::string ObjectName)
{
	FindNamedNodeVisitor* fnnv = new FindNamedNodeVisitor(ObjectName);
    root->accept(*fnnv);
	if (!fnnv->_foundNodes.empty())
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
		fnnv->_foundNodes.front().get()->setStateSet ( createVolumeTextureState());
	}
	else
	{
		std::cout << "Find objects in Root - FALSE! " << ObjectName << std::endl;
		delete fnnv;
		return false;
	}
	delete fnnv;
	return true;	
}

osg::StateSet* GraphicSceneClass::createVolumeTextureState()
{
	MyGraphicsContext gc;
    if (!gc.valid()) 
    {
		std::cout <<"Unable to create the graphics context required to build 3d image."<<std::endl;
        return 0;
    }
	 // read 4 2d images
    osg::ref_ptr<osg::Image> image_0 = osgDB::readImageFile("D:/DEVELOP/OSG/OpenSceneGraph-2.7.1/bin/Images/lz.rgb");
    osg::ref_ptr<osg::Image> image_1 = osgDB::readImageFile("D:/DEVELOP/OSG/OpenSceneGraph-2.7.1/bin/Images/reflect.rgb");
    osg::ref_ptr<osg::Image> image_2 = osgDB::readImageFile("D:/DEVELOP/OSG/OpenSceneGraph-2.7.1/bin/Images/tank.rgb");
    osg::ref_ptr<osg::Image> image_3 = osgDB::readImageFile("D:/DEVELOP/OSG/OpenSceneGraph-2.7.1/bin/Images/skymap.jpg");
    if (!image_0 || !image_1 || !image_2 || !image_3)
    {
        std::cout << "Warning: could not open files."<<std::endl;
        return new osg::StateSet;
    }
    if (image_0->getPixelFormat()!=image_1->getPixelFormat() || image_0->getPixelFormat()!=image_2->getPixelFormat() || image_0->getPixelFormat()!=image_3->getPixelFormat())
    {
        std::cout << "Warning: image pixel formats not compatible."<<std::endl;
        return new osg::StateSet;
    }
    // get max 3D texture size
	/*
    GLint textureSize = osg::Texture3D::getExtensions(0,true)->maxTexture3DSize();
    if (textureSize > 256)
	*/
    GLint   textureSize = 256;
    // scale them all to the same size.
    image_0->scaleImage(textureSize,textureSize,1);
    image_1->scaleImage(textureSize,textureSize,1);
    image_2->scaleImage(textureSize,textureSize,1);
    image_3->scaleImage(textureSize,textureSize,1);
    // then allocated a 3d image to use for texturing.
    osg::Image* image_3d = new osg::Image;
    image_3d->allocateImage(textureSize,textureSize,4, image_0->getPixelFormat(),image_0->getDataType());
    // copy the 2d images into the 3d image.
    image_3d->copySubImage(0,0,0,image_0.get());
    image_3d->copySubImage(0,0,1,image_1.get());
    image_3d->copySubImage(0,0,2,image_2.get());
    image_3d->copySubImage(0,0,3,image_3.get());
    image_3d->setInternalTextureFormat(image_0->getInternalTextureFormat());        
    // set up the 3d texture itself,
    // note, well set the filtering up so that mip mapping is disabled,
    // gluBuild3DMipsmaps doesn't do a very good job of handled the
    // inbalanced dimensions of the 256x256x4 texture.
    osg::Texture3D* texture3D = new osg::Texture3D;
    texture3D->setFilter(osg::Texture3D::MIN_FILTER,osg::Texture3D::LINEAR);
    texture3D->setFilter(osg::Texture3D::MAG_FILTER,osg::Texture3D::LINEAR);
    texture3D->setWrap(osg::Texture3D::WRAP_R ,osg::Texture3D::REPEAT);
	texture3D->setWrap(osg::Texture3D::WRAP_S ,osg::Texture3D::CLAMP); 
	texture3D->setWrap(osg::Texture3D::WRAP_T ,osg::Texture3D::CLAMP); 
    texture3D->setImage(image_3d);
    // create a texgen to generate a R texture coordinate, the geometry
    // itself will supply the S & T texture coordinates.
    // in the animateStateSet callback well alter this R value to
    // move the texture through the 3d texture, 3d texture filtering
    // will do the blending for us.
    osg::TexGen* texgen = new osg::TexGen;
    texgen->setMode(osg::TexGen::OBJECT_LINEAR);
    texgen->setPlane(osg::TexGen::R, osg::Plane(0.0f,0.0f,0.0f,0.2f));
    // create the StateSet to store the texture data
    osg::StateSet* stateset = new osg::StateSet;
    stateset->setTextureMode(0,GL_TEXTURE_GEN_R,osg::StateAttribute::ON);
    stateset->setTextureAttribute(0,texgen);
    stateset->setTextureAttributeAndModes(0,texture3D,osg::StateAttribute::ON);
    return stateset;
}

//---------------------------------------ТЕНИ-------------------------------------
//Создание отдельной группы теней, параметр - имя группы
bool GraphicSceneClass::AddShadowedGroup(std::string name)
{
	#ifdef WIN32
	osgShadow::ShadowedScene* newShadowedGroup = new osgShadow::ShadowedScene;
	int ReceivesShadowTraversalMask = 0x1;
	int CastsShadowTraversalMask = 0x2;
	newShadowedGroup->setReceivesShadowTraversalMask(ReceivesShadowTraversalMask);
	newShadowedGroup->setCastsShadowTraversalMask(CastsShadowTraversalMask);
	//технология получения теней	
	osgShadow::ShadowMap* sm = new osgShadow::ShadowMap;
    newShadowedGroup->setShadowTechnique(sm);
    int mapres = 1024;
    sm->setTextureSize(osg::Vec2s(mapres,mapres));

	//объект-посредник для NodeVisitora
	osg::MatrixTransform *m_MatrixTransform = new osg::MatrixTransform();
	m_MatrixTransform->setName(name);
	newShadowedGroup->addChild (m_MatrixTransform);

	//добавляем эту группу в сцену. Все объекты, которые находятся в этой группе и имеют маску 0x1 - принимают тени, 0x2 - передают, & - и то и другое, 0 - ничего не делают.
	root->addChild(newShadowedGroup);
	#endif
	return true;
}

//Определение маски 4-ничего, 1-принимать, 2-бросать, 3 - и то и другое.
bool GraphicSceneClass::SetShadowMask(std::string ObjectName, int mask)
{
	FindNamedNodeVisitor* fnnv = new FindNamedNodeVisitor(ObjectName);
    root->accept(*fnnv);
	if (!fnnv->_foundNodes.empty())
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
		fnnv->_foundNodes.front().get()->setNodeMask(mask);
	}
	else
	{
		std::cout << "Find objects in Root - FALSE! " << ObjectName << std::endl;
		delete fnnv;
		return false;
	}
	delete fnnv;
	return true;
}
//--------------------------------------------------------------------------------


//-------------------------------OLD---------------------------------------------



//Уточнение модели освещения
/*
osg::LightModel* lightModel = new osg::LightModel;
//lightModel->setColorControl (osg::LightModel::ColorControl::SEPARATE_SPECULAR_COLOR );
lightModel->setAmbientIntensity (osg::Vec4(0.0, 0.0, 0.0,1));
lightModel->setLocalViewer (true);
//
rootStateSet->setAttributeAndModes(lightModel, osg::StateAttribute::ON);
*/
	
