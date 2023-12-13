/***************************************************************************
FindNodeByName.cpp  -  Класс для нахождения списка найденых Matrix transform по имени
-------------------
begin                : 1 марта 2023
modefed              : 21 мая 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/



#include "FindNodeByName.h"

std::map <std::string, Cache> FindNamedNodeVisitor::FindNameCache;

void FindNamedNodeVisitor::apply(osg::MatrixTransform& node)
{
    //
    if (inCache==true) return;

    if (node.getName()==_name)
    {
        _foundNodes.push_back(&node);
        osg::NodePath other = getNodePath();
        accumulated = osg::computeLocalToWorld(other);
        //в кеш!
        FindNameCache[_name]._foundNodes = _foundNodes;
        FindNameCache[_name].accumulated = accumulated;
    }
    traverse(node);
}

/*
void FindNamedNodeVisitor::apply(osg::Group& group) 
{
	traverse(group); 
}
*/


int FindNamedNodeVisitor::getCacheSize()
{
    return FindNameCache.size();
}

void FindNamedNodeVisitor::ClearCache()
{
    FindNameCache.clear();
}

//проверка кеша на отсутствие "открепленных" со сцены объектов
void FindNamedNodeVisitor::CheckCache()
{
    FindNameCache.clear();
    return;

        for(std::map<std::string, Cache>::iterator hitr=FindNameCache.begin(); hitr!=FindNameCache.end();	hitr++)
        {
            //std::string name = hitr->first;
            //if (hitr->second._foundNodes.front()->getNumParents() == 0 )
            if ((hitr->second._foundNodes.front()->referenceCount() <= 1 )||(hitr->second._foundNodes.front()->getNumParents() == 0 ))
            {
                std::map<std::string, Cache>::iterator next = hitr;
                ++next;
                FindNameCache.erase (hitr);
                if (next != FindNameCache.end())
                {
                    hitr = next;
                    continue;
                }
                else
                {
                    break;
                }
            }
           // continue;
        }

}

