//------------------------------------------------------------------------
//
//  Author: Anthony Serra and Josh Rutz
//
//
//------------------------------------------------------------------------


#include "GraphicsEngine.h"
#include <time.h>
//#include "Input.h"
#include "SkinnedMesh.h"
#include "../GameManager.h"



GraphicsEngine::GraphicsEngine() : m_pBGTexture(NULL), m_pFont(NULL)
{

}


GraphicsEngine::~GraphicsEngine()
{
  Shutdown();
}




void GraphicsEngine::GetWorldMtx(D3DXMATRIX *worldMatrix, const D3DXVECTOR3 &translation, const D3DXVECTOR3 &scale, const D3DXVECTOR3 &rotation)
{
  D3DXMatrixIdentity(worldMatrix);
  D3DXMATRIX temp;
  D3DXMatrixIdentity(&temp);  

  D3DXMatrixTranslation(&temp, translation.x, translation.y, translation.z);
  *worldMatrix = temp * *worldMatrix;

  D3DXMatrixRotationY(&temp, rotation.y);
  *worldMatrix = temp * *worldMatrix;
  D3DXMatrixRotationX(&temp, rotation.x);
  *worldMatrix = temp * *worldMatrix;
  D3DXMatrixRotationZ(&temp, rotation.z);
  *worldMatrix = temp * *worldMatrix;

  D3DXMatrixScaling(&temp, scale.x, scale.y, scale.z);
  *worldMatrix = temp * *worldMatrix;



}
/////////////////////////////////////////////////////////
void GraphicsEngine::Render()
{
  this->camera.Update();
  

  //////////////CAMERA CODE//////////////////////
  // Define camera information.
  /*D3DXVECTOR3 cameraPos(camera.m_pos.x, camera.m_pos.y,
    camera.m_pos.z);
  D3DXVECTOR3 lookAtPos(camera.m_view.x, camera.m_view.y,
    camera.m_view.z);
  D3DXVECTOR3 upDir(camera.m_up.x, camera.m_up.y,
    camera.m_up.z);*/

  //A Matrix to hold the world transforms of the objects
  D3DXMATRIX worldTransform;
  D3DXMatrixIdentity(&worldTransform);

  // Build view matrix.
  /*D3DXMatrixLookAtLH(&g_ViewMatrix, &cameraPos,
    &lookAtPos, &upDir);*/
  g_ViewMatrix = *camera.GetViewMatrix();

  // Apply the view (camera).
  g_D3DDevice->SetTransform(D3DTS_VIEW, &g_ViewMatrix);

  // Set the projection matrix.
  D3DXMatrixPerspectiveFovLH(&g_projection, 45.0f,
    WINDOW_WIDTH/WINDOW_HEIGHT, 0.1f, 1000.0f);

  g_D3DDevice->SetTransform(D3DTS_PROJECTION, &g_projection);
  //////////////////////END CAMERA CODE///////////



  // Clear the backbuffer.
  g_D3DDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
    D3DCOLOR_ARGB(40,40,40,100), 1.0f, 0);


  //Sort the Objects By their z value in view space
  SortBackToFront();


  //////////////////////////////////////////////////////////////////////////////
  ///////////////////////////BEGIN SCENE////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  
  // Begin the scene.  Start rendering.
  g_D3DDevice->BeginScene();

  //Render Skybox

    static int time = 0; 
    time += 16;
    g_D3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
    g_D3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    
    
    D3DXMatrixIdentity(&worldTransform);
    unsigned int passes;
    effectMap["HackMapping"]->GetEffect()->Begin(&passes, 0);
    effectMap["HackMapping"]->GetEffect()->BeginPass(0);
    GetWorldMtx(&worldTransform, D3DXVECTOR3(gmMgr->graphics_->camera.GetPos().x,gmMgr->graphics_->camera.GetPos().y, gmMgr->graphics_->camera.GetPos().z), D3DXVECTOR3(1,1,1),D3DXVECTOR3(0,time/10000.0f,0));
    effectMap["HackMapping"]->GetEffect()->SetMatrix("g_world", &worldTransform);
    effectMap["HackMapping"]->GetEffect()->SetMatrix("g_wvp", &(worldTransform * g_ViewMatrix * g_projection));


    effectMap["HackMapping"]->GetEffect()->SetTexture("g_texture", this->GetTexture("glow"));
    effectMap["HackMapping"]->GetEffect()->SetTexture("g_normals", textureMap["default"]);

    effectMap["HackMapping"]->GetEffect()->SetBool("Lighting", false);
    effectMap["HackMapping"]->GetEffect()->CommitChanges();

    skybox->mesh->DrawSubset(0);

    effectMap["HackMapping"]->GetEffect()->SetBool("Lighting", true);
    effectMap["HackMapping"]->GetEffect()->CommitChanges();

    effectMap["HackMapping"]->GetEffect()->EndPass();
    effectMap["HackMapping"]->GetEffect()->End();


    g_D3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    g_D3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
  //}

    // Clear the backbuffer.
  g_D3DDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER,
    D3DCOLOR_ARGB(0,0,0,0), 1.0f, 0);


  // Texture filter.
  g_D3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
  g_D3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

  g_D3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
  RenderScene();

  g_D3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
  RenderScene();

  RenderText();

  
  if(m_pBGTexture)
  {
    //GraphicsEngine::CreateComponent(FOSSA);
    g_pSprite->Begin(D3DXSPRITE_ALPHABLEND);

    D3DXMATRIX mat4Transform;
    D3DXMATRIX mat4OldTransform;

    D3DXVECTOR2 vec2Scale;
    vec2Scale.x = static_cast<float>(WINDOW_WIDTH)/1024.f;
    vec2Scale.y = static_cast<float>(WINDOW_HEIGHT)/1024.f;

    D3DXVECTOR3 vec3Position(0.0f, 0.0f, 0.0f);



    g_pSprite->GetTransform(&mat4OldTransform);

    D3DXMatrixTransformation2D(&mat4Transform, NULL, 0.0f, &vec2Scale, NULL, 0.0f, NULL);

    g_pSprite->SetTransform(&mat4Transform);

    g_pSprite->Draw(m_pBGTexture, NULL, NULL, &vec3Position, m_BGColor);

    g_pSprite->SetTransform(&mat4OldTransform);

    g_pSprite->End();
  }


  // End the scene.  Stop rendering.
  g_D3DDevice->EndScene();

  //////////////////////////////////////////////////////////////////////////////
  ///////////////////////////END SCENE//////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

  // Display the scene.
  g_D3DDevice->Present(NULL, NULL, NULL, NULL);

}
/////////////////////////////////////////////////////////
void GraphicsEngine::RenderText()
{
  if(!TextList.empty())
  {
    g_pSprite->Begin(D3DXSPRITE_ALPHABLEND);

    RECT rcSize;
    rcSize.left = 0;
    rcSize.top = 0;
    rcSize.right = WINDOW_WIDTH;
    rcSize.bottom = WINDOW_HEIGHT;

		std::list<TextComponent *>::iterator iter = TextList.begin(), iter2;
		while( iter != TextList.end() )
		{
			rcSize.left = (long)(*iter)->position.x;
			rcSize.top = (long)(*iter)->position.y;
			D3DXCOLOR col = (*iter)->color;
			if( (*iter)->GetLifetime() < TextComponent::SelfDestructTime )
				col.a *= (*iter)->GetLifetime() / (float)TextComponent::SelfDestructTime;
			m_pFont->DrawTextA(g_pSprite, (*iter)->GetStringText().c_str(), -1, &rcSize, DT_TOP|DT_LEFT, col);
			(*iter)->SetLifetime((*iter)->GetLifetime() - 1);
			if( (*iter)->GetLifetime() == 0 )
			{
		    TextComponent *tcomp = *iter;
				iter++;
				this->RemoveText(tcomp);
			}
			else
				iter++;
		}

	 


   
    g_pSprite->End();
  }
}

GraphicsComponent::GraphicsComponent()
{
	this->position = this->rotation = D3DXVECTOR3(0,0,0);
	this->scale = D3DXVECTOR3(1,1,1);
	visible = true;
	shader = NULL;
	ViewSpaceZ = 0;
}
/////////////////////////////////////////////////////////
void GraphicsEngine::RenderScene()
{

  

  D3DXVECTOR4 lightpos;
  if( gmMgr->graphics_->camera.GetTarget() )
    lightpos = D3DXVECTOR4(*gmMgr->graphics_->camera.GetTarget() + 30*D3DXVECTOR3(0,1,0),1);
  else
    lightpos = D3DXVECTOR4(gmMgr->graphics_->camera.GetPos() ,1);

    
	

  std::list<GraphicsComponent *>::iterator iter = this->ComponentList.begin();

	//static float blabla = 0;
	//blabla+=.01f;
  

  effectMap["HackMapping"]->GetEffect()->SetVector("glight", &lightpos);
	effectMap["SkinnedMesh"]->GetEffect()->SetVector("g_light", &lightpos);
  //iterate through all graphics components
  while( iter != ComponentList.end() )
  {
		if( (*iter)->type == FOSSA )
			effectMap["SkinnedMesh"]->GetEffect()->SetBool("fossa", true);
		else
			effectMap["SkinnedMesh"]->GetEffect()->SetBool("fossa", false);
    if ((*iter)->visible)
      (*iter)->Render(g_projection, g_ViewMatrix);
    /*
    (*iter)->shader->GetEffect()->Begin(&passes, 0);
    (*iter)->shader->GetEffect()->BeginPass(0);
    GetWorldMtx(&worldTransform, (*iter)->position, (*iter)->scale,(*iter)->rotation);
    (*iter)->shader->GetEffect()->SetMatrix("g_world", &worldTransform);
    (*iter)->shader->GetEffect()->SetMatrix("g_wvp", &(worldTransform * g_ViewMatrix * g_projection));

    for( int j = 0; j < (*iter)->model->numMaterials; j++ )
    {    
    (*iter)->shader->GetEffect()->SetTexture("g_texture", (*iter)->model->textures[j]);
    (*iter)->shader->GetEffect()->SetTexture("g_normals", (*iter)->Normal);

    (*iter)->shader->GetEffect()->SetBool("showBump", showBump);
    (*iter)->shader->GetEffect()->CommitChanges();

    (*iter)->model->mesh->DrawSubset(j);

    }
    (*iter)->shader->GetEffect()->EndPass();
    (*iter)->shader->GetEffect()->End();
    */
    ++iter;
  }
}



void GraphicsEngine::SortBackToFront()
{
  std::list<GraphicsComponent *>::iterator iter = this->ComponentList.begin();
  D3DXVECTOR3 temp;

  //iterate through all graphics components
  while( iter != ComponentList.end() )
  {
    D3DXVec3TransformCoord( &temp, &(*iter)->position, &g_ViewMatrix );
    (*iter)->ViewSpaceZ = temp.z;
    
    
    ++iter;
  }

  ComponentList.sort();
}

//void SkinnedComponent::RecalculateBB()
//{
//	AnimationVertex *a = NULL;
//	model->GetMesh()->LockVertexBuffer(0, (void**)&a);
//	//D3DXComputeBoundingBox(&a[0].pos,model->GetMesh()->GetNumVertices(),sizeof(AnimationVertex),&this->BBmin,&BBmax);
//	model->GetMesh()->UnlockVertexBuffer();
//}


void SkinnedComponent::ChangeAnim(int animNum)
{
	
	if( CurrentAnim == animNum )
		return;
	CurrentAnim = animNum;
	CurrentTime = 0;
	LPD3DXANIMATIONSET animSet = NULL;
			this->mAnimCtrl->SetTrackSpeed(0, 1.0f);
			this->mAnimCtrl->GetAnimationSet(animNum, &animSet);
			this->mAnimCtrl->SetTrackAnimationSet(0, animSet);
			this->mAnimCtrl->SetTrackPosition(0,CurrentTime);
			CurrentPeriod = (float)animSet->GetPeriod();
			NumFrames = ((ID3DXKeyframedAnimationSet*)animSet)->GetNumTranslationKeys(0);
			NumFrames = max(NumFrames, (int)((ID3DXKeyframedAnimationSet*)animSet)->GetNumTranslationKeys(0));
			NumFrames = max(NumFrames, (int)((ID3DXKeyframedAnimationSet*)animSet)->GetNumRotationKeys(0));
			animSet->Release();

			//RecalculateBB();
			
		
}
void SkinnedComponent::ChangeAnimSpeed(float scalar)
{
	SpeedMultiplier = scalar;
}
void SkinnedComponent::ChangeAnimPosition(float time)
{
	CurrentTime = time;
	this->mAnimCtrl->SetTrackPosition(0,time);
}

bool SkinnedComponent::EndOfAnimation()
{
	const float period = CurrentPeriod * 29.0f /NumFrames;

	return CurrentTime >= period;
}
GraphicsComponent * GraphicsEngine::CreateComponent(GraphicObjects object)
{
  GraphicsComponent *comp = NULL;

  switch(object)
  {
  case FOSSA:
    {
      /*BlockGraphicComponent *fossa = new BlockGraphicComponent();
      comp = fossa;
      fossa->model = modelMap["fossa"];
      fossa->shader = effectMap["HackMapping"];
      fossa->scale = D3DXVECTOR3(.2,.2,.2);
      fossa->position = D3DXVECTOR3(0,20,0);
      fossa->rotation = D3DXVECTOR3(0,0,0);

      fossa->Normal = textureMap["default"];*/
      //fossa->m_vec4Color = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);


      SkinnedComponent *fossa = new SkinnedComponent();
      comp = fossa;
      fossa->model = skinnedMeshMap["animations"];
      fossa->shader = effectMap["SkinnedMesh"];
      fossa->scale = D3DXVECTOR3(.25,.25,.25);
      fossa->position = D3DXVECTOR3(10,10,10);
      fossa->rotation = D3DXVECTOR3(0,0,0);
      
      

      HR(fossa->model->getAnimCtrl()->CloneAnimationController(fossa->model->getAnimCtrl()->GetMaxNumAnimationOutputs(),
        fossa->model->getAnimCtrl()->GetMaxNumAnimationSets(),fossa->model->getAnimCtrl()->GetMaxNumTracks(),fossa->model->getAnimCtrl()->GetMaxNumEvents(),
        &fossa->mAnimCtrl));

			//SETUP////////////
			for( unsigned i = 0; i < fossa->mAnimCtrl->GetMaxNumTracks(); i++ )
				fossa->mAnimCtrl->SetTrackEnable(i, TRUE);

			///////////////////

			//TEST PLAYING AN ANIMATION TRACK!!!!
			fossa->mAnimCtrl->SetTrackEnable(0, TRUE);
			LPD3DXANIMATIONSET animSet = NULL;
			//fossa->mAnimCtrl->SetTrackSpeed(0, .01f);
			fossa->mAnimCtrl->GetAnimationSet(2, &animSet);
			fossa->mAnimCtrl->SetTrackAnimationSet(0, animSet);
			animSet->Release();
			/////////
    }
    break;  

  case BLOCK_STONE:
    {
      BlockGraphicComponent *block = new BlockGraphicComponent();
      comp = block;
      block->model = modelMap["block_stone"];
      block->shader = effectMap["HackMapping"];
      block->scale = D3DXVECTOR3(1,1,1);
      block->position = D3DXVECTOR3(0,0,0);
      block->rotation = D3DXVECTOR3(0,0,0);
      block->visible = true;
      block->Normal = textureMap["rocks_normals"];
    }
    break;  

  case BLOCK_ICE:
    {
      BlockGraphicComponent *block = new BlockGraphicComponent();
      comp = block;
      block->model = modelMap["block_ice"];
      block->shader = effectMap["HackMapping"];
      block->scale = D3DXVECTOR3(1,1,1);
      block->position = D3DXVECTOR3(0,0,0);
      block->rotation = D3DXVECTOR3(0,0,0);
      block->visible = true;
      block->Normal = textureMap["ice_normals"];
    }
    break;

    case BLOCK_FAN:
    {
			/*BlockGraphicComponent *block = new BlockGraphicComponent();
			comp = block;
			block->model = modelMap["block_fan"];
			block->shader = effectMap["HackMapping"];
			block->scale = D3DXVECTOR3(1,1,1);
			block->position = D3DXVECTOR3(0,0,0);
			block->rotation = D3DXVECTOR3(0,0,0);
			block->visible = true;
			block->Normal = textureMap["default"];*/

			SkinnedComponent *fan = new SkinnedComponent();
      comp = fan;
      fan->model = skinnedMeshMap["fanblock"];
      fan->shader = effectMap["SkinnedMesh"];
      fan->scale = D3DXVECTOR3(.05f,.05f,.05f);
      fan->position = D3DXVECTOR3(10,10,10);
      fan->rotation = D3DXVECTOR3(0,0,0);
      
      

      HR(fan->model->getAnimCtrl()->CloneAnimationController(fan->model->getAnimCtrl()->GetMaxNumAnimationOutputs(),
        fan->model->getAnimCtrl()->GetMaxNumAnimationSets(),fan->model->getAnimCtrl()->GetMaxNumTracks(),fan->model->getAnimCtrl()->GetMaxNumEvents(),
        &fan->mAnimCtrl));

			//SETUP////////////
			for( unsigned i = 0; i < fan->mAnimCtrl->GetMaxNumTracks(); i++ )
				fan->mAnimCtrl->SetTrackEnable(i, FALSE);

			///////////////////

			//TEST PLAYING AN ANIMATION TRACK!!!!
			fan->mAnimCtrl->SetTrackEnable(0, TRUE);
			LPD3DXANIMATIONSET animSet = NULL;
			//fan->mAnimCtrl->SetTrackSpeed(0, .01f);
			fan->mAnimCtrl->GetAnimationSet(0, &animSet);
			fan->mAnimCtrl->SetTrackAnimationSet(0, animSet);
			animSet->Release();
			////////////////////////////////////////////
    }
    break;

    case BLOCK_TRAMPOLINE:
    {
      /*BlockGraphicComponent *block = new BlockGraphicComponent();
      comp = block;
      block->model = modelMap["block_trampoline"];
      block->shader = effectMap["HackMapping"];
      block->scale = D3DXVECTOR3(1,1,1);
      block->position = D3DXVECTOR3(0,0,0);
      block->rotation = D3DXVECTOR3(0,0,0);
      block->visible = true;
      block->Normal = textureMap["trampoline_normals"];*/

			
			SkinnedComponent *block = new SkinnedComponent();
      comp = block;
      block->model = skinnedMeshMap["block_trampoline"];
      block->shader = effectMap["SkinnedMesh"];
      block->scale = D3DXVECTOR3(.01f,.01f,.01f);
      block->position = D3DXVECTOR3(10,10,10);
      block->rotation = D3DXVECTOR3(0,0,0);
      //block->Normal = textureMap["trampoline_normals"];
      

      HR(block->model->getAnimCtrl()->CloneAnimationController(block->model->getAnimCtrl()->GetMaxNumAnimationOutputs(),
        block->model->getAnimCtrl()->GetMaxNumAnimationSets(),block->model->getAnimCtrl()->GetMaxNumTracks(),block->model->getAnimCtrl()->GetMaxNumEvents(),
        &block->mAnimCtrl));

			//SETUP////////////
			for( unsigned i = 0; i < block->mAnimCtrl->GetMaxNumTracks(); i++ )
				block->mAnimCtrl->SetTrackEnable(i, FALSE);

			///////////////////

			//TEST PLAYING AN ANIMATION TRACK!!!!
			block->mAnimCtrl->SetTrackEnable(0, TRUE);
			LPD3DXANIMATIONSET animSet = NULL;
			block->mAnimCtrl->SetTrackSpeed(0, 1.f);
			block->mAnimCtrl->GetAnimationSet(0, &animSet);
			block->mAnimCtrl->SetTrackAnimationSet(0, animSet);
			animSet->Release();

			block->ChangeAnimSpeed(.3f);
 			block->ChangeAnim(0);
			////////////////////////////////////////////
    }
    break;

    case BLOCK_SAND:
    {
      BlockGraphicComponent *block = new BlockGraphicComponent();
      comp = block;
      block->model = modelMap["block_sand"];
      block->shader = effectMap["HackMapping"];
      block->scale = D3DXVECTOR3(1,1,1);
      block->position = D3DXVECTOR3(0,0,0);
      block->rotation = D3DXVECTOR3(0,0,0);
      block->visible = true;
      block->Normal = textureMap["sand_normals"];
    }
    break;

  case DIGIBLOCK:
    {
      BlockGraphicComponent *block = new BlockGraphicComponent();
      comp = block;
      block->model = modelMap["digiblock"];
      block->shader = effectMap["HackMapping"];
      block->scale = D3DXVECTOR3(1,1,1);
      block->position = D3DXVECTOR3(0,0,0);
      block->rotation = D3DXVECTOR3(0,0,0);


      block->Normal = textureMap["default"];
    }
    break;  
  case RIGIDBLOCK:
    {
      BlockGraphicComponent *block = new BlockGraphicComponent();
      comp = block;
      block->model = modelMap["rigidblock"];
      block->shader = effectMap["HackMapping"];
      block->scale = D3DXVECTOR3(1,1,1);
      block->position = D3DXVECTOR3(0,0,0);
      block->rotation = D3DXVECTOR3(0,0,0);


      block->Normal = textureMap["default"];
    }
    break;  
  case JEWEL:
    {
      BlockGraphicComponent *block = new BlockGraphicComponent();
      comp = block;
      block->model = modelMap["jewel_off"];
      block->shader = effectMap["HackMapping"];
      block->scale = D3DXVECTOR3(1,1,1);
      block->position = D3DXVECTOR3(0,0,0);
      block->rotation = D3DXVECTOR3(0,0,0);
      block->Normal = textureMap["jewelNorm"];
    }
    break;    
  case JEWEL_1:
    {
      BlockGraphicComponent *block = new BlockGraphicComponent();
      comp = block;
       block->model = modelMap["jewel1"];
      block->shader = effectMap["HackMapping"];
      block->scale = D3DXVECTOR3(1,1,1);
      block->position = D3DXVECTOR3(0,0,0);
      block->rotation = D3DXVECTOR3(0,0,0);
      block->Normal = textureMap["jewelNorm"];
    }
    break;  
  case JEWEL_2:
    {
      BlockGraphicComponent *block = new BlockGraphicComponent();
      comp = block;
      block->model = modelMap["jewel2"];
      block->shader = effectMap["HackMapping"];
      block->scale = D3DXVECTOR3(1,1,1);
      block->position = D3DXVECTOR3(0,0,0);
      block->rotation = D3DXVECTOR3(0,0,0);
      block->Normal = textureMap["jewelNorm"];
    }
    break;
  case JEWEL_3:
    {
      BlockGraphicComponent *block = new BlockGraphicComponent();
      comp = block;
      block->model = modelMap["jewel3"];
      block->shader = effectMap["HackMapping"];
      block->scale = D3DXVECTOR3(1,1,1);
      block->position = D3DXVECTOR3(0,0,0);
      block->rotation = D3DXVECTOR3(0,0,0);
      block->Normal = textureMap["jewelNorm"];
    }
    break;  
  case JEWEL_4:
    {
      BlockGraphicComponent *block = new BlockGraphicComponent();
      comp = block;
      block->model = modelMap["jewel4"];
      block->shader = effectMap["HackMapping"];
      block->scale = D3DXVECTOR3(1,1,1);
      block->position = D3DXVECTOR3(0,0,0);
      block->rotation = D3DXVECTOR3(0,0,0);
      block->Normal = textureMap["jewelNorm"];
    }
    break;
  case JEWEL_5:
    {
      BlockGraphicComponent *block = new BlockGraphicComponent();
      comp = block;
      block->model = modelMap["jewel5"];
      block->shader = effectMap["HackMapping"];
      block->scale = D3DXVECTOR3(1,1,1);
      block->position = D3DXVECTOR3(0,0,0);
      block->rotation = D3DXVECTOR3(0,0,0);
      block->Normal = textureMap["jewelNorm"];
    }
    break;  
  case HUB_FLOOR:
    {
      BlockGraphicComponent *block = new BlockGraphicComponent();
      comp = block;
      block->model = modelMap["hub_floor"];
      block->shader = effectMap["HackMapping"];
      block->scale = D3DXVECTOR3(1,1,1);
      block->position = D3DXVECTOR3(0,0,0);
      block->rotation = D3DXVECTOR3(0,0,0);
      block->Normal = textureMap["rocks_normals"];
    }  
    break;
  case HUB_WALL1:
    {
      BlockGraphicComponent *block = new BlockGraphicComponent();
      comp = block;
      block->model = modelMap["hub_wall1"];
      block->shader = effectMap["HackMapping"];
      block->scale = D3DXVECTOR3(1,1,1);
      block->position = D3DXVECTOR3(0,0,0);
      block->rotation = D3DXVECTOR3(0,0,0);
      block->Normal = textureMap["default"];
    }
    break;
  case HUB_WALL2:
    {
      BlockGraphicComponent *block = new BlockGraphicComponent();
      comp = block;
      block->model = modelMap["hub_wall2"];
      block->shader = effectMap["HackMapping"];
      block->scale = D3DXVECTOR3(1,1,1);
      block->position = D3DXVECTOR3(0,0,0);
      block->rotation = D3DXVECTOR3(0,0,0);
      block->Normal = textureMap["default"];
    }
    break;  
  case HUB_WALL3:
    {
      BlockGraphicComponent *block = new BlockGraphicComponent();
      comp = block;
      block->model = modelMap["hub_wall3"];
      block->shader = effectMap["HackMapping"];
      block->scale = D3DXVECTOR3(1,1,1);
      block->position = D3DXVECTOR3(0,0,0);
      block->rotation = D3DXVECTOR3(0,0,0);
      block->Normal = textureMap["default"];
    }
    break;
  default:
    return NULL;

  }
	comp->type = object;

  ComponentList.push_back(comp);
  return comp;
}

void GraphicsEngine::RemoveComponent(GraphicsComponent* &gc)
{
  if( gc == NULL )
    return;
  ComponentList.remove(gc);
  delete gc;
  gc = NULL;
}





//void GraphicsComponent::Fonts(void)
//{
//LPDIRECT3DDEVICE9 f_device;
//	LPD3DXFONT g_Font = NULL; //font object
//	
//	D3DXFONT_DESC FontDesc = {24,
//		0,
//		400,
//		0,
//		false,
//		DEFAULT_CHARSET,
//		OUT_TT_PRECIS,
//		CLIP_DEFAULT_PRECIS,
//		DEFAULT_PITCH,
//		"Arial"};
//	RECT FontPosition;
//	FontPosition.top = 0;
//	FontPosition.left = 0;
//	FontPosition.right = 50;//g_D3DDevice.GetWidth();//g_App.GetWidth();
//	FontPosition.bottom = 50;//g_App.GetHeight();
//	//create font
//	D3DXCreateFontIndirect(f_,&FontDesc,&g_Font);
//	FontPosition.top = 0;         //position
//	g_Font->DrawText(NULL,
//		"Direct3D Tutorial 09: Text With D3DXFont",
//		-1,
//		&FontPosition,
//		DT_CENTER,
//		0xffffffff); //draw text
//
//	FontPosition.top = 100;       //position
//	g_Font->DrawText(NULL,
//		"Text Sample Using D3DXFont",
//		-1,
//		&FontPosition,
//		DT_CENTER,
//		0xffff0000); //draw text
//
//}


void GraphicsEngine::SetSplashScreen(const std::string& strSplashScreen, D3DXCOLOR color)
{
  if (strSplashScreen == "NONE")
  {
    m_pBGTexture = NULL;
    m_BGColor = color;
  }
  else 
  {
    m_pBGTexture = textureMap[strSplashScreen];
    m_BGColor = color;
  }
}


//void GraphicsEngine::SetEndText(const std::string& strEndText)
//{
//  m_strEndText = strEndText;
//}

TextComponent * GraphicsEngine::CreateText(std::string message, int lifetime, D3DXVECTOR2 screenPosition)
{
	TextComponent *tc = new TextComponent();
	tc->lifetime = lifetime;
	tc->message = message;
	tc->position = screenPosition;
	TextList.push_back(tc);
	return tc;
}

void GraphicsEngine::RemoveText(TextComponent * &tc)
{
	if( tc == NULL )
		return;
	std::list<TextComponent*>::iterator iter = TextList.begin();
	while( iter != TextList.end() )
	{
		if( tc == *iter )
		{
			break;
		}
		++iter;
	}
	if( iter == TextList.end() )
	{
		tc = NULL;
		return;
	}
	TextList.remove(tc);
			delete tc;
			tc = NULL;
	
}