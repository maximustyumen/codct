#ifndef FOOTDATACLASS
#define FOOTDATACLASS

#include <iostream>
#include <string>

class FootData
{
public:
	//конструктор
	FootData();

	//режим 
	enum FootMode
	{
		NONE,			//режим не определен, все данные не достоверны
		VECTORS,		//вектора направления по xyz
		TRACKER,		//абсолютные координаты с трекеров (xyz)
		ALL				//и то и то
    };
	void SetMode(FootMode);
	FootMode GetMode();

	//направление движения
	float getVectorX();
	float getVectorY();
	float getVectorZ();
	//абсолютные координаты
	float GetX(); 
	float GetY(); 
	float GetZ();
	//
	void SetVectorX(float);
	void SetVectorY(float);
	void SetVectorZ(float);
	void SetX(float);
	void SetY(float);
	void SetZ(float);

	//zoom
	bool GetZoom();
	void SetZoom(bool);



private:
	FootMode mode;

	float vectorX;
	float vectorY;
	float vectorZ;
	float x;
	float y;
	float z;
	bool zoom;
};

#endif


