/*****************************************************************************/
/*!
\file GraphicsEngine.cpp
\author Zhou Zhihua
\par zhihua.z\@digipen.edu
\date August 31, 2017
\brief

This file contains the actual implementation of the graphical utilities in our
game engine, including:
- drawing of objects
- manage internal graphics states
- manage shaders
- loading of textures, models, animations...
- resource management of graphical objects

Copyright (C) 2017 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/*****************************************************************************/
#include "stdafx.h"
#include "GraphicsEngine.h"
#include <algorithm>
#include "Camera.h"
#include "ext.hpp"
#include "GameScene.h"
#include "BaseMeshLoader.h"
#include "ParticleEmitter.h"
#include "2DSpriteC.h"
#include "PPHDR.h"
#include "CollisionTests.h"

#pragma warning (push)
#pragma warning (disable : 4312)
#pragma warning (disable : 4267)

#define PI 3.141592653f

unsigned int attachments[16] = {
  GL_COLOR_ATTACHMENT0,
  GL_COLOR_ATTACHMENT1,
  GL_COLOR_ATTACHMENT2,
  GL_COLOR_ATTACHMENT3,
  GL_COLOR_ATTACHMENT4,
  GL_COLOR_ATTACHMENT5,
  GL_COLOR_ATTACHMENT6,
  GL_COLOR_ATTACHMENT7,
  GL_COLOR_ATTACHMENT8,
  GL_COLOR_ATTACHMENT9,
  GL_COLOR_ATTACHMENT10,
  GL_COLOR_ATTACHMENT11,
  GL_COLOR_ATTACHMENT12,
  GL_COLOR_ATTACHMENT13,
  GL_COLOR_ATTACHMENT14,
  GL_COLOR_ATTACHMENT15
};

GraphicsEngine::GraphicsEngine(HWND window) :
	m_window{ window }, trans{ Matrix4x4(1.0f) }, rotate{ Matrix4x4(1.0f) }, scale{ Matrix4x4(1.0f) },
	messageQueue{ std::make_unique<ThreadsafeQueue<GraphicsMessage>>() }, mainLayer{ 0 }, scenes{ size_t{ 1 } }
{
	CoreSystem::SetName("Graphics");
	CoreSystem::OverrideUpdate();

	scenes[0].camera = &main_camera;
	//scenes[0].drawSkybox = DrawSky;
}

void GraphicsEngine::Init()
{
	InitializeRenderEnvironment();
	m_shaderManager = std::make_unique<ShaderManager>();
	m_shaderManager->Init();


	texLoader = std::make_unique<TextureLoader>(m_windowDC);
	smeshLoader = std::make_unique<BaseMeshLoader<StaticMesh>>();
	skmeshLoader = std::make_unique<BaseMeshLoader<SkeletalMesh>>();
	UpdateModelList();
	UpdateTextureList();
	UpdateMaterialList();


	// TODO: Fix this resolution later, for now use application screen width and height first
	// setup render resolution
	// RECT xy;
	// SystemParametersInfo(SPI_GETWORKAREA, 0, &xy, 0);
	// render_x_resolution = static_cast<float>(xy.right - xy.left);
	// render_y_resolution = static_cast<float>(xy.bottom - xy.top);
	RECT xy;
	GetClientRect(m_window, &xy);
	render_x_resolution = (int)(xy.right - xy.left);
	render_y_resolution = (int)(xy.bottom - xy.top);

	GLint GlewInitResult = glewInit();
	TOOLS::Assert(GlewInitResult == GLEW_OK, "ERROR: ", glewGetErrorString(GlewInitResult));

	// graphic testing here
	// MeshStart();
	// {
	//   cr = 1.0f;
	//   cg = 0.0f;
	//   cb = 0.3f;
	//   str = 0.001f;
	//   stg = 0.005f;
	//   stb = 0.003f;
	// }

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LESS);


	RECT rect;
	GetWindowRect(m_window, &rect);
	// Vector2 center = Vector2(rect.left + (rect.right - rect.left) / 2, rect.top + (rect.bottom - rect.top) / 2);
	//SetCursorPos(center.x, center.y);


	glGenBuffers(1, &ubohandle);
	std::vector<Vector3> lines;
	std::vector<Vector4> lineColors;

	lines.emplace_back(Vector3(90, 90, 90));
	lines.emplace_back(Vector3(91, 91, 91));
	lineColors.emplace_back(Vector4(1, 0, 0, 1));
	lineColors.emplace_back(Vector4(1, 0, 0, 1));
	glGenVertexArrays(1, &lineID);
	glGenBuffers(2, lineBuffer);
	glBindVertexArray(lineID);
	glBindBuffer(GL_ARRAY_BUFFER, lineBuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, lines.size() * sizeof(Vector3), &lines[0].x, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, lineBuffer[1]);
	glBufferData(GL_ARRAY_BUFFER, lineColors.size() * sizeof(Vector4), &lineColors[0].x, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, lineBuffer[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, lineBuffer[1]);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindVertexArray(0);

  m_post_processing_arr.push_back(std::make_unique<PPHDR>(render_x_resolution, render_y_resolution));

	FramebufferSetting mainbufsetting;
	mainbufsetting.HasColorAttachment = true;
	mainbufsetting.ColorXResolution = render_x_resolution;
	mainbufsetting.ColorYResolution = render_y_resolution;
	mainbufsetting.HasDepthAttachment = true;
	mainbufsetting.DepthFormat = GL_DEPTH_COMPONENT32;
	mainbufsetting.DepthXResolution = render_x_resolution;
	mainbufsetting.DepthYResolution = render_y_resolution;
	MainDrawBuffer[0].Setup(mainbufsetting);
  MainDrawBuffer[0].AddColorAttachment(render_x_resolution, render_y_resolution, GL_COLOR_ATTACHMENT1);

	mainbufsetting.IsMultisample = true;
	mainbufsetting.MultiSampleAmount = 2;
	MainDrawBuffer[1].Setup(mainbufsetting);
  MainDrawBuffer[1].AddColorAttachment(render_x_resolution, render_y_resolution, GL_COLOR_ATTACHMENT1);

	mainbufsetting.IsMultisample = true;
	mainbufsetting.MultiSampleAmount = 4;
	MainDrawBuffer[2].Setup(mainbufsetting);
  MainDrawBuffer[2].AddColorAttachment(render_x_resolution, render_y_resolution, GL_COLOR_ATTACHMENT1);

	mainbufsetting.IsMultisample = true;
	mainbufsetting.MultiSampleAmount = 8;
	MainDrawBuffer[3].Setup(mainbufsetting);
  MainDrawBuffer[3].AddColorAttachment(render_x_resolution, render_y_resolution, GL_COLOR_ATTACHMENT1);

	FramebufferSetting shadowbufsetting;
	shadowbufsetting.HasColorAttachment = true;
	shadowbufsetting.ColorFormat = GL_DEPTH_COMPONENT;
	shadowbufsetting.DataType = GL_FLOAT;
	shadowbufsetting.ColorTarget = GL_DEPTH_ATTACHMENT;
	shadowbufsetting.DepthXResolution = 1024;
	shadowbufsetting.DepthYResolution = 1024;
	shadowbufsetting.NoColorAttachment = true;
  shadowbufsetting.WhiteBorder = true;
	for (unsigned i = 0; i < Light_container::MAX_LIGHTS; ++i)
		shadowBuffer[i].Setup(shadowbufsetting);

	m_sky_model = &smeshLoader->GetMesh("CompiledAssets\\Models\\skybox_plane.mdl");

	m_sky_texture[0] = texLoader->GetTexture("CompiledAssets\\Sprites\\blood-stain-gorge_lf.dds");
	m_sky_texture[1] = texLoader->GetTexture("CompiledAssets\\Sprites\\blood-stain-gorge_rt.dds");
	m_sky_texture[2] = texLoader->GetTexture("CompiledAssets\\Sprites\\blood-stain-gorge_dn.dds");
	m_sky_texture[3] = texLoader->GetTexture("CompiledAssets\\Sprites\\blood-stain-gorge_up.dds");
	m_sky_texture[4] = texLoader->GetTexture("CompiledAssets\\Sprites\\blood-stain-gorge_bk.dds");
	m_sky_texture[5] = texLoader->GetTexture("CompiledAssets\\Sprites\\blood-stain-gorge_ft.dds");

	glGenFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	glGenTextures(1, &hdrColorBuffer);
	glBindTexture(GL_TEXTURE_2D, hdrColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, render_x_resolution, render_y_resolution, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdrColorBuffer, 0);

	glGenTextures(2, colorBuffer);
	for (unsigned i = 0; i < 2; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, colorBuffer[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, render_x_resolution, render_y_resolution,
			0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1 + i, GL_TEXTURE_2D, colorBuffer[i], 0);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, colorBuffer[0], 0);

	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, render_x_resolution, render_y_resolution);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		CONSOLE_LN("Framebuffer not complete!");

	glDrawBuffers(3, attachments);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongBuffers);
	for (unsigned i = 0; i < 2; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, render_x_resolution, render_y_resolution, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffers[i], 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	FramebufferSetting gcbsetting;
	gcbsetting.HasColorAttachment = true;
	gcbsetting.ColorXResolution = render_x_resolution;
	gcbsetting.ColorYResolution = render_y_resolution;
	GammaCorrectionBuffer.Setup(gcbsetting);

	setMSAA(2);


	// test
	glGenTextures(1, &testTexture);
	glBindTexture(GL_TEXTURE_2D, testTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, render_x_resolution, render_y_resolution, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// VSYNC
	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
	TOOLS::Assert(wglSwapIntervalEXT, "Could not find proc process!");
	wglSwapIntervalEXT(false);
}

void GraphicsEngine::CalculateGammaCorrection()
{
	GammaCorrectionBuffer.Use();
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	GammaCorrectionBuffer.Unuse();

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GammaCorrectionBuffer.GetID());
	glBlitFramebuffer(0, 0, render_x_resolution, render_y_resolution,
		0, 0, render_x_resolution, render_y_resolution, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//extract light
	auto& shader = m_shaderManager->Get(S_GAMMA);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader.Start();
	shader.setInt1i("screen_image", 0);
	shader.setFloat("Gamma", scenes[0].camera->gamma);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GammaCorrectionBuffer.GetTexture());
	renderScreen();
	shader.End();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, testTexture);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, render_x_resolution, render_y_resolution, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GraphicsEngine::Draw(SceneData& scene)
{
	glEnable(GL_DEPTH_TEST);

 	if (scene.lines.size())
	{
		auto& shader = m_shaderManager->Get(S_LINE);
		glBindBuffer(GL_ARRAY_BUFFER, lineBuffer[0]);
		glBufferData(GL_ARRAY_BUFFER, scene.lines.size() * sizeof(Vector3), &scene.lines[0].x, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, lineBuffer[1]);
		glBufferData(GL_ARRAY_BUFFER, scene.lineColors.size() * sizeof(Vector4), &scene.lineColors[0].x, GL_DYNAMIC_DRAW);

		shader.Start();
		shader.setMat4fv("model", 1, Matrix4x4());
		shader.setMat4fv("projView", 1, scene.camera->GetPV());
		GLuint _indices[2] = { 0, 1 };
		glBindVertexArray(lineID);
		glDrawArrays(GL_LINES, 0, (GLsizei)scene.lines.size());
		glBindVertexArray(0);
		shader.End();
	}

	// render all opaque models
	for (auto& item : scene.modelDetails)
	{
		if (item.opacity != 1.0f) continue;
		if (item.always_front) continue;
		RenderModel(item, scene);
	}

	// render all translucent models
	for (auto& item : scene.modelDetails)
	{
		if (item.opacity == 1.0f) continue;
		if (item.always_front) continue;
		//if (item.type == MODELTYPE::PARTICLE) continue;
		RenderModel(item, scene);
	}
	for (auto& item : scene.particleDetails)
	{
		RenderParticle(std::move(item), scene);
	}
	for (auto&item : scene.spriteDetails)
	{
		if (!item.always_front)
		{
			GFXSetTransformation(item.transformation);
			RHSmesh = GetStaticMesh("CompiledAssets/Models/skybox_plane.mdl");
			RHtexture = GetTexture(item.texture);
			SpriteDraw(RHSmesh, item, scene);
		}
	}

  // render all particles

  // turn off depth test for UI, so it is on top of everything
  glDisable(GL_DEPTH_TEST);
  for (auto&item : scene.spriteDetails)
  {
	  if (item.always_front)
	  {
		  GFXSetTransformation(item.transformation);
		  RHSmesh = GetStaticMesh("CompiledAssets/Models/skybox_plane.mdl");
		  RHtexture = GetTexture(item.texture);
		  SpriteDraw(RHSmesh, item, scene);
	  }
  }
  
  // no need to enable gl_depth_test now, it will be enabled later

	//CalculateHDR();
	//EndHDRBuffer();
	scene.modelDetails.clear();
	scene.particleDetails.clear();
	scene.spriteDetails.clear();

	//render lines that go on top
	if (scene.topLines.size())
	{
		auto& shader = m_shaderManager->Get(S_LINE);
		glBindBuffer(GL_ARRAY_BUFFER, lineBuffer[0]);
		glBufferData(GL_ARRAY_BUFFER, scene.topLines.size() * sizeof(Vector3), &scene.topLines[0].x, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, lineBuffer[1]);
		glBufferData(GL_ARRAY_BUFFER, scene.topLineColors.size() * sizeof(Vector4), &scene.topLineColors[0].x, GL_DYNAMIC_DRAW);

		shader.Start();
		shader.setMat4fv("model", 1, Matrix4x4());
		shader.setMat4fv("projView", 1, scene.camera->GetPV());
		GLuint _indices[2] = { 0, 1 };
		glBindVertexArray(lineID);
		glDrawArrays(GL_LINES, 0, (GLsizei)scene.topLines.size());
		glBindVertexArray(0);
		shader.End();
	}
	glEnable(GL_DEPTH_TEST);
}
bool GraphicsEngine::ObjectsLoaded() const
{
	if (texLoader->loadingMesh.has_value()) return false;
	if (smeshLoader->loadingMesh.has_value()) return false;
	if (skmeshLoader->loadingMesh.has_value()) return false;
	return true;
}
void GraphicsEngine::RenderModel(const ModelDetails& item, const SceneData& scene)
{
	GFXSetTransformation(item.transformation);
	//GFXSetTexture(item.texture);
	GFXSetTextureUV(item.uv);
	GFXSetTransparency(item.opacity);
	GFXSetColorTint(item.tint);
	GFXSetAnimationFrame(item.row, item.column, item.frame);
  recv_shadow = item.recv_shadow;
	if (item.type == MODELTYPE::STATIC)
	{
		auto mesh = GetStaticMesh(item.model);
		if (FrustumCulling(item, scene, *mesh))
		{
			AssignMaterials(mesh, item.materials);
			MeshDraw(mesh, DM_TRIANGLE, scene);
		}
	}
	else if (item.type == MODELTYPE::SKELETAL)
	{
		auto sMesh = GetSkeletalMesh(item.model);
		if (FrustumCulling(item, scene, *sMesh))
		{
			AssignMaterials(sMesh, item.materials);
			if (sMesh->animations.size())
			{
				sMesh->animationNum = item.animNum;
				sMesh->calculate_transformation(item.animTime);
			}
			MeshDraw(sMesh, DM_TRIANGLE, scene);
		}
	}
}

void GraphicsEngine::RenderParticle(ParticleDetails && item, const SceneData & scene)
{
  GFXSetTransformation(item.transformation);
  GFXSetTexture(item.texture);
  RHSmesh = GetStaticMesh(item.model);
  RHtexture = GetTexture(item.texture);
  ParticleDraw(RHSmesh, std::move(item), scene);
}



// testing variables
float timer = 0.0f;
bool mouse_lock = true;
bool initial_frame = true;
Vector2 old_mouse_position;
float camera_speed = 1.0f;

void GraphicsEngine::Update()
{
	Internal_CheckCameraDirty();

	// testing console
	bool clear_color_test = true;
	bool draw_object_test = true;
	bool camera_orbit_test = false;
	bool camera_move_test = false;
	bool mouse_lock_test = false;
	bool free_camera_test = true;

	UNREFERENCED_PARAMETER(clear_color_test);
	UNREFERENCED_PARAMETER(draw_object_test);
	UNREFERENCED_PARAMETER(camera_orbit_test);
	UNREFERENCED_PARAMETER(camera_move_test);
	UNREFERENCED_PARAMETER(mouse_lock_test);
	UNREFERENCED_PARAMETER(free_camera_test);

	if (initial_frame)
		initial_frame = false;
	texLoader->Update();
	smeshLoader->Update();
	skmeshLoader->Update();

	// Draw Skybox first
	for (auto& elem : scenes)
		if (DrawSky)
			GFXDrawSky(elem);

	// Draw Scene
	for (auto& elem : scenes)
		Draw(elem);

	/*auto end = scenes.rend();
	for (auto iter = scenes.rbegin(); iter != end; ++iter)
		Draw(*iter);*/

	HandleMessages();
}

void GraphicsEngine::Exit()
{
	m_shaderManager->Clean();
	glDeleteTextures(1, &testTexture);
}

bool GraphicsEngine::InitializeRenderEnvironment()
{
	// Initialize Render Environment

	// get device context from current hwnd
	// A device context is a Windows data structure containing information 
	// about the drawing attributes of a device such as a display or a printer. 
	// All drawing calls are made through a device-context object, which 
	// encapsulates the Windows APIs for drawing lines, shapes, and text.
	m_windowDC = GetDC(m_window);

	// Check DEVMODE of the device
	// The DEVMODE data structure contains information about the initialization 
	// and environment of a printer or a display device.
	DEVMODE devmode = { 0 };

	// Specifies the size, in bytes, of the DEVMODE structure, not including 
	// any private driver-specific data that might follow the structure's public 
	// members. Set this member to sizeof (DEVMODE) to indicate the version of 
	// the DEVMODE structure being used.
	devmode.dmSize = sizeof(DEVMODE);

	// The EnumDisplayDevices function lets you obtain information about the 
	// display devices in the current session.
	// If the function succeeds, the return value is nonzero.
	// The function fails if iDevNum is greater than the largest device index. 
	BOOL b = EnumDisplaySettings(0, ENUM_CURRENT_SETTINGS, &devmode);
	if (!b)
		return false;

	// create a pixel format descriptor and allocate space
	// The PIXELFORMATDESCRIPTOR structure describes the pixel format of a drawing surface.
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_GENERIC_ACCELERATED | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = (BYTE)devmode.dmBitsPerPel;//32; //24 bit color for front and back buffer
	pfd.cDepthBits = 24;//24 bit depth buffer - not used in this demo
	pfd.cStencilBits = 8; //8 bit stencil buffer - not used in this demo

						  // checks if the graphics card can support the pixel format requested
	int pixelformat = ChoosePixelFormat(m_windowDC, &pfd);
	if (pixelformat == 0)
	{
		// The ReleaseDC function releases a device context (DC), freeing it for use by other applications.
		ReleaseDC(m_window, m_windowDC);
		return false;
	}

	// set the chosen pixel format to windows device context
	BOOL ok = SetPixelFormat(m_windowDC, pixelformat, &pfd);
	if (!ok)
	{
		ReleaseDC(m_window, m_windowDC);
		return false;
	}

	// set the OpenGL context
	// It represents the (potentially visible) default framebuffer that rendering 
	// commands will draw to when not drawing to a framebuffer object.
	// The wglCreateContext function creates a new OpenGL rendering context, 
	// which is suitable for drawing on the device referenced by hdc. The rendering 
	// context has the same pixel format as the device context.
	m_glDC = wglCreateContext(m_windowDC);
	if (!m_glDC)
	{
		ReleaseDC(m_window, m_windowDC);
		return false;
	}

	// The wglMakeCurrent function makes a specified OpenGL rendering context the 
	// calling thread's current rendering context. All subsequent OpenGL calls 
	// made by the thread are drawn on the device identified by hdc.
	ok = wglMakeCurrent(m_windowDC, m_glDC);
	if (!ok)
	{
		wglDeleteContext(m_glDC);
		ReleaseDC(m_window, m_windowDC);
		return false;
	}

	// Process :
	//    Get current device context 
	//    Create DEVMODE and see if there are available slot in this session
	//    Initialize DEVMODE
	//    Create a Pixel Format Descriptor
	//    Check if graphics card support PFD
	//    Set PFD to current window context
	//    Create GL context
	//    Make all GL draws in this thread draw to current HDC
	//    Done initializing
	return true;
}

void GraphicsEngine::CleanRenderingEnvironment()
{
	if (m_glDC)
	{
		if (!wglMakeCurrent(NULL, NULL))
		{
			//log
		}
	}

	if (!wglDeleteContext(m_glDC))
	{
		//log
	}
	m_glDC = NULL;

	if (m_windowDC && !ReleaseDC(m_window, m_windowDC))
	{
		m_windowDC = NULL;
	}
}

void GraphicsEngine::SwapBuffers()
{
	::SwapBuffers(m_windowDC);
}

void GraphicsEngine::GraphicsFrameStart()
{
  if (RenderShadow)
  {
	  CalculateShadows(scenes[0]);

   /* for (auto& item : scenes)
    {
		if (item.calculateShadows)
			 CalculateShadows(item.modelDetails); 
    }*/
  }

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	StartMainBuffer();
}

void GraphicsEngine::GraphicsFrameEnd()
{

	SwapBuffers();
}

void GraphicsEngine::RegisterLight(Light * l, size_t space)
{
	if (space < scenes.size())
	{
		auto& lights = scenes[space].lights;
		if (lights.HasSpace())
		{
			lights.mValue[lights.mCount++] = l;
		}
		else
		{
			CONSOLE_ERROR("Failed to register light ", l, ". Too many lights!");
		};
	}
}

void GraphicsEngine::UnregisterLight(Light * l, size_t space)
{
	if (space < scenes.size())
	{
		auto& lights = scenes[space].lights;
		if (!lights.Empty())
		{
			unsigned pos = lights.find(l);
			if (pos != static_cast<unsigned>(-1))
			{
				std::swap(lights.mValue[pos], lights.mValue[--lights.mCount]);
			}
		}
	}
}
void GraphicsEngine::ChangeScreenResolution(int x, int y)
{
	render_x_resolution = x;
	render_y_resolution = y;

	for (auto& item : scenes)
	{
		if (item.camera)
			item.camera->ChangeResolution(x, y);
	}
	MainDrawBuffer[0].ChangeAttachmentResolution(x, y);
	MainDrawBuffer[1].ChangeAttachmentResolution(x, y);
	MainDrawBuffer[2].ChangeAttachmentResolution(x, y);
	MainDrawBuffer[3].ChangeAttachmentResolution(x, y);

	// for (unsigned i = 0; i < Light_container::MAX_LIGHTS; ++i)
	// {
	// 	shadowBuffer[i].ChangeAttachmentResolution(x, y);
	// }

	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	glBindTexture(GL_TEXTURE_2D, hdrColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, render_x_resolution, render_y_resolution, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdrColorBuffer, 0);

	for (unsigned i = 0; i < 2; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, colorBuffer[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, render_x_resolution, render_y_resolution,
			0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1 + i, GL_TEXTURE_2D, colorBuffer[i], 0);
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, render_x_resolution, render_y_resolution);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		CONSOLE_LN("Framebuffer not complete!");

	unsigned attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	for (unsigned i = 0; i < 2; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, render_x_resolution, render_y_resolution, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffers[i], 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	GammaCorrectionBuffer.ChangeAttachmentResolution(x, y);

	glViewport(0, 0, x, y);
}

void GraphicsEngine::StartHDRBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[0]);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, render_x_resolution, render_y_resolution);
}
void GraphicsEngine::EndHDRBuffer()
{
	glDisable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GraphicsEngine::CalculateHDR()
{
	// glCopyImageSubData(currentBuffer->GetTexture(1), GL_TEXTURE_2D_MULTISAMPLE, 0, 0, 0, 0,
	// 	pingpongBuffers[0], GL_TEXTURE_2D, 0, 0, 0, 0,
	// 	render_x_resolution, render_y_resolution, 1);

  glBindFramebuffer(GL_READ_FRAMEBUFFER, currentBuffer->GetID());
  glReadBuffer(GL_COLOR_ATTACHMENT1);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pingpongFBO[0]);
  glDrawBuffer(GL_COLOR_ATTACHMENT0);
  glBlitFramebuffer(0, 0, render_x_resolution, render_y_resolution, 0, 0, render_x_resolution, render_y_resolution, GL_COLOR_BUFFER_BIT, GL_LINEAR);
  glBindFramebuffer(GL_FRAMEBUFFER, currentBuffer->GetID());
  glReadBuffer(GL_COLOR_ATTACHMENT0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  

  // ImGui::Begin("aha");
  // ImGui::Image((ImTextureID)pingpongBuffers[0], ImVec2(256, 256), ImVec2(0, 0), ImVec2(1, 1));
  // ImGui::End();


	//blur it
	auto& BlurShader = m_shaderManager->Get(S_BLUR);
	bool h = true;
	for (int i = 0; i < scenes[0].camera->bloomAmount; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[h]);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		BlurShader.Start();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, pingpongBuffers[!h]);
		BlurShader.setInt1i("horizontal", h);
		BlurShader.setInt1i("image", 0);
		renderScreen();
		h = !h;
		BlurShader.End();
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	auto& Shader = m_shaderManager->Get(S_HDR);
	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Shader.Start();
	glActiveTexture(GL_TEXTURE0);
	//Texture* test = texLoader->GetTexture("Resources\\Texture\\sky\\blood-stain-gorge_lf.dds");
	//glBindTexture(GL_TEXTURE_2D, test->GetID());
	glBindTexture(GL_TEXTURE_2D, hdrColorBuffer);
	//extra texture for shader
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, colorBuffer[0]);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, colorBuffer[1]);
	//print the real screen
	glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, pingpongBuffers[0]);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, pingpongBuffers[1]);
	Shader.setInt1i("hdrBuffer", 0);
	Shader.setInt1i("color1", h ? 3 : 4);
	Shader.setInt1i("color2", 2);
	//Shader.setInt1i("hdr", HdrEnabled);
	Camera* cam = scenes[mainLayer].camera;
	Shader.setInt1i("bloom", cam->bloom);
	Shader.setInt1i("hdr", cam->hdr);
	Shader.setFloat("exposure", cam->hdrExposure);
	renderScreen();
	Shader.End();
}

void GraphicsEngine::calculateShadowTextureStart(int index)
{
	shadowBuffer[index].Use();

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, 1024, 1024);
}

void GraphicsEngine::calculateShadowMeshDraw(StaticMesh* smesh, int i, const SceneData& scene)
{
	Shader& shader = m_shaderManager->Get(S_DEPTH);
	shader.Start();

	// texture?
	if (!transform_set)
	{
		model = trans * rotate * scale;
	}

	// render with texture shading only
	// texture shader

	Vector3 lightInvDir = scene.lights.mValue[i]->dir;
	lightInvDir.Normalize();
	lightInvDir *= 2000.0f;

	// Compute the MVP matrix from the light's point of view

	Matrix4x4 depthproj = Matrix4x4::Ortho(-4096.0f, 4096.0f, -4096.0f, 4096.0f, 5.0f, 7000.5f);
	Matrix4x4 depthview = Matrix4x4::LookAt(shadowCenter + Vector3(lightInvDir.x, lightInvDir.y, lightInvDir.z), shadowCenter, Vector3(0, 1, 0));
	depthMV = depthproj * depthview;


	for (size_t i0 = 0; i0 < smesh->meshes.size(); ++i0)
	{
		shader.setMat4fv("DepthMVP", 1, depthMV);
		shader.setMat4fv("Model", 1, model);


		Matrix4x4 biasMatrix(1.0f);

		Matrix4x4 depthBiasMVP = biasMatrix * depthMV * model;

		shader.setMat4fv("DepthBiasedMVP", 1, depthBiasMVP);

		Matrix4x4 m = Matrix4x4(
			model[0], model[1], model[2], model[3], model[4], model[5], model[6], model[7],
			model[8], model[9], model[10], model[11], model[12], model[13], model[14], model[15]
		);
		auto m2 = Matrix4x4::AffineInverse(m);


		/*glm::mat4 m = glm::mat4(
			model[0], model[1], model[2], model[3], model[4], model[5], model[6], model[7],
			model[8], model[9], model[10], model[11], model[12], model[13], model[14], model[15]
		);
		m = glm::inverse(m);
		Matrix4x4 m2 = Matrix4x4(
			m[0][0], m[1][0], m[2][0], m[3][0],
			m[0][1], m[1][1], m[2][1], m[3][1],
			m[0][2], m[1][2], m[2][2], m[3][2],
			m[0][3], m[1][3], m[2][3], m[3][3]);*/
		
		shader.setMat4fv("MVP", 1, scene.camera->GetPV() * model);
		shader.setMat4fv("World", 1, model);
		shader.setMat4fv("invWorld", 1, m2);// Matrix4x4::Inverse(model));
		shader.setFloat2fv("extrauv", 1, &texture_uv[0]);
		shader.setFloat4fv("color_tint", 1, &tint[0]);
		shader.setInt1i("gNumBones", 0);


		glBindVertexArray(smesh->meshes[i0].vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, smesh->meshes[i0].ebo);
		glBindBuffer(GL_ARRAY_BUFFER, smesh->meshes[i0].vbo);
		glDrawElements(GL_TRIANGLES, (GLsizei)smesh->meshes[i0].indices.size(), GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	// use default program
	shader.End();

	Internal_CleanUp();
}

void GraphicsEngine::calculateShadowMeshDraw(SkeletalMesh * smesh, int i, const SceneData& scene)
{
	Shader& shader = m_shaderManager->Get(S_DEPTH);
	shader.Start();

	// texture?
	if (!transform_set)
	{
		model = trans * rotate * scale;
	}

	// render with texture shading only
	// texture shader

	Vector3 lightInvDir = scene.lights.mValue[i]->dir;
	lightInvDir.Normalize();
	lightInvDir *= 2000.0f;

	// Compute the MVP matrix from the light's point of view


	Matrix4x4 depthproj = Matrix4x4::Ortho(-4096.0f, 4096.0f, -4096.0f, 4096.0f, 5.0f, 7000.5f);
	Matrix4x4 depthview = Matrix4x4::LookAt(shadowCenter + Vector3(lightInvDir.x, lightInvDir.y, lightInvDir.z), shadowCenter, Vector3(0, 1, 0));
	depthMV = depthproj * depthview;


	for (size_t i0 = 0; i0 < smesh->meshes.size(); ++i0)
	{
		shader.setMat4fv("DepthMVP", 1, depthMV);
		shader.setMat4fv("Model", 1, model);


		Matrix4x4 biasMatrix(1.0f);

		Matrix4x4 depthBiasVP = biasMatrix * depthMV;

		shader.setInt1i("gNumBones", (int)smesh->bones.size());
		if (!smesh->gBoneTransformations.empty())
			shader.setMat4fv("gBones", (int)smesh->gBoneTransformations.size(), smesh->gBoneTransformations[0]);

		glBindVertexArray(smesh->meshes[i0].vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, smesh->meshes[i0].ebo);
		glBindBuffer(GL_ARRAY_BUFFER, smesh->meshes[i0].vbo);
		glDrawElements(GL_TRIANGLES, (GLsizei)smesh->meshes[i0].indices.size(), GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	// use default program
	shader.End();

	Internal_CleanUp();
}

void GraphicsEngine::calculateShadowTextureEnd()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, render_x_resolution, render_y_resolution);
}

bool GraphicsEngine::FrustumCulling(const ModelDetails & item, const SceneData & scene, const StaticMesh& mesh) const
{
	auto pv = scene.camera->GetPV() * item.transformation;
	std::array<Vector4, 8> points;
	points[0] = pv * Vector4{ mesh.min.x, mesh.min.y, mesh.min.z, 1 };
	points[1] = pv * Vector4{ mesh.max.x, mesh.max.y, mesh.max.z, 1 };
	points[2] = pv * Vector4{ mesh.max.x, mesh.min.y, mesh.max.z, 1 };
	points[3] = pv * Vector4{ mesh.min.x, mesh.min.y, mesh.max.z, 1 };
	points[4] = pv * Vector4{ mesh.min.x, mesh.max.y, mesh.max.z, 1 };
	points[5] = pv * Vector4{ mesh.min.x, mesh.min.y, mesh.max.z, 1 };
	points[6] = pv * Vector4{ mesh.max.x, mesh.min.y, mesh.min.z, 1 };
	points[7] = pv * Vector4{ mesh.max.x, mesh.max.y, mesh.min.z, 1 };
	Vector3 min = Vector3(points[0].x, points[0].y, points[0].z) / points[0].w;
	Vector3 max = Vector3(points[0].x, points[0].y, points[0].z) / points[0].w;
	for (const auto& pt : points)
	{
		Vector3 currPoint = Vector3(pt.x, pt.y, pt.z) / pt.w;
		if (min.x > currPoint.x) min.x = currPoint.x;
		if (min.y > currPoint.y) min.y = currPoint.y;
		if (min.z > currPoint.z) min.z = currPoint.z;
		if (max.x < currPoint.x) max.x = currPoint.x;
		if (max.y < currPoint.y) max.y = currPoint.y;
		if (max.z < currPoint.z) max.z = currPoint.z;
	}
	Aabb meshNDC{ min, max };
	Aabb frustumNDC{ {-1,-1,-1}, {1,1,1} };
	return AabbAabb(meshNDC, frustumNDC);
}


bool GraphicsEngine::FrustumCulling(const ModelDetails & item, const SceneData & scene, const SkeletalMesh& mesh) const
{
	auto pv = scene.camera->GetPV() * item.transformation;
	std::array<Vector4, 8> points;
	points[0] = pv * Vector4{ mesh.min.x, mesh.min.y, mesh.min.z, 1 };
	points[1] = pv * Vector4{ mesh.max.x, mesh.max.y, mesh.max.z, 1 };
	points[2] = pv * Vector4{ mesh.max.x, mesh.min.y, mesh.max.z, 1 };
	points[3] = pv * Vector4{ mesh.min.x, mesh.min.y, mesh.max.z, 1 };
	points[4] = pv * Vector4{ mesh.min.x, mesh.max.y, mesh.max.z, 1 };
	points[5] = pv * Vector4{ mesh.min.x, mesh.min.y, mesh.max.z, 1 };
	points[6] = pv * Vector4{ mesh.max.x, mesh.min.y, mesh.min.z, 1 };
	points[7] = pv * Vector4{ mesh.max.x, mesh.max.y, mesh.min.z, 1 };
	Vector3 min = Vector3(points[0].x, points[0].y, points[0].z) / points[0].w;
	Vector3 max = Vector3(points[0].x, points[0].y, points[0].z) / points[0].w;
	for (const auto& pt : points)
	{
		Vector3 currPoint = Vector3(pt.x, pt.y, pt.z) / pt.w;
		if (min.x > currPoint.x) min.x = currPoint.x;
		if (min.y > currPoint.y) min.y = currPoint.y;
		if (min.z > currPoint.z) min.z = currPoint.z;
		if (max.x < currPoint.x) max.x = currPoint.x;
		if (max.y < currPoint.y) max.y = currPoint.y;
		if (max.z < currPoint.z) max.z = currPoint.z;
	}
	Aabb meshNDC{ min, max };
	Aabb frustumNDC{ { -1,-1,-1 },{ 1,1,1 } };
	return AabbAabb(meshNDC, frustumNDC);
}
void GraphicsEngine::StartMainBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, currentBuffer->GetID());
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, render_x_resolution, render_y_resolution);
}

void GraphicsEngine::EndMainBuffer()
{
	glDisable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GraphicsEngine::MeshStart()
{
	// Assert(m_currentMesh == nullptr, "GraphicsEngine.cpp : don't start mesh twice without end mesh");
	internal_mesh = std::make_unique<Mesh>();

	/*
	Previous code was:
	internal_mesh = new Mesh();
	// where internal_mesh was a Mesh* [not std::unique_ptr<Mesh>]
	*/
}

void GraphicsEngine::MeshAddTriangle(const Vertex& v1, const Vertex& v2, const Vertex& v3)
{
	UNREFERENCED_PARAMETER(v1);
	UNREFERENCED_PARAMETER(v2);
	UNREFERENCED_PARAMETER(v3);
	// internal_mesh->Add3Vertex(std::move(v1), std::move(v2), std::move(v3));
}

std::unique_ptr<Mesh>&& GraphicsEngine::MeshEnd()
{
	std::unique_ptr<Mesh> tmp{ std::make_unique<Mesh>() };
	*tmp = *internal_mesh;
	tmp->AssignBuffers();
	internal_mesh.reset();
	return std::move(tmp);

	/*
	Previous code was:
	Mesh* tmp = internal_mesh;
	internal_mesh = nullptr;
	tmp->AssignBuffers();
	return tmp;
	*/
}

void GraphicsEngine::MeshDraw(Mesh * mesh, DrawMode dm, const SceneData& scene)
{
	UNREFERENCED_PARAMETER(dm);

	if (!transform_set)
	{
		model = trans * rotate * scale;
	}
	// use shader program
	if (dm == DM_TRIANGLE)
	{
		// render with texture shading only
		// texture shader

		Shader& shader = m_shaderManager->Get(S_TEXTURE);

		shader.Start();

		shader.setInt1i("diffuseMap", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, RHtexture->GetID());


		shader.setMat4fv("MVP", 1, scene.camera->GetPV() * model);
		shader.setFloat2fv("extrauv", 1, &texture_uv[0]);
		shader.setFloat4fv("color_tint", 1, &tint[0]);


		glBindVertexArray(mesh->vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
		glDrawElements(GL_TRIANGLES, (GLsizei)mesh->indices.size(), GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// use default program
		shader.End();
	}
	else if (dm == DM_WIREFRAME)
	{
		Shader& shader = m_shaderManager->Get(S_PLAIN);

		shader.Start();

		Internal_SetTransform(shader);
		shader.setFloat2fv("extrauv", 1, &texture_uv[0]);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glBindVertexArray(mesh->vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
		glDrawElements(GL_TRIANGLES, (GLsizei)mesh->indices.size(), GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// use default program
		shader.End();
	}


	Internal_CleanUp();
}

void GraphicsEngine::renderScreen()
{
	if (screenVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &screenVAO);
		glGenBuffers(1, &screenVBO);
		glBindVertexArray(screenVAO);
		glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(screenVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void GraphicsEngine::GFXPostProcessing()
{
	EndMainBuffer();
	// Now we have the scene rendered in the main draw buffer, now blit it to hdr buffer to post process



  glBindFramebuffer(GL_READ_FRAMEBUFFER, currentBuffer->GetID());
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, hdrFBO);
  glBlitFramebuffer(
    0, 0, render_x_resolution, render_y_resolution,
    0, 0, render_x_resolution, render_y_resolution,
    GL_COLOR_BUFFER_BIT, GL_NEAREST
  );
  glBindFramebuffer(GL_FRAMEBUFFER, 0);



	CalculateHDR();
	CalculateGammaCorrection();
}

void GraphicsEngine::ApplyBloom(Shader& blurshader, Shader& blendshader)
{
	// bind fbo as read / draw fbo
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, hdrFBO);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, hdrFBO);

	// bind source texture to color attachment
	glBindTexture(GL_TEXTURE_2D, hdrColorBuffer);
	glFramebufferTexture2D(GL_TEXTURE_2D, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdrColorBuffer, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	// bind destination texture to another color attachment
	glBindTexture(GL_TEXTURE_2D, colorBuffer[0]);
	glFramebufferTexture2D(GL_TEXTURE_2D, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, colorBuffer[0], 0);
	glReadBuffer(GL_COLOR_ATTACHMENT1);


	// specify source, destination drawing (sub)rectangles. 
	glBlitFramebuffer(0, 0, render_x_resolution, render_y_resolution,
		0, 0, render_x_resolution, render_y_resolution, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);


	// bind destination texture to another color attachment
	glBindTexture(GL_TEXTURE_2D, colorBuffer[1]);
	glFramebufferTexture2D(GL_TEXTURE_2D, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, colorBuffer[1], 0);
	glReadBuffer(GL_COLOR_ATTACHMENT2);


	// specify source, destination drawing (sub)rectangles. 
	glBlitFramebuffer(0, 0, render_x_resolution, render_y_resolution,
		0, 0, render_x_resolution, render_y_resolution, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	// release state
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	blurshader.Start();
	blurshader.setInt1i("image", 2);
	blendshader.Start();
	blendshader.setInt1i("scene", 2);
	blendshader.setInt1i("blur", 4);
	bool h = true, first_iteration = true;
	unsigned amount = 10;
	blurshader.Start();
	for (unsigned i = 0; i < amount; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[h]);
		blurshader.setInt1i("horizontal", h);
		glBindTexture(GL_TEXTURE_2D, first_iteration ?
			colorBuffer[1] : pingpongBuffers[!h]);
		//render?
		renderScreen();
		h = !h;
		if (first_iteration)
			first_iteration = false;
	}
	blurshader.End();
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	blendshader.Start();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorBuffer[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, pingpongBuffers[!h]);
	blendshader.setInt1i("bloom", 1);
	blendshader.setFloat("exposure", 1);
	renderScreen();
	blendshader.End();
	glBindTexture(GL_TEXTURE_2D, 0);
}


void GraphicsEngine::ApplyLighting(Shader& shader, const Mesh& mesh, const SceneData& scene) const
{
	UNREFERENCED_PARAMETER(mesh);
	GLuint uboindex = 9999999u;
	GLint ubosize = 0;
	uboindex = glGetUniformBlockIndex(shader.GetID(), "LightArray");
	if (uboindex == GL_INVALID_INDEX)
	{
		//TOOLS::CreateErrorMessageBox("LightArray could not be found in the shader!");
		return;
	}
	glGetActiveUniformBlockiv(shader.GetID(), uboindex, GL_UNIFORM_BLOCK_DATA_SIZE, &ubosize);

	const unsigned int buffer_size = Light_container::MAX_LIGHTS * sizeof(Light);
	unsigned buffer[buffer_size]{};

	char* bufferPtr = &(char&)buffer[0];
	for (unsigned i = 0; i < scene.lights.mCount; ++i)
	{
		memcpy(bufferPtr, &scene.lights.mValue[i]->pos[0], sizeof(Light));
		bufferPtr += sizeof(Light);
	}

	glBindBuffer(GL_UNIFORM_BUFFER, ubohandle);
	glBufferData(GL_UNIFORM_BUFFER, ubosize, buffer, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, uboindex, ubohandle);

	shader.setInt1i("num_lights", scene.lights.mCount);
	shader.setFloat3fv("Iemissive", 1, &Iemissive[0]);
	shader.setFloat3fv("camera_position", 1, &scene.camera->GetPosition()[0]);
	shader.setFloat("shininess", shininess);

	shader.setFloat("zNear", zNear);
	shader.setFloat("zFar", zFar);
	shader.setFloat3fv("gAmbient", 1, gAmbient);
	shader.setInt1i("gFogEnabled", scene.drawFog);
	shader.setFloat4fv("gFog", 1, &scene.fogColor.x);
	shader.setFloat("gFogDistance", gFogDistance);
}

void GraphicsEngine::ApplyMaterial(Shader & shader, const StaticMesh & mesh, int i)
{
	if (mesh.materials[i] == nullptr) return;
	const Material& matM = *mesh.materials[i];

	if (matM.HasDiffuse)
	{
		shader.setInt1i("m_diffuse", 5);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, matM.m_diffuse->GetID());
	}

	if (matM.HasSpecular)
	{
		shader.setInt1i("m_specular", 6);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, matM.m_specular->GetID());

		shader.setFloat("m_specular_power", matM.m_specular_power);
	}

	if (matM.HasNormal)
	{
		shader.setInt1i("m_normal", 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, matM.m_normal->GetID());

		shader.setFloat("m_normal_power", matM.m_normal_power);
	}

	if (matM.HasOpacity)
	{
		shader.setInt1i("m_opacity_mask", 3);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, matM.m_opacity_mask->GetID());
	}

  shader.setFloat4fv("m_emissive_clr", 1, &matM.m_emissive_clr.x);
  shader.setFloat("m_illumination_power", matM.m_illumination_power);


	//shader.setFloat4fv("m_tint", 1, &matM.m_tint[0]);
}

void GraphicsEngine::ApplyMaterial(Shader & shader, const SkeletalMesh & mesh, int i)
{
	if (mesh.materials[i] == nullptr) return;
	const Material& matM = *mesh.materials[i];

	if (matM.HasDiffuse)
	{
		shader.setInt1i("m_diffuse", 5);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, matM.m_diffuse->GetID());
	}

	if (matM.HasSpecular)
	{
		shader.setInt1i("m_specular", 6);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, matM.m_specular->GetID());

		shader.setFloat("m_specular_power", matM.m_specular_power);
	}

	if (matM.HasNormal)
	{
		shader.setInt1i("m_normal", 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, matM.m_normal->GetID());

		shader.setFloat("m_normal_power", matM.m_normal_power);
	}

	if (matM.HasOpacity)
	{
		shader.setInt1i("m_opacity_mask", 3);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, matM.m_opacity_mask->GetID());
	}

  shader.setFloat4fv("m_emissive_clr", 1, &matM.m_emissive_clr.x);
  shader.setFloat("m_illumination_power", matM.m_illumination_power);

	//shader.setFloat4fv("m_tint", 1, &matM.m_tint[0]);
}

void BindMesh(const Mesh& mesh)
{
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glDrawElements(GL_TRIANGLES, (GLsizei)mesh.indices.size(), GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void GraphicsEngine::MeshDraw(StaticMesh * smesh, DrawMode dm, const SceneData& scene)
{
	UNREFERENCED_PARAMETER(dm);
	// texture?
	if (!transform_set)
	{
		model = trans * rotate * scale * Matrix4x4::Scale({ SCALINGFACTOR, SCALINGFACTOR, SCALINGFACTOR });
	}

	// use shader program
	if (dm == DM_TRIANGLE)
	{
		// render with texture shading only
		// texture shader

		Shader& shader = m_shaderManager->Get(S_TEXTURE);

		shader.Start();
		for (size_t i = 0; i < smesh->meshes.size(); ++i)
		{
			/*shader.setInt1i("diffuseMap", 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture->GetID());
*/
			size_t dir_index = 1;
			for (; dir_index < Light_container::MAX_LIGHTS; ++dir_index)
			{
				if (shadowCalculated[dir_index])
				{
					shader.setInt1i("shadowMap", 1);
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, shadowBuffer[dir_index].GetTexture());

					break;
				}
			}

      shader.setInt1i("gSoftShadow", DrawSoftShadow);
      shader.setInt1i("ReceiveShadow", recv_shadow);

			Matrix4x4 biasMatrix(1.0f);

			Matrix4x4 depthBiasMVP = biasMatrix * depthMV * model;

			shader.setMat4fv("DepthBiasedMVP", 1, depthBiasMVP);
			shader.setMat4fv("MVP", 1, scene.camera->GetPV() * model);

			//shader.setMat4fv("MVP", 1, cam.GetPV() * model);
			shader.setMat4fv("World", 1, model);
			shader.setMat4fv("invWorld", 1, Matrix4x4::AffineInverse(model));
			shader.setFloat2fv("extrauv", 1, &texture_uv[0]);
			shader.setFloat("m_global_transparency", m_transparency);
			shader.setFloat4fv("m_tint", 1, &tint[0]);
			shader.setInt1i("gNumBones", 0);
			shader.setInt1i("DirectionalLightIndex", static_cast<int>(dir_index));
			shader.setInt2i("frameDimension", frameDimension[0], frameDimension[1]);
			shader.setInt1i("frameIndex", frameIndex);
			ApplyLighting(shader, smesh->meshes[i], scene);
			ApplyMaterial(shader, *smesh, i);

			BindMesh(smesh->meshes[i]);
		}

		// use default program
		shader.End();
	}
	else if (dm == DM_WIREFRAME)
	{
		Shader& shader = m_shaderManager->Get(S_PLAIN);

		shader.Start();
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		for (size_t i = 0; i < smesh->meshes.size(); ++i)
		{
			Internal_SetTransform(shader);
			shader.setFloat2fv("extrauv", 1, &texture_uv[0]);

			glBindVertexArray(smesh->meshes[i].vao);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, smesh->meshes[i].ebo);
			glBindBuffer(GL_ARRAY_BUFFER, smesh->meshes[i].vbo);
			glDrawElements(GL_TRIANGLES, (GLsizei)smesh->meshes[i].indices.size(), GL_UNSIGNED_INT, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}


		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// use default program
		shader.End();
	}

	Internal_CleanUp();
}

void GraphicsEngine::MeshDraw(SkeletalMesh * smesh, DrawMode dm, const SceneData& scene)
{
	UNREFERENCED_PARAMETER(dm);
	// texture?
	if (!transform_set)
	{
		model = trans * rotate * scale * Matrix4x4::Scale({ SCALINGFACTOR, SCALINGFACTOR, SCALINGFACTOR });
	}


	// use shader program
	if (dm == DM_TRIANGLE)
	{
		// render with texture shading only
		// texture shader

		Shader& shader = m_shaderManager->Get(S_TEXTURE);

		shader.Start();


			shader.setMat4fv("MVP", 1, scene.camera->GetPV() * model);
			shader.setMat4fv("World", 1, model);
			shader.setMat4fv("invWorld", 1, Matrix4x4::AffineInverse(model));
			shader.setFloat2fv("extrauv", 1, &texture_uv[0]);
			shader.setFloat4fv("m_tint", 1, &tint[0]);
			shader.setInt2i("frameDimension", frameDimension[0], frameDimension[1]);
			shader.setInt1i("frameIndex", frameIndex);

			shader.setInt1i("gSoftShadow", DrawSoftShadow);
			shader.setInt1i("ReceiveShadow", recv_shadow);

    shader.setInt1i("gNumBones", (int)smesh->bones.size());
    if (!smesh->gBoneTransformations.empty())
      shader.setMat4fv("gBones", (int)smesh->gBoneTransformations.size(), smesh->gBoneTransformations[0]);

		for (size_t i = 0; i < smesh->meshes.size(); ++i)
		{
			/*shader.setInt1i("diffuseMap", 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture->GetID());*/

			ApplyLighting(shader, smesh->meshes[i], scene);
			ApplyMaterial(shader, *smesh, i);
			glBindVertexArray(smesh->meshes[i].vao);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, smesh->meshes[i].ebo);
			glBindBuffer(GL_ARRAY_BUFFER, smesh->meshes[i].vbo);
			glDrawElements(GL_TRIANGLES, (GLsizei)smesh->meshes[i].indices.size(), GL_UNSIGNED_INT, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}

		// use default program
		shader.End();
	}
	else if (dm == DM_WIREFRAME)
	{
		Shader& shader = m_shaderManager->Get(S_PLAIN);

		shader.Start();
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		for (size_t i = 0; i < smesh->meshes.size(); ++i)
		{
			Internal_SetTransform(shader);
			shader.setFloat2fv("extrauv", 1, &texture_uv[0]);

			glBindVertexArray(smesh->meshes[i].vao);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, smesh->meshes[i].ebo);
			glBindBuffer(GL_ARRAY_BUFFER, smesh->meshes[i].vbo);
			glDrawElements(GL_LINES, (GLsizei)smesh->meshes[i].indices.size(), GL_UNSIGNED_INT, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}


		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// use default program
		shader.End();
	}


	Internal_CleanUp();
}
void GraphicsEngine::SpriteDraw(StaticMesh * smesh, const SpriteDetails& md, const SceneData & scene)
{
  if (!transform_set)
  {
    model = trans * rotate * scale * Matrix4x4::Scale({ SCALINGFACTOR, SCALINGFACTOR, SCALINGFACTOR });
  }

  float invscaling = 1.0f / SCALINGFACTOR;

  Shader& shader = m_shaderManager->Get(S_SPRITE2D);

  shader.Start();

  shader.setMat4fv("MVP", 1, scene.camera->GetPV() * model);
  shader.setMat4fv("World", 1, model);
  shader.setFloat("m_global_transparency", m_transparency);
  shader.setFloat4fv("m_tint", 1, &tint[0]);

  shader.setInt1i("Texture", 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, RHtexture->GetID());
  
  shader.setInt1i("Mode", static_cast<int>(md.SAtype));
  shader.setFloat("Value", md.SAdata.arr[0]);

  if (md.SAtype == SpriteAnimType::None)
  {
    shader.setFloat("startSize", 0);
    shader.setFloat("endSize", 1);
  }
  else if (md.SAtype == SpriteAnimType::Radio)
  {
    shader.setFloat("startSize", 0);
    shader.setFloat("endSize", 2.0f * PI);
  }
  else if (md.SAtype == SpriteAnimType::Horizontal)
  {
    shader.setFloat("startSize", RHSmesh->meshes[0].btmleftfront.x * invscaling);
    shader.setFloat("endSize", RHSmesh->meshes[0].toprightback.x * invscaling);
  }
  else if (md.SAtype == SpriteAnimType::Vertical)
  {
    shader.setFloat("startSize", RHSmesh->meshes[0].btmleftfront.y * invscaling);
    shader.setFloat("endSize", RHSmesh->meshes[0].toprightback.y * invscaling);
  }

  BindMesh(smesh->meshes[0]);

  // use default program
  shader.End();

  Internal_CleanUp();
}

void GraphicsEngine::ParticleDraw(StaticMesh * smesh, ParticleDetails&& md, const SceneData & scene)
{
	if (md.particles.size() == 0) return;
  if (!transform_set)
  {
    model = trans * rotate * scale * Matrix4x4::Scale({ SCALINGFACTOR, SCALINGFACTOR, SCALINGFACTOR });
  }
  // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  Shader& shader = m_shaderManager->Get(S_PARTICLE);
  {
    Vector3 camerapos = scene.camera->GetPosition();
    Vector3 camerafront = scene.camera->GetFront();

    std::sort(md.particles.begin(), md.particles.end(), [camerapos, camerafront](ParticleGL& l, ParticleGL& r) -> bool {
      Vector3 vecl = l.pos - camerapos;
      Vector3 vecr = r.pos - camerapos;
      
      return vecl * camerafront > vecr * camerafront;
    });

    glBindBuffer(GL_ARRAY_BUFFER, md.vbo);
    glBufferData(GL_ARRAY_BUFFER, 1024 * sizeof(ParticleGL), NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, md.particles.size() * sizeof(ParticleGL), &md.particles[0].pos.x);
  
    glBindBuffer(GL_ARRAY_BUFFER, RHSmesh->meshes[0].vbo);
  
    // set attribute for the first argument
    // the vertex shader accesses this position-buffer data using "layout (location = 0) in vec3 position;" shader code
    glVertexAttribPointer(0, FLOATS_PER_POSITION, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
  
    GLint stride = FLOATS_PER_POSITION;
    glEnableVertexAttribArray(1);
    //the vertex shader accesses this color-buffer data using "layout (location = 1) in vec4 color;" shader code
    glVertexAttribPointer(1, FLOATS_PER_NORMAL, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX * sizeof(GLfloat), (GLvoid*)(stride * sizeof(GLfloat)));
  
    glEnableVertexAttribArray(2);
    stride = FLOATS_PER_POSITION + FLOATS_PER_NORMAL;
    glVertexAttribPointer(2, FLOATS_PER_UV, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX * sizeof(GLfloat), (GLvoid*)(stride * sizeof(GLfloat)));
  
    glBindBuffer(GL_ARRAY_BUFFER, md.vbo);
    glEnableVertexAttribArray(3); // world position of each particle
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleGL), (GLvoid*)0);
  
    glEnableVertexAttribArray(4); // init size
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleGL), (GLvoid*)(3 * sizeof(GLfloat)));
  
    glEnableVertexAttribArray(5); // final size
    glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleGL), (GLvoid*)(4 * sizeof(GLfloat)));
  
    glEnableVertexAttribArray(6); // percentage
    glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleGL), (GLvoid*)(5 * sizeof(GLfloat)));
  
    glEnableVertexAttribArray(7); // init alpha
    glVertexAttribPointer(7, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleGL), (GLvoid*)(6 * sizeof(GLfloat)));
  
    glEnableVertexAttribArray(8); // final alpha
    glVertexAttribPointer(8, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleGL), (GLvoid*)(7 * sizeof(GLfloat)));
  
    glEnableVertexAttribArray(9); // rotation
    glVertexAttribPointer(9, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleGL), (GLvoid*)(8 * sizeof(GLfloat)));
  }
  
  
  
  glVertexAttribDivisor(0, 0);
  glVertexAttribDivisor(1, 0);
  glVertexAttribDivisor(2, 0);
  glVertexAttribDivisor(3, 1);
  glVertexAttribDivisor(4, 1);
  glVertexAttribDivisor(5, 1);
  glVertexAttribDivisor(6, 1);
  glVertexAttribDivisor(7, 1);
  glVertexAttribDivisor(8, 1);
  glVertexAttribDivisor(9, 1);
  

  shader.Start();

  shader.setMat4fv("VP", 1, scene.camera->GetPV());
  shader.setMat4fv("Model", 1, model);

  shader.setInt1i("diffuseMap", 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, RHtexture->GetID());

  Vector4 blank = Vector4(0, 0, 0, 1);

  shader.setInt1i("emissive", md.emissive);

  glBindBuffer(GL_ARRAY_BUFFER, RHSmesh->meshes[0].vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RHSmesh->meshes[0].ebo);
  glDrawElementsInstanced(GL_TRIANGLES, RHSmesh->meshes[0].indices.size(), GL_UNSIGNED_INT, 0, md.particles.size());
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);

  // use default program
  shader.End();

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
  glDisableVertexAttribArray(3);
  glDisableVertexAttribArray(4);
  glDisableVertexAttribArray(5);
  glDisableVertexAttribArray(6);
  glDisableVertexAttribArray(7);
  glDisableVertexAttribArray(8);
  glDisableVertexAttribArray(9);
  
  glBindVertexArray(0);
  
  
  Internal_CleanUp();
}

void GraphicsEngine::GFXSetTransformation(const Matrix4x4 & matT)
{
	model = matT * Matrix4x4::Scale({ SCALINGFACTOR, SCALINGFACTOR, SCALINGFACTOR });
	transform_set = true;
}

void GraphicsEngine::GFXSetPosition(Vector3 pos)
{
	GFXSetPosition(pos.x, pos.y, pos.z);
}

void GraphicsEngine::GFXSetPosition(float x, float y, float z)
{
	trans = Matrix4x4(
		Vector4(1, 0, 0, 0),
		Vector4(0, 1, 0, 0),
		Vector4(0, 0, 1, 0),
		Vector4(x, y, z, 1));
}

void GraphicsEngine::GFXSetRotation(float r)
{
	rotate = Matrix4x4(
		Vector4(cosf(r), -sinf(r), 0, 0),
		Vector4(sinf(r), cosf(r), 0, 0),
		Vector4(0, 0, 1, 0),
		Vector4(0, 0, 0, 1));
}

void GraphicsEngine::GFXSetScale(Vector3 s)
{
	GFXSetScale(s.x, s.y, s.z);
}

void GraphicsEngine::GFXSetScale(float sx, float sy, float sz)
{
	scale = Matrix4x4(
		Vector4(sx, 0, 0, 0),
		Vector4(0, sy, 0, 0),
		Vector4(0, 0, sz, 0),
		Vector4(0, 0, 0, 1));
}

void GraphicsEngine::UnloadTexture(Texture * tex)
{
	GLuint id = tex->GetID();
	glDeleteTextures(1, &id);
}


void GraphicsEngine::unload_model(Mesh * m)
{
	if (m)
	{
		m->UnassignBuffers();
	}
}

void GraphicsEngine::unload_model(StaticMesh * m)
{
	if (m)
	{
		for (auto& elem : m->meshes)
			elem.UnassignBuffers();
	}
}

void GraphicsEngine::unload_model(SkeletalMesh * m)
{
	if (m)
	{
		for (auto& elem : m->meshes)
			elem.UnassignBuffers();
	}
}

void GraphicsEngine::GFXSetTexture(Texture * tex)
{
	RHtexture = tex;
}

void GraphicsEngine::GFXSetTexture(const std::string & tex)
{
	RHtexture = GetTexture(tex);
}

void GraphicsEngine::GFXSetTextureUV(float u, float v)
{
	texture_uv = Vector2(u, v);
}

void GraphicsEngine::GFXSetTransparency(float f)
{
	m_transparency = f;
}

void GraphicsEngine::GFXSetColorTint(Vector4 _tint)
{
	this->tint = _tint;
}

void GraphicsEngine::GFXSetShininess(float s)
{
	shininess = s;
}

void GraphicsEngine::GFXSetAnimationFrame(int dimx, int dimy, int frameindex)
{
	frameDimension[0] = dimx;
	frameDimension[1] = dimy;
	frameIndex = frameindex;
}

void GraphicsEngine::GFXSetSkyModel(StaticMesh * m)
{
	m_sky_model = m;
}

void GraphicsEngine::GFXSetSkyTexture(Texture * t)
{
	m_sky_texture[0] = t;
}

void GraphicsEngine::GFXDrawSky(SceneData& scene)
{
	// Do not draw any skyboxes if the boolean is not toggled
	if (!scene.drawSkybox)
		return;
	GFXSetTextureUV(0, 0);
	GFXSetAnimationFrame(1, 1, 1);
	Shader& skybox_shader = m_shaderManager->Get(S_SKYBOX);
	GLuint programid = skybox_shader.GetID();
	glDisable(GL_DEPTH_TEST);

	skybox_shader.Start();

	// Uniform transformation (vertex shader)
	GLint vTransformLoc = glGetUniformLocation(programid, "Model");
	GLint vMVPLoc = glGetUniformLocation(programid, "MVP");


	skybox_shader.setInt1i("sky_lt", 0);
	skybox_shader.setInt1i("sky_rt", 1);
	skybox_shader.setInt1i("sky_dn", 2);
	skybox_shader.setInt1i("sky_up", 3);
	skybox_shader.setInt1i("sky_bk", 4);
	skybox_shader.setInt1i("sky_ft", 5);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_sky_texture[0]->GetID());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_sky_texture[1]->GetID());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_sky_texture[2]->GetID());
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_sky_texture[3]->GetID());
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_sky_texture[4]->GetID());
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, m_sky_texture[5]->GetID());

	Matrix4x4 pv = scene.camera->GetPV();

	// front
	{
		Matrix4x4 _trans = Matrix4x4(
			20.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 20.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 20.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
		_trans[12] = scene.camera->GetPosition().x;
		_trans[13] = scene.camera->GetPosition().y;
		_trans[14] = scene.camera->GetPosition().z - 10.0f;

		Matrix4x4 _mvp = pv * _trans;

		glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &(_trans[0]));
		glUniformMatrix4fv(vMVPLoc, 1, GL_FALSE, &(_mvp[0]));
		skybox_shader.setMat4fv("InvModel", 1, Matrix4x4::Inverse(_trans));

		glBindVertexArray(m_sky_model->meshes[0].vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_sky_model->meshes[0].vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_sky_model->meshes[0].ebo);
		glDrawElements(GL_TRIANGLES, static_cast<int>(m_sky_model->meshes[0].indices.size()), GL_UNSIGNED_INT, 0); // 3 indices starting at 0 -> 1 triangle
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	{ // left
		Matrix4x4 _trans = Matrix4x4(
			20.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 20.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 20.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		//TODO:: change to our rotate
		_trans = Matrix4x4::Rotate((Matrix4x4)_trans, PI * 0.5f, Vector3(0.0f, 1.0f, 0.0f));
		_trans[12] = scene.camera->GetPosition().x - 10.0f;
		_trans[13] = scene.camera->GetPosition().y;
		_trans[14] = scene.camera->GetPosition().z;

		Matrix4x4 _mvp = pv * _trans;

		glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &(_trans[0]));
		glUniformMatrix4fv(vMVPLoc, 1, GL_FALSE, &(_mvp[0]));
		skybox_shader.setMat4fv("InvModel", 1, Matrix4x4::Transpose(Matrix4x4::Inverse(_trans)));

		glBindVertexArray(m_sky_model->meshes[0].vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_sky_model->meshes[0].vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_sky_model->meshes[0].ebo);
		glDrawElements(GL_TRIANGLES, static_cast<int>(m_sky_model->meshes[0].indices.size()), GL_UNSIGNED_INT, 0); // 3 indices starting at 0 -> 1 triangle
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	{ // back
		Matrix4x4 _trans = Matrix4x4(
			20.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 20.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 20.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		//TODO:: change to our rotate
		_trans = Matrix4x4::Rotate((Matrix4x4)_trans, PI, Vector3(0.0f, 1.0f, 0.0f));
		_trans[12] = scene.camera->GetPosition().x;
		_trans[13] = scene.camera->GetPosition().y;
		_trans[14] = scene.camera->GetPosition().z + 10.0f;

		Matrix4x4 _mvp = pv * _trans;

		glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &(_trans[0]));
		glUniformMatrix4fv(vMVPLoc, 1, GL_FALSE, &(_mvp[0]));
		skybox_shader.setMat4fv("InvModel", 1, Matrix4x4::Transpose(Matrix4x4::Inverse(_trans)));

		glBindVertexArray(m_sky_model->meshes[0].vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_sky_model->meshes[0].vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_sky_model->meshes[0].ebo);
		glDrawElements(GL_TRIANGLES, static_cast<int>(m_sky_model->meshes[0].indices.size()), GL_UNSIGNED_INT, 0); // 3 indices starting at 0 -> 1 triangle
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	{ // back
		Matrix4x4 _trans = Matrix4x4(
			20.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 20.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 20.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		//TODO:: change to our rotate
		_trans = Matrix4x4::Rotate((Matrix4x4)_trans, -PI * 0.5f, Vector3(0.0f, 1.0f, 0.0f));
		_trans[12] = scene.camera->GetPosition().x + 10.0f;
		_trans[13] = scene.camera->GetPosition().y;
		_trans[14] = scene.camera->GetPosition().z;

		Matrix4x4 _mvp = pv * _trans;

		glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &(_trans[0]));
		glUniformMatrix4fv(vMVPLoc, 1, GL_FALSE, &(_mvp[0]));
		skybox_shader.setMat4fv("InvModel", 1, Matrix4x4::Transpose(Matrix4x4::Inverse(_trans)));

		glBindVertexArray(m_sky_model->meshes[0].vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_sky_model->meshes[0].vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_sky_model->meshes[0].ebo);
		glDrawElements(GL_TRIANGLES, static_cast<int>(m_sky_model->meshes[0].indices.size()), GL_UNSIGNED_INT, 0); // 3 indices starting at 0 -> 1 triangle
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	{ // up
		Matrix4x4 _trans = Matrix4x4(
			20.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 20.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 20.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		//TODO:: change to our rotate
		_trans = Matrix4x4::Rotate((Matrix4x4)_trans, PI * 0.5f, Vector3(1.0f, 0.0f, 0.0f));
		_trans[12] = scene.camera->GetPosition().x;
		_trans[13] = scene.camera->GetPosition().y + 10.0f;
		_trans[14] = scene.camera->GetPosition().z;

		Matrix4x4 _mvp = pv * _trans;

		glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &(_trans[0]));
		glUniformMatrix4fv(vMVPLoc, 1, GL_FALSE, &(_mvp[0]));
		skybox_shader.setMat4fv("InvModel", 1, Matrix4x4::Transpose(Matrix4x4::Inverse(_trans)));

		glBindVertexArray(m_sky_model->meshes[0].vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_sky_model->meshes[0].vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_sky_model->meshes[0].ebo);
		glDrawElements(GL_TRIANGLES, static_cast<int>(m_sky_model->meshes[0].indices.size()), GL_UNSIGNED_INT, 0); // 3 indices starting at 0 -> 1 triangle
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	{ // down
		Matrix4x4 _trans = Matrix4x4(
			20.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 20.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 20.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		//TODO:: change to our rotate
		_trans = Matrix4x4::Rotate((Matrix4x4)_trans, -PI * 0.5f, Vector3(1.0f, 0.0f, 0.0f));
		_trans[12] = scene.camera->GetPosition().x;
		_trans[13] = scene.camera->GetPosition().y - 10.0f;
		_trans[14] = scene.camera->GetPosition().z;

		Matrix4x4 _mvp = pv * _trans;

		glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &(_trans[0]));
		glUniformMatrix4fv(vMVPLoc, 1, GL_FALSE, &(_mvp[0]));
		skybox_shader.setMat4fv("InvModel", 1, Matrix4x4::Transpose(Matrix4x4::Inverse(_trans)));

		glBindVertexArray(m_sky_model->meshes[0].vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_sky_model->meshes[0].vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_sky_model->meshes[0].ebo);
		glDrawElements(GL_TRIANGLES, static_cast<int>(m_sky_model->meshes[0].indices.size()), GL_UNSIGNED_INT, 0); // 3 indices starting at 0 -> 1 triangle
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	glUseProgram(0);

	Internal_CleanUp();
	glEnable(GL_DEPTH_TEST);
}

Texture* GraphicsEngine::GetTexture(const std::string& path)
{
	return texLoader->GetTexture(path);
}
StaticMesh* GraphicsEngine::GetStaticMesh(const std::string& path)
{
	return &smeshLoader->GetMesh(path);
}

SkeletalMesh* GraphicsEngine::GetSkeletalMesh(const std::string& path)
{
	return &skmeshLoader->GetMesh(path);
}

void GraphicsEngine::setMSAA(int amount)
{
	switch (amount)
	{
	case 1:
		currentBuffer = &MainDrawBuffer[0]; // NO MSAA
		break;
	case 2:
		currentBuffer = &MainDrawBuffer[1]; // MSAA 2x
		break;
	case 4:
		currentBuffer = &MainDrawBuffer[2]; // MSAA 4x
		break;
	case 8:
		currentBuffer = &MainDrawBuffer[3]; // MSAA 8x
		break;
	default:
		CONSOLE_LN("Selected MSAA Setting not supported");
		currentBuffer = &MainDrawBuffer[0]; // NO MSAA
		break;
	}
}

void GraphicsEngine::Internal_CheckCameraDirty()
{
	for (auto& item : scenes)
		if(item.camera)
			item.camera->CheckRecalculate();
}

void GraphicsEngine::Internal_SetTransform(Shader & shader)
{
	UNREFERENCED_PARAMETER(shader);
}

void GraphicsEngine::Internal_CleanUp()
{
	glBindTexture(GL_TEXTURE_2D, 0);
	transform_set = false;
	material_set = false;
	m_transparency = 1.0f;
  cast_shadow = true;
  recv_shadow = true;
  RHSmesh = nullptr;
  RHtexture = nullptr;
  memset(&tint, 0, 4 * sizeof(float));

	frameDimension[0] = 0;
	frameDimension[1] = 0;
	frameIndex = 0;
}

void GraphicsEngine::DrawLine(const LineDetails& details, int sceneNum)
{
	scenes[sceneNum].lines.emplace_back(details.p0);
	scenes[sceneNum].lines.emplace_back(details.p1);
	scenes[sceneNum].lineColors.emplace_back(details.color);
	scenes[sceneNum].lineColors.emplace_back(details.color);
}
void GraphicsEngine::DrawTopLine(const LineDetails& details, int sceneNum)
{
	scenes[sceneNum].topLines.emplace_back(details.p0);
	scenes[sceneNum].topLines.emplace_back(details.p1);
	scenes[sceneNum].topLineColors.emplace_back(details.color);
	scenes[sceneNum].topLineColors.emplace_back(details.color);
}

void GraphicsEngine::CalculateShadows(const SceneData& space)
{
	glEnable(GL_DEPTH_TEST);
	for (unsigned i = 0; i < space.lights.mCount; ++i)
	{
		if (space.lights.mValue[i]->light_type == DIRECTIONAL_LIGHT)
		{
			calculateShadowTextureStart(i);

			for (const auto & item : space.modelDetails)
			{
        // check whether it will cast shadow
        if (!item.cast_shadow) continue;

        // set render status
				GFX_S.GFXSetTransformation(item.transformation);
				GFX_S.GFXSetTextureUV(item.uv);
				GFX_S.GFXSetTransparency(item.opacity);
				GFX_S.GFXSetColorTint(item.tint);
				if (item.type == MODELTYPE::STATIC)
				{
					auto mesh = GetStaticMesh(item.model);
					AssignMaterials(mesh, item.materials);
					calculateShadowMeshDraw(mesh, i, space);
				}
				else if (item.type == MODELTYPE::SKELETAL)
				{
					auto mesh = GetSkeletalMesh(item.model);
					AssignMaterials(mesh, item.materials);
					calculateShadowMeshDraw(mesh, i, space);
				}
			}

			calculateShadowTextureEnd();
			shadowCalculated[i] = true;
			break;
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
}

void GraphicsEngine::ClearShadowMaps()
{
  for (unsigned i = 0; i < Light_container::MAX_LIGHTS; ++i)
  {
    shadowBuffer[i].Use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shadowBuffer[i].Unuse();
  }
}

void GraphicsEngine::UpdateModelList()
{
	compiledModels.clear();
	modelList.clear();
	for (auto & p : std::experimental::filesystem::directory_iterator("CompiledAssets\\Models"))
		compiledModels.emplace_back(p.path().string());
	// Push everything but the first value
	for (unsigned i = 0; i < compiledModels.size(); ++i)
		modelList.push_back(compiledModels[i].data());

}

void GraphicsEngine::UpdateTextureList()
{
	compiledTextures.clear();
	textureList.clear();
	for (auto & p : std::experimental::filesystem::directory_iterator("CompiledAssets\\Sprites"))
		compiledTextures.emplace_back(p.path().string());
	// Push everything but the first value
	for (unsigned i = 0; i < compiledTextures.size(); ++i)
		textureList.push_back(compiledTextures[i].data());
}

void GraphicsEngine::UpdateMaterialList()
{
	std::string path = MATERIAL_DIRECTORY;
	materials.clear();
	materialList.clear();
	materialData.clear();
	for (const auto &p : fs::directory_iterator(path))
	{
		auto fileName = TOOLS::ExtractFileName(p.path().string());
		materials.emplace_back(fileName);
		Serializer ss;
		ss.LoadFile(p.path().string());
		auto root = ss.FirstChild();
		if (root == nullptr)
		{
			continue;
		}
		materialData[fileName].ConvertSerializedData(*root->FirstChildElement());
		materialData[fileName].SetupTextures();
	}
	for (unsigned i = 0; i < materials.size(); ++i)
		materialList.push_back(materials[i].data());
}

void GraphicsEngine::DisplayWindow()
{
	ImGui::Text("Draw Skybox");
	ImGui::SameLine();
	ImGui::Checkbox("Draw Skybox", &DrawSky);


  ImGui::Text("Render Shadow");
  ImGui::SameLine();
  if (ImGui::Checkbox("Render Shadow", &RenderShadow))
  {
    ClearShadowMaps();
  }

  
	ImGui::Text("Draw Soft Shadow");
	ImGui::SameLine();
	ImGui::Checkbox("Draw Soft Shadow", &DrawSoftShadow);

	ImGui::Text("Draw Fog");
	ImGui::SameLine();
	bool oldFog = DrawFog;
	ImGui::Checkbox("Draw Fog", &DrawFog);
	if (oldFog != DrawFog)
	{
		for (auto& item : scenes)
			item.drawFog = DrawFog;
	}

	ImGui::Text("HDR Enabled");
	ImGui::SameLine();
	ImGui::Checkbox("HDR Enabled", &HdrEnabled);
	ImGui::Text("Gamma Correction");
	ImGui::SliderFloat("Gamma Correction", &gamma, 0.5f, 3.0f);


  ImGui::Text("Shadow Projection Center");
  ImGui::InputFloat3("input_shadow_float_3", &shadowCenter.x, 10.0f);

	ImGui::Separator();

  ImGui::Text("Emissive color buffer");
  ImGui::Image((ImTextureID)currentBuffer->GetTexture(1), ImVec2{ 256, 256 }, ImVec2{ 0,1 }, ImVec2{ 1,0 });
  ImGui::Separator();

	for (size_t i = 0; i != Light_container::MAX_LIGHTS; ++i)
	{
		if (shadowCalculated[i])
			ImGui::Image((ImTextureID)shadowBuffer[i].GetTexture(), ImVec2{ 256, 256 }, ImVec2{ 0,1 }, ImVec2{ 1,0 });
	}

	ImGui::Separator();
	ImGui::Text("Before Gamma Correction");
	ImGui::Image((ImTextureID)GammaCorrectionBuffer.GetTexture(), ImVec2{ 256, 256 }, ImVec2{ 0,1 }, ImVec2{ 1,0 });
	ImGui::Separator();
	ImGui::Text("Ping Pong");
	ImGui::Image((ImTextureID)pingpongBuffers[1], ImVec2{ 256, 256 }, ImVec2{ 0,1 }, ImVec2{ 1,0 });
}

void GraphicsEngine::ClearDebug()
{
	for (auto& item : scenes)
	{
		item.lines.clear();
		item.lineColors.clear();
		item.topLines.clear();
		item.topLineColors.clear();
	}
}

void GraphicsEngine::HandleMessages()
{
	ClearDebug();
	bool updateModelList = false;
	bool updateTextureList = false;
	bool updateMaterialList = false;
	auto messages = messageQueue->ReadMessages();
	for (const auto& item : messages)
	{
		switch (item.type)
		{
		case GFXMSG::DRAWLINE:
			DrawLine(std::get<LineDetails>(item.data), item.spaceNum);
			break;
		case GFXMSG::DRAWTOPLINE:
			DrawTopLine(std::get<LineDetails>(item.data), item.spaceNum);
			break;
		case GFXMSG::DRAWMODEL:
			if(scenes.size() > item.spaceNum)
				scenes[item.spaceNum].modelDetails.emplace_back(std::move(std::get<ModelDetails>(item.data)));
			break;
		case GFXMSG::DRAWPARTICLES:
			if (scenes.size() > item.spaceNum)
				scenes[item.spaceNum].particleDetails.emplace_back(std::move(std::get<ParticleDetails>(item.data)));
			break;
		case GFXMSG::DRAWSPRITE:
			if (scenes.size() > item.spaceNum)
				scenes[item.spaceNum].spriteDetails.emplace_back(std::move(std::get<SpriteDetails>(item.data)));
			break;
		case GFXMSG::RECOMPILESHADERS:
			m_shaderManager->Recompile();
			break;
		case GFXMSG::UPDATEMODELLIST:
			updateModelList = true;
			break;
		case GFXMSG::UPDATETEXTURELIST:
			updateTextureList = true;
			break;
		case GFXMSG::UPDATEMATERIALLIST:
			updateMaterialList = true;
			break;
		case GFXMSG::REMOVEMATERIAL:
			updateMaterialList = true;
			break;
		case GFXMSG::COMPILEDFILE:
		{
			auto& modelDetails = std::get<ModelDetails>(item.data);
			updateTextureList = true;
			updateModelList = true;
			if (modelDetails.model.size())
			{
				smeshLoader->RecompileFile(modelDetails.model);
				skmeshLoader->RecompileFile(modelDetails.model);
			}
			if (modelDetails.texture.size())
				texLoader->RecompileFile(modelDetails.texture);
			break;
		}
		case GFXMSG::UPDATESCENESETTINGS:
		{
			auto settings = std::get<LevelSettings*>(item.data);
			scenes[item.spaceNum].drawSkybox = settings->renderSkybox;
			if (scenes[item.spaceNum].drawSkybox)
			{
				for (unsigned i = 0; i < 6; ++i)
					m_sky_texture[i] = GetTexture(settings->skyboxTextures[i]);
			}
			scenes[item.spaceNum].drawFog = settings->enableFog;
			scenes[item.spaceNum].fogColor = settings->fogColor;
			gFogDistance = settings->fogDistance;
			break;
		}
		}
	}
	if (updateTextureList)
		UpdateTextureList();
	if (updateModelList)
		UpdateModelList();
	if (updateMaterialList)
		UpdateMaterialList();
}

void GraphicsEngine::SetupSceneData(const std::vector<GameSpacesInfo>& allGameSpaces)
{
	scenes.clear();
	size_t size = allGameSpaces.size();
	scenes.resize(size);
	for (unsigned i = 0; i != size; ++i)
	{
		// Assign all of the camera
		GameSpace& current = *allGameSpaces[i].gamespace.get();
		scenes[i].camera = current.camera;
		// scenes[i].drawSkybox = current.entity_list->settings.renderSkybox;
	}
}

void GraphicsEngine::SetupSceneData(const std::vector<std::unique_ptr<GameSpace>>& allGameSpaces)
{
	//scenes.clear();
	size_t size = allGameSpaces.size();
	scenes.resize(size + 1);
	for (unsigned i = 1; i < scenes.size(); ++i)
	{
		// Assign all of the camera
		GameSpace& current = *allGameSpaces[i - 1].get();
		scenes[i].camera = current.camera;
		// scenes[i].drawSkybox = current.entity_list->settings.renderSkybox;
	}
}



void internal_gen_vertex_normal(Mesh & m, GLuint p0, GLuint p1, GLuint p2)
{
	std::vector<Vertex>& vert = m.vertices;
	auto& vertnorm = m.vertexnormal;
	Vector3 p0p1 = Vector3(vert[p1].position[0] - vert[p0].position[0],
		vert[p1].position[1] - vert[p0].position[1],
		vert[p1].position[2] - vert[p0].position[2]);

	Vector3 p0p2 = Vector3(vert[p2].position[0] - vert[p0].position[0],
		vert[p2].position[1] - vert[p0].position[1],
		vert[p2].position[2] - vert[p0].position[2]);

	Vector3 n = Vector3(p0p1.y * p0p2.z - p0p1.z * p0p2.y,
		p0p1.z * p0p2.x - p0p1.x * p0p2.z,
		p0p1.x * p0p2.y - p0p1.y * p0p2.x);
	n.Normalize();

	vertnorm[p0].push_back(n);
	vertnorm[p1].push_back(n);
	vertnorm[p2].push_back(n);
}

#pragma warning (pop)