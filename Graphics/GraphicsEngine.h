//------------------------------------------------------------------------
//
//  Author: Anthony Serra and Josh Rutz
//
//
//------------------------------------------------------------------------
#pragma once
#pragma message("Knows about GraphicsEngine.h")
#include<d3d9.h>
#include<d3dx9.h>
#include<dxerr9.h>
#include<map>



#include <vector>
#include <string>
#include <list>
#include "Camera.h"

#include "VertexDeclarations.h"
#include "myFirstShader.h"

#define WINDOW_CLASS    "UGPDX"
#define WINDOW_NAME     "FossaNova"
#define WINDOW_WIDTH    1024
#define WINDOW_HEIGHT   768

#define SHADOW_MAP_SIZE  512
#define ReleaseCOM(x) if(x) {x->Release();x=0;}

#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x) \
  { \
  HRESULT hr = x; \
  if(FAILED(hr)) \
    { \
    DXTrace(__FILE__,__LINE__,hr,#x,TRUE); \
    } \
  } 
#endif
#else
#ifndef HR
#define HR(x) x
#endif
#endif

#if defined(_DEBUG)
#define SHADER_DEBUG_FLAG D3DXSHADER_DEBUG
#else
#define SHADER_DEBUG_FLAG 0
#endif


class SkinnedMesh;
class myEffect;
class Model;
//MAKE SURE THAT count IS THE LAST ELEMENT IN THE ENUM
enum GraphicObjects {FOSSA=0, BLOCK_STONE, BLOCK_ICE, BLOCK_FAN, BLOCK_TRAMPOLINE, BLOCK_SAND, DIGIBLOCK, RIGIDBLOCK, JEWEL, JEWEL_1, JEWEL_2, JEWEL_3, JEWEL_4, JEWEL_5, HUB_FLOOR, HUB_WALL1, HUB_WALL2, HUB_WALL3, COUNT};

class GraphicsComponent
{
public:

  virtual ~GraphicsComponent(){}
  friend class GraphicsEngine;

  //Holds World Position, scale, and rotation of the model for the world transform in the render cycle
  D3DXVECTOR3 position;
  D3DXVECTOR3 scale;
  D3DXVECTOR3 rotation;
  bool visible;

	GraphicObjects type;
  virtual D3DXVECTOR3 GetBBMinTransformed(){return D3DXVECTOR3();}
  virtual D3DXVECTOR3 GetBBMaxTransformed(){return D3DXVECTOR3();}
  virtual const D3DXVECTOR3 & GetBBMin()=0;
  virtual const D3DXVECTOR3 & GetBBMax()=0;
  void SetColor(const D3DXVECTOR4& vec4Color);
  const D3DXVECTOR4& GetColor() const;
  void GraphicsComponent::Fonts(void);


  virtual void Render(const D3DXMATRIX &proj, const D3DXMATRIX &view){}


//protected

  //The shader associated with the model
  myEffect *shader;

  virtual void Init(void)=0;

  //Constructor can only be called by graphics engine
	GraphicsComponent();

  float ViewSpaceZ;

  bool operator < (const GraphicsComponent& rhs)
  {
    return ViewSpaceZ < rhs.ViewSpaceZ;
  }
  
};

class BlockGraphicComponent : public GraphicsComponent
{
public:
 
  BlockGraphicComponent(){Init();}
  virtual ~BlockGraphicComponent();

  virtual void Render(const D3DXMATRIX &proj, const D3DXMATRIX &view);

   virtual void Init(void);

   //pointer to the mesh stored in the graphics engine
  Model *model;

   // Holds a texture image used for rendering with the scene
  LPDIRECT3DTEXTURE9 Normal;
  
 


  D3DXVECTOR3 GetBBMinTransformed();
  D3DXVECTOR3 GetBBMaxTransformed();
  const D3DXVECTOR3 & GetBBMin();
  const D3DXVECTOR3 & GetBBMax();

private:
 
};

class SkinnedComponent : public GraphicsComponent
{
public:
	SkinnedComponent() {mAnimCtrl = 0;
	model = 0; NumFrames = 0; CurrentAnim = -1; CurrentTime = CurrentPeriod = 0;
	SpeedMultiplier = 1;}
   virtual ~SkinnedComponent();

   virtual void Render(const D3DXMATRIX &proj, const D3DXMATRIX &view);

	 

   virtual void Init(void);

	 void ChangeAnim(int animNum);
	 void ChangeAnimSpeed(float scalar);
	 void ChangeAnimPosition(float time);
	 bool EndOfAnimation();
	 int GetCurrentAnim() {return CurrentAnim;}
   SkinnedMesh *model;

   ID3DXAnimationController* mAnimCtrl;


    D3DXVECTOR3 GetBBMinTransformed();
    D3DXVECTOR3 GetBBMaxTransformed();
    const D3DXVECTOR3 & GetBBMin();
    const D3DXVECTOR3 & GetBBMax();
		
		void Update(double time);
   
private:
	void AdvanceMyTime(double time);
	int CurrentAnim;
	//unimplemented
	float CurrentTime;
	float CurrentPeriod;
	int NumFrames;
	float SpeedMultiplier;

};


class Model
{
public:
  Model(){mesh = 0;}
  ~Model(){ReleaseCOM(mesh); delete[] materials;delete[] textures;}

  friend class GraphicsEngine;

  LPD3DXMESH mesh;

  int numMaterials;

  LPD3DXMATERIAL materials;
  LPDIRECT3DTEXTURE9* textures;

  //BoundingBox Min and Max
  D3DXVECTOR3 BBmin, BBmax;  
};

class TextComponent
{
public:
	

	friend class GraphicsEngine;
	D3DXVECTOR2 position;
	D3DXCOLOR color;
	void SetLifetime(int lt) {if(lt<0)lifetime=MAXWORD;else lifetime = lt;}
	int GetLifetime(){return lifetime;}
	void SetStringText(const char *msg){message = msg;}
	std::string GetStringText() {return message;}
	void Suicide() {lifetime = SelfDestructTime;}
	static const int SelfDestructTime = 30;
private:
	TextComponent() {lifetime = 0; message = ""; position = D3DXVECTOR2(0,0);color=D3DXCOLOR(1,1,1,1);}
	~TextComponent(){}
	
	unsigned int lifetime;
	std::string message;
	

};
class GraphicsEngine
{
public:

  GraphicsEngine( );
  ~GraphicsEngine();


  LPDIRECT3DDEVICE9 GetDevice() {return g_D3DDevice;}
  // Function Prototypes...
  bool InitializeD3D(HWND hWnd, bool fullscreen);

  void Render();
  


  GraphicsComponent * CreateComponent(GraphicObjects object);
	TextComponent *CreateText(std::string message, int lifetime, D3DXVECTOR2 screenPosition);

	
  void RemoveComponent(GraphicsComponent* &gc);


  void SetSplashScreen(const std::string& strSplashScreen, D3DXCOLOR color);
  //void SetEndText(const std::string& strEndText);
  

  //
  void GetWorldMtx(D3DXMATRIX *worldMatrix,
    const D3DXVECTOR3 &translation,
    const D3DXVECTOR3 &scale,
    const D3DXVECTOR3 &rotation);

  Camera camera;

  // Direct3D object and device.
  LPDIRECT3D9 g_D3D;
  LPDIRECT3DDEVICE9 g_D3DDevice;
  void ToggleFullScreen(HWND hWnd,bool fullscreen);

  D3DXMATRIX GetVPTransform(){return g_ViewMatrix * g_projection;}
  //friend class SkinnedMesh;

  HWND GetWnd() {return hWnd;}

  const std::map<std::string, Model *> &GetModelMap(){return modelMap;}
  const std::map<std::string, SkinnedMesh *> &GetSkinnedMeshMap(){return skinnedMeshMap;}
  const std::map<std::string, myEffect *> &GetEffectMap(){return effectMap;}
  const std::map<std::string, LPDIRECT3DTEXTURE9> &GetTextureMap(){return textureMap;}
	LPDIRECT3DTEXTURE9 GetTexture(const std::string &texName);
private:

  void RenderScene();
  void RenderText();
	void RemoveText(TextComponent * &tc);

  void SortBackToFront();
	ID3DXSprite* g_pSprite;
	IDirect3DTexture9* m_pBGTexture, *shadowTexture;
	IDirect3DSurface9* shadowSurface;
	D3DXCOLOR m_BGColor;
	  
	Model *skybox;
	ID3DXFont* m_pFont;

	std::string m_strEndText;

	HWND hWnd;

  D3DPRESENT_PARAMETERS d3dpp;

  // Matrices.
  D3DXMATRIX g_projection;
  D3DXMATRIX g_ViewMatrix;

  //Compiles the shader effects and puts them in the effectmap
  void InitializeEffects();
  //Loads in all the models from file and puts their materials,textures, and meshes in the corresponding maps
  void InitializeMeshes();

  bool InitializeObjects();

  void Shutdown();


  std::list<GraphicsComponent *> ComponentList;
	std::list<TextComponent *> TextList;

  std::map<std::string, Model *> modelMap;
  std::map<std::string, SkinnedMesh *> skinnedMeshMap;
  //std::map<std::string, LPD3DXMATERIAL> materialMap;
  std::map<std::string, myEffect *> effectMap;
  std::map<std::string, LPDIRECT3DTEXTURE9> textureMap;
};



