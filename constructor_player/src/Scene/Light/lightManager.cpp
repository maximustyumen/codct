#include "lightManager.h"

LightManager:: LightManager()
  : groupOfLights(NULL),
    lights(8),
    valid(false) {
  /*
  Initialize our list to have all NULL values. This is done so when
  the user calls the get_or_create_light() function, the function
  can properly detect whether a light has been created for a particular
  slot.
  */
  for(unsigned int i = 0; i < lights.size(); ++i){
      lights[i] = NULL;
  }
        
}

void LightManager::init(osg::Group* root) {
  groupOfLights = root;
  valid = true;
}

osg::Light * LightManager::getOrCreateLight(LightManager::LIGHTNUMBER number) {
  if(valid){
     if(lights[number]){
        return lights[number]->getLight();
     }
    //       Create a new light, and set the light number according to our parameter.
    osg::Light * light = new osg::Light();
    light->setLightNum(number);
    /*
    Create a new lightsource. This is the object which holds a light, there
    is a 1-1 correspondence between a light and a lightsource. The vector
    actually holds lightsources.
    Set the lightsource to contain our newly created light
    */
    osg::LightSource * lightsource = new osg::LightSource();
    lightsource->setLight(light);  
    /*
    Now we need to enable the lightsource and thereby light in the SetState
    of the group node that will contain the lightsource.
    */
    lightsource->setStateSetModes(*groupOfLights->getOrCreateStateSet(), osg::StateAttribute::ON);
    /*
    Add the lightsource to the osg::Group
    */
    groupOfLights->addChild(lightsource);
    /*
    Now we stick the address of the lightsource in the proper slot in
    the vector of lights, so future calls with this number will extract
    this light, and not allocate a new one.
    */
    lights[number] = lightsource;
    // Return a pointer to the osg::Light so the user may modify the properties.
    return light;
  }
  return NULL;
}


void LightManager::CreateLights()
{
	{
	osg::Light * 
    light = this->getOrCreateLight(LightManager::ONE);
    // We turn off ambient light as it is on slightly as default by osg::Light.
    light->setAmbient(osg::Vec4d(0.0, 0.0, 0.0, 1.0));
    light->setDiffuse(osg::Vec4d(1.0, 0.0, 0.0, 1.0));      //Red light
    light->setPosition(osg::Vec4d(0.0, 0.0, 100.0, 1.0));
            
    /*
    This determines how far the light will travel away from the light
    source, by defining the level of attenuation. Note that this is
    quadratic, meaning the light power is inversely proportional to
    the distance squared. There is also constant and linear attenuation,
    but those do not really provide a realistic method for attenuation.
    */
    light->setConstantAttenuation(0.05f);
    //light->setLinearAttenuation(0.05);
    light->setQuadraticAttenuation(0.05f);
	}

  //Create second light
  {
    osg::Light *light = this->getOrCreateLight(LightManager::TWO);
    light->setAmbient(osg::Vec4d(0.0, 0.0, 0.0, 1.0));
    light->setDiffuse(osg::Vec4d(0.0, 1.0, 0.0, 1.0));      //Green light
    light->setPosition(osg::Vec4d(200, 200.0, 10.0, 1.0));
    light->setConstantAttenuation(0.05f);
    //light->setLinearAttenuation(0.05);
    light->setQuadraticAttenuation(0.05f);
  }
        
  //Create third light
  {
    osg::Light * light = this->getOrCreateLight(LightManager::THREE);
    light->setAmbient(osg::Vec4d(0.0, 0.0, 0.0, 1.0));
    light->setDiffuse(osg::Vec4d(0.0, 0.0, 1.0, 1.0));      //Blue Light
    light->setPosition(osg::Vec4d(100.0, 100.0, 50.0, 1.0));
    light->setConstantAttenuation(0.05f);
    //light->setLinearAttenuation(0.05);
    light->setQuadraticAttenuation(0.05f);
  }
}