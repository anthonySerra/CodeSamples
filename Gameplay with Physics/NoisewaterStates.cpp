#pragma once

#include "NoisewaterStates.h"
#include "Noisewater.h"
#include "../GameManager.h"
#include "../Input/Input.hpp"
#include "../Sound/Sound.h"
#include "../MessageTypes.h"


#define XZ_PROJECT D3DXMATRIX(1,0,0,0,\
	0,0,0,0,\
	0,0,1,0,\
	0,0,0,1)

//------------------------------------------------------------------------
//
//  Name:   NoisewaterStates.cpp
//
//  Desc:   A class implementing the states for the Noisewater.
//
//  Author: Anthony Serra
//
//------------------------------------------------------------------------
namespace noisewater
{

	Global* Global::Instance()
	{
		static Global instance;
		return &instance;
	}

	void Global::Enter(Noisewater* pN)
	{
	}

	void Global::Execute(Noisewater* pN)
	{

		//CAMERA
		// JDR make sure to use Graphics component positions
		gmMgr->graphics_->camera.destEyeTarget = pN->GetGC()->GetPosition();
		gmMgr->graphics_->camera.destEyePos = pN->GetGC()->GetPosition() - D3DXVECTOR3(400,-300,1100);


		// JDR Testing the SmartTrack2D function, allows the camera to track multiple targets 
		std::vector<D3DXVECTOR3> trackingVector;
		trackingVector.push_back(D3DXVECTOR3(0,0,0));
		trackingVector.push_back(pN->GetGC()->GetPosition());

		gmMgr->graphics_->fpCam.UpdateTarget(gmMgr->input_->MouseX(), gmMgr->input_->MouseY());






		//INPUT

		if( gmMgr->input_->IsTriggered(RIGHT_BUTTON, pN->ControllerNum) )
		{
			pN->ParryTimer = pN->ParryCutoffTime;
			pN->ParryDir = 1;
		}
		if( gmMgr->input_->IsTriggered(LEFT_BUTTON, pN->ControllerNum) )
		{
			pN->ParryTimer = pN->ParryCutoffTime;
			pN->ParryDir = -1;
		}
		if( pN->ParryTimer > 0 )
		{
			pN->ParryTimer -= Clock->Get_dt();
			pN->Parrying = true;
		}
		else
			pN->Parrying = false;
		


		if( gmMgr->input_->GetXboxController(1)->LeftJoystick().x > .5f )
			gmMgr->input_->GetXboxController(1)->Vibrate(0,1);

		else if( gmMgr->input_->GetXboxController(1)->LeftJoystick().x < -.5f )
			gmMgr->input_->GetXboxController(1)->Vibrate(1,0);

		else
			gmMgr->input_->GetXboxController(1)->Vibrate();


		//End Input

	}

	void Global::Exit(Noisewater* pN)
	{

	}

	bool Global::OnMessage(Noisewater* pN, const Telegram& msg)
	{

		switch( msg.Msg )
		{
		/*case Msg_Collision:
			{
				CollisionMsgStruct * cms = static_cast<CollisionMsgStruct*>(msg.ExtraInfo);
				D3DXVECTOR2 norm(cms->CollNormal);

				float dot = D3DXVec2Dot(D3DXVec2Normalize(&norm,&norm), &D3DXVECTOR2(0,-1));
				if( dot > .5f && cms->CollType == BLOCK && pN->GetFSM()->CurrentState() != Damaged::Instance())
				{
					pN->OnGround = true;
					pN->GetFSM()->ChangeState(Ground::Instance());

				}
				else if( cms->CollType == ENEMY_HIT )
				{
					if( pN->Parrying == false )
						pN->GetFSM()->ChangeState(Damaged::Instance());

				}

				pN->GetPC()->setOrientation(norm.x, -norm.y, pN->GetPC()->UsedBodyIDs[0]); 
				delete msg.ExtraInfo;
				return true;
			}*/
		case Msg_PlayerFaint:
			pN->Faint();
			return true;
		}
		

		//IF THERE IS NO CODE THAT IS MESSAGE DEPENDENT, JUST RETURN FALSE
		return false; //send message to global message handler
	}
#pragma region INIT
	Init* Init::Instance()
	{
		static Init instance;
		return &instance;
	}

	void Init::Enter(Noisewater* pN)
	{



	}

	void Init::Execute(Noisewater* pN)
	{ 
		if( pN->JumpSound == NULL )
		{
			pN->JumpSound = gmMgr->sound_->CreateSoundComp("First Jump");
			pN->addComponent(pN->JumpSound);
		}
		if( pN->ParrySound == NULL )
		{
			pN->ParrySound = gmMgr->sound_->CreateSoundComp("Parry");
			pN->addComponent(pN->ParrySound);
		}
		if( pN->JumpGrunt == NULL )
		{
			pN->JumpGrunt = gmMgr->sound_->CreateSoundComp("JumpGrunt");
			pN->addComponent(pN->JumpGrunt);
		}
		if( pN->LandGrunt == NULL )
		{
			pN->LandGrunt = gmMgr->sound_->CreateSoundComp("LandGrunt");
			pN->addComponent(pN->LandGrunt);
		}
		if( pN->ParryGrunt == NULL )
		{
			pN->ParryGrunt = gmMgr->sound_->CreateSoundComp("ParryGrunt");
			pN->addComponent(pN->ParryGrunt);
		}
		if( pN->DeathYell == NULL )
		{
			pN->DeathYell = gmMgr->sound_->CreateSoundComp("DeathYell");
			pN->addComponent(pN->DeathYell);
		}
		if( pN->DamageGrunt[0] == NULL )
		{
			pN->DamageGrunt[0] = gmMgr->sound_->CreateSoundComp("HurtGrunt1");
			pN->addComponent(pN->DamageGrunt[0]);
		}
		if( pN->DamageGrunt[1] == NULL )
		{
			pN->DamageGrunt[1] = gmMgr->sound_->CreateSoundComp("HurtGrunt2");
			pN->addComponent(pN->DamageGrunt[1]);
		}
		pN->GetFSM()->ChangeState(Air::Instance());
	}

	void Init::Exit(Noisewater* pN)
	{

	}

	bool Init::OnMessage(Noisewater* pN, const Telegram& msg)
	{
		
		return false; //send message to global message handler
	}
#pragma endregion

#pragma region READYPARRY
	ReadyParry* ReadyParry::Instance()
	{
		static ReadyParry instance;
		return &instance;
	}
	
	void ReadyParry::Enter(Noisewater* pN)
	{
		pN->GetAC()->ChangeAnimation("IDLE");
		pN->ParryTimer = 0;
		pN->Parrying = true;
		 

	}
	
	void ReadyParry::Execute(Noisewater* pN)
	{ 
		pN->ParryTimer += Clock->Get_dt();
		if( pN->ParryTimer < pN->ParryCutoffTime * .3f )
		{
			pN->Parrying = true;
			pN->ParryFlash = !pN->ParryFlash;

			pN->GetAC()->ChangeColorsManually(pN->ParryFlash);
			pN->GetAC()->ChangeModelColor(D3DXCOLOR(1,1,0,1));

		}
		else
			pN->Parrying = false;

		if( pN->ParryTimer > pN->ParryCutoffTime )
		{
			pN->ParryFlash = false;

			pN->GetAC()->ChangeColorsManually(pN->ParryFlash);
			pN->GetFSM()->ChangeState(Air::Instance());
		}
	}

	void ReadyParry::Exit(Noisewater* pN)
	{
		pN->ParryFlash = false;
		pN->GetAC()->ChangeColorsManually(pN->ParryFlash);
		pN->Parrying = false;
	}

	bool ReadyParry::OnMessage(Noisewater* pN, const Telegram& msg)
	{
		switch( msg.Msg )
		{
		case Msg_Collision:
			{
				CollisionMsgStruct * cms = static_cast<CollisionMsgStruct*>(msg.ExtraInfo);
				//D3DXVECTOR2 norm(cms->CollNormal);

				//float dot = D3DXVec2Dot(D3DXVec2Normalize(&norm,&norm), &D3DXVECTOR2(0,-1));
				 
				 if( cms->CollType == ENEMY_HIT )
				{
					if( pN->Parrying )
						pN->GetFSM()->ChangeState(Parry::Instance());
					else
						pN->GetFSM()->ChangeState(Damaged::Instance());

				}
				 Dispatch->DispatchMsg(0,pN->ID(), msg.Sender, Msg_EnemyParried, NULL);
			  
 
				delete msg.ExtraInfo;
				//msg.ExtraInfo = NULL;
				return true;
			}
	 
		}
		return false; //send message to global message handler
	}
#pragma endregion
#pragma region PARRY
	Parry* Parry::Instance()
	{
		static Parry instance;
		return &instance;
	}

	void Parry::Enter(Noisewater* pN)
	{
		pN->GetAC()->AnimSpeedMultiplier = 1;
		pN->GetAC()->ChangeAnimation("PARRY");
		pN->ParryTimer = .0001;
		 
		pN->ParrySound->Play(false);
		pN->PlayVoice(pN->ParryGrunt);
	}

	void Parry::Execute(Noisewater* pN)
	{ 
		
		pN->GetPC()->setVelocityForAllBodies(0,0);
		pN->GetAC()->SetInterpTimeManually(0);
		if( pN->GetAC()->EndOfAnimation() )
			pN->GetFSM()->ChangeState(Ground::Instance());
		
		/*if( gmMgr->input_->IsPressed(TRANSFORM,pN->ControllerNum) )
		{
			pN->GetFSM()->ChangeState(ReadyParry::Instance());
		}*/
	}

	void Parry::Exit(Noisewater* pN)
	{
		 /*pN->Parrying = false;*/
	}

	bool Parry::OnMessage(Noisewater* pN, const Telegram& msg)
	{
		switch( msg.Msg )
		{
		case Msg_Collision:
			{
				CollisionMsgStruct * cms = static_cast<CollisionMsgStruct*>(msg.ExtraInfo);
				D3DXVECTOR2 norm(cms->CollNormal);

				//float dot = D3DXVec2Dot(D3DXVec2Normalize(&norm,&norm), &D3DXVECTOR2(0,-1));

				if( cms->CollType == ENEMY_HIT && pN->Parrying == false)
				{
					
					pN->GetFSM()->ChangeState(Damaged::Instance());

				}
 


				delete msg.ExtraInfo;
				//msg.ExtraInfo = NULL;
				return true;
			}

		}
		return false; //send message to global message handler
	}
#pragma endregion

#pragma region DAMAGED
	Damaged* Damaged::Instance()
	{
		static Damaged instance;
		return &instance;
	}

	void Damaged::Enter(Noisewater* pN)
	{
		
		float vx,vy;
		pN->GetPC()->getVelocity(vx,vy,pN->GetPC()->UsedBodyIDs[0]);
		if( abs(vx) > pN->MaxSpeed * 4)
			pN->GetPC()->setVelocityForAllBodiesXONLY(vx / fabs(vx) * pN->MaxSpeed * 1.5f);
		//Upon Enemy Collision Blur Screen
		//HACK - REMOVE ASAP
		pN->GetAC()->AnimSpeedMultiplier = 1;
		pN->GetAC()->ChangeAnimation("DAMAGED");
		pN->PlayVoice(pN->DamageGrunt[rand()%2]);
	}

	void Damaged::Execute(Noisewater* pN)
	{ 
		if( pN->GetAC()->EndOfAnimation() )
			pN->GetFSM()->ChangeState(Ground::Instance());
	}

	void Damaged::Exit(Noisewater* pN)
	{

	}

	bool Damaged::OnMessage(Noisewater* pN, const Telegram& msg)
	{
		return false; //send message to global message handler
	}
#pragma endregion

	Air* Air::Instance()
	{
		static Air instance;
		return &instance;
	}

	void Air::Enter(Noisewater* pN)
	{
	}

	void Air::Execute(Noisewater* pN)
	{  
		D3DXVECTOR2 temp;
		pN->GetPC()->getVelocity(temp.x, temp.y, pN->bodyID);

		//Rising or Falling?
		if(temp.y >= 0)
			pN->GetAC()->ChangeAnimation("RISING");
		else
			pN->GetAC()->ChangeAnimation("FALLING");


		if( gmMgr->input_->IsPressed(RIGHT_BUTTON, pN->ControllerNum) )
		{
			pN->GetAC()->Flipped(false);


			pN->GetPC()->applyForce(pN->Acceleration,0);


			float vx=0,vy=0;
			pN->GetPC()->getVelocity(vx,vy,pN->GetPC()->UsedBodyIDs[0]);
			if( vx > pN->MaxSpeed )
				pN->GetPC()->setVelocityForAllBodiesXONLY(pN->MaxSpeed);
			pN->GetAC()->AnimSpeedMultiplier = fabs(vx) / 150.0f;
		}

		else if( gmMgr->input_->IsPressed(LEFT_BUTTON, pN->ControllerNum) )
		{


			pN->GetAC()->Flipped(true);

			pN->GetPC()->applyForce(-pN->Acceleration,0);


			//gmMgr->graphics_->camera.StrafeRight(-1.f);
			float vx=0,vy=0;
			pN->GetPC()->getVelocity(vx,vy,pN->GetPC()->UsedBodyIDs[0]);
			if( vx < -pN->MaxSpeed )
				pN->GetPC()->setVelocityForAllBodiesXONLY(-pN->MaxSpeed);
			pN->GetAC()->AnimSpeedMultiplier = fabs(vx) / 150.0f;
		}
		else if( gmMgr->input_->IsPressed(DOWN_BUTTON, pN->ControllerNum))
			pN->GetPC()->applyForce(0,-4000);

	}

	void Air::Exit(Noisewater* pN)
	{
	}

	bool Air::OnMessage(Noisewater* pN, const Telegram& msg)
	{
		switch( msg.Msg )
		{
		case Msg_Collision:
			{
				CollisionMsgStruct * cms = static_cast<CollisionMsgStruct*>(msg.ExtraInfo);
				D3DXVECTOR2 norm(cms->CollNormal);

				float dot = D3DXVec2Dot(D3DXVec2Normalize(&norm,&norm), &D3DXVECTOR2(0,-1));
				if( dot > .5f && cms->CollType == BLOCK && pN->GetFSM()->CurrentState() != Damaged::Instance())
				{
					pN->OnGround = true;
					pN->PlayVoice(pN->LandGrunt);
					pN->GetFSM()->ChangeState(Ground::Instance());

				}
				else if( cms->CollType == ENEMY_HIT )
				{
					if( pN->Parrying == true && pN->ParryDir * norm.x > 0)
					{
						Dispatch->DispatchMsg(0,pN->ID(), msg.Sender, Msg_EnemyParried, NULL);
						pN->GetFSM()->ChangeState(Parry::Instance());
					}
					else
						pN->GetFSM()->ChangeState(Damaged::Instance());
					
				}

				pN->GetPC()->setOrientation(norm.x, -norm.y, pN->GetPC()->UsedBodyIDs[0]); 
				delete msg.ExtraInfo;
//				msg.ExtraInfo = NULL;
				return true;
			}
		case Msg_PlayerFaint:
			pN->Faint();
			return true;
		}
		return false; //send message to global message handler
	}







	Ground* Ground::Instance()
	{
		static Ground instance;
		return &instance;
	}

	void Ground::Enter(Noisewater* pN)
	{
	}

	void Ground::Execute(Noisewater* pN)
	{  
		if( gmMgr->input_->IsTriggered(UP_BUTTON, pN->ControllerNum) )
		{
			//pN->GetPC()->setVelocityForAllBodiesYONLY(1200);
			//pN->GetPC()->ApplyImpulse(0,pN->JumpPower);
			pN->GetPC()->setVelocityForAllBodiesYONLY(pN->JumpPower);
			pN->GroundCheck = pN->OnGround = false;
			++pN->JumpsTaken;
			pN->JumpSound->Play(false);
			pN->PlayVoice(pN->JumpGrunt);
		}

		if( pN->OnGround == false )
		{
			pN->GetPC()->setAngle(0,pN->GetPC()->UsedBodyIDs[0]);
			pN->GetFSM()->ChangeState(Air::Instance());
			return;
		}

		if( gmMgr->input_->IsPressed(RIGHT_BUTTON, pN->ControllerNum) )
		{
			pN->GetAC()->Flipped(false);
			pN->GetAC()->ChangeAnimation("RUNNING");

			pN->GetPC()->applyForce(pN->Acceleration,0);


			float vx=0,vy=0;
			pN->GetPC()->getVelocity(vx,vy,pN->GetPC()->UsedBodyIDs[0]);
			if( vx > pN->MaxSpeed )
				pN->GetPC()->setVelocityForAllBodiesXONLY(pN->MaxSpeed);
			pN->GetAC()->AnimSpeedMultiplier = fabs(vx) / 150.0f;
		}

		else if( gmMgr->input_->IsPressed(LEFT_BUTTON, pN->ControllerNum) )
		{

			pN->GetAC()->ChangeAnimation("RUNNING");
			pN->GetAC()->Flipped(true);

			pN->GetPC()->applyForce(-pN->Acceleration,0);


			//gmMgr->graphics_->camera.StrafeRight(-1.f);
			float vx=0,vy=0;
			pN->GetPC()->getVelocity(vx,vy,pN->GetPC()->UsedBodyIDs[0]);
			if( vx < -pN->MaxSpeed )
				pN->GetPC()->setVelocityForAllBodiesXONLY(-pN->MaxSpeed);
			pN->GetAC()->AnimSpeedMultiplier = fabs(vx) / 150.0f;
		}
		else if( gmMgr->input_->IsPressed(TRANSFORM,pN->ControllerNum) )
		{
			//pN->GetFSM()->ChangeState(ReadyParry::Instance());
		}
		else
			pN->GetAC()->ChangeAnimation("IDLE");

	}

	void Ground::Exit(Noisewater* pN)
	{

	}

	bool Ground::OnMessage(Noisewater* pN, const Telegram& msg)
	{
		switch( msg.Msg )
		{
		case Msg_Collision:
			{
				CollisionMsgStruct * cms = static_cast<CollisionMsgStruct*>(msg.ExtraInfo);
				D3DXVECTOR2 norm(cms->CollNormal);

				float dot = D3DXVec2Dot(D3DXVec2Normalize(&norm,&norm), &D3DXVECTOR2(0,-1));
				if( dot > .5f && cms->CollType == BLOCK && pN->GetFSM()->CurrentState() != Damaged::Instance())
				{
					pN->OnGround = true;
					pN->GetFSM()->ChangeState(Ground::Instance());

				}
				else if( cms->CollType == ENEMY_HIT )
				{
					if( pN->Parrying == true && pN->ParryDir * norm.x > 0)
					{
						Dispatch->DispatchMsg(0,pN->ID(), msg.Sender, Msg_EnemyParried, NULL);
						pN->GetFSM()->ChangeState(Parry::Instance());
						
					}
					else
					{
						pN->GetPC()->ApplyImpulse(pN->DamageKnockback * norm.x,pN->DamageKnockback * norm.y);
						pN->GetFSM()->ChangeState(Damaged::Instance());
					}
				}

				pN->GetPC()->setOrientation(norm.x, -norm.y, pN->GetPC()->UsedBodyIDs[0]); 
				delete msg.ExtraInfo;
//				msg.ExtraInfo = NULL;
				return true;
			}
		case Msg_PlayerFaint:
			pN->Faint();
			return true;
		}
		return false; //send message to global message handler
	}


	Faint* Faint::Instance()
	{
		static Faint instance;
		return &instance;
	}

	void Faint::Enter(Noisewater* pN)
	{
	}

	void Faint::Execute(Noisewater* pN)
	{  
		//Fading or effect goes here

		//When finished, turn off the current AND global states for the player
		pN->GetFSM()->SetCurrentState(0);
		pN->GetFSM()->SetGlobalState(0);		
	}

	void Faint::Exit(Noisewater* pN)
	{
	}

	bool Faint::OnMessage(Noisewater* pN, const Telegram& msg)
	{
		return false; //send message to global message handler
	}

}
