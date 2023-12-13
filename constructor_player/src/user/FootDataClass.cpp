/***************************************************************************
FootDataClass.cpp  -  стандартный интерфейс для описания состояния "ног"
-------------------
begin                : 29 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

#include "FootDataClass.h"


FootData::FootData()
{
	mode = NONE;
	vectorX = 0.0f;
	vectorY = 0.0f;
	vectorZ = 0.0f;
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	zoom=false;
}


void FootData::SetMode(FootMode value)
{
	mode=value;
}

FootData::FootMode FootData::GetMode()
{
	return mode;
}

float	FootData::getVectorX() {return vectorX;}
void	FootData::SetVectorX(float value) {vectorX = value;}
float	FootData::getVectorY() {return vectorY;}
void	FootData::SetVectorY(float value) {vectorY = value;}
float	FootData::getVectorZ() {return vectorZ;}
void	FootData::SetVectorZ(float value) {vectorZ = value;}
//
float	FootData::GetX() {return x;}
void	FootData::SetX(float value) {x = value;}
float	FootData::GetY() {return y;}
void	FootData::SetY(float value) {y = value;}
float	FootData::GetZ() {return z;}
void	FootData::SetZ(float value) {z = value;}


//
//zoom
bool FootData::GetZoom() 
{
	return zoom; 
}
void FootData::SetZoom(bool value) 
{
	zoom = value; 
}
