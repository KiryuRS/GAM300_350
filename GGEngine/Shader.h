/*****************************************************************************/
/*!
\file CEShader.h
\author Zhou Zhihua 
\par zhihua.z\@digipen.edu
\date August 31, 2017
\brief

Contains implementation of a shader object

Copyright (C) 2017 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/*****************************************************************************/
#include <string>
#include <glew.h>
#include <glm.hpp>
#include "Matrix3x3.h"
#include "Matrix4x4.h"
#pragma once

class PointLight;

class Shader
{
public:
  Shader();

  Shader(GLuint id, GLuint v, GLuint f);

  void Start();
  void End();

  void SetVContent(std::string& shadercontent);
  void SetFContent(std::string& shadercontent);

  void setInt1i(const char* name, int value);
  void setFloat(const char* name, float f1);
  void setFloat2f(const char* name, float f1, float f2);
  void setFloat2fv(const char* name, int count, const float* value_ptr);
  void setFloat3f(const char* name, float f1, float f2, float f3);
  void setFloat3fv(const char* name, int count, const float* value_ptr);
  void setFloat4f(const char* name, float f1, float f2, float f3, float f4);
  void setFloat4fv(const char* name, int count, const float* value_ptr);
  void setMat3fv(const char* name, int count, const glm::tmat3x3<float>& mat);
  void setMat4fv(const char* name, int count, const glm::tmat4x4<float>& mat);
  void setMat3fv(const char * name, int count, const Matrix3x3& mat);
  void setMat4fv(const char * name, int count, const Matrix4x4& mat);
  void setInt2i(const char * name, int first, int second);


  bool IsCompiled() const;
  void SetCompiled(bool f);

  GLuint GetID() const;
  GLuint GetVertexShaderID() const;
  GLuint GetFragmentShaderID() const;
  GLuint GetProgramID() const;

private:
  GLuint programID;
  GLuint vertexShaderID;
  GLuint fragmentShaderID;
  bool compiled;
  std::string vcontent;
  std::string fcontent;
};