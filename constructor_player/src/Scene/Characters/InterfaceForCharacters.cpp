/***************************************************************************
InterfaceForCharacters.cpp  -  интерфейс для работы с персонажами (агентами)
-------------------
begin                : 26 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

#include "./InterfaceForCharacters.h"

//конструктор
InterfaceForCharacters::InterfaceForCharacters(osg::ref_ptr<osg::Group> _root)
{
	root = _root;
}

//сброс
bool InterfaceForCharacters::Reset()
{
	//Загруженные персонажи
	if (characters.empty() != true)
	{
		for(std::map<std::string, BaseCharater*>::iterator hitr=characters.begin();hitr!=characters.end();	++hitr)
		{
			std::cout << " delete .. " << hitr->first << std::endl;
			//или так или удалять из своего предка (GraphicSceneClass::UnloadObject)
			//root->removeChild(hitr->second->Character_MatrixTransform); 
			osg::Node::ParentList parents = hitr->second->Character_MatrixTransform->getParents();
			for(osg::Node::ParentList::iterator pitr=parents.begin();
				pitr!=parents.end();
				++pitr)
			{
				(*pitr)->removeChild(hitr->second->Character_MatrixTransform.get());
			}
		}
	}
	characters.clear();
	return true;
}


//создание нового персонажа
bool InterfaceForCharacters::CreateCharacter(std::string nameCharacter, std::string filename)
{
	//+проверка на совпадение имен с уже имеющимися
	if (characters.count(nameCharacter)!=0) 
	{
		std::cout << "Warning: Character '" << nameCharacter << "' already exsist!" << std::endl;
		return false;
	}

	characters[nameCharacter] = new BaseCharater(nameCharacter, filename);
	//Добавляем к сцене
    root->addChild( characters[nameCharacter]->Character_MatrixTransform.get() );
	return true;
}
//установка позиции персонажа
bool InterfaceForCharacters::SetCharacterPosition(std::string nameCharacter, float x, float y, float z)
{
	//+проверка на наличие такого имени
	if (characters.count(nameCharacter)==0) 
	{
		std::cout << "Warning: Character '" << nameCharacter << "' not exsist!" << std::endl;
		return false;
	}
	//
	characters[nameCharacter]->SetPosition(x,y,z);
	return true;
}
//установка места-назначения персонажа
bool InterfaceForCharacters::SetCharacterGoTo(std::string nameCharacter, float x, float y, float z)
{
	//+проверка на наличие такого имени
	if (characters.count(nameCharacter)==0) 
	{
		std::cout << "Warning: Character '" << nameCharacter << "' not exsist!" << std::endl;
		return false;
	}
	//
	characters[nameCharacter]->GoTo(x,y,z);
	return true;
}

//установка анимации персонажа
bool InterfaceForCharacters::SetCharacterAction(std::string nameCharacter, int numAnimation = 0)
{
	//+проверка на наличие такого имени
	if (characters.count(nameCharacter)==0) 
	{
		std::cout << "Warning: Character '" << nameCharacter << "' not exsist!" << std::endl;
		return false;
	}
	//
	characters[nameCharacter]->SetAnimation (numAnimation);
	return true;
}

//Прикрепляетобъект к кости
bool InterfaceForCharacters::ConnectObjectToBone(std::string nameCharacter, std::string ObjectName, int boneID)
{
	//+проверка на наличие такого имени
	if (characters.count(nameCharacter)==0) 
	{
		std::cout << "Warning: Character '" << nameCharacter << "' not exsist!" << std::endl;
		return false;
	}
	//
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(ObjectName);
    root->accept(*fnnv.get());
	if (!fnnv->_foundNodes.empty())
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
		//отключаем объект от сцены
		osg::Node::ParentList parents = fnnv->_foundNodes.front().get()->getParents();
		for(osg::Node::ParentList::iterator pitr=parents.begin();
			pitr!=parents.end();
			++pitr)
		{
			(*pitr)->removeChild(fnnv->_foundNodes.front().get());
		}
		characters[nameCharacter]->ConnectObjectToBone (fnnv->_foundNodes.front().get() ,boneID);
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
	