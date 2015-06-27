//------------------------------------------------------------------------
//
//  Author: Anthony Serra
//
//
//------------------------------------------------------------------------
#include"Camera.h"


void Camera::SetPosLerp(const D3DXVECTOR3& pos1, const D3DXVECTOR3& pos2, float tval)
{
  D3DXVec3Lerp(&_ActualPos, &pos1, &pos2, tval);

    
}

Camera::Camera()
{
   _ActualPos = oldpos = pos = D3DXVECTOR3(0,0,0);
   D3DXQuaternionIdentity(&orient);
   D3DXQuaternionIdentity(&oldorient); 
   D3DXQuaternionIdentity(&_ActualOrient); 
   t = 0;
   CameraLag = 1.2f;
	 target = NULL;

}
void Camera::SetPos(D3DXVECTOR3 _pos)
{
  if( _pos == pos )
    return;
  t = 0;
  
  oldpos = _ActualPos;
  pos = _pos;  
}

void Camera::MoveForward(float length)
{
  SetPos(pos + length * GetLookAtVector(*GetViewMatrix()));
}

void Camera::MoveRight(float length)
{
  D3DXVECTOR3 sideways;
  D3DXVec3Cross(&sideways, &GetLookAtVector(*GetViewMatrix()), &D3DXVECTOR3(0,1,0));
  D3DXVec3Normalize(&sideways, &sideways);
  SetPos(pos + length * sideways);
}

void Camera::MoveUp(float length)
{
  SetPos(pos + length * D3DXVECTOR3(0,1,0));
}

void Camera::SetOrient(D3DXQUATERNION _orient)
{
  if( D3DXQuaternionLengthSq(&(_orient - orient)) <= .000001f || target != NULL )
    return;
  t = 0;
  D3DXQuaternionNormalize(&_orient, &_orient);
  D3DXQuaternionNormalize(&_ActualOrient, &_ActualOrient);
  oldorient = _ActualOrient;
  orient = _orient;
  
}

void Camera::Update(float timeElapsed)
{
  if( t >= 1 )
	{
		CreateViewMatrix();
    return;
	}
  if( CameraLag == 0 )
  {
    t = 1;
  }
  else
    t += timeElapsed/CameraLag;

  if( _ActualPos != pos )
  {
		const float PI = 3.1415f;
    SetPosLerp(oldpos, pos, (sin((t*PI)-PI/2)+1)/2.0f);
  }

  if( D3DXQuaternionLengthSq(&(_ActualOrient - orient)) > .000001f )
  {
    D3DXQuaternionSlerp(&_ActualOrient, &oldorient, &orient, t);
  }
	CreateViewMatrix();
}
void Camera::CreateViewMatrix()
{
	if( target == NULL )
	{
		D3DXMATRIX rot, position;
		D3DXMatrixTranslation(  &position ,
			-_ActualPos.x ,
			-_ActualPos.y ,
			-_ActualPos.z );


		// Now calculate rotation, by taking the conjucate of the
		// quaternion
		D3DXMatrixRotationQuaternion( &rot,
			&D3DXQUATERNION( -_ActualOrient.x ,
			-_ActualOrient.y ,
			-_ActualOrient.z ,
			_ActualOrient.w ));
		D3DXMatrixMultiply(&viewMatrix , &position , &rot );
	}
	else
	{
		D3DXMatrixLookAtLH(&viewMatrix, &_ActualPos, target, &D3DXVECTOR3(0,1,0));
	}
}
const D3DXMATRIX *Camera::GetViewMatrix()
{
	return &viewMatrix;
}

D3DXVECTOR3 Camera::GetLookAtVector( const D3DXMATRIX& view )
{

  D3DXVECTOR3 vLook;

  vLook.x= view._13;
  vLook.y= view._23;
  vLook.z= view._33;

  return vLook;

}

void Camera::RotateX(float angle)
{
  D3DXQUATERNION rot;
  D3DXQuaternionRotationAxis(&rot, &D3DXVECTOR3(1,0,0), angle);
  orient *= rot;
  t = 0;
}
void Camera::RotateY(float angle)
{
  D3DXQUATERNION rot;
  D3DXQuaternionRotationAxis(&rot, &D3DXVECTOR3(0,1,0), angle);
  orient *= rot;
  t = 0;
}
void Camera::RotateZ(float angle)
{
  D3DXQUATERNION rot;
  D3DXQuaternionRotationAxis(&rot, &D3DXVECTOR3(0,0,1), angle);
  orient *= rot;
  t = 0;
}
void Camera::RotateVertically(float angle)
{
  D3DXVECTOR3 lookat;
  D3DXVec3TransformCoord(&lookat, &D3DXVECTOR3(1,0,0), &GetOrientationMatrix());
  D3DXQUATERNION rot;
  D3DXQuaternionIdentity(&rot);
  D3DXQuaternionRotationAxis(&rot, &lookat, angle);
  orient *= rot;
  t = 0;
}
D3DXMATRIX Camera::GetOrientationMatrix()
{
	D3DXQUATERNION rot;
	D3DXVECTOR3 throwaway1,ta2;
	D3DXMATRIX result;
	D3DXMatrixDecompose(&throwaway1,&rot,&ta2,&viewMatrix);
	rot = -rot;
	rot.w = -rot.w;
	D3DXMatrixRotationQuaternion(&result,&rot);
	return result;
}
D3DXMATRIX Camera::GetDirectionMatrix() //Flattened to the XZ plane
{
  D3DXMATRIX OrientationMatrix = GetOrientationMatrix();

	D3DXVECTOR3 forward, right, up;
	D3DXVec3TransformCoord(&forward, &D3DXVECTOR3(0,0,1), &OrientationMatrix);
	D3DXVec3TransformCoord(&right, &D3DXVECTOR3(1,0,0), &OrientationMatrix);
	D3DXVec3TransformCoord(&up, &D3DXVECTOR3(0,1,0), &OrientationMatrix);

	forward.y = 0;
	D3DXVec3Normalize(&forward, &forward);
	D3DXVec3Normalize(&right, &right);
	D3DXVec3Normalize(&up, &up);
	
	return D3DXMATRIX(right.x, right.y, right.z, 0,
										up.x, up.y, up.z, 0,
										forward.x, forward.y, forward.z, 0,
										0,0,0,1);
}
void Camera::RotateArbitrary(const D3DXVECTOR3& axis, float angle)
{
  D3DXQUATERNION rot;
  D3DXQuaternionRotationAxis(&rot, &axis, angle);
  orient *= rot;
  t = 0;
}
