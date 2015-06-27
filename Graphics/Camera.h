//------------------------------------------------------------------------
//
//  Author: Anthony Serra
//
//
//------------------------------------------------------------------------
#ifndef CCAMERA_H
#define CCAMERA_H
#pragma message("Knows about Camera.h")
#include<math.h>
#include<d3dx9math.h>
//#include"Vector.h"



class Camera
{
   public:
     Camera();

      
      void SetPosLerp(const D3DXVECTOR3& pos1, const D3DXVECTOR3& pos2, float tval); //t between 0 and 1
      
      void Update(float timeElapsed = .033f);
      const D3DXMATRIX *GetViewMatrix();
      D3DXVECTOR3 GetLookAtVector( const D3DXMATRIX& view );
      D3DXMATRIX GetOrientationMatrix();
			D3DXMATRIX GetDirectionMatrix();
      void RotateX(float angle);
      void RotateY(float angle);
      void RotateZ(float angle);
      void MoveForward(float length);
      void MoveRight(float length);
      void MoveUp(float length);
      void RotateVertically(float angle); //dependent upon look at vector
      void RotateArbitrary(const D3DXVECTOR3& axis, float angle);

			void SetTarget(const D3DXVECTOR3 *t) {target = t;}
			void SetNoTarget() {target = NULL;}
			const D3DXVECTOR3 *GetTarget() {return target;}

      void SetPos(D3DXVECTOR3 _pos);
      void SetOrient(D3DXQUATERNION _orient);

      D3DXVECTOR3 GetPos() {return _ActualPos;}
      D3DXQUATERNION GetOrient() {return _ActualOrient;}

      void SetCameraLag(float c) {if(c>=0) CameraLag = c; else CameraLag = 0;}
      float GetCameraLag() {return CameraLag;}

private:
	void CreateViewMatrix();
	D3DXMATRIX viewMatrix;

      D3DXQUATERNION orient;
      D3DXQUATERNION oldorient;
      D3DXQUATERNION _ActualOrient;

      D3DXVECTOR3 pos;
      D3DXVECTOR3 oldpos;
      D3DXVECTOR3 _ActualPos;

			const D3DXVECTOR3 *target;

      float t;
      float CameraLag;
};

#endif