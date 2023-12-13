/***************************************************************************
UseHands.cpp  -  класс расчета всего, что связано с рукоблудием персонажа
-------------------
begin                : 13 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

#include "UserHands.h"
#include <osgAnimation/Bone>
#include <osgAnimation/StackedRotateAxisElement>
#include <osgAnimation/StackedTransformElement>
#include <osgAnimation/UpdateBone>

void UserHandsClass::CreateHydraHands()
{
	/*
	//Загрузка курсора - руки для Hydra
    osg::ref_ptr<osg::Node> HandCursor = osgDB::readNodeFile("./HandCursor.fbx");
    HandCursor->setName("__HAND__CURSOR__AIR");

	osg::ref_ptr<osg::StateSet> stateset = HandCursor->getOrCreateStateSet();
    stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	//Это для 3D HUD MENU
	//stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	stateset->setRenderBinDetails( 199, "RenderBin", osg::StateSet::OVERRIDE_RENDERBIN_DETAILS);


    if (!HandCursor.valid())
    {
            throw (ErrorClass (__FILE__, __LINE__ , "InputHydraClass::InputHydraClass()", "HandCursor.fbx not load.", 6));
    }
					
    root->asGroup()->addChild(HandCursor.get()); 
	*/
		
    osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor("__Hand1__AIR_");
    root->asGroup()->accept(*fnnv.get());
    if (!fnnv->_foundNodes.empty())
    {
		cursorHand = fnnv->_foundNodes.front().get();
    }
    else
    {
		throw (ErrorClass (__FILE__, __LINE__ , "InputHydraClass::InputHydraClass()", "__Hand1__AIR_ not find in HandCursor.fbx !!!", 6));
    }

	osg::ref_ptr<FindNamedNodeVisitor> fnnv2 = new FindNamedNodeVisitor("PALEZ_AIR");
	root->asGroup()->accept(*fnnv2.get());
    if (!fnnv2->_foundNodes.empty())
    {
		cursorHandPalez = fnnv2->_foundNodes.front().get();
    }
    else
    {
		throw (ErrorClass (__FILE__, __LINE__ , "InputHydraClass::InputHydraClass()", "PALEZ_AIR not find in HandCursor.fbx !!!", 6));
    }

	osg::ref_ptr<FindNamedNodeVisitor> fnnv3 = new FindNamedNodeVisitor("__Hand2__AIR_");
    root->asGroup()->accept(*fnnv3.get());
    if (!fnnv3->_foundNodes.empty())
    {
		cursorHand2 = fnnv3->_foundNodes.front().get();
    }
    else
    {
		throw (ErrorClass (__FILE__, __LINE__ , "InputHydraClass::InputHydraClass()", "__Hand2__AIR_ not find in HandCursor.fbx !!!", 6));
    }
}

/// найти элемент стэка трансформаций
osgAnimation::StackedTransformElement* find_stacked_transform_element(osgAnimation::UpdateBone* update_bone,const std::string& name)
{
	osgAnimation::StackedTransform& stacked = update_bone->getStackedTransforms();
	for(osgAnimation::StackedTransform::const_iterator it=stacked.begin(), end=stacked.end(); it!=end; ++it)
	{
		osgAnimation::StackedTransformElement* te = it->get();
		if(te->getName() == name)
			return te;
	}
	return 0;
}

void UserHandsClass::UpdateVRHands(HtcViveData data, float body_x, float body_y, float body_z)
{
	if (data.presend==false)
	{
		hydra_presend = false;
		cursorHand->setNodeMask(0x000000);
		cursorHand2->setNodeMask(0x000000);
		hydra_temp_command=-1;
		return;
	}
	else
	{
		hydra_presend = true;
		cursorHand->setNodeMask(0xffffff);
		cursorHand2->setNodeMask(0xffffff);
	}


	



	//передвигаем и передвигаем руку RIGHT
	{
		osg::Matrixd leftHandRotateMatrix;
		osg::Matrixd leftHandTransMatrix;
		osg::Matrixd leftHandResultMatrix;
		osg::Quat _lquat = data.LeftHandOrientationQuat;
		osg::Quat lquat (- _lquat.x(), _lquat.z(), -_lquat.y(), _lquat.w());
		leftHandRotateMatrix.makeRotate(lquat); 
		leftHandRotateMatrix.preMult (osg::Matrixd::rotate( -3.14f / 2.0, 1, 0, 0 ));

		leftHandTransMatrix.makeTranslate(body_x + data.LeftHandPosition.x() * 25.0f, body_y - data.LeftHandPosition.z() * 25.0f, body_z + data.LeftHandPosition.y() * 25.0f - 15 );
		leftHandResultMatrix = leftHandRotateMatrix * leftHandTransMatrix;

		cursorHand->setMatrix(leftHandResultMatrix);
	}

	//передвигаем и передвигаем руку LEFT
	{
		osg::Matrixd rightHandRotateMatrix;
		osg::Matrixd rightHandTransMatrix;
		osg::Matrixd rightHandResultMatrix;
		osg::Quat _rquat = data.RightHandOrientationQuat;
		osg::Quat rquat (-_rquat.x(), _rquat.z(), -_rquat.y(), _rquat.w());
		rightHandRotateMatrix.makeRotate(rquat); 
		rightHandRotateMatrix.preMult (osg::Matrixd::rotate( -3.14f / 2.0, 1, 0, 0 ));

		rightHandTransMatrix.makeTranslate(body_x + data.RightHandPosition.x() * 25.0f, body_y - data.RightHandPosition.z() *25.0f, body_z + data.RightHandPosition.y() * 25.0f -15 );
		rightHandResultMatrix = rightHandRotateMatrix * rightHandTransMatrix ;

		cursorHand2->setMatrix(rightHandResultMatrix);
	}

	
	
	

	//СОГНУТЬ ПАЛЕЦ
	{
		float sognut = -data.LeftHand_Trigger_x;

		//std::cout << "data.LeftHand_TrackPad_y" << data.LeftHand_TrackPad_y << std::endl;
		//std::cout << "data.LeftHand_EButton_ApplicationMenu" << data.LeftHand_EButton_ApplicationMenu << std::endl;
		///std::cout << "data.LeftHand_EButton_SteamVR_Touchpad" << data.LeftHand_EButton_SteamVR_Touchpad << std::endl;
		//std::cout << "data.LeftHand_EButton_SteamVR_Trigger" << data.LeftHand_EButton_SteamVR_Trigger << std::endl;
		

		RotateFinger ("ValveBiped_Bip01_L_Finger4",sognut);
		RotateFinger ("ValveBiped_Bip01_L_Finger41", sognut);
		RotateFinger ("ValveBiped_Bip01_L_Finger42", sognut);

		RotateFinger ("ValveBiped_Bip01_L_Finger3", sognut);
		RotateFinger ("ValveBiped_Bip01_L_Finger31", sognut);
		RotateFinger ("ValveBiped_Bip01_L_Finger32", sognut);

		//RotateFinger ("ValveBiped_Bip01_L_Finger2", data.zoom2/4);
		//RotateFinger ("ValveBiped_Bip01_L_Finger21", data.zoom2/4);
		//RotateFinger ("ValveBiped_Bip01_L_Finger22", data.zoom2/4);

		RotateFinger ("ValveBiped_Bip01_L_Finger2", sognut);
		RotateFinger ("ValveBiped_Bip01_L_Finger21", sognut);
		RotateFinger ("ValveBiped_Bip01_L_Finger22", sognut);

		RotateFinger ("ValveBiped_Bip01_L_Finger1", sognut);
		RotateFinger ("ValveBiped_Bip01_L_Finger11", sognut);
		RotateFinger ("ValveBiped_Bip01_L_Finger12", sognut);

		RotateFinger ("ValveBiped_Bip01_L_Finger0", sognut);
		RotateFinger ("ValveBiped_Bip01_L_Finger01", sognut);
		RotateFinger ("ValveBiped_Bip01_L_Finger02", sognut);




		
	}
	

	/*

	///

	//передвигаем руку
	osg::Matrix _i = viewer->getCamera()->getViewMatrix();
	osg::Matrix i = _i.inverse(_i);
	osg::Vec3 a = i.getTrans();
	osg::Quat q = i.getRotate();

	float yaw = atan2(2.0*(q.y()*q.z() + q.w()*q.x()), q.w()*q.w() - q.x()*q.x() - q.y()*q.y() + q.z()*q.z());
	float pitch = asin(-2.0*(q.x()*q.z() - q.w()*q.y()));
	float roll = atan2(2.0*(q.x()*q.y() + q.w()*q.z()), q.w()*q.w() + q.x()*q.x() - q.y()*q.y() - q.z()*q.z());

	{
		cursorHand->setMatrix(osg::Matrix::translate(a)); 
		osg::Matrix matrix2 = cursorHand->getMatrix();
		float radius_ =30-data.pos2/20.0f;

		if (radius_ <5) radius_ = 5;

		matrix2.preMult(osg::Matrix::translate(osg::Vec3 (     sin (-roll+ data.pos0/200.0f ) * radius_   ,       cos (-roll + data.pos0/200.0f) * radius_    ,  0+    data.pos1/15.0f            )));
		matrix2.preMult(osg::Matrix::rotate(roll + data.angle1/50.0f, osg::Vec3(0,0,1)));
		matrix2.preMult(osg::Matrix::rotate(- data.angle2/50.0f, osg::Vec3(0,1,0)));  //3.14/2.0f -
		matrix2.preMult(osg::Matrix::rotate(-3.14/2.0f + data.angle0/50.0f, osg::Vec3(1,0,0)));

		cursorHand->setMatrix(matrix2);
	}
	{
		cursorHand2->setMatrix(osg::Matrix::translate(a)); 
		osg::Matrix matrix2 = cursorHand2->getMatrix();
		float radius_ =30-data.left_pos2/20.0f;

		if (radius_ <5) radius_ = 5;

		matrix2.preMult(osg::Matrix::translate(osg::Vec3 (     sin (-roll+ data.left_pos0/200.0f ) * radius_   ,       cos (-roll + data.left_pos0/200.0f) * radius_    ,  0+    data.left_pos1/15.0f            )));
		matrix2.preMult(osg::Matrix::rotate(roll + data.left_angle1/50.0f, osg::Vec3(0,0,1)));
		matrix2.preMult(osg::Matrix::rotate(- data.left_angle2/50.0f, osg::Vec3(0,1,0)));  //3.14/2.0f -
		matrix2.preMult(osg::Matrix::rotate(-3.14/2.0f + data.left_angle0/50.0f, osg::Vec3(1,0,0)));

		cursorHand2->setMatrix(matrix2);
	}

	*/


	////находим абсолютные координаты пальца в 3D
	osg::Vec3f worldPosition;
	{
		//медленный вариант
		
		osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor("PALEZ_AIR", true);
		cursorHand->asGroup()->accept(*fnnv.get());
		if (!fnnv->_foundNodes.empty())
		{
			osg::Vec3f xyz =  fnnv->accumulated.getTrans();
			//std::cout  << xyz.x() << " "  << xyz.y() << " "  << xyz.z() << std::endl;
			worldPosition =  xyz;
		}

		//быстрый вариант
		//osg::MatrixList ml = cursorHandPalez->getWorldMatrices();
		//osg::Matrixd Msum = osg::Matrixd::identity();
		//for(unsigned int i = 0; i< ml.size() ; i++)
		//{
		//	Msum *= ml[i];
		//}
	}

	///// находим проекцию на экранную плоскость (2D)
	osg::Vec2d screenPosition; 
	{
		osg::Matrixd MVPW = viewer->getCamera()->getViewMatrix() * viewer->getCamera()->getProjectionMatrix() * viewer->getCamera()->getViewport()->computeWindowMatrix(); 
		osg::Vec4d screenPosition4d = osg::Vec4d(worldPosition, 1.0) * MVPW; 
		screenPosition4d = screenPosition4d / screenPosition4d.w(); 
		//screenPosition4d.y() = viewer->getCamera()->getViewport()->height() - screenPosition4d.y(); 
		screenPosition.set(screenPosition4d.x(), screenPosition4d.y()); 

		
		
		float x = screenPosition.x(); //+ WINDOW_WIDTH/(float)4;
		float y = screenPosition.y();

		if (x<0) x=0;
		if (x>WINDOW_WIDTH) x = WINDOW_WIDTH;
		if (y<0) y=0;
		if (y>WINDOW_HEIGHT) y = WINDOW_HEIGHT;

		//std::cout  << x << " "  << y  << std::endl;
		
		hydra_temp_x =x;
		hydra_temp_y = y;

		//std::cout << "RightHand_EButton_SteamVR_Trigger=" << data.RightHand_EButton_SteamVR_Trigger << std::endl;
		//std::cout << "old_hydra_temp_command=" << old_hydra_temp_command << std::endl;

		if ((data.RightHand_EButton_SteamVR_Trigger==true)&&(old_hydra_temp_command!=0))
		{
			hydra_temp_command=0; //push
			old_hydra_temp_command=0;
			return;
		}

		
		if ((data.RightHand_EButton_SteamVR_Trigger==true)&&(old_hydra_temp_command==0))
		{
			hydra_temp_command=1; //drag
			return;
		}

		if ((data.RightHand_EButton_SteamVR_Trigger==false)&&(old_hydra_temp_command==0))
		{
			hydra_temp_command=2; //release
			old_hydra_temp_command=2;
			//hydra_temp_command2=0; //
			return;
		}

		if ((data.RightHand_EButton_SteamVR_Trigger==false)&&(old_hydra_temp_command!=0))
		{
			hydra_temp_command=-1; //ничего
			old_hydra_temp_command=-1;
			return;
		}
		

		
	}

	
	
}


void UserHandsClass::RotateFinger (std::string name, float angle)
{
	FindNamedNodeVisitor fnnv(name);
    cursorHand->accept(fnnv);

	if (fnnv._foundNodes.empty()==true)
	{
		cursorHand2->accept(fnnv);
	}


    if (!fnnv._foundNodes.empty())
    {
		//float angle = osg::PI_2;
		osg::Vec3 axis(0,0,1);
		std::string rotate_name("rotate");
		osgAnimation::Bone* bone = dynamic_cast<osgAnimation::Bone*>(fnnv._foundNodes.front().get());
		if(osgAnimation::UpdateBone* update_bone = dynamic_cast<osgAnimation::UpdateBone*>(bone->getUpdateCallback()))
		{
			if(osgAnimation::StackedTransformElement* transform = find_stacked_transform_element(update_bone,rotate_name))
			{
				if(osgAnimation::StackedRotateAxisElement* rotation = dynamic_cast<osgAnimation::StackedRotateAxisElement*>(transform))
				{
					rotation->setAngle(angle);
					rotation->setAxis(axis);
				}
				else std::cout << "Hand is not StackedRotateAxisElement: " << rotate_name << " bone: " << std::endl;
			}
			else update_bone->getStackedTransforms().push_back(new osgAnimation::StackedRotateAxisElement(rotate_name,axis,angle));
		}
    }
	
}


UserHandsClass::UserHandsClass(osg::ref_ptr<osg::Node> _root, osg::ref_ptr<osgViewer::Viewer> _viewer, bool oculus_enable)
{

	old_hydra_temp_command=-1;
	hydra_temp_command2=-1;
	hydra_temp_command=-1;


	if(oculus_enable)
	{
		m_camera = new osg::Camera();
		m_camera->addChild(_root);
		m_camera->setReferenceFrame(osg::Camera::RELATIVE_RF);
	}

    try
    {
        std::cout << "UserHandsClass constructor ... " <<std::endl;
        //ссылка на геометрию сцены для обработки пересечений
        root = _root;
        //ссылка на окно просмотра сцены для обработки пересечений в случае манипулятора мышь
        viewer = _viewer;
        //прозрачный объект
        TransparentObjectsName="AIR";//"AIR";
        HUDCursorEnabled=false;

        //создает камеру (hud) и размещает там курсор (для Hud) и "курсорный текст"
        CreateHudCursorAndText();

        //FIX для horizontal split mode (используется для перемещения курсора в стереорежиме)
        HSmode = false;
        if (osg::DisplaySettings::instance()->getStereo()==true)
        {
            if (osg::DisplaySettings::instance()->getStereoMode()==osg::DisplaySettings::HORIZONTAL_SPLIT)
            {
                HSmode = true;
            }
        }


		//hydra
		CreateHydraHands();

		//создание 3D HUD MENU
		{
			osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor("HUD3D");
            root->accept(*fnnv.get());
			if (fnnv->_foundNodes.empty()==false)
			{
					std::cout << "HUD3D found... ok" << std::endl;
					hud3D = fnnv->_foundNodes.front().get();
			}
			else
			{
					throw (ErrorClass (__FILE__, __LINE__ , "UserHandsClass::UserHandsClass()", "HUD3D not find in root !!!", 6));
			};

			osg::StateSet* stateset = hud3D->getOrCreateStateSet();
			stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
			stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
			stateset->setRenderBinDetails( 100, "RenderBin", osg::StateSet::OVERRIDE_RENDERBIN_DETAILS);
			//ТОЖЕ САМОЕ мЫ ДОЛЖНЫ СДЕЛАТЬ ДЛЯ ВСЕЙ ГЕОМЕТРИИ КОТОРАЯ ОКАЗЫВАЕТСЯ В ЭТОМ МЕНЮ!	
		}

        //находим курсор
        {
            osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor("Sphere01");
            root->accept(*fnnv.get());
            if (!fnnv->_foundNodes.empty())
            {
                cursor = fnnv->_foundNodes.front().get();
            }
            else
            {
                throw (ErrorClass (__FILE__, __LINE__ , "UserHandsClass::UserHandsClass()", "Cursor not find in root !!!", 6));
            }
        }

        //узнаем начальные размеры экрана (вдруг изменятся) для корректировки пересечения с HUD
        {
            osgViewer::Viewer::Windows windows;
            viewer->getWindows(windows);
            osgViewer::Viewer::Windows::iterator itr = windows.begin();
            int tempx,tempy;
            int width, height;
            (*itr)->getWindowRectangle(tempx, tempy, width, height);
            START_WINDOW_WIDTH =width;
            START_WINDOW_HEIGHT = height;
        }
    }
    catch (ErrorClass error)
    {
        error.print();
    }

}

void UserHandsClass::update3DHUD(osg::Matrixd m)
{
	hud3D->setMatrix(m);
}

//возврат копии буфера накопленных событий во внутреннем формате (на интерпритацию)...
InternalEventsClass UserHandsClass::getHandsEvents()
{
    InternalEventsClass data;
    for (unsigned int i=0; i < events.size();i++)
    {
            data.addEvent (events.at(i));
    }
    events.clear();
    return data;
}

//Событие "рукоблудия" ...
void UserHandsClass::AddUserHandsEvent(std::string val, float a1=0,float a2=0, float a3=0, float a4=0)
{
    InternalEventsClass::OneInternalEventClass temp;
    temp.Event = val;
    if (val.find("_DRAG")!=std::string::npos)
    {
        temp.number_param.push_back(a1);
        temp.number_param.push_back(a2);
        temp.number_param.push_back(a3);
        temp.number_param.push_back(a4);
    }
    temp.time = osg::Timer::instance()->time_s();
    events.push_back (temp);
}


void UserHandsClass::setDataAndCalc(HandsData Hand)
{
	if(m_camera)
	{
		m_camera->setProjectionMatrix(viewer->getCamera()->getProjectionMatrix());
		m_camera->setViewMatrix(viewer->getCamera()->getViewMatrix());
		m_camera->setViewport(viewer->getCamera()->getViewport());
	}

	if (hydra_presend==true)
	{
		
		HandsData::InterpretationOperationsClass::InterpretationModeOperations operation = HandsData::InterpretationOperationsClass::NO;

		Hand.SetMode (HandsData::INTERPRETATION2D);


		if (hydra_temp_command==0)
		{
			operation = HandsData::InterpretationOperationsClass::PUSH;
			Hand.SetMode (HandsData::INTERPRETATION2D);

			//std::cout << "PUSH" << std::endl;
		}
		if (hydra_temp_command==1)
		{
			operation = HandsData::InterpretationOperationsClass::DRAG;
			Hand.SetMode (HandsData::INTERPRETATION2D);

			//std::cout << "drag" << std::endl;
		}
		if (hydra_temp_command==2)
		{
			operation = HandsData::InterpretationOperationsClass::RELEASE;
			Hand.SetMode (HandsData::INTERPRETATION2D);

			//std::cout << "RELEASE" << std::endl;
		}
		if ((hydra_temp_command==3))
		{
			Hand.SetMode (HandsData::INTERPRETATION2D);
			cursor->setNodeMask (0x000000);
			cursorForHud->setNodeMask (0x000000);
			textForCursor->setNodeMask (0x000000);

				//std::cout << "NONE" << std::endl;
		}
		Hand.addOperation (operation,hydra_temp_x,hydra_temp_y,osg::Timer::instance()->time_s());
		
			
		if (hydra_temp_command2==0)
		{
			HandsData::InterpretationOperationsClass::InterpretationModeOperations operation = HandsData::InterpretationOperationsClass::USE;
			Hand.SetMode (HandsData::INTERPRETATION2D);
			Hand.addOperation (operation,hydra_temp_x,hydra_temp_y,osg::Timer::instance()->time_s());
			
			//std::cout << "USE" << std::endl;
		}
			
			
		
	}


    //узнаем размеры экрана (вдруг изменились)
    {
        osgViewer::Viewer::Windows windows;
        viewer->getWindows(windows);
        osgViewer::Viewer::Windows::iterator itr = windows.begin();
        int tempx,tempy;
        int width, height;
        (*itr)->getWindowRectangle(tempx, tempy, width, height);
        WINDOW_WIDTH =width;
        WINDOW_HEIGHT = height;
    }


    //если руки вообще не используются....
    if (Hand.GetMode() == HandsData::NONE)
    {
        //прячем курсор... или руки
        cursor->setNodeMask (0x000000);
        cursorForHud->setNodeMask (0x000000);
        textForCursor->setNodeMask (0x000000);

        //std::cout << "HandsData::NONE" << std::endl;

        //если хотим знать значения переменных CurrentObjectName и т.д. когда НЕ РЕЖИМ КУРСОРА
        //GetObjectInfoFromXY(800,600);
        //Отпускание объекта при переключении режима/убратья руки
        if (oldCurrentObjectName!="")
        {
            //std::cout << oldCurrentObjectName << "_RELEASE" << " " << osg::Timer::instance()->time_s() << std::endl;
            AddUserHandsEvent(oldCurrentObjectName+"_RELEASE");
            AddUserHandsEvent("_RELEASE");
            oldCurrentObjectName="";
        }

        //FIX для того, чтобы имя объекта было "" если не управляем руками.....
        {
            CurrentObjectName="not using hands";
            ParentObjectName="not using hands";
            CurrentObjectIntersectsCoordLOCAL = osg::Vec3(0,0,0);
            CurrentObjectIntersectsCoordWORLD  = osg::Vec3(0,0,0);
        }

        return;
    }

    //если данные необходимо интерпретировать (2D-3D), начинаем
    static float oldx, oldy;
    float cursorx=0;
    float cursory=0;
    if (Hand.GetMode() == HandsData::INTERPRETATION2D)
    {
        for (unsigned int i=0; i< Hand.getOperations().size(); i++)
        {
            //std::cout << Hand.getOperations().at(i).operation << " " << Hand.getOperations().at(i).x << " " << Hand.getOperations().at(i).y << " " << Hand.getOperations().at(i).time <<  std::endl;
            //std::cout << Hand.getOperations().size () <<std::endl ;
            //определяем значения переменных CurrentObjectName, ParentObjectName, координыты пересечения и т.д.
            cursorx = Hand.getOperations().at(i).x;
            cursory = Hand.getOperations().at(i).y;

            GetObjectInfoFromXY(Hand.getOperations().at(i).x, Hand.getOperations().at(i).y);
            //при режиме перетаскивания изменение имени объекта происходить не должно! Иначе перетаскивать мы будем все на что попал курсор.
            //Формирования событий пользователя
            //
            if (Hand.getOperations().at(i).operation  == HandsData::InterpretationOperationsClass::PUSH)
            {
                //необходимо, чтобы перетаскивание не прерывалось, если курсор уходит с объекта
                oldCurrentObjectName = CurrentObjectName;
                //
                //std::cout << CurrentObjectName << "_PUSH" << " " << osg::Timer::instance()->time_s() << std::endl;
                AddUserHandsEvent(CurrentObjectName+"_PUSH");
                AddUserHandsEvent("_PUSH");
                oldx=Hand.getOperations().at(i).x;
                oldy=Hand.getOperations().at(i).y;
            }
            if (Hand.getOperations().at(i).operation  == HandsData::InterpretationOperationsClass::RELEASE)
            {
                //std::cout << oldCurrentObjectName << "_RELEASE" << " " << osg::Timer::instance()->time_s() << std::endl;
                AddUserHandsEvent(oldCurrentObjectName+"_RELEASE");
                AddUserHandsEvent("_RELEASE");
                oldCurrentObjectName="";
            }
            if (Hand.getOperations().at(i).operation  == HandsData::InterpretationOperationsClass::USE)
            {
                //std::cout << CurrentObjectName << "_USE" << " " << osg::Timer::instance()->time_s() << std::endl;
                AddUserHandsEvent(CurrentObjectName+"_USE");
                AddUserHandsEvent("_USE");

            }
            if (Hand.getOperations().at(i).operation  == HandsData::InterpretationOperationsClass::DRAG)
            {
                //необходимо, чтобы перетаскивание не прерывалось, если курсор уходит с объекта
                //
                CurrentObjectName =	oldCurrentObjectName;
                //std::cout << CurrentObjectName << "_DRAG" << " " << osg::Timer::instance()->time_s() << std::endl;
                AddUserHandsEvent(CurrentObjectName+"_DRAG",Hand.getOperations().at(i).x*2/WINDOW_WIDTH-1,Hand.getOperations().at(i).y*2/WINDOW_HEIGHT-1,oldx*2/WINDOW_WIDTH-1,oldy*2/WINDOW_HEIGHT-1);
                AddUserHandsEvent("_DRAG",Hand.getOperations().at(i).x*2/WINDOW_WIDTH-1,Hand.getOperations().at(i).y*2/WINDOW_HEIGHT-1,oldx*2/WINDOW_WIDTH-1,oldy*2/WINDOW_HEIGHT-1);
                oldx=Hand.getOperations().at(i).x;
                oldy=Hand.getOperations().at(i).y;
            }

        }
        Hand.getOperations().clear();
    }

    if (HUDCursorEnabled==false)
    {
        osg::Matrix _i = viewer->getCamera()->getViewMatrix();
        osg::Matrix i = _i.inverse(_i);

        osg::Vec3 a = i.getTrans();
        osg::Vec3 b = CurrentObjectIntersectsCoordWORLD;

        float Length = sqrtf( osg::square(a.x()-b.x())  +  osg::square(a.y()-b.y()) + osg::square(a.z()-b.z())  );

        float k = 1;
        if (Length>40) k = Length/40.0f;

        //std::cout << "Length = " << Length << " k= " << k << std::endl;

        //cursor->sermatrix.....(x,y,z);
        cursor->setNodeMask (0xffffff);
        //cursorForHud->setNodeMask (0xffffff);
        textForCursor->setNodeMask (0xffffff);
        //устанавливаем позицию курсора + сохраняем его масштаб
        //osg::Vec3d scale = cursor->getMatrix().getScale();
        cursor->setMatrix(osg::Matrix::translate(CurrentObjectIntersectsCoordWORLD));
        osg::Matrix matrix2 = cursor->getMatrix();
        //matrix2.preMult(osg::Matrix::scale(scale));
        matrix2.preMult(osg::Matrix::scale(osg::Vec3 (k,k,k)));
        cursor->setMatrix(matrix2);

    }
    else
    {
        //прячем курсор... или руки
        cursor->setNodeMask (0x000000);
    }



    //Передвигаем HUD-курсор и "курсорный" текст
    {
        //
        float kostilX = ((float)START_WINDOW_WIDTH / (float)WINDOW_WIDTH);
        float kostilY = ((float)START_WINDOW_HEIGHT /(float)WINDOW_HEIGHT);
        cursorx = cursorx * kostilX;
        cursory = cursory * kostilY;

        if ((HSmode==true)&&(hydra_presend==false))
        {
            cursorx = (cursorx  - (float)START_WINDOW_WIDTH/(float)4) * 2.0f ; //* kostilX
        }
        //собственно передвигаем.....
        cursorForHud->setMatrix(osg::Matrix::translate(osg::Vec3(cursorx, cursory, 0)));
        osg::Matrix matrix2 = cursorForHud->getMatrix();
        matrix2.preMult(osg::Matrix::scale(osg::Vec3 (WINDOW_HEIGHT/768,WINDOW_HEIGHT/768,1)));
        cursorForHud->setMatrix(matrix2);
        //updateText->setPosition(osg::Vec3(cursorx+20*WINDOW_HEIGHT/1024, cursory, 0));
        //float cursorwidth = updateText->getBound().xMax() -updateText->getBound().xMin()  ;
		float cursorwidth = updateText->getBound().radius();
        if ((cursorx+cursorwidth+20*WINDOW_HEIGHT/1024) <= (float)START_WINDOW_WIDTH)
        {
            textForCursor->setMatrix(osg::Matrix::translate(osg::Vec3(cursorx+20*WINDOW_HEIGHT/1024, cursory, 0)));
        }
        else
        {
            textForCursor->setMatrix(osg::Matrix::translate(osg::Vec3(cursorx-(cursorwidth*1.3f+60)*WINDOW_HEIGHT/1024, cursory, 0)));
        }
    }

    return;
}

osg::ref_ptr<osg::Node> UserHandsClass::CreateCursorShape()
{
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    osg::ref_ptr<osg::Vec4Array> shared_colors = new osg::Vec4Array;
    shared_colors->push_back(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
    osg::ref_ptr<osg::Vec3Array> shared_normals = new osg::Vec3Array;
    shared_normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));

    // create Geometry object to store all the vertices and lines primitive.
    osg::ref_ptr<osg::Geometry> polyGeom = new osg::Geometry();
    const float k=0.2;
    osg::Vec3 myCoords[] =
    {
        osg::Vec3(0, 0, 0.0f ),
        osg::Vec3(100*k, -50*k, 0),
        osg::Vec3(80*k, -60 *k, 0),
        osg::Vec3(100*k, -80 *k, 0 ),
        osg::Vec3(90*k, -90 *k, 0),  //polovina
        osg::Vec3(80*k, -100 *k, 0), //polovina
        osg::Vec3(60*k, -80 *k, 0),
        osg::Vec3(50*k, -100 *k, 0)

    };

    int numCoords = sizeof(myCoords)/sizeof(osg::Vec3);
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(numCoords,myCoords);
    // pass the created vertex array to the points geometry object.
    polyGeom->setVertexArray(vertices);
    // use the shared color array.
    polyGeom->setColorArray(shared_colors.get());
    polyGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
    // use the shared normal array.
    polyGeom->setNormalArray(shared_normals.get());
    polyGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);
    // This time we simply use primitive, and hardwire the number of coords to use
    // since we know up front,
    polyGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON,0,numCoords));
    // add the points geometry to the geode.
    geode->addDrawable(polyGeom);



    //line
    // create Geometry object to store all the vertices and lines primitive.
    osg::ref_ptr<osg::Geometry> linesGeom = new osg::Geometry();
    // pass the created vertex array to the points geometry object.
    linesGeom->setVertexArray(vertices);
    // set the colors as before, plus using the above
    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
    colors->push_back(osg::Vec4(0.0f,0.0f,0.0f,1.0f));
    linesGeom->setColorArray(colors);
    linesGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
    normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));
    linesGeom->setNormalArray(normals);
    linesGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);
    linesGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP,0,numCoords));
    geode->addDrawable(linesGeom);

    return geode;
}

//создает камеру (hud) и размещает там курсор (для Hud) и "курсорный текст"
void UserHandsClass::CreateHudCursorAndText()
{
    //osg::ref_ptr<osg::Node> CursorForHud = osgDB::readNodeFile("./CursorForHUD.osg");
    osg::ref_ptr<osg::Node> CursorForHud = CreateCursorShape();

    CursorForHud->setName("__CURSORFORHUD__AIR");
    if (!CursorForHud.valid())
    {
         throw (ErrorClass (__FILE__, __LINE__ , "UserHandsClass::UserHandsClass()", "CursorForHUD.osg not load.", 6));
    }
    osg::ref_ptr<osg::StateSet> stateset = CursorForHud->getOrCreateStateSet();
    stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
    stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);

	//это для 3d HUD MENU
		//	stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
		//	stateset->setRenderBinDetails( 50, "RenderBin", osg::StateSet::OVERRIDE_RENDERBIN_DETAILS);
	//

    cursorForHud = new osg::MatrixTransform;
    cursorForHud->setName("__CURSORFORHUD__AIR");
    cursorForHud->addChild(CursorForHud);

	cursorForHud->setNodeMask (0x000000);

    //узнаем размеры экрана (вдруг изменились)
    {
        osgViewer::Viewer::Windows windows;
        viewer->getWindows(windows);
        osgViewer::Viewer::Windows::iterator itr = windows.begin();
        int tempx,tempy;
        int width, height;
        (*itr)->getWindowRectangle(tempx, tempy, width, height);
        WINDOW_WIDTH =width;
        WINDOW_HEIGHT = height;
    }

    osg::ref_ptr<osg::Camera> camera = createHUDCamera (0,WINDOW_WIDTH,0,WINDOW_HEIGHT);
    camera->setName("__CURSOR_CAMERA__");
    {
        updateText = new osgText::Text;
        osg::ref_ptr<osg::Geode> geode = new osg::Geode();
        osg::ref_ptr<osg::StateSet> stateset = geode->getOrCreateStateSet();
        stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
        stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
        geode->setName("CURSOR_AIR");
        geode->addDrawable( updateText.get() );
        textForCursor = new osg::MatrixTransform;
        textForCursor->addChild(geode);
        camera->addChild(textForCursor.get());
        std::string fontName("fonts/verdana.ttf");
        updateText->setFont(fontName);
        updateText->setCharacterSize(24.0f*WINDOW_HEIGHT/1200);
        updateText->setColor(osg::Vec4(1.0f,0.0f,0.0f,0.5f));
        updateText->setText("");
    }
    camera->addChild(cursorForHud);
    root->asGroup()->addChild(camera.get());
    //root->asGroup()->removeChild(CursorForHud.get());
    osg::Vec3 position(WINDOW_WIDTH*0.5,WINDOW_HEIGHT*0.5,-89000.0f);
    cursorForHud->setMatrix(osg::Matrix::translate(position));
}

//функция изменяет значения переменных CurrentObjectName, ParentObjectName, координыты пересечения и т.д.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
//ДОДЕЛАТЬ: проходить все пересеченные объекты! в поисках меню и еще раз сравнивать расстояния!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
void UserHandsClass::GetObjectInfoFromXY(float x, float y)
{
	osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector (osgUtil::Intersector::WINDOW, x, y);
	osgUtil::IntersectionVisitor iv( intersector.get() );

	if(m_camera) m_camera->accept( iv );
	else viewer->getCamera()->accept( iv );


	/*
	osg::ref_ptr<osgUtil::LineSegmentIntersector> terrainIsect;
	terrainIsect = new osgUtil::LineSegmentIntersector( osg::Vec3f(BodyX  , BodyY ,BodyZ ) ,
                                                         osg::Vec3f(BodyX +x , BodyY +y , BodyZ-100) );
	 osgUtil::IntersectionVisitor isectVisitor( terrainIsect.get() );
    // проверка на пересечение с ROOM / или со всеми объектами сцены
    root->accept(isectVisitor);

	*/

    //самый первый объект
	if (intersector->getIntersections().size()==0)
    {
            CurrentObjectName="";
           // return;
    }
    //
    for(osgUtil::LineSegmentIntersector::Intersections::iterator hitr=intersector->getIntersections().begin();  hitr!=intersector->getIntersections().end(); ++hitr)
    {
        CurrentObjectName="";
        ParentObjectName="";
        //определяем имя
        if (!hitr->nodePath.empty() && !(hitr->nodePath.back()->getName().empty()))
        {
            //запоминаем имя объекта, на который прицелен курсор
            CurrentObjectName = hitr->nodePath.back()->getName();
        }
        else if (hitr->drawable.valid())
        {
            //отображаем предка _geode
            //запоминаем имя объекта, на который прицелен курсор

			// Старый вариант поиска объектов. У всех Instance опказывает одно имя.
            // CurrentObjectName = hitr->nodePath.back()->getParent(0)->getName();
            // ParentObjectName = hitr->nodePath.back()->getParent(0)->getParent(0)->getName();

			//! Новый вариант поиска объектов. Instance работеат верно.
			for(int kk = hitr->nodePath.size() - 1; kk > 0; kk--)
			{
				if (hitr->nodePath.at(kk)->getName() != "")
				{
					CurrentObjectName = hitr->nodePath.at(kk)->getName();
					ParentObjectName = CurrentObjectName;
					break;
				}
			}				
        }
        if ((CurrentObjectName=="")&&(ParentObjectName!=""))
        {
            CurrentObjectName=ParentObjectName;
        }
        //Курсор является "прозрачным"
        if ((CurrentObjectName=="hand_right")||(CurrentObjectName=="Sphere01")||(CurrentObjectName.find (TransparentObjectsName)!=std::string::npos))
        {
            CurrentObjectName="";
            continue;
        }
        //запоминаем ЛОКАЛЬНЫЕ координыты пересечения
        CurrentObjectIntersectsCoordLOCAL = hitr->getLocalIntersectPoint();
        //запоминаем МИРОВЫЕ координыты пересечения
        CurrentObjectIntersectsCoordWORLD = hitr->getWorldIntersectPoint();
        CurrentObjectIntersectsNormalWORLD = hitr->getWorldIntersectNormal();
        //std::cout << "object: "<<CurrentObjectName<< std::endl;
        //return;
        break;
    }
  

    //HUD
    //1. обработка пересеченеия со сценой в координатах x,y,-90000..-110000
    {
        //определение высоты над объектом 1 лучем вниз


        osg::ref_ptr<osgUtil::LineSegmentIntersector> terrainIsect;
        //
        float kostilX = ((float)START_WINDOW_WIDTH / (float)WINDOW_WIDTH);
        float kostilY = ((float)START_WINDOW_HEIGHT /(float)WINDOW_HEIGHT);

        //создание сегмента линии
        //FIX для horizontal split mode
        bool HSmode = false;
        if (osg::DisplaySettings::instance()->getStereo()==true)
        {
            if (osg::DisplaySettings::instance()->getStereoMode()==osg::DisplaySettings::HORIZONTAL_SPLIT)
            {
                HSmode = true;
            }
        }

        if (HSmode==false)
        {
            terrainIsect = new osgUtil::LineSegmentIntersector( osg::Vec3f(x * kostilX , y * kostilY, -90000) ,
                                                    osg::Vec3f(x * kostilX , y * kostilY, -110000) );
        }
        else
        {
            terrainIsect = new osgUtil::LineSegmentIntersector( osg::Vec3f((x * kostilX - (float)START_WINDOW_WIDTH/(float)4) * 2.0f , y * kostilY, -90000) ,
                                                    osg::Vec3f((x * kostilX - (float)START_WINDOW_WIDTH/(float)4) * 2.0f , y * kostilY, -110000) );
        }
		if ((HSmode==true)&&(hydra_presend==true))
		{
			terrainIsect = new osgUtil::LineSegmentIntersector( osg::Vec3f(x * kostilX  + 0 , y * kostilY, -90000) ,
                                                    osg::Vec3f(x * kostilX  + 0, y * kostilY, -110000) );
		}
		 

		osgUtil::IntersectionVisitor isectVisitor( terrainIsect.get() );
		//viewer->getCamera()->accept( iv );

        // проверка на пересечение с ROOM / или со всеми объектами сцены
        root->accept(isectVisitor);
        //получение объектов, пересеченных сегментом линии с индексом 0
		osgUtil::LineSegmentIntersector::Intersections hitList = terrainIsect->getIntersections();
        //перебор объектов ( начиная с самого ближнего по пересечению и  заканчивая самим дальним)
        std::string _ObjectName="";

        if (hitList.empty()==true)
        {

            if (HUDCursorEnabled==true)
            {
                HUDCursorEnabled=false;
                //выключаем курсор
                //osgViewer::Viewer::Windows windows;
                //viewer->getWindows(windows);
                //for(osgViewer::Viewer::Windows::iterator itr = windows.begin();	itr != windows.end();++itr)
                //{
                   //(*itr)->useCursor(false);
                   //(*itr)->setCursor(osgViewer::GraphicsWindow::NoCursor);
                   cursorForHud->setNodeMask (0x000000);
                   //textForCursor->setNodeMask (0x000000);
                //}
            }
            return;
        }


        for(osgUtil::LineSegmentIntersector::Intersections::iterator hitr=hitList.begin(); hitr!=hitList.end();	hitr++)
        {
            _ObjectName = GetName(hitr);

            //"прозрачные" объекты
            if ((_ObjectName.find(TransparentObjectsName)!=std::string::npos) || (_ObjectName=="Sphere01") || (_ObjectName=="hand_right") )
            {
                _ObjectName="";
                continue;
            }

            if (_ObjectName!="")
            {
                //ПЕРЕБИВАЕМ ДАННЫЕ
                CurrentObjectName = _ObjectName;
                //запоминаем ЛОКАЛЬНЫЕ координыты пересечения
                CurrentObjectIntersectsCoordLOCAL = hitr->getLocalIntersectPoint();
                //запоминаем МИРОВЫЕ координыты пересечения
                CurrentObjectIntersectsCoordWORLD = hitr->getWorldIntersectPoint();
                CurrentObjectIntersectsNormalWORLD = hitr->getWorldIntersectNormal();
                //ВОТ!!!!!
                //if (HUDCursorEnabled==false)
                {
                    HUDCursorEnabled=true;
                    //включаем курсор
                    cursorForHud->setNodeMask (0xffffff);
                    textForCursor->setNodeMask (0xffffff);
                    //osgViewer::Viewer::Windows windows;
                    //viewer->getWindows(windows);
                    //for(osgViewer::Viewer::Windows::iterator itr = windows.begin();	itr != windows.end();++itr)
                    //{
                        //(*itr)->useCursor(true);
                        //(*itr)->setCursor(osgViewer::GraphicsWindow::RightArrowCursor);
                        //(*itr)->setCursor(osgViewer::GraphicsWindow::NoCursor);
                        //cursorForHud->setNodeMask (0xffffff);
                        //textForCursor->setNodeMask (0xffffff);
                        //std::cout  << ":)" << std::endl;
                    //}
                }
                return;
            }
        }
    }

}

//получение имени объекта
std::string UserHandsClass::GetName(osgUtil::LineSegmentIntersector::Intersections::iterator hitr)
{
    std::string CurrentObjectName="";
    //определяем имя
    CurrentObjectName = hitr->drawable->getName();

	if (CurrentObjectName=="")
	{
		if (!hitr->nodePath.empty() && !(hitr->nodePath.back()->getName().empty()))
		{
			// the geodes are identified by name.
			CurrentObjectName = hitr->nodePath.back()->getName();
		}
		else if (hitr->drawable.valid())
		{
			CurrentObjectName =  hitr->drawable->getName();
		}
	}

   
    if (CurrentObjectName=="")
    {
        CurrentObjectName = hitr->drawable->getParent(0)->getName();
    }
    if (CurrentObjectName=="")
    {
	  // osg::NodePath::const_iterator hitNodeIt = hitr->nodePath.begin();
      // CurrentObjectName = (*hitNodeIt)->getParent(0)->getParent(0)->getName();

		CurrentObjectName = hitr->nodePath.back()->getParent(0)->getParent(0)->getName();
    }
    if (CurrentObjectName=="")
    {
        CurrentObjectName = hitr->drawable->getParent(0)->getParent(0)->getName();
    }
    return CurrentObjectName;
}

//возврат значений по текущему объекту управления
std::string UserHandsClass::GetCurrentObjectName()
{
    return CurrentObjectName;
}
std::string UserHandsClass::GetParentObjectName()
{
    return ParentObjectName;
}
osg::Vec3 UserHandsClass::GetCurrentObjectIntersectsCoordLOCAL()
{
    return CurrentObjectIntersectsCoordLOCAL;
}
osg::Vec3 UserHandsClass::GetCurrentObjectIntersectsCoordWORLD()
{
    return CurrentObjectIntersectsCoordWORLD;
}

//замена значения текста рядом с курсором
void UserHandsClass::SetCursorText(std::string value)
{
    updateText->setText(value);
}
//замена значения размера текста рядом с курсором
void UserHandsClass::SetCursorTextSize (int newsize)
{
    updateText->setCharacterSize(newsize);
}
//замена значения шрифта текста рядом с курсором
void UserHandsClass::SetCursorTextFont (std::string filename)
{
    updateText->setFont(filename);
}
//замена значения цвета текста рядом с курсором
void UserHandsClass::SetCursorTextColor (float r,float g ,float b ,float a)
{
    updateText->setColor(osg::Vec4(r,g,b,a));
}


osg::Camera* UserHandsClass::createHUDCamera( double left, double right, double bottom, double top )
{
	osg::ref_ptr<osg::Camera> camera = new osg::Camera;
	camera->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
	camera->setClearMask( GL_DEPTH_BUFFER_BIT );
	camera->setRenderOrder( osg::Camera::POST_RENDER );
	camera->setAllowEventFocus( false );
	camera->setProjectionMatrix(	osg::Matrix::ortho2D(left, right, bottom, top) );
	//camera->getOrCreateStateSet()->setMode(	GL_LIGHTING, osg::StateAttribute::OFF );
	camera->setNodeMask(0x2);
	return camera.release();
}