/***************************************************************************
RenameAllNode.cpp  -  Класс для переименовывания всех найденных объектов
-------------------
begin                : 11 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/



#include "RenameAllNode.h"


void RenameNodeVisitor::apply(osg::MatrixTransform& node)
{
	if (node.getName()!="")
	{
		node.setName (node.getName()+_name);
	}
	traverse(node);
}


