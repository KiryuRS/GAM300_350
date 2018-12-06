///*****************************************************************************/
///*!
//\file CEPrimitives.cpp
//\author Zhou Zhihua, zhihua.z, 390001016
//\par zhihua.z\@digipen.edu
//\date August 12, 2017
//\brief
//
//This file contains Graphics Engine Vertex format, and Graphics Engine Mesh that is
//used to describe a drawable object
//
//Copyright (C) 2017 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the
//prior written consent of DigiPen Institute of Technology is prohibited.
//*/
///*****************************************************************************/
#include "stdafx.h"
//#include "GraphicsPrimitives.h"
//#include "GraphicsEnums.h"
//#include <algorithm>
//
//#define GLEW_STATIC
//#include <glew.h>
//
//Mesh::Mesh()
//	:
//	m_num_indices{ 0 },
//	m_num_triangles{ 0 },
//	m_num_vertex{ 0 },
//	vertices{ 0 },
//	indices{ 0 }
//{
//}
//
//void Mesh::AddVertex(const Vertex& v)
//{
//	float * newarray = new float[(m_num_vertex + 1) * FLOATS_PER_VERTEX];
//	if (vertices)
//	{
//		std::copy(vertices, vertices + m_num_vertex * FLOATS_PER_VERTEX, newarray);
//		delete[] vertices;
//	}
//	vertices = newarray;
//
//	unsigned int * newindices = new unsigned int[m_num_indices + 3];
//	if (indices)
//	{
//		std::copy(indices, indices + m_num_indices, newindices);
//		delete[] indices;
//	}
//	indices = newindices;
//
//	int stride = m_num_vertex * FLOATS_PER_VERTEX;
//	vertices[stride + 0] = v.pos[0];
//	vertices[stride + 1] = v.pos[1];
//	vertices[stride + 2] = v.pos[2];
//	vertices[stride + 3] = v.normal[0];
//	vertices[stride + 4] = v.normal[1];
//	vertices[stride + 5] = v.normal[2];
//	vertices[stride + 6] = v.texCoords[0];
//	vertices[stride + 7] = v.texCoords[1];
//
//	indices[m_num_indices] = m_num_indices;
//
//	m_num_indices += 1;
//	m_num_vertex += 1;
//	m_num_triangles = m_num_vertex / 3;
//}
//
//void Mesh::Add3Vertex(const Vertex& v1, const Vertex& v2, const Vertex& v3)
//{
//	float * newarray = new float[(m_num_vertex + 3) * FLOATS_PER_VERTEX];
//	if (vertices)
//	{
//		std::copy(vertices, vertices + m_num_vertex * FLOATS_PER_VERTEX, newarray);
//		delete[] vertices;
//	}
//	vertices = newarray;
//
//	unsigned int * newindices = new unsigned int[m_num_indices + 3];
//	if (indices)
//	{
//		std::copy(indices, indices + m_num_indices, newindices);
//		delete[] indices;
//	}
//	indices = newindices;
//
//	int stride = m_num_vertex * FLOATS_PER_VERTEX;
//  vertices[stride + 0] = v1.pos[0];
//  vertices[stride + 1] = v1.pos[1];
//  vertices[stride + 2] = v1.pos[2];
//  vertices[stride + 3] = v1.normal[0];
//  vertices[stride + 4] = v1.normal[1];
//  vertices[stride + 5] = v1.normal[2];
//  vertices[stride + 6] = v1.texCoords[0];
//  vertices[stride + 7] = v1.texCoords[1]; 
//  vertices[stride + 8] = v2.pos[0];
//  vertices[stride + 9] = v2.pos[1];
//  vertices[stride + 10] = v2.pos[2];
//  vertices[stride + 11] = v2.normal[0];
//  vertices[stride + 12] = v2.normal[1];
//  vertices[stride + 13] = v2.normal[2];
//  vertices[stride + 14] = v2.texCoords[0];
//  vertices[stride + 15] = v2.texCoords[1]; 
//  vertices[stride + 16] = v3.pos[0];
//  vertices[stride + 17] = v3.pos[1];
//  vertices[stride + 18] = v3.pos[2];
//  vertices[stride + 19] = v3.normal[0];
//  vertices[stride + 20] = v3.normal[1];
//  vertices[stride + 21] = v3.normal[2];
//  vertices[stride + 22] = v3.texCoords[0];
//  vertices[stride + 23] = v3.texCoords[1];
//
//	indices[m_num_indices + 0] = m_num_indices;
//	indices[m_num_indices + 1] = m_num_indices + 1;
//	indices[m_num_indices + 2] = m_num_indices + 2;
//
//
//	m_num_indices += 3;
//	m_num_vertex += 3;
//	m_num_triangles = m_num_vertex / 3;
//}
//
//void Mesh::AddQuad(const Vertex& v1, const Vertex& v2, const Vertex& v3, const Vertex& v4)
//{
//	float * newarray = new float[(m_num_vertex + 4) * FLOATS_PER_VERTEX];
//	if (vertices)
//	{
//		std::copy(vertices, vertices + m_num_vertex * FLOATS_PER_VERTEX, newarray);
//		delete[] vertices;
//	}
//	vertices = newarray;
//
//	unsigned int * newindices = new unsigned int[m_num_indices + 4];
//	if (indices)
//	{
//		std::copy(indices, indices + m_num_indices, newindices);
//		delete[] indices;
//	}
//	indices = newindices;
//
//	int stride = m_num_vertex * FLOATS_PER_VERTEX;
//  vertices[stride + 0] = v1.pos[0];
//  vertices[stride + 1] = v1.pos[1];
//  vertices[stride + 2] = v1.pos[2];
//  vertices[stride + 3] = v1.normal[0];
//  vertices[stride + 4] = v1.normal[1];
//  vertices[stride + 5] = v1.normal[2];
//  vertices[stride + 6] = v1.texCoords[0];
//  vertices[stride + 7] = v1.texCoords[1];
//  vertices[stride + 8] = v2.pos[0];
//  vertices[stride + 9] = v2.pos[1];
//  vertices[stride + 10] = v2.pos[2];
//  vertices[stride + 11] = v2.normal[0];
//  vertices[stride + 12] = v2.normal[1];
//  vertices[stride + 13] = v2.normal[2];
//  vertices[stride + 14] = v2.texCoords[0];
//  vertices[stride + 15] = v2.texCoords[1];
//  vertices[stride + 16] = v3.pos[0];
//  vertices[stride + 17] = v3.pos[1];
//  vertices[stride + 18] = v3.pos[2];
//  vertices[stride + 19] = v3.normal[0];
//  vertices[stride + 20] = v3.normal[1];
//  vertices[stride + 21] = v3.normal[2];
//  vertices[stride + 22] = v3.texCoords[0];
//  vertices[stride + 23] = v3.texCoords[1];
//  vertices[stride + 24] = v4.pos[0];
//  vertices[stride + 25] = v4.pos[1];
//  vertices[stride + 26] = v4.pos[2];
//  vertices[stride + 27] = v4.normal[0];
//  vertices[stride + 28] = v4.normal[1];
//  vertices[stride + 29] = v4.normal[2];
//  vertices[stride + 30] = v4.texCoords[0];
//  vertices[stride + 31] = v4.texCoords[1];
//
//	indices[m_num_indices + 0] = m_num_indices;
//	indices[m_num_indices + 1] = m_num_indices + 1;
//	indices[m_num_indices + 2] = m_num_indices + 2;
//	indices[m_num_indices + 3] = m_num_indices + 3;
//
//
//	m_num_indices += 4;
//	m_num_vertex += 4;
//	m_num_triangles = m_num_vertex / 3;
//}
//
//void Mesh::AddCircle(int points, float z)
//{
//	float * newarray = new float[(m_num_vertex + points) * FLOATS_PER_VERTEX];
//	if (vertices)
//	{
//		std::copy(vertices, vertices + m_num_vertex * FLOATS_PER_VERTEX, newarray);
//		delete[] vertices;
//	}
//	vertices = newarray;
//
//	unsigned int * newindices = new unsigned int[m_num_indices + points + 1];
//	if (indices)
//	{
//		std::copy(indices, indices + m_num_indices, newindices);
//		delete[] indices;
//	}
//	indices = newindices;
//
//	/* calculate vertices and indices */
//
//	int interval = points;
//
//	float angle = 0;
//	float dtheta = (2 * static_cast<float>(PI) / interval);
//
//	int stride = m_num_vertex * FLOATS_PER_VERTEX;
//	for (int i = 0; i < FLOATS_PER_VERTEX * (interval)-1; i += FLOATS_PER_VERTEX, angle += dtheta)
//	{
//		float sine = sinf(angle);
//		float cosine = cosf(angle);
//
//		vertices[stride + i] = cosine / 2.0f;
//		vertices[stride + i + 1] = sine / 2.0f;
//		vertices[stride + i + 2] = z;
//		vertices[stride + i + 3] = 0.0f;
//		vertices[stride + i + 4] = 0.0f;
//		vertices[stride + i + 5] = 1.0f;
//		vertices[stride + i + 6] = cosine / 2 + 0.5f;
//		vertices[stride + i + 7] = sine / 2 + 0.5f;
//	}
//
//	stride = m_num_indices;
//	for (int i = 0; i < interval + 1; ++i)
//	{
//		indices[m_num_indices + i] = i % interval;
//
//	}
//
//
//	/* update data */
//	m_num_indices += points + 1;
//	m_num_vertex += points;
//	m_num_triangles = m_num_vertex / 3;
//}
//
//int Mesh::GetNumOfTriangles()
//{
//	return m_num_triangles;
//}
//
//int Mesh::GetNumOfVertices()
//{
//	return m_num_vertex;
//}
//
//int Mesh::GetNumOfIndices()
//{
//	return m_num_indices;
//}
//
//float * Mesh::GetVertices()
//{
//	return vertices;
//}
//
//unsigned int * Mesh::GetIndices()
//{
//	return indices;
//}
//
//bool Mesh::AssignBuffers()
//{
//	glGenVertexArrays(1, &vao);
//	glGenBuffers(1, &vbo);
//	glGenBuffers(1, &ebo);
//
//
//	glBindVertexArray(vao);
//
//	// Indices
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*m_num_indices, indices, GL_STATIC_DRAW);
//
//	// Vertices
//	glBindBuffer(GL_ARRAY_BUFFER, vbo);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*m_num_vertex*FLOATS_PER_VERTEX, vertices, GL_STATIC_DRAW);
//
//	// set attribute for the first argument
//	// the vertex shader accesses this position-buffer data using "layout (location = 0) in vec3 position;" shader code
//	glVertexAttribPointer(0, FLOATS_PER_POSITION, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX * sizeof(GLfloat), (GLvoid*)0);
//	glEnableVertexAttribArray(0);
//
//	GLint stride = FLOATS_PER_POSITION;
//	//the vertex shader accesses this color-buffer data using "layout (location = 1) in vec4 color;" shader code
//	glVertexAttribPointer(1, FLOATS_PER_NORMAL, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX * sizeof(GLfloat), (GLvoid*)(stride * sizeof(GLfloat)));
//	glEnableVertexAttribArray(1);
//
//	stride = FLOATS_PER_POSITION + FLOATS_PER_NORMAL;
//	glVertexAttribPointer(2, FLOATS_PER_UV, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX * sizeof(GLfloat), (GLvoid*)(stride * sizeof(GLfloat)));
//	glEnableVertexAttribArray(2);
//
//	// illustration : 1 1 1 1 1 1 1  1 
//	// name:          |pos| | n | |uv|
//	// location:      | 0 | | 1 | | 2|
//
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//	glBindVertexArray(0);
//
//	return true;
//}
//
//GLuint Mesh::GetVAO() const
//{
//	return vao;
//}
//
//GLuint Mesh::GetEBO() const
//{
//	return ebo;
//}
//
//GLuint Mesh::GetVBO() const
//{
//	return vbo;
//}
//
//void Mesh::Clear()
//{
//	delete[] vertices;
//	delete[] indices;
//
//	vertices = nullptr;
//	indices = nullptr;
//	m_num_vertex = 0;
//	m_num_indices = 0;
//	m_num_triangles = 0;
//
//	glDeleteBuffers(1, &vao);
//	glDeleteBuffers(1, &vbo);
//	glDeleteBuffers(1, &ebo);
//}
//
//Mesh::~Mesh()
//{
//	Clear();
//}
