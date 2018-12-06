/*****************************************************************************/
/*!
\file CEShader.cpp
\author Zhou Zhihua 
\par zhihua.z\@digipen.edu
\date August 31, 2017
\brief

Copyright (C) 2017 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/*****************************************************************************/
#include "stdafx.h"
#include "Shader.h"
#include "GraphicsEnums.h"

Shader::Shader()
  : programID(static_cast<GLuint>(-1)),
  vertexShaderID(static_cast<GLuint>(-1)),
  fragmentShaderID(static_cast<GLuint>(-1))
{
}

Shader::Shader(GLuint id, GLuint v, GLuint f)
  : programID(id),
  vertexShaderID(v),
  fragmentShaderID(f)
{
}

void Shader::Start()
{
#ifdef _DEBUG
  // Assert(programID != -1, __FILE__, __func__, __LINE__, "Shader program can't be compiled");
  // Assert(vertexShaderID != -1, __FILE__, __func__, __LINE__, "Vertex shader can't be compiled");
  // Assert(fragmentShaderID != -1, __FILE__, __func__, __LINE__, "Fragment shader can't be compiled");
#endif
  glUseProgram(programID);
}

void Shader::End()
{
  glUseProgram(0);
}

void Shader::SetVContent(std::string & shadercontent)
{
  vcontent = shadercontent;
}

void Shader::SetFContent(std::string & shadercontent)
{
  fcontent = shadercontent;
}

void Shader::setInt1i(const char * name, int value)
{
  glUniform1i(glGetUniformLocation(programID, name), value);
}

void Shader::setFloat(const char * name, float f1)
{
  glUniform1f(glGetUniformLocation(programID, name), f1);
}

void Shader::setFloat2f(const char * name, float f1, float f2)
{
  glUniform2f(glGetUniformLocation(programID, name), f1, f2);
}

void Shader::setFloat2fv(const char * name, int count, const float * value_ptr)
{
  glUniform2fv(glGetUniformLocation(programID, name), count, value_ptr);
}

void Shader::setFloat3f(const char * name, float f1, float f2, float f3)
{
  glUniform3f(glGetUniformLocation(programID, name), f1, f2, f3);
}

void Shader::setFloat3fv(const char * name, int count, const float * value_ptr)
{
  GLint pos = glGetUniformLocation(programID, name);
  glUniform3fv(pos, count, value_ptr);
}

void Shader::setFloat4f(const char * name, float f1, float f2, float f3, float f4)
{
  glUniform4f(glGetUniformLocation(programID, name), f1, f2, f3, f4);
}

void Shader::setFloat4fv(const char * name, int count, const float * value_ptr)
{
  glUniform4fv(glGetUniformLocation(programID, name), count, value_ptr);
}

void Shader::setMat3fv(const char * name, int count, const glm::tmat3x3<float>& mat)
{
  glUniformMatrix3fv(glGetUniformLocation(programID, name), count, GL_FALSE, &(mat[0][0]));
}

void Shader::setMat4fv(const char * name, int count, const glm::tmat4x4<float>& mat)
{
  glUniformMatrix4fv(glGetUniformLocation(programID, name), count, GL_FALSE, &(mat[0][0]));
}
void Shader::setMat3fv(const char * name, int count, const Matrix3x3& mat)
{
	glUniformMatrix3fv(glGetUniformLocation(programID, name), count, GL_FALSE, mat.Begin());
}

void Shader::setMat4fv(const char * name, int count, const Matrix4x4& mat)
{
	glUniformMatrix4fv(glGetUniformLocation(programID, name), count, GL_FALSE, mat.Begin());
}

void Shader::setInt2i(const char * name, int first, int second)
{
  glUniform2i(glGetUniformLocation(programID, name), first, second);
}

bool Shader::IsCompiled() const
{
  return compiled;
}

void Shader::SetCompiled(bool f)
{
  compiled = f;
}

GLuint Shader::GetID() const
{
  return programID;
}

GLuint Shader::GetVertexShaderID() const
{
  return vertexShaderID;
}

GLuint Shader::GetFragmentShaderID() const
{
  return fragmentShaderID;
}

GLuint Shader::GetProgramID() const
{
  return programID;
}
