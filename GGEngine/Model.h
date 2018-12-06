/* Start Header -------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: model.h

Purpose: model loading and creation
Language: c++, Visual Studio 2017
Platform: Windows, SDK version 10.0.17134.0, Platform toolset Visual Studio 2017

(v141)
Project: zhihua.z_CS300_2
Author: Zhihua Zhou, zhihua.z, 270000013
Creation date: July 7, 2018
End Header --------------------------------------------------------*/
#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>
#include <set>
#include <map>
#include <glew.h>
#include <glm.hpp>
#include <limits>
#include "Animation.h"
#include <utility>
#include "Matrix4x4.h"
#include "Material.h"
#define FLOATS_PER_VERTEX 8
#define NORMAL_DISPLAY_LENGTH 0.1f


struct Vertex
{
  Vertex()
    : position{ 0, 0, 0 }, normal{ 0, 0, 0 }, uv{ 0.f, 0.f }
  {
  }
  Vertex(GLfloat px, GLfloat py, GLfloat pz)
	  : position{ px, py, pz }, normal{ 0.f,0.f,0.f }, uv{ 0.f, 0.f }
  {
  }
  Vertex(GLfloat px, GLfloat py, GLfloat pz, GLfloat nx, GLfloat ny, GLfloat nz)
    : position{ px, py, pz }, normal{ nx, ny, nz }, uv{0.f, 0.f}
  {
  }
  Vertex(GLfloat px, GLfloat py, GLfloat pz, GLfloat nx, GLfloat ny, GLfloat nz, GLfloat u, GLfloat v)
    : position{ px, py, pz }, normal{ nx, ny, nz }, uv{ u, v }
  {}

  GLfloat position[3];
  GLfloat normal[3];
  GLfloat uv[2];
};

struct Weight
{
  Weight(unsigned id, float w)
    : vertex_id{ id }, weight{ w }
  {}

  unsigned vertex_id;
  float weight;
};

struct Bone
{
  Bone()
  {}

  Bone(std::string n)
    :name{ n }, weights{ std::vector<Weight>() }, offset_matrix{ Matrix4x4(1.0f) }
  {}

  std::string name;
  std::vector<Weight> weights;
  Matrix4x4 offset_matrix;
  Matrix4x4 finalTransformation;
};

struct Mesh
{
  void AssignBuffers();
  void UnassignBuffers();
  void ReassignBuffers();
  Mesh();
  void AddVertex(const Vertex& v);
  void AddFace(GLuint a, GLuint b, GLuint c);
  void WeightNormal();
  void ResizeBB(float x, float y, float z); // resize bounding box
  void ResizeModelNDC();
  void GenerateVertexNormal();
  void GenerateFaceNormal();
  void GenerateUV(int projector_mode, int value_type);
  void GenerateHierachy();
  void AllocateBoneData();
  // buffers
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  std::vector<GLfloat> vertex_normals; // draw as mesh
  std::vector<GLfloat> face_normals;   // draw as mesh
  std::vector<unsigned int> bone_ids;
  std::vector<float> bone_weights;

                                       // calculation variables
  std::map<GLuint, std::vector<Vector3>> vertexnormal;
  Vector3 toprightback = {
    Vector3(
      -std::numeric_limits<float>::max(),
      -std::numeric_limits<float>::max(),
      -std::numeric_limits<float>::max()
    )
  }; // minimum
  Vector3 btmleftfront = {
    Vector3(
      std::numeric_limits<float>::max(),
      std::numeric_limits<float>::max(),
      std::numeric_limits<float>::max()
    )
  }; // maximum
  Vector3 sumvertices = { Vector3() };

  // rendering variables
  GLuint num_of_triangles = { 0 };
  GLuint num_of_fnormals = { 0 };
  GLuint num_of_vnormals = { 0 };
  GLuint vbo = { static_cast<unsigned int>(-1) };
  GLuint ebo = { static_cast<unsigned int>(-1) };
  GLuint vao = { static_cast<unsigned int>(-1) };
  GLuint vnvao = { static_cast<unsigned int>(-1) };
  GLuint vnvbo = { static_cast<unsigned int>(-1) };
  GLuint fnvao = { static_cast<unsigned int>(-1) };
  GLuint fnvbo = { static_cast<unsigned int>(-1) };
  GLuint idvbo = { static_cast<unsigned int>(-1) };
  GLuint wtvbo = { static_cast<unsigned int>(-1) };
  bool inverted = { false };
  void OpenGLBinding();
  
private:
  void project_cylinder(GLfloat v[3], GLfloat uv[2], float miny, float maxy);
  void project_spherical(GLfloat v[3], GLfloat uv[2]);
  void project_cube(GLfloat v[3], GLfloat uv[2]);
};

struct StaticMesh
{
  bool loaded = false;
  std::vector<Mesh> meshes;
  std::vector<Material*> materials;
  Vector3 min, max;
};

#endif