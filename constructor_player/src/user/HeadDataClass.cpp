/***************************************************************************
HeadDataClass.cpp  -  стандартный интерфейс для описания состояния "головы"
-------------------
begin                : 29 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

#include "HeadDataClass.h"


HeadData::HeadData()
{
	mode = NONE;
	pitch=0.0f;
	roll=0.0f;
	yaw=0.0f;
	x=0.0f;
	y=0.0f;
	z=0.0f;
}


void HeadData::SetMode(HeadMode value)
{
	mode = value;	
}

HeadData::HeadMode HeadData::GetMode()
{
	return mode;
}

float	HeadData::GetPitch() {return pitch;}
void	HeadData::SetPitch(float value) {pitch = value;}
float	HeadData::GetRoll() {return roll;}
void	HeadData::SetRoll(float value) {roll = value;}
float	HeadData::GetYaw() {return yaw;}
void	HeadData::SetYaw(float value) {yaw = value;}

float	HeadData::GetX() {return x;}
void	HeadData::SetX(float value) {x = value;}
float	HeadData::GetY() {return y;}
void	HeadData::SetY(float value) {y = value;}
float	HeadData::GetZ() {return z;}
void	HeadData::SetZ(float value) {z = value;}


