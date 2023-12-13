/*
 * openvrupdateslavecallback.cpp
 *
 *  Created on: Dec 18, 2015
 *      Author: Chris Denham
 */

#include "openvrupdateslavecallback.h"

void OpenVRUpdateSlaveCallback::updateSlave(osg::View& view, osg::View::Slave& slave)
{
	if (m_cameraType == LEFT_CAMERA)
	{
		m_device->updatePose();
	}

	osg::Vec3 position = osg::Vec3(0.0f, 0.0f, 0.0f);
	osg::Quat orientation =osg::Quat(0.0f, 0.0f, 0.0f, 1.0f);
	
	if (m_device->position().size()>0) position = m_device->position().front();
	if (m_device->orientation().size()>0) orientation = m_device->orientation().front();

	

	osg::Matrix viewOffset = (m_cameraType == LEFT_CAMERA) ? m_device->viewMatrixLeft() : m_device->viewMatrixRight();

	//viewOffset.preMultRotate(orientation);
	//viewOffset.setTrans(position);

	slave._viewOffset = viewOffset;

	slave.updateSlaveImplementation(view);
}
