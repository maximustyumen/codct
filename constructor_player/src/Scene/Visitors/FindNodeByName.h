#ifndef FINDNODEBYNAMECLASS
#define FINDNODEBYNAMECLASS

#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <osg/CoordinateSystemNode>
#include <osg/PositionAttitudeTransform>
#include <osg/MatrixTransform>
#include <iostream>
#include <map>

//кэш
class Cache
{
    public:
    typedef std::vector< osg::ref_ptr<osg::MatrixTransform> > NodeList;
    NodeList _foundNodes;
    osg::Matrix accumulated;
};

class FindNamedNodeVisitor : public osg::NodeVisitor
{
     static std::map <std::string, Cache> FindNameCache;

public:
    FindNamedNodeVisitor(const std::string& name, bool reFind = false): osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN), _name(name)
    {
        //1. Проверяем, есть ли имя в кеше
        if ((FindNamedNodeVisitor::FindNameCache.count(_name)!=0)&&(reFind==false))
        {
            inCache = true;
            _foundNodes = FindNamedNodeVisitor::FindNameCache[_name]._foundNodes;
            accumulated = FindNamedNodeVisitor::FindNameCache[_name].accumulated;
            reset();
        }
        else
        {
            inCache=false;
            setTraversalMask(0xffffffff);
            setNodeMaskOverride(0xffffffff);
        }
    }
    
    virtual void apply(osg::MatrixTransform& node);

	//virtual void apply(osg::Group& group);

    typedef std::vector< osg::ref_ptr<osg::MatrixTransform> > NodeList;

    std::string _name;
    NodeList _foundNodes;
    //Абсолютная матрица!
    osg::Matrix accumulated;
    //найден в кеше
    bool inCache;
    //
    static int getCacheSize();
	//очистка (полная) кеша
    static void ClearCache();
	//проверка кеша на отсутствие "открепленных" со сцены объектов
	static void CheckCache();
	

};



#endif

