#ifndef OCULUSCLASS
#define OCULUSCLASS

#ifdef WIN32
	#include <iostream>

	#include <OVR.h>
	#include <osg/Matrix>


	#include "../../../HeadDataClass.h"

	//класс обработчиков устройства
	class OculusClass
	{
	public:
		OculusClass();
		~OculusClass();
		
		//возврат параметров головы
		HeadData GetHeadData();

		void update();

	private:
		static float Yaw;
		static float Pitch;
		static float Roll;	

		//
		OVR::Ptr<OVR::DeviceManager> m_deviceManager;
		OVR::Ptr<OVR::HMDDevice> m_hmdDevice;
		OVR::Ptr<OVR::SensorDevice> m_sensor;
		OVR::HMDInfo* m_hmdInfo;
		OVR::SensorFusion* m_sensorFusion;
		float m_predictionDelta;
	};
	
#endif


#endif

