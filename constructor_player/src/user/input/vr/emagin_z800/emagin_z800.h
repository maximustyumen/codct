#ifndef Z800CLASS
#define Z800CLASS

#ifdef WIN32
	#include <D3DX9math.h>
	#include <EMADevice.h>
	#include <EMATracker.h>
	#include <EMAGestureAnalyzer.h>

	#include <iostream>
	#include "../../../HeadDataClass.h"

	//класс обработчиков устройства
	class EmaginZ800Class
	{
	public:
		EmaginZ800Class();
		
		//функция определяет наличие устройства, если устройство найдено, имеет смысл создавать класс....
		static bool DetectEmaginZ800();
		//флаг предыдущего результата //0-не вроводилость, 1 -не был обнаружен, 2 - был обнаружен
		static int EmaginZ800detected;

		//возврат параметров головы
		HeadData GetHeadData();

	private:
		EMADevice * Z800 ;
		EMADeviceInfo Z800Info ; 
		EMATracker *emaTracker;
		//EMAGestureAnalyzer* emaGestureAnalyzer;
		EMADeviceData dData;
		static HRESULT CALLBACK TrackProc(EMADeviceData* data, void* param);
		//static HRESULT CALLBACK GestureProc (EMAGestureCommand* command, void* param);

		bool useEmagin;
		bool useEmaginTracker;
		//bool useGestureAnalyzer;

		static float Yaw;
		static float Pitch;
		static float Roll;	

		void DeInitializeTracker();
		void DeInitializeHeadset ();

		void InitializeHeadset (); 
		void InitializeTracker();
		//void InitializeGestureAnalyzer();
	};
	
	
	

#endif


#endif

