/***************************************************************************
BaseCharacter.h  -  Проект виртуальной лаборатории
-------------------
begin                : 14 марта 2023
рефакторинг          : 22 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/
//Определение базового персонажа 3D сцены

#include "./BaseCharacter.h"

//конструктор
BaseCharater::BaseCharater(std::string CharacterName, std::string filename)
{
	std::cout  << "Player: Create character ... " << CharacterName << " from " << filename <<std::endl;
	// -- Load model --
	// scope for model ref_ptr
    osg::ref_ptr< osgCal::CoreModel > coreModel( new osgCal::CoreModel() );
    osg::ref_ptr< osgCal::BasicMeshAdder > meshAdder( new osgCal::DefaultMeshAdder );
    osg::ref_ptr< osgCal::MeshParameters > p( new osgCal::MeshParameters );
    p->software = true; // default false - через шейдеры
    coreModel->load(filename, p.get() );
	std::cout << "Find animations:" << std::endl;
    for ( size_t i = 0; i < coreModel->getAnimationNames().size(); i++ )
    {
		std::cout << coreModel->getAnimationNames()[i] << std::endl; 
    }
	animationNames = coreModel->getAnimationNames();
	//Строим модель
	osg::ref_ptr<osg::Node> NewCharacter = makeModel( coreModel.get(), meshAdder.get() );
	//обертка
	Character_MatrixTransform = new osg::MatrixTransform();
	Character_MatrixTransform->addChild(NewCharacter.get());
	Character_MatrixTransform->setName (CharacterName);
	//object = new osg::PositionAttitudeTransform();
	//object->addChild(m_MatrixTransform);
	//Указатель на модель
	model = (osgCal::Model*)NewCharacter.get();
	//0 - номер анимации (в порядке следования в cfg-файле
	//model->executeAction(0);
	//model->blendCycle( 0   , 1.0f, 1.0 ); 
	// end of model's ref_ptr scope
	//
	CharacterAnimation = new CharecterUpdateCallback();
	Character_MatrixTransform->setUpdateCallback(CharacterAnimation.get());
	CharacterAnimation->HappyExecuted = true;
}


osg::ref_ptr<osg::Node> BaseCharater::makeModel( osg::ref_ptr<osgCal::CoreModel> cm, osg::ref_ptr<osgCal::BasicMeshAdder> ma)
{
	osg::ref_ptr<osgCal::Model> model = new osgCal::Model();
	model->load( cm.get(), ma.get() );
	return model.get();
}

//принудительное перемещение персонажа
void BaseCharater::SetPosition (float x, float y, float z)
{
	//перемещаем объект
	Character_MatrixTransform->setMatrix(
									osg::Matrix::translate(x,y,z)*
									osg::Matrix::scale(Character_MatrixTransform->getMatrix().getScale())*
                                    osg::Matrix::rotate(0.0f,0.0f,0.0f,1.0f)
									);
	//настраиваем анимацию
	CharacterAnimation->currentX = x;
	CharacterAnimation->newX =x;
	CharacterAnimation->currentY =y;
	CharacterAnimation->newY =y;
	CharacterAnimation->currentZ = z;
	CharacterAnimation->HappyExecuted=true;
}

//Переключение анимации
void BaseCharater::SetAnimation(int AnimationNum)
{
	//AnimationNum - номер анимации (в порядке следования в cfg-файле
	model->executeAction(AnimationNum);
	model->blendCycle(AnimationNum , 1.0f, 1.0 ); 
}

//анимационное перемещение объекта в указанные координаты
void BaseCharater::GoTo(float x,float y, float z)
{
	//настраиваем анимацию
	CharacterAnimation->newX =x;
	CharacterAnimation->newY =y;
        CharacterAnimation->newZ = z;
	CharacterAnimation->HappyExecuted=false;
}

//Прикрепляетобъект к кости
void BaseCharater::ConnectObjectToBone(osg::ref_ptr<osg::Node> node, int boneID)
{
	model->addNode(boneID,node.get());
}


//конструктор
CharecterUpdateCallback::CharecterUpdateCallback()
{
	currentX=0;
	currentY=0;
	currentZ=0;
	newX=0;
	newY=0;
	currentRotation=0;
	newRotation=0;
	HappyExecuted=false;
	oldtime=0;
	// Скорость перемещения (единиц в секунду).
    pSpeed=10.0f;
	// Скорость поворота (радиан в секунду).
    rSpeed=0.5f;
}

void CharecterUpdateCallback::operator() (osg::Node* node, osg::NodeVisitor* nv)
{
	if (HappyExecuted==true) 
	{
		// must continue subgraph traversal.
		traverse(node,nv);  
		return;
	}

	osg::ref_ptr<osg::MatrixTransform> transform = dynamic_cast<osg::MatrixTransform*>(node);                
	if (nv && transform.get() && nv->getFrameStamp())
	{
		//расчет прошедшего времени
     	double time = nv->getFrameStamp()->getReferenceTime();
		if (oldtime == 0) oldtime = time;
		float dtime = (float)time - oldtime;
		oldtime = (float)time;

		float dx = (newX - currentX);
		float dy = (newY - currentY);
		float d = (float)sqrt(dx * dx + dy * dy);

		// Если нужно пройти расстояние меньше чем скорость, то переходим в конечное положение сразу,
		// иначе мы можем промахнуться мимо нее.
		if (fabs(d) < (pSpeed * dtime))
		{
			currentX = newX;
			currentY = newY;
			// Дабы дальше ничего не считать.
			d = 0;
			HappyExecuted=true;
		}

		// Новый угол поворота. Конечныый.
		//float newA = 0;

		// Если необходимо идти, то поворачиваемся в нужном направлении.
		if (d != 0)
		{
			// Считаем косинус угла поворота.
			newRotation = (float)acos(dx / d);
			// Cos(-X) = Cos(X). Поэтому при отрицательном dy нужно менять знак угла.
			if (dy < 0) newRotation = -newRotation;

			float da = (newRotation - currentRotation);
			// Если нужно повернуть на угол меньше чем скорость поворота, то сразу установим нужный угол,
			// Иначе мы можем промахнуться.
			// Math.Abs(da) / da - нормировка поворота.
			if (fabsf(da) > (rSpeed * dtime))
				currentRotation += fabsf(da) / da * rSpeed * dtime;
			else
				currentRotation = newRotation;
		}

		// Идем только если мы смотрим в нужную сторону.
		if ((currentRotation == newRotation) && (d != 0))
		{
			if (dx != 0)
				currentX += (float)cos(currentRotation) * pSpeed * dtime;
			if (dy != 0)
				currentY += (float)sin(currentRotation) * pSpeed * dtime;
		}
		
		
		transform->setMatrix(
			osg::Matrix::rotate(currentRotation+osg::PI /2.0,osg::Vec3(0,0,1))*
			osg::Matrix::translate(osg::Vec3d(currentX,currentY,currentZ))*
               osg::Matrix::scale (transform->getMatrix().getScale())
            );
			

		
    }
    // must continue subgraph traversal.
    traverse(node,nv);  
}


