//------------------------------------------------------------------------
//
//  Author: Anthony Serra
//
//
//------------------------------------------------------------------------
#include "Quaternion.h"



Quaternion::Quaternion()
	: x(0.0f), y(0.0f), z(0.0f), w(1.0f)
{

}

Quaternion::~Quaternion()
{

}

void Quaternion::CreateFromAxisAngle(const float &in_x, const float &in_y, const float &in_z, const float &in_degrees)
{
	
	float angle = float((in_degrees / 180.0f) * PI);
	float result = float(sin(angle/2.0f));
	w = float(cos(angle/2.0f));

	// Calculate the x, y and z of the quaternion
	x = float(in_x * result);
	y = float(in_y * result);
	z = float(in_z * result);
}

void Quaternion::CreateMatrix(LPD3DXMATRIX pMatrix)
{
	if(pMatrix)
	{
		// First row
      pMatrix->_11 = 1.0f - 2.0f * ( y * y + z * z );  
      pMatrix->_12 = 2.0f * ( x * y - w * z );  
	    pMatrix->_13 = 2.0f * ( x * z + w * y );  
      pMatrix->_14 = 0.0f;  
	
	    // Second row
	    pMatrix->_21 = 2.0f * ( x * y + w * z );  
	    pMatrix->_22 = 1.0f - 2.0f * ( x * x + z * z );  
	    pMatrix->_23 = 2.0f * ( y * z - w * x );  
	    pMatrix->_24 = 0.0f;  
	
	    // Third row
	    pMatrix->_31 = 2.0f * ( x * z - w * y );  
	    pMatrix->_32 = 2.0f * ( y * z + w * x );  
	    pMatrix->_33 = 1.0f - 2.0f * ( x * x + y * y );  
	    pMatrix->_34 = 0.0f;  
	
	    // Fourth row
	    pMatrix->_41 = 0;  
	    pMatrix->_42 = 0;  
	    pMatrix->_43 = 0;  
	    pMatrix->_44 = 1.0f;
	}
}

Quaternion Quaternion::operator *(const Quaternion &q)
{
	Quaternion r;

	r.w = w*q.w - x*q.x - y*q.y - z*q.z;
	r.x = w*q.x + x*q.w + y*q.z - z*q.y;
	r.y = w*q.y + y*q.w + z*q.x - x*q.z;
	r.z = w*q.z + z*q.w + x*q.y - y*q.x;
	
	return r;
}
