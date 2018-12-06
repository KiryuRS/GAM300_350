/*****************************************************************************/
/*!
\file GraphicsEngine.h
\author Zhou Zhihua, zhihua.z, 390001016
\par zhihua.z\@digipen.edu
\date August 12, 2017
\brief

This file contains basic structure and core functions of GraphicsEngine

Copyright (C) 2017 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/*****************************************************************************/
#pragma once

#define GLEW_STATIC
#include <glew.h>
#include <unordered_map>

#include "GraphicsEnums.h"
#include "ShaderManager.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"
#include <memory>
#include "Light.h"
#include <mutex>
#include <utility>
#include "CubeMap.h"
#include "Framebuffer.h"
#include "Material.h"
#include "LevelSettings.h"
#include <variant>
#include "BaseMeshLoader.h"
#include "PostProcessing.h"


class TextureLoader;
class StaticMeshLoader;
class SkeletalMeshLoader;
struct GameSpacesInfo;
template<typename T>
struct ThreadsafeQueue;
struct LineSegment;
struct SkeletalMesh;
struct Particles;
class ParticleEmitterC;
class Sprite2DC;
struct GameSpace;
struct ParticleGL;

typedef BOOL(WINAPI * PFNWGLSWAPINTERVALEXTPROC) (int interval);
extern PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;

enum class GFXMSG {
	DRAWLINE, DRAWMODEL, DRAWSHADOW, RECOMPILESHADERS,
	UPDATEMODELLIST, COMPILEDFILE, DRAWTOPLINE, UPDATETEXTURELIST,
	UPDATESCENESETTINGS, UPDATEMATERIALLIST, REMOVEMATERIAL, DRAWPARTICLES, DRAWSPRITE
};

enum class MODELTYPE {
	STATIC, SKELETAL
};

enum class SpriteAnimType
{
  None = 0,
  Radio,
  Horizontal,
  Vertical
};

struct SpriteAnimData
{
  float operator[] (int index) const
  {
    return arr[index];
  }

  float arr[4] = { 0.0f };
};



struct ParticleDetails
{
	std::vector<ParticleGL> particles;
	GLuint vbo = 999;
	bool emissive = false;
	Matrix4x4 transformation;
	std::string texture, model;
};

struct SpriteDetails
{
	Matrix4x4 transformation;
	SpriteAnimType SAtype = { SpriteAnimType::Radio };
	SpriteAnimData SAdata;
	std::string texture;
	bool always_front = { false };
	float opacity;
	Vector2 uv;
	Vector4 tint;
};

struct ModelDetails
{
	Matrix4x4 transformation;
	MODELTYPE type;
	std::string texture, model;
	Vector2 uv;
	int row = { 1 };
	int column = { 1 };
	int frame = { 0 };
	float opacity;
	Vector4 tint;
	bool shadow = false;
	int animNum = 0;
	float animTime = 0.f;
	std::vector<std::string> materials;
	bool cast_shadow = { true };
	bool recv_shadow = { true };
  bool always_front = { false };
  bool emissive = { false };
  std::weak_ptr<char> proxy;
};

struct LineDetails
{
	Vector3					p0, p1;
	Vector4					color;
};

struct GraphicsMessage
{
	GFXMSG					type;
	std::variant<ModelDetails, LineDetails, 
		LevelSettings*, ParticleDetails, SpriteDetails> data;
	unsigned				spaceNum = 0;
};

struct SceneData
{
	std::vector<Vector3>		lines;
	std::vector<Vector4>		lineColors;
	std::vector<Vector3>		topLines;
	std::vector<Vector4>		topLineColors;
	std::vector<ModelDetails>	modelDetails;
	std::vector<ParticleDetails>	particleDetails;
	std::vector<SpriteDetails>	spriteDetails;
	Camera*						camera = nullptr;
	bool						calculateShadows = true;
	bool						drawSkybox;
	Light_container lights;
	Vector4 fogColor;
	bool drawFog = { false };
};

class GraphicsEngine : public CoreSystem
{
	/****************************************************************************
	Private Container Declarations
	***************************************************************************/
	std::unique_ptr<ThreadsafeQueue<GraphicsMessage>>	messageQueue;
	std::vector<SceneData>								scenes;
	std::vector<std::string>							compiledModels;
	std::vector<std::string>							compiledTextures;
	std::vector<std::string>							materials;
	std::vector<const char*>							modelList;
	std::vector<const char*>							textureList;
	std::vector<const char*>							materialList;
	std::map<std::string, Material>						materialData;
	unsigned											mainLayer;
	PFNWGLSWAPINTERVALEXTPROC							wglSwapIntervalEXT = nullptr;

	template<typename T>
	void AssignMaterials(T& sMesh, const std::vector<std::string>& materials)
	{
		sMesh->materials.clear();
		size_t newSize = sMesh->meshes.size() > materials.size() ? sMesh->meshes.size() : materials.size();
		sMesh->materials.resize(newSize);
		if (materials.size())
		{
			for (unsigned i = 0; i < materials.size(); ++i)
			{
				auto iter = materialData.find(materials[i]);
				if (iter != materialData.end())
					sMesh->materials[i] = &iter->second;
				else
					sMesh->materials[i] = &materialData.begin()->second;
			}
		}
		else
		{
			for (unsigned i = 0; i < materials.size(); ++i)
				sMesh->materials[i] = &materialData.begin()->second;
		}
	}
	bool DrawFog = { false };

public:

	/****************************************************************************
	Resource Loaders
	****************************************************************************/
	std::unique_ptr<TextureLoader> texLoader;
	std::unique_ptr<BaseMeshLoader<StaticMesh>>smeshLoader;
	std::unique_ptr<BaseMeshLoader<SkeletalMesh>>skmeshLoader;

	void UnloadTexture(Texture* tex);
	void unload_model(Mesh* m);
	void unload_model(StaticMesh* m);
	void unload_model(SkeletalMesh* m);

	Texture* GetTexture(const std::string& path);
	StaticMesh* GetStaticMesh(const std::string& path);
	SkeletalMesh* GetSkeletalMesh(const std::string& path);


	/****************************************************************************
	Core Graphics Engine Functions
	****************************************************************************/
	GraphicsEngine(HWND window);

	void Init();
	void Update();
	void Exit();

	bool InitializeRenderEnvironment();
	void CleanRenderingEnvironment();

	void SwapBuffers();

	void GraphicsFrameStart();
	void GraphicsFrameEnd();

	void RegisterLight(Light* l, size_t gamespace);
	void UnregisterLight(Light* l, size_t gamespace);

	void ChangeScreenResolution(int x, int y);
	void SetupSceneData(const std::vector<GameSpacesInfo>& allGameSpaces);
	void SetupSceneData(const std::vector<std::unique_ptr<GameSpace>>& allGameSpaces);

	/****************************************************************************
	Draw Functions
	****************************************************************************/
	void StartMainBuffer();
	void EndMainBuffer();
	void MeshStart();
	void MeshAddTriangle(const Vertex& v1, const Vertex& v2, const Vertex& v3);
	std::unique_ptr<Mesh>&& MeshEnd();
	void MeshDraw(Mesh* mesh, DrawMode dm, const SceneData& cam);
	void MeshDraw(StaticMesh* smesh, DrawMode dm, const SceneData& cam);
	void MeshDraw(SkeletalMesh* smesh, DrawMode dm, const SceneData& cam);
  void SpriteDraw(StaticMesh* smesh, const SpriteDetails& md, const SceneData& cam);
  void ParticleDraw(StaticMesh* smesh, ParticleDetails&& md, const SceneData& cam);

	void GFXSetTransformation(const Matrix4x4& mat);
	//void GFXSetTransformation(const Matrix4x4x4& mat);
	void GFXSetPosition(Vector3 pos);
	void GFXSetPosition(float x, float y, float z);
	void GFXSetRotation(float r);
	void GFXSetScale(Vector3 pos);
	void GFXSetScale(float sx, float sy, float sz);
	void GFXSetTexture(Texture* tex);
	void GFXSetTexture(const std::string& tex);
	void GFXSetTextureUV(float u = 0, float v = 0);
	void GFXSetTransparency(float f);
	inline void GFXSetTextureUV(Vector2 vv)
	{
		GFXSetTextureUV(vv.x, vv.y);
	}
	void GFXSetColorTint(Vector4 tint);
	void GFXSetShininess(float s);
	void GFXSetAnimationFrame(int dimx, int dimy, int frameindex);

	void GFXSetSkyModel(StaticMesh* m);
	void GFXSetSkyTexture(Texture* t);
	void GFXDrawSky(SceneData& scene);

	void SetCamera(Camera& cam, unsigned layer = 0)			{ scenes[layer].camera = &cam; }
	void RemoveCamera(unsigned layer = 0)					{ scenes[layer].camera = &main_camera; }
	Camera* GetCurrentCamera() const						{ return scenes[mainLayer].camera; }


	/****************************************************************************
	Post Effects
	****************************************************************************/

	void renderScreen();

	void GFXPostProcessing();

	// directional light shadow
	void CalculateShadows(const SceneData&);
	void ClearShadowMaps();
	void calculateShadowTextureStart(int index);
	void calculateShadowMeshDraw(StaticMesh* smesh, int i, const SceneData&);
	void calculateShadowMeshDraw(SkeletalMesh* smesh, int i, const SceneData&);
	void calculateShadowTextureEnd();
	bool FrustumCulling(const ModelDetails&, const SceneData&, const StaticMesh&) const;
	bool FrustumCulling(const ModelDetails&, const SceneData&, const SkeletalMesh&) const;

	// HDR
	void StartHDRBuffer();
	void EndHDRBuffer();
	void CalculateHDR();

	// Bloom
	void ApplyBloom(Shader& blurshader, Shader& blendshader);

	// Anti aliasing MSAA, assuming current draw buffer is hdr buffer
	void setMSAA(int amount);
	void RenderModel(const ModelDetails&, const SceneData&); 
  void RenderParticle(ParticleDetails&&, const SceneData&);


	/****************************************************************************
	Internal Helper Functions
	****************************************************************************/
	void DrawLine(const LineDetails&, int sceneNum = 0);
	void DrawTopLine(const LineDetails&, int sceneNum = 0);
	void ApplyLighting(Shader& shader, const Mesh& mesh, const SceneData& cam)const;
	void ApplyMaterial(Shader& shader, const StaticMesh& sm, int index);
	void ApplyMaterial(Shader& shader, const SkeletalMesh& sm, int index);

	// Gamma Correction
	void CalculateGammaCorrection();

	auto GetCameraPosition() const							{ return scenes[0].camera->GetPosition(); }
	const std::vector<const char*>& GetModelList()			{ return modelList; }
	const std::vector<const char*>& GetTextureList()		{ return textureList; }
	const std::vector<const char*>& GetMaterialList()		{ return materialList; }
	void UpdateModelList();
	void UpdateTextureList();
	void UpdateMaterialList();
	void DisplayWindow();
	void ClearDebug();
	const auto& GetCurrentBuffer() const { return currentBuffer->GetTexture(); }
	bool ObjectsLoaded() const;


private:

	void Internal_CheckCameraDirty();
	void Internal_SetTransform(Shader& shader);
	void Internal_CleanUp();

	// core components
	HWND m_window;
	HDC m_windowDC;
	HGLRC  m_glDC;
	std::unique_ptr<ShaderManager> m_shaderManager;
  std::vector<std::unique_ptr<::PostProcessing>> m_post_processing_arr;

	GLuint lineBuffer[2]{};
	GLuint lineID{};

	// render helpers
	Framebuffer MainDrawBuffer[4];
	// GLuint MainDrawBuffer[4]; // for |NO MSAA|MSAA 2x|MSAA 4x|MSAA 8x|
	// GLuint MainDrawTexture[4];
	// GLuint MainDrawRBO[4];
	Framebuffer* currentBuffer = &MainDrawBuffer[0];
	
	unsigned screenVAO = 0;
	unsigned screenVBO;
	GLuint rboDepth;
	GLuint hdrFBO;
	GLuint hdrColorBuffer;
	GLuint colorBuffer[2];
	GLuint pingpongFBO[2];
	GLuint pingpongBuffers[2];
	Framebuffer GammaCorrectionBuffer;
	bool HdrEnabled = { false };
	float gamma = { 2.2f };
	float hdrexposure = { 1.0f };
	bool DrawSoftShadow = { true };

	// settings
	int render_x_resolution;
	int render_y_resolution;


	// temporary state
	Camera main_camera;
	bool transform_set = { false };
	bool material_set = { false };
	Matrix4x4 model, mvp;
	Matrix4x4 trans, scale, rotate;
	Material mat;
  StaticMesh* RHSmesh = { nullptr };
	Texture* RHtexture = { nullptr };
	Vector4 tint = { Vector4() };
	float shininess = { 10.0f };
	GLuint ubohandle = { 0xDEAD };
	int light_current = { 0 };
	Vector3 Iemissive = { 0.f, 0.f, 0.f };
	float m_transparency = { 1.0f };
	int frameDimension[2] = { 1, 1 };
	int frameIndex = { 0 };
	float f = 0.0f;
  Vector3 PlayerPosition = { Vector3(0.0f, 0.0f, 0.0f) };
  bool cast_shadow = { true };
  bool recv_shadow = { true };

  Vector3 shadowCenter = { Vector3(1000.0f, 0.0f, -500.0f) };

	// TODO: Dennis Add following graphics settings into editor
	float zFar = { 1000.0f };
	float zNear = { 0.001f };
	float gAmbient[3] = { 0.01f };
	float gFog[3] = { 0.8f, 0.8f, 0.8f };
	float gFogDistance = 500.0f;
	bool DrawSky = true;
  bool RenderShadow = { true };

	CubeMap cm;
	StaticMesh* m_sky_model = { nullptr };
	Texture* m_sky_texture[6] = { nullptr };


	// internal holders
	std::unique_ptr<Mesh> internal_mesh;
	Vector2 texture_uv;
	Framebuffer shadowBuffer[Light_container::MAX_LIGHTS];
	bool shadowCalculated[Light_container::MAX_LIGHTS];
	Matrix4x4 depthMV;

	// testing purpose
	//Camera* currentCamera;
	void HandleMessages();
	//  Draws all objects based on the messages
	void Draw(SceneData& sceneData);
	GLuint testTexture;
};

void internal_gen_vertex_normal(Mesh & m, GLuint p0, GLuint p1, GLuint p2);
