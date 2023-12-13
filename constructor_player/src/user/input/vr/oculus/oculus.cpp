/***************************************************************************
oculus.cpp  -  стандартный интерфейс для описания шлема VR OCULUS
-------------------
begin                : 6 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

#include "./oculus.h"

#ifdef WIN32

float OculusClass::Yaw =0.0f;
float OculusClass::Pitch =0.0f;
float OculusClass::Roll =0.0f;

//конструктор
OculusClass::OculusClass()
{
	std::cout << "\tOculus constructor ... " <<std::endl;

	
	if (OVR::System::IsInitialized()==false)
	{
	// Init Oculus HMD
	OVR::System::Init(OVR::Log::ConfigureDefaultLog(OVR::LogMask_All));
	m_deviceManager = *OVR::DeviceManager::Create();
	m_hmdDevice = *m_deviceManager->EnumerateDevices<OVR::HMDDevice>().CreateDevice();
	}

	if (m_hmdDevice)
	{
		m_hmdInfo = new OVR::HMDInfo;

		if (!m_hmdDevice->GetDeviceInfo(m_hmdInfo)) 
		{
			std::cout << "OculusClass::OculusClass()  Error: Unable to get device info" << std::endl;
		} 
		else 
		{
			m_sensor = *m_hmdDevice->GetSensor();

			if (m_sensor) 
			{
				m_sensorFusion = new OVR::SensorFusion;
				m_sensorFusion->AttachToSensor(m_sensor);
				m_sensorFusion->SetPredictionEnabled(true);
				// Get default sensor prediction delta
				m_predictionDelta = m_sensorFusion->GetPredictionDelta();
				//?
				m_sensorFusion->SetPredictionEnabled(true);
				m_predictionDelta = 0.03f;

			}
			std::cout << "OculusClass::OculusClass() OK" << std::endl;;
		}
	} 
	else 
	{
		std::cout << "OculusClass::OculusClass() Warning: Unable to find HMD Device." << std::endl;
	}
}



OculusClass::~OculusClass()
{
	if (m_sensorFusion) 
	{
		// Detach sensor
		m_sensorFusion->AttachToSensor(NULL);
		delete m_sensorFusion;
		m_sensorFusion = NULL;
	}

	delete m_hmdInfo;
	m_hmdInfo = NULL;
	m_sensor.Clear();
	m_hmdDevice.Clear();
	m_deviceManager.Clear();

	// Do a nice shutdown of the Oculus HMD
	if (OVR::System::IsInitialized()) 
	{
		OVR::System::Destroy();
	}
}



//возврат параметров головы
HeadData OculusClass::GetHeadData()
{
	HeadData data;
	data.SetMode (HeadData::ORIENTATION);
	data.SetPitch (-Pitch*100.0f);
	data.SetYaw (-Yaw * 60.0f);
	data.SetRoll(Roll*46.0f);
	return data;
}




void OculusClass::update()
{
	// Create identity quaternion
	osg::Quat osgQuat(0.0f, 0.0f, 0.0f, 1.0f);

	if (m_sensorFusion && m_sensorFusion->IsAttachedToSensor()) {
		OVR::Quatf quat;

		if (m_sensorFusion->IsPredictionEnabled()) {
			quat = m_sensorFusion->GetPredictedOrientation(m_predictionDelta);
		} else {
			quat = m_sensorFusion->GetOrientation();
		}

		osgQuat.set(quat.x, quat.y, quat.z, -quat.w);

		std::cout << "YES  DA DA" << osgQuat.x() << " " << osgQuat.y() << " " << osgQuat.z() << " " << osgQuat.w() << std::endl;
	}

	

	//return osgQuat;
}




#endif