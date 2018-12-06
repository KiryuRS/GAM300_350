/******************************************************************************/
/*!
\file		CETexture.h
\author Zhou Zhihua, zhihua.z, 390001016
\par zhihua.z\@digipen.edu
\date August 12, 2017
\brief

This file is an interface for Graphics engine texture object, a texture is loaded
in the beginning of the program and it will be unloaded at the end.

It should only be default constructed and loaded thru GraphicsEngine
GFXLoadTextureFromFile function
It should only be unloaded thru GraphicsEngine GFXUnloadTexture function
That is, the Clear function should never be called by the user

Copyright (C) 2017 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/*****************************************************************************/
#include "stdafx.h"
#include "Texture.h"

Texture::Texture(int w, int h, int n, unsigned char * d, int textureID)
	: width(w),
	height(h),
	nrChannels(n),
	data(d),
	texture_id(textureID)
{}

Texture::~Texture()
{
	Clear();
}

int Texture::GetWidth() const
{
	return width;
}

int Texture::GetHeight() const
{
	return height;
}

int Texture::GetNumColorChannel() const
{
	return nrChannels;
}

unsigned char * Texture::GetImagePointer()
{
	return data;
}

unsigned char Texture::operator[](int index) const
{
	return data[index];
}

GLuint Texture::GetID()
{
	return texture_id;
}

void Texture::Clear()
{
	width = 0;
	height = 0;
	nrChannels = 0;

	if (data)
	{
		delete[] data;
		data = 0;
	}
}