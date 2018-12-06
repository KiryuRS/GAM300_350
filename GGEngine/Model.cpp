/* Start Header -------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: model.cpp

Purpose: model loading implementation
Language: c++, Visual Studio 2017
Platform: Windows, SDK version 10.0.17134.0, Platform toolset Visual Studio 2017

(v141)
Project: zhihua.z_CS300_2
Author: Zhihua Zhou, zhihua.z, 270000013
Creation date: July 7, 2018
End Header --------------------------------------------------------*/
#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include "common.h"
#include <assimp/scene.h>

#include "Model.h"
#include <glm.hpp>
#include "GraphicsEngine.h"

#define max(x, y) (x > y ? x : y)
#define min(x, y) (x < y ? x : y)


Mesh::Mesh()
  :vertices{ std::vector<Vertex>() },
  indices{ std::vector<GLuint>() },
  vertexnormal{ std::map<GLuint, std::vector<Vector3>>() },
  face_normals { std::vector<GLfloat>()},
  bone_ids { std::vector< unsigned int >() },
  bone_weights { std::vector< float >() }
{
  
}

void Mesh::AssignBuffers()
{
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);
  glGenBuffers(1, &idvbo);
  glGenBuffers(1, &wtvbo);

  glBindVertexArray(vao);

  // Indices
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indices.size(), &indices[0], GL_STATIC_DRAW);

  // Vertices
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices.size() * FLOATS_PER_VERTEX, &vertices[0], GL_STATIC_DRAW);

  // set attribute for the first argument
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX * sizeof(GLfloat), (GLvoid*)0); // pos
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat))); // normal
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat))); // uv

                                                                                                                        // illustration : 1 1 1   1 1 1  1 1 
                                                                                                                        // name:          |pos| |normal| uv |
                                                                                                                        // location:      | 0 | |  1   | 2  |
  if (!bone_ids.empty())
  {
    glBindBuffer(GL_ARRAY_BUFFER, idvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned int) * bone_ids.size(), &bone_ids[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 4, GL_UNSIGNED_INT, 4 * sizeof(unsigned int), (GLvoid*)0);

    glBindBuffer(GL_ARRAY_BUFFER, wtvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * bone_weights.size(), &bone_weights[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, false, 4 * sizeof(float), (GLvoid*)0);
  }


  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);

  if (!bone_ids.empty())
  {
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
  }
}

void Mesh::UnassignBuffers()
{
  if (vao != (unsigned)-1)
    glDeleteVertexArrays(1, &vao);

  if (vbo != (unsigned)-1)
    glDeleteBuffers(1, &vbo);

  if (ebo != (unsigned)-1)
    glDeleteBuffers(1, &ebo);

  if (vnvao != (unsigned)-1)
    glDeleteBuffers(1, &vnvao);

  if (vnvbo != (unsigned)-1)
    glDeleteBuffers(1, &vnvbo);

  if (fnvao != (unsigned)-1)
    glDeleteBuffers(1, &fnvao);

  if (fnvbo != (unsigned)-1)
    glDeleteBuffers(1, &fnvbo);
}

void Mesh::ReassignBuffers()
{
  if (vao != (unsigned)-1)
    glDeleteVertexArrays(1, &vao);

  if (vbo != (unsigned)-1)
    glDeleteBuffers(1, &vbo);

  if (ebo != (unsigned)-1)
    glDeleteBuffers(1, &ebo);

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao);

  // Indices
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indices.size(), &indices[0], GL_STATIC_DRAW);

  // Vertices
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices.size() * FLOATS_PER_VERTEX, &vertices[0], GL_STATIC_DRAW);

  // set attribute for the first argument
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX * sizeof(GLfloat), (GLvoid*)0); // pos
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat))); // normal
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat))); // uv

                                                                                                                        // illustration : 1 1 1   1 1 1  1 1 
                                                                                                                        // name:          |pos| |normal| uv |
                                                                                                                        // location:      | 0 | |  1   | 2  |

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
}

void Mesh::AddVertex(const Vertex & v)
{
  vertices.push_back(v);
  ResizeBB(v.position[0] * SCALINGFACTOR, 
	  v.position[1] * SCALINGFACTOR, v.position[2] * SCALINGFACTOR);
}

void Mesh::AddFace(GLuint a, GLuint b, GLuint c)
{
  indices.push_back(a);
  indices.push_back(b);
  indices.push_back(c);
  ++num_of_triangles;
}


template<typename T>
void remove_duplicates(std::vector<T>& vec)
{
  size_t num = vec.size();

  std::vector<size_t> dup_id;
  for (size_t i = 0; i < num; ++i)
  {
    for (size_t j = i + 1; j < num; ++j)
    {
      if (vec[i] == vec[j])
      {
        dup_id.push_back(i);
        break;
      }
    }
  }

  std::vector<T> newvec;

  for (size_t i = 0; i < num; ++i)
  {
    bool found = false;

    for (size_t j = 0; j < dup_id.size(); ++j)
    {
      if (i == dup_id[j])
      {
        found = true;
        break;
      }
    }

    if (!found)
      newvec.push_back(vec[i]);
  }

  vec.swap(newvec);
}

void Mesh::WeightNormal()
{
  // clear duplicates
  for (auto& elem : vertexnormal)
  {
    remove_duplicates<Vector3>(elem.second);
  }

  // calculate average
  for (auto& elem : vertexnormal)
  {
    Vector3 sum = Vector3();

    for (auto& elem2 : elem.second)
    {
      sum += elem2;
    }
	sum.Normalize();
    vertices[elem.first].normal[0] = sum.x;
    vertices[elem.first].normal[1] = sum.y;
    vertices[elem.first].normal[2] = sum.z;
  }

  vertexnormal.clear();
}

void Mesh::ResizeBB(float x, float y, float z)
{
  btmleftfront.x = min(x, btmleftfront.x);
  btmleftfront.y = min(y, btmleftfront.y);
  btmleftfront.z = min(z, btmleftfront.z);

  toprightback.x = max(x, toprightback.x);
  toprightback.y = max(y, toprightback.y);
  toprightback.z = max(z, toprightback.z);
}

void Mesh::ResizeModelNDC()
{
  float max_x_range = max(btmleftfront.x, toprightback.x);
  float max_y_range = max(btmleftfront.y, toprightback.y);
  float max_z_range = max(btmleftfront.z, toprightback.z);

  float rev_shrink_ratio = 1.0f / max(max_x_range, max(max_y_range, max_z_range));

  Vector3 center = Vector3{ btmleftfront + (toprightback - btmleftfront) * 0.5f };

  for (unsigned i = 0; i < vertices.size(); ++i)
  {
    vertices[i].position[0] -= center.x;
    vertices[i].position[1] -= center.y;
    vertices[i].position[2] -= center.z;

    vertices[i].position[0] *= rev_shrink_ratio;
    vertices[i].position[1] *= rev_shrink_ratio;
    vertices[i].position[2] *= rev_shrink_ratio;
  }
  btmleftfront -= center;
  toprightback -= center;
  btmleftfront *= rev_shrink_ratio;
  toprightback *= rev_shrink_ratio;
}

void Mesh::GenerateVertexNormal()
{
  // empty the buffer
  if (vertex_normals.size() > 0)
    vertex_normals.clear();

  if (inverted)
  {
    for (auto& elem : vertices)
    {
      // push back two vertices, forming the two ends of the normal
      vertex_normals.push_back(elem.position[0]);
      vertex_normals.push_back(elem.position[1]);
      vertex_normals.push_back(elem.position[2]);
      vertex_normals.push_back(elem.position[0] - elem.normal[0] * NORMAL_DISPLAY_LENGTH);
      vertex_normals.push_back(elem.position[1] - elem.normal[1] * NORMAL_DISPLAY_LENGTH);
      vertex_normals.push_back(elem.position[2] - elem.normal[2] * NORMAL_DISPLAY_LENGTH);
    }
  }
  else
  {
    for (auto& elem : vertices)
    {
      // push back two vertices, forming the two ends of the normal
      vertex_normals.push_back(elem.position[0]);
      vertex_normals.push_back(elem.position[1]);
      vertex_normals.push_back(elem.position[2]);
      vertex_normals.push_back(elem.position[0] + elem.normal[0] * NORMAL_DISPLAY_LENGTH);
      vertex_normals.push_back(elem.position[1] + elem.normal[1] * NORMAL_DISPLAY_LENGTH);
      vertex_normals.push_back(elem.position[2] + elem.normal[2] * NORMAL_DISPLAY_LENGTH);
    }
  }


}

void Mesh::GenerateFaceNormal()
{
  // empty the buffer
  if (face_normals.size() > 0)
    face_normals.clear();

  // preventing internal fragmentation of memory
  face_normals.reserve(num_of_triangles * 6 + 100);

  for (unsigned i = 0; i < indices.size(); i += 3)
  {
    Vertex& v0 = vertices[indices[i]];
    Vertex& v1 = vertices[indices[i + 1]];
    Vertex& v2 = vertices[indices[i + 2]];

    Vector3 p0p1 = Vector3(
      v1.position[0] - v0.position[0],
      v1.position[1] - v0.position[1],
      v1.position[2] - v0.position[2]
    );

    Vector3 p0p2 = Vector3(
      v2.position[0] - v0.position[0],
      v2.position[1] - v0.position[1],
      v2.position[2] - v0.position[2]
    );

	Vector3 n = p0p1 ^ p0p2;
	n.Normalize();
    Vector3 p = (1.0f / 3.0f) * Vector3(
      v0.position[0] + v1.position[0] + v2.position[0],
      v0.position[1] + v1.position[1] + v2.position[1],
      v0.position[2] + v1.position[2] + v2.position[2]
    );

    face_normals.push_back(p.x);
    face_normals.push_back(p.y);
    face_normals.push_back(p.z);
    face_normals.push_back(p.x + n.x * NORMAL_DISPLAY_LENGTH);
    face_normals.push_back(p.y + n.y * NORMAL_DISPLAY_LENGTH);
    face_normals.push_back(p.z + n.z * NORMAL_DISPLAY_LENGTH);
  }

  // clear the local memory
  num_of_fnormals = static_cast<unsigned>(face_normals.size());
}

void Mesh::GenerateUV(int projector_mode, int value_type)
{
	UNREFERENCED_PARAMETER(projector_mode);
	UNREFERENCED_PARAMETER(value_type);
  //if (projector_mode == P_CYLINDRICAL)
  //{
  //  if (value_type == VT_VERTEX_POSITION)
  //  {
  //    for (size_t i = 0; i < vertices.size(); ++i)
  //      project_cylinder(vertices[i].position, vertices[i].uv, btmleftfront.y, toprightback.y);
  //  }
  //  else
  //  {
  //    for (size_t i = 0; i < vertices.size(); ++i)
  //      project_cylinder(vertices[i].normal, vertices[i].uv, btmleftfront.y, toprightback.y);
  //  }
  //}
  //else if (projector_mode == P_SPHERICAL)
  //{
  //  if (value_type == VT_VERTEX_POSITION)
  //  {
      for (size_t i = 0; i < vertices.size(); ++i)
        project_spherical(vertices[i].position, vertices[i].uv);
  //  }
  //  else
  //  {
  //    for (size_t i = 0; i < vertices.size(); ++i)
  //      project_spherical(vertices[i].normal, vertices[i].uv);
  //  }
  //}
  //else // cube
  //{
  //  if (value_type == VT_VERTEX_POSITION)
  //  {
  //    for (size_t i = 0; i < vertices.size(); ++i)
  //      project_cube(vertices[i].position, vertices[i].uv);
  //  }
  //  else
  //  {
  //    for (size_t i = 0; i < vertices.size(); ++i)
  //      project_cube(vertices[i].normal, vertices[i].uv);
  //  }
  //}
}

void Mesh::GenerateHierachy()
{

}

void Mesh::AllocateBoneData()
{
  bone_ids.resize(vertices.size() * 4);
  bone_weights.resize(vertices.size() * 4);
}

void Mesh::OpenGLBinding()
{
	// face normal vao and vbo
	if (face_normals.size())
	{
		glGenVertexArrays(1, &fnvao);
		glGenBuffers(1, &fnvbo);

		glBindVertexArray(fnvao);

		// Vertices
		glBindBuffer(GL_ARRAY_BUFFER, fnvbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * face_normals.size(), &face_normals[0], GL_STATIC_DRAW);

		// set attribute for the first argument
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0); // pos

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		glDisableVertexAttribArray(0);
		face_normals.clear();
	}

	// vertex normal vao and vbo
	if (vertex_normals.size())
	{
		glGenVertexArrays(1, &vnvao);
		glGenBuffers(1, &vnvbo);

		glBindVertexArray(vnvao);

		// Vertices
		glBindBuffer(GL_ARRAY_BUFFER, vnvbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_normals.size(), &vertex_normals[0], GL_STATIC_DRAW);

		// set attribute for the first argument
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0); // pos

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glDisableVertexAttribArray(0);
	}

	// clear the local memory
	num_of_vnormals = static_cast<unsigned>(vertex_normals.size());
	vertex_normals.clear();
	AssignBuffers();
}

void Mesh::project_cylinder(GLfloat v[3], GLfloat uv[2], float miny, float maxy)
{
  float theta = atan2(abs(v[2]), abs(v[0]));
  float tv = (v[1] - miny) / (maxy - miny);

  if (v[2] < 0.f)
    theta += 3.1415926535f;

  uv[0] = static_cast<float>(theta / (3.1415926535f * 2));
  uv[1] = tv;
}

void Mesh::project_spherical(GLfloat v[3], GLfloat uv[2])
{
  float theta = atan2(abs(v[2]), abs(v[0]));
  float r = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
  float phi = acos(v[1] / r);

  if (v[2] < 0.f)
    theta += 3.1415926535f;
  uv[0] = theta / (3.1415926535f * 2);
  uv[1] = (3.1415926535f - phi) / 3.1415926535f;
}

void Mesh::project_cube(GLfloat v[3], GLfloat uv[2])
{
  float x = v[0];
  float y = v[1];
  float z = v[2];

  float abx = fabs(x);
  float aby = fabs(y);
  float abz = fabs(z);

  if (x > 0 && abx >= aby && abx >= abz)
  {
    uv[0] = 0.5f * (-z / abx + 1.f);
    uv[1] = 0.5f * (-y / abx + 1.f);
  }

  if (x <= 0 && abx >= aby && abx >= abz)
  {
    uv[0] = 0.5f * (z / abx + 1.f);
    uv[1] = 0.5f * (y / abx + 1.f);
  }

  if (y > 0 && aby >= abx && aby >= abz)
  {
    uv[0] = 0.5f * (x / aby + 1.f);
    uv[1] = 0.5f * (-z / aby + 1.f);
  }

  if (y <= 0 && aby >= abx && aby >= abz)
  {
    uv[0] = 0.5f * (x / aby + 1.f);
    uv[1] = 0.5f * (z / aby + 1.f);
  }

  if (z > 0 && abz >= abx && abz >= aby)
  {
    uv[0] = 0.5f * (x / abz + 1.f);
    uv[1] = 0.5f * (y / abz + 1.f);
  }

  if (z <= 0 && abz >= abx && abz >= aby)
  {
    uv[0] = 0.5f * (-x / abz + 1.f);
    uv[1] = 0.5f * (y / abz + 1.f);
  }
}
