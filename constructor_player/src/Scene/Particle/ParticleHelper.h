//see
//http://faculty.nps.edu/jasullivan/osgTutorials/osgParticleHelper.htm
//http://faculty.nps.edu/jasullivan/osgTutorials/Download/psHelper.cpp
//Author: jasullivan@nps.edu

#ifndef PARTICLEHELPER
#define PARTICLEHELPER

class psGeodeTransform : public osg::MatrixTransform
{
public:
   class psGeodeTransformCallback : public osg::NodeCallback
   {
      virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
      {
         if ( psGeodeTransform* ps = dynamic_cast<psGeodeTransform*>( node ) )
         {
            osg::NodePath& fullNodePath = nv->getNodePath();
            fullNodePath.pop_back();

            osg::Matrix localCoordMat = osg::computeLocalToWorld( fullNodePath );
            osg::Matrix inverseOfAccum = osg::Matrix::inverse( localCoordMat );

            ps->setMatrix( inverseOfAccum );
         }
		 traverse(node, nv); 
         
      }
   };

   psGeodeTransform() {setUpdateCallback( new psGeodeTransformCallback() );}

};

class findGeodeVisitor : public osg::NodeVisitor
{
public:
   findGeodeVisitor() : osg::NodeVisitor(TRAVERSE_ALL_CHILDREN)
   {
      foundGeode = NULL;
   }
   virtual void apply(osg::Node &searchNode)
   {
      if (osg::Geode* g = dynamic_cast<osg::Geode*> (&searchNode) )
         foundGeode = g;
      else
         traverse(searchNode);
   }
   osg::ref_ptr<osg::Geode> getGeode() {return foundGeode.get();}
protected:
   osg::ref_ptr<osg::Geode> foundGeode;
};

class particleSystemHelper : public osg::Group 
{ 
public: 
   particleSystemHelper(osg::Group* psGroup) : osg::Group(*psGroup) 
   { 
      osg::ref_ptr<findGeodeVisitor> fg = new findGeodeVisitor(); 
      accept(*fg.get()); 
      osg::ref_ptr<osg::Geode> psGeode = fg->getGeode().get(); 
      psGeodeXForm = new psGeodeTransform(); 
      psGeodeXForm->addChild (psGeode.get()); 
      replaceChild(psGeode.get(),psGeodeXForm.get()); 
   } 
   void addEffect(osg::Group* psGroup) 
   { 
      this->addChild(psGroup); 
      osg::ref_ptr<findGeodeVisitor> fg = new findGeodeVisitor(); 
      psGroup->accept(*fg.get()); 
      osg::ref_ptr<osg::Geode> psGeode = fg->getGeode().get(); 
      psGeodeXForm->addChild(psGeode.get()); 
      psGroup->removeChild( psGroup->getChildIndex(psGeode.get()) ); 
   } 
protected: 
   osg::ref_ptr<psGeodeTransform> psGeodeXForm; 
}; 


#endif