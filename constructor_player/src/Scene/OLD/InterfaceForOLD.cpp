/***************************************************************************
InterfaceForOLD.cpp  -  интерфейс для работы с объектами 3D "по старому"
-------------------
begin                : 25 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

#include "./InterfaceForOLD.h"

//конструктор
InterfaceForOLD::InterfaceForOLD(osg::ref_ptr<osg::Group> _root)
{
	root = _root;
}
//
InterfaceForOLD::~InterfaceForOLD()
{
	std::cout << "Destructor InterfaceForOLD."  << std::endl;
	Reset();
}

//сброс 
bool InterfaceForOLD::Reset()
{
	//Загруженные old-объекты
	if (objects.empty() != true)
	{
		for(std::map<std::string , Base3DObjectClass*>::iterator hitr=objects.begin();hitr!=objects.end();	++hitr)
		{
			std::cout << " delete .. " << hitr->first << std::endl;
			root->removeChild(hitr->second->_Model);
		}
	}
	objects.clear();
	return true;
}

osg::ref_ptr<osg::PositionAttitudeTransform> InterfaceForOLD::GetModel(std::string Name)
{
	return objects[Name]->_Model;
}

//получение Base3DObjectClass
Base3DObjectClass* InterfaceForOLD::GetObject(std::string Name)
{
	return objects[Name];
}

//вспомогательная функция, выполняет загрузку объектов
void InterfaceForOLD::LoadAnimModel(osg::ref_ptr<osg::PositionAttitudeTransform> object, std::string FileName)
{
	// load the model from the osg file
	std::cout << "Load file .. " << FileName;
	osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile(FileName);
	//
	if(!loadedModel)
	{
		std::cout << "no data loaded.";
		return;
	}
	//create a new MatrixTransform object
	osg::ref_ptr<osg::MatrixTransform> m_MatrixTransform = new osg::MatrixTransform();
	m_MatrixTransform->addChild(loadedModel.get());
	//
	object->addChild(m_MatrixTransform);
	root->addChild(object);
	std::cout <<  " done." << std::endl;
}

//Добавление нового объекта
void InterfaceForOLD::NewObject(std::string Name, std::string FileName)
{
	//временная переменная
	//Base3DObjectClass _temp_;
	//добавляем в контейнер
	objects[Name] = new Base3DObjectClass;
        LoadAnimModel(objects[Name]->_Model,FileName);
	//имя объекта
	objects[Name]->_Model->setName(Name);
	osg::Vec3f Center = objects[Name]->_Model->getPivotPoint();
	objects[Name]->SetCenterPosition(Center.x(), Center.y(), Center.z());
}
