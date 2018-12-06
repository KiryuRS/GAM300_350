/*****************************************************************************/
/*!
\file CETexture.h
\author Zhou Zhihua, zhihua.z, 390001016
\par zhihua.z\@digipen.edu
\date August 12, 2017
\brief

This file is an interface for Graphics engine texture object, a texture is loaded
in the beginning of the program and it will be unloaded at the end.

It should only be default constructed and loaded thru GraphicsEngine
CELoadTextureFromFile function
It should only be unloaded thru GraphicsEngine CEUnloadTexture function
That is, the Clear function should never be called by the user

Copyright (C) 2017 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/*****************************************************************************/
#pragma once

typedef unsigned int GLuint;

class Texture
{
public:
	/***************************************************************************/
	/*!
	\brief
	Constructor of a CETexture

	\param w
	width

	\param h
	height

	\param n
	number of color channels

	\param d
	pointer to the texture data
	*/
	/***************************************************************************/
	Texture(int w, int h, int n, unsigned char * d, int textureID);

	Texture():width(0), height(0), nrChannels(0), data(nullptr), texture_id(0)
	{}

	/***************************************************************************/
	/*!
	\brief
	destructor
	*/
	/***************************************************************************/
	~Texture();

	/***************************************************************************/
	/*!
	\brief
	Get width of a texture

	\return
	width
	*/
	/***************************************************************************/
	int GetWidth() const;


	/***************************************************************************/
	/*!
	\brief
	Get height of a texture

	\return
	height
	*/
	/***************************************************************************/
	int GetHeight() const;

	/***************************************************************************/
	/*!
	\brief
	Get number of channels

	\return
	number of channels
	*/
	/***************************************************************************/
	int GetNumColorChannel() const;

	/***************************************************************************/
	/*!
	\brief
	Get pointer to the image data

	\return
	address of the image data
	*/
	/***************************************************************************/
	unsigned char * GetImagePointer();

	/***************************************************************************/
	/*!
	\brief
	Get an element of the texture

	\return
	element
	*/
	/***************************************************************************/
	unsigned char operator[](int index) const;

	GLuint GetID();

	/***************************************************************************/
	/*!
	\brief
	Clear everything that is allocated for the texture
	This should not be called by the user,
	Unload a texture using CETextureUnload please.
	*/
	/***************************************************************************/
	void Clear();

private:
	int width;
	int height;
	int nrChannels;  // number of color channels
	unsigned char * data;

	GLuint texture_id;
};
