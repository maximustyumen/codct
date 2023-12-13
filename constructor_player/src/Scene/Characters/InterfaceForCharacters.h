#ifndef INTERFACEFORCHARACTER
#define INTERFACEFORCHARACTER


#include "./BaseCharacter.h"
#include "../Visitors/FindNodeByName.h"

class InterfaceForCharacters
{	
public:
	//конструктор
	InterfaceForCharacters(osg::ref_ptr<osg::Group> _root);
	//создание нового персонажа
	bool CreateCharacter(std::string name, std::string filename);
	//установка позиции персонажа
	bool SetCharacterPosition(std::string name, float x, float y, float z);
	//установка места-назначения персонажа
	bool SetCharacterGoTo(std::string name, float x, float y, float z);
	//устанговка анимации персонажа
	bool SetCharacterAction(std::string nameCharacter, int numAnimation);
	//Прикрепляетобъект к кости
	bool ConnectObjectToBone(std::string nameCharacter, std::string ObjectName, int boneID);
	//сброс
	bool Reset();

private:
	//персонажи
	std::map <std::string, BaseCharater*> characters;
	//указатель на главную группу сцены
	osg::ref_ptr<osg::Group> root;
};

#endif