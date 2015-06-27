//------------------------------------------------------------------------
//
//  Author: Anthony Serra
//
//
//------------------------------------------------------------------------
#pragma once


#define PI			3.14159265358979323846
#include <d3dx9.h>
class Quaternion  
{
	public:
		
		Quaternion();
		~Quaternion();
		
    void CreateMatrix(LPD3DXMATRIX pMatrix);
		void CreateFromAxisAngle(const float &in_x,
								 const float &in_y,
								 const float &in_z,
								 const float &in_degrees);

		Quaternion operator *(const Quaternion &q);


	private:
		
		float x,
			  y,
			  z,
			  w;
};
