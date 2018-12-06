/*****************************************************************************/
/*!
\file CEShaderManager.cpp
\author Zhou Zhihua 
\par zhihua.z\@digipen.edu
\date August 31, 2017
\brief

Provides management of shaders

Copyright (C) 2017 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/*****************************************************************************/
#include "stdafx.h"
#include <fstream>
#include <sstream>

#include "ShaderManager.h"
#include "Shader.h"


static std::map<CurrentShader, std::string> shadermap
{
  { S_PLAIN, "colorshader" },
  { S_CUSTOM, "customshader" },
  { S_TEXTURE, "textureshader" },
  { S_IMGUI, "imguishader" },
  { S_BOUNDING_BOX, "bbshader" },
  { S_PARTICLE, "particleshader" },
  { S_PARTICLE_INDEPENDENT, "particleIndshader" },
  { S_FONT , "fontshader" },
  { S_LINE , "lineshader" },  
  { S_DEPTH, "depthShader" },
  { S_SKYBOX, "Skybox_shader" },
  { S_EXTRACT, "extract" },
  { S_BLUR, "blur" },
  { S_BLEND, "bloomblend" },
  { S_HDR , "hdr"},
  { S_GAMMA, "GammaCorrection" },
  { S_SPRITE2D, "planeRender"}
};

ShaderManager::ShaderManager()
{
  shaderpath = "shaders/";
}

bool ShaderManager::Init()
{
  //Initialization required for glew - Currently we are using the static library version (x64) of glew
  GLint res = glewInit();
  if (res != GLEW_OK)
  {
    //Log it
    return false;
  }
  Recompile();

  return true;
}

void ShaderManager::Clean()
{
  for (auto& elem : m_shaders)
  {
    if (elem.second.IsCompiled())
    {
      glDetachShader(elem.second.GetID(), elem.second.GetVertexShaderID());
      glDeleteShader(elem.second.GetVertexShaderID());

      glDetachShader(elem.second.GetID(), elem.second.GetVertexShaderID());
      glDeleteShader(elem.second.GetVertexShaderID());

      glDeleteShader(elem.second.GetID());
    }
  }
}

void ShaderManager::Recompile()
{
	m_shaders.clear();
	CreateShader(S_PLAIN);
	CreateShader(S_CUSTOM);
	CreateShader(S_TEXTURE);
	CreateShader(S_IMGUI);
	CreateShader(S_BOUNDING_BOX);
	CreateShader(S_PARTICLE);
	CreateShader(S_PARTICLE_INDEPENDENT);
	CreateShader(S_FONT);
	CreateShader(S_LINE);
  CreateShader(S_DEPTH);
  CreateShader(S_SKYBOX);
  CreateShader(S_BLEND);
  CreateShader(S_BLUR);
  CreateShader(S_EXTRACT);
  CreateShader(S_HDR);
  CreateShader(S_GAMMA);
  CreateShader(S_SPRITE2D);
  CONSOLE_LN("Recompiled shaders!");
}

bool ShaderManager::CreateShader(CurrentShader cs)
{
  std::ifstream file;

  file.open(shaderpath + shadermap[cs] + ".v");
  std::stringstream vcontent;
  vcontent << file.rdbuf();
  file.close();

  file.open(shaderpath + shadermap[cs] + ".f");
  std::stringstream fcontent;
  fcontent << file.rdbuf();
  file.close();

  // Assert(vcontent.str().size() > 0, __FILE__, __func__, __LINE__, "must have content (vertex shader)");
  // Assert(fcontent.str().size() > 0, __FILE__, __func__, __LINE__, "must have content (fragment shader)");

  GLuint shaderid = glCreateProgram();
  GLint compiled = 0;
  GLuint vid = 0;
  GLuint fid = 0;

  GLenum glErr;
  glErr = glGetError();
  if (glErr != GL_NO_ERROR)
  {
    //Log it
    return false;
  }

  std::string s1 = vcontent.str();
  std::string s2 = fcontent.str();

  const char * vtext = s1.c_str();
  const char * ftext = s2.c_str();

  // create and compile vertex shader i
  vid = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vid, 1, &vtext, NULL);
  glCompileShader(vid);
  glGetShaderiv(vid, GL_COMPILE_STATUS, &compiled);//verifies compilation
  if (!compiled)
  {
    GLchar infoLog[512];
    glGetShaderInfoLog(vid, 512, NULL, infoLog);
	CONSOLE_ERROR("Could not compile shader ", shadermap[cs] + ".v!");
	CONSOLE_ERROR(infoLog);
  }
  // Assert(compiled != 0, __FILE__, __func__, __LINE__, "must successfully compile vertex shader"); // must successfully compile
  
  compiled = 0;
  //fid = glCreateShader(GL_FRAGMENT_SHADER);
  // Create and compile fragment shader
  fid = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fid, 1, &ftext, NULL);
  glCompileShader(fid);
  glGetShaderiv(fid, GL_COMPILE_STATUS, &compiled);//verifies compilation
  if (!compiled)
  {
    GLchar infoLog[512];
    glGetShaderInfoLog(fid, 512, NULL, infoLog);
	CONSOLE_ERROR("Could not compile shader ", shadermap[cs] + ".f!");
	CONSOLE_ERROR(infoLog);
  }
  

  // Assert(compiled != 0, __FILE__, __func__, __LINE__, "must successfully compile fragment shader"); // must successfully compile fragment shader
  
  //attach the shaders to the main shader program
  glAttachShader(shaderid, vid);
  glAttachShader(shaderid, fid);
  
  GLint linked = 0;
  glLinkProgram(shaderid);//link all shaders
  glGetProgramiv(shaderid, GL_LINK_STATUS, &linked);//verifies linkage
  
  if (!linked)
  {
	  GLint maxLen = 0;
	  GLchar infoLog[4096];
	  glGetProgramiv(shaderid, GL_INFO_LOG_LENGTH, &maxLen);
	  glGetProgramInfoLog(shaderid, maxLen, &maxLen, &infoLog[0]);
    glGetShaderInfoLog(shaderid, 512, NULL, infoLog);
	CONSOLE_ERROR(infoLog);
  }


  // Assert(linked != 0, __FILE__, __func__, __LINE__, "must link successfully"); // must link successfully

  
  m_shaders.emplace(cs, Shader { shaderid, vid, fid });

  return true;
}

Shader & ShaderManager::Get(CurrentShader cs)
{
  return m_shaders[cs];
}
