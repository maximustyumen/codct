#ifndef HEADDATACLASS
#define HEADDATACLASS

class HeadData
{
public:
	//конструктор
	HeadData();

	//режим 
	enum HeadMode
	{
		NONE,			//не ориентации в пространстве, ни координат
		ORIENTATION,	//только ориентация
		POSITION,		//только координаты
		ALL				//и то и то
    };
	//
	void SetMode(HeadMode);
	HeadMode GetMode();

	//ориентация головы (градусы)
	float GetPitch(); //вверх-вниз
	float GetRoll();  //влево-вправо
	float GetYaw();	  //по Z

	//координаты головы
	float GetX(); 
	float GetY(); 
	float GetZ();
	//
	void SetPitch(float);
	void SetRoll(float);
	void SetYaw(float);
	void SetX(float);
	void SetY(float);
	void SetZ(float);

private:
	float pitch;
	float roll;
	float yaw;
	HeadMode mode;
	float x;
	float y;
	float z;
};

#endif


