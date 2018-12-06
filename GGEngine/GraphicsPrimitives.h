///*****************************************************************************/
///*!
//\file CEPrimitives.h
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
//#pragma once
//
//#define PI 3.14159265358979323846f
//typedef unsigned int GLuint;
//
///***************************************************************************/
///*!
//\brief
//Default vertex format
//3 * 32 bit for position, 3 * 32 bit for color, 2 * 32 bit for uv
//*/
///***************************************************************************/
//struct Vertex
//{
//	float pos[3];
//	float normal[3];
//	float texCoords[2];
//
//  Vertex(float px, float py, float pz, float nx, float ny, float nz, float ux, float uy)
//    : pos{ px, py, pz }, normal{ nx, ny, nz }, texCoords{ ux, uy }
//  {}
//};
//
///***************************************************************************/
///*!
//\brief
//Standard mesh format for Graphics Engine
//*/
///***************************************************************************/
//class Mesh
//{
//public:
//	/***************************************************************************/
//	/*!
//	\brief
//	Default initialize a CE Mesh
//	*/
//	/***************************************************************************/
//	Mesh();
//
//	void AddVertex(const Vertex& v);
//
//	/***************************************************************************/
//	/*!
//	\brief
//	Add a triangle to the vertex list
//
//	\param p1x - normalized x position within model space
//	\param p1y - normalized y position within model space
//	\param p1z - normalized z position within model space
//	\param n1x - normalized x position within model space
//	\param n1y - normalized y position within model space
//	\param n1z - normalized z position within model space
//	\param u1 - u coordinate (texture)
//	\param v1 - v coordinate (texture)
//
//	\param p2x - normalized x position within model space
//	\param p2y - normalized y position within model space
//	\param p2z - normalized z position within model space
//	\param n2x - normalized x position within model space
//	\param n2y - normalized y position within model space
//	\param n2z - normalized z position within model space
//	\param u2 - u coordinate (texture)
//	\param v2 - v coordinate (texture)
//
//	\param p3x - normalized x position within model space
//	\param p3y - normalized y position within model space
//	\param p3z - normalized z position within model space
//	\param n3x - normalized x position within model space
//	\param n3y - normalized y position within model space
//	\param n3z - normalized z position within model space
//	\param u3 - u coordinate (texture)
//	\param v3 - v coordinate (texture)
//	*/
//	/***************************************************************************/
//	void Add3Vertex(const Vertex& v1, const Vertex& v2, const Vertex& v3);
//
//	/***************************************************************************/
//	/*!
//	\brief
//	Add a triangle to the vertex list
//
//	\param p1x - normalized x position within model space
//	\param p1y - normalized y position within model space
//	\param p1z - normalized z position within model space
//	\param p1cr - red
//	\param p1cg - green
//	\param p1cb - blue
//	\param p1ca - alpha
//	\param u1 - u coordinate (texture)
//	\param v1 - v coordinate (texture)
//
//	... etc
//	*/
//	/***************************************************************************/
//	void AddQuad(const Vertex& v1, const Vertex& v2, const Vertex& v3, const Vertex& v4);
//
//	void AddCircle(int points, float z);
//
//	/***************************************************************************/
//	/*!
//	\brief
//	Get the number of triangles of a mesh
//
//	\return
//	number of triangles
//	*/
//	/***************************************************************************/
//	int GetNumOfTriangles();
//
//	/***************************************************************************/
//	/*!
//	\brief
//	Get the number of vertices of a mesh
//
//	\return
//	number of vertices of the mesh
//	*/
//	/***************************************************************************/
//	int GetNumOfVertices();
//
//	/***************************************************************************/
//	/*!
//	\brief
//	Get the number of indices of a mesh
//
//	\return
//	number of indices of the mesh
//	*/
//	/***************************************************************************/
//	int GetNumOfIndices();
//
//	/***************************************************************************/
//	/*!
//	\brief
//	Get the pointer to the vertices array
//
//	\return
//	pointer to the array
//	*/
//	/***************************************************************************/
//	float * GetVertices();
//
//	/***************************************************************************/
//	/*!
//	\brief
//	Get the pointer to the indices array
//
//	\return
//	pointer to the array
//	*/
//	/***************************************************************************/
//	unsigned int * GetIndices();
//
//	bool AssignBuffers();
//
//	GLuint GetVAO() const;
//
//	GLuint GetEBO() const;
//
//	GLuint GetVBO() const;
//
//	void Clear();
//
//	/***************************************************************************/
//	/*!
//	\brief
//	Default destructor
//	*/
//	/***************************************************************************/
//	~Mesh();
//
//private:
//	int m_num_vertex;
//	int m_num_indices;
//	int m_num_triangles;
//	float * vertices;
//	unsigned int * indices;
//
//	GLuint vao;
//	GLuint ebo;
//	GLuint vbo;
//};