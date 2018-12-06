
#include "stdafx.h"
#define STB_IMAGE_IMPLEMENTATION
#include "std_image.h"
#include "TextureLoader.h"
#include <string.h>
#define FOURCC_DXT1 0x31545844
#define FOURCC_DXT3 0x33545844
#define FOURCC_DXT5 0x35545844

TextureLoader::TextureLoader(HDC context) :windowDC(context), image(nullptr), mipmapCount{ 0 }, mipmapFormat{ 0 }
{
	GetTexture(defaultTex);
	LoadNewTexture();
	JoinThread();
}

void TextureLoader::Update()
{
	LoadNewTexture();
}

void TextureLoader::JoinThread()
{
	loadResources.join();
	BindTexture();
	loadingMesh.reset();
}

Texture * TextureLoader::GetTexture(const std::string & path)
{
	if (m_textureManager.find(path) == m_textureManager.end())
	{
		texLoadQueue.emplace_back(path);
		auto& tex = m_textureManager[defaultTex];
		m_textureManager[path] = Texture(tex.GetWidth(), tex.GetHeight(), 
			tex.GetNumColorChannel(), nullptr, tex.GetID());
	}
	return &m_textureManager[path];
}

TextureLoader::~TextureLoader()
{
	if(loadingMesh)
		loadResources.join();
}

void TextureLoader::BindTexture()
{
	GLuint id = 0;
	if (!image)
	{
		std::stringstream ss;
		ss << "Could not load image \"" << loadingMesh.value() << "\"!\n";
		TOOLS::CreateErrorMessageBox(ss.str());
		return;
	}

	if (w <= 4096 && h <= 4096)
	{
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		unsigned width = w, height = h;
		if (mipmapFormat)
		{
			unsigned blockSize = (mipmapFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
			unsigned offset = 0;
			for (unsigned level = 0; level < mipmapCount && (w || h); ++level)
			{
				unsigned size = ((width + 3) / 4)*((height + 3) / 4)*blockSize;
				glCompressedTexImage2D(GL_TEXTURE_2D, level, 
					mipmapFormat, width, height, 0, size, image + offset);
				offset += size;
				width /= 2;
				height /= 2;
			}
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else
	{
		std::string tmp{ loadingMesh.value() };
		tmp = tmp.substr(tmp.find_first_of('/') + 1, tmp.size());
		STBI_FREE(image);
		return;
	}

	STBI_FREE(image);
	image = nullptr;
	m_textureManager[loadingMesh.value()] = Texture(w, h, n, nullptr, id);
}

void TextureLoader::LoadNewTexture()
{
	if (!loadingMesh && texLoadQueue.size())
	{
		mainContext = wglGetCurrentContext();
		loaderContext = wglCreateContext(windowDC);
		wglShareLists(loaderContext, mainContext); // Order matters
		task = std::packaged_task<void(std::string ss)>{
			[&](std::string ss) {
			wglMakeCurrent(windowDC, loaderContext);
      // add check, if DDS format will use dds library, otherwise stbi
			if (strstr(ss.c_str(), ".dds"))
			{
				//HRESULT CreateDDSTextureFromFile(_In_ ID3D11Device* d3dDevice,
				//	_In_opt_ ID3D11DeviceContext* d3dContext,
				//	_In_z_ const wchar_t* szFileName,
				//	_Outptr_opt_ ID3D11Resource** texture,
				//	_Outptr_opt_ ID3D11ShaderResourceView** textureView,
				//	_In_ size_t maxsize = 0,
				//	_Out_opt_ DDS_ALPHA_MODE* alphaMode = nullptr
				//);
				//CreateDDSTextureFromFile()
				unsigned char header[124];
				FILE* File;
				auto err = fopen_s(&File,ss.c_str(), "rb");
        if (err) return;
				//check file
				char fileType[4];
				fread(fileType, 1, 4, File);
				if (strncmp(fileType, "DDS ", 4) != 0)
				{
					fclose(File);
					CONSOLE_LN("Invalid or unsupported DDS type.");
					return;
				}
				//texture description
				fread(&header, 124, 1, File);
				h = *(unsigned*)&(header[8]);
				w = *(unsigned*)&(header[12]);
				unsigned linearSize = *(unsigned*)&(header[16]);
				mipmapCount = *(unsigned*)&(header[24]);
				unsigned fourCC = *(unsigned*)&(header[80]);
				unsigned bufferSize;
				//read mipmap
				bufferSize = mipmapCount > 1 ? linearSize * 2 : linearSize;
				image = (unsigned char*)malloc(bufferSize * sizeof(unsigned char));
				fread(image, 1, bufferSize, File);
				fclose(File);
				unsigned components = fourCC == FOURCC_DXT1 ? 3 : 4;
				UNREFERENCED_PARAMETER(components);
				switch (fourCC)
				{
				case FOURCC_DXT1:
					mipmapFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
					break;
				case FOURCC_DXT3:
					mipmapFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
					break;
				case FOURCC_DXT5:
					mipmapFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
					break;
				default:
					free(image);
					image = nullptr;
					CONSOLE_LN("Invalid or unsupported DDS type.");
					return;
				}

			}
			else
			{
				image = stbi_load(ss.c_str(), &w, &h, &n, STBI_rgb_alpha);
				mipmapFormat = mipmapCount = 0;
			}
			wglMakeCurrent(nullptr, nullptr);
			wglDeleteContext(loaderContext);
		} };
		future = task.get_future();
		loadResources = std::thread(std::move(task), texLoadQueue.front());
		loadingMesh = texLoadQueue.front();
		texLoadQueue.erase(texLoadQueue.begin());
	}
	else if (loadingMesh)
	{
		if (!future.valid())
		{
			CONSOLE_LN(loadingMesh.value(), " has an invalid future!");
			loadingMesh.reset();
		}
		else
		{
			auto status = future.wait_for(std::chrono::seconds(0));
			if (status == std::future_status::ready)
			{
				loadResources.join();
				//CONSOLE_LN(textureName);
				//m_textureManager[loadingMesh.value()] = std::make_unique<Texture>(w, h, n, nullptr, image);
				// may need to remove this drawobject if texture cannot bind
				BindTexture();
				loadingMesh.reset();
			}

		}
	}
}

void TextureLoader::RecompileFile(const std::string & str)
{
	auto newPath = Compiler::GetMappedDirectory(str);
	if (m_textureManager.find(newPath) == m_textureManager.end())
	{
		return;
	}
	texLoadQueue.emplace_back(newPath);
	m_textureManager[newPath] = m_textureManager[defaultTex];
}