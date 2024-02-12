    #include <iostream>
#include <iomanip>
#include <time.h>



//define memoryleaks
//memoryleaks
#ifdef memoryleaks
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif


#include "./BuildConfig.h"


#include "./Configure/Console.h"
#include "./Configure/Configure.h"
Configure* configuration;

#include "./SCORM/interfaceforscorm.h"
interfaceForScorm* SCORM;

#include "./TinCan/InterfaceForTinCan.h"
InterfaceForTinCan *TINCAN;




#include "./user/User.h"

#include "./Scene/SceneClass.h"
//крупные блоки
SceneClass* scene;
UserClass* localUser; 



//Интерпритатор
#include "./lua/Interpritator.h"
//Пример создания потоков
//#include "./ThreadExample.h"




//главные функции manager'а
void Init(bool);
void Start();
void Bue();
void SimulationStep();

//сервисные
void MouseInCenterScreen();
//включить/выкл вертикальную синхронизацию
void setVSync(int interval);





