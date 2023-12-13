/*
    An example to rotate a model using a sequence of quaternion - Euler -quaternion conversions.
    The Euler-to-conversion code is based on the equations on www.euclideanspace.com. JG
*/


#ifndef ANGLE_H
#define ANGLE_H

void getEulerFromQuat(osg::Quat q, double& heading, double& attitude, double& bank)
{
    double limit = 0.499999;

    double sqx = q.x()*q.x(); double sqy = q.y()*q.y();
    double sqz = q.z()*q.z();

    double t = q.x()*q.y() + q.z()*q.w();

    if (t>limit) // gimbal lock ?
    {
        heading = 2 * atan2(q.x(),q.w());
        attitude = osg::PI_2;
        bank = 0;
    }
    else if (t<-limit)
    {
        heading = -2 * atan2(q.x(),q.w());
        attitude = - osg::PI_2;
        bank = 0;
    }
    else
    {
        heading = atan2(2*q.y()*q.w()-2*q.x()*q.z() , 1 - 2*sqy - 2*sqz);
        attitude = asin(2*t);
        bank = atan2(2*q.x()*q.w()-2*q.y()*q.z() , 1 - 2*sqx - 2*sqz);
    }
}



void getQuatFromEuler(double heading, double attitude, double bank, osg::Quat& q)
{
    double c1 = cos(heading/2); double s1 = sin(heading/2); double c2 = cos(attitude/2); double s2 = sin(attitude/2); double c3 = cos(bank/2);
    double s3 = sin(bank/2);
    double c1c2 = c1*c2; double s1s2 = s1*s2;

    double w =c1c2*c3 - s1s2*s3;
    double x =c1c2*s3 + s1s2*c3;
    double y =s1*c2*c3 + c1*s2*s3;
    double z =c1*s2*c3 - s1*c2*s3;
    q[0] = x; q[1] = y;
    q[2] = z; q[3] = w;
}

//This does some minor variation of the result to conform to how I work with
/*
osg::Vec3d QuatToEulerAngles (const osg::Quat &q1)
{
        double heading,attitude,bank;
        getEulerFromQuat(q1,heading,attitude,bank);
        //attitude hack...
        //Not sure why attitude gives a smaller range so I'm scaling it here: :(
        attitude*=2;

        //Not sure why these are shuffled around
        return osg::Vec3d(bank+M_PI,attitude+M_PI,heading+M_PI);
}
*/


#endif // ANGLE_H
