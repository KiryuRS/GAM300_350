#include "stdafx.h"
#ifdef EDITOR
std::unique_ptr<DevIL> DevIL::devIL;

ILenum DevIL::ConvertFormat(DevILMode mode, const std::string& format)
{
	ILenum ret_val = 0;
	// Refer to FormatSupported Function for the list of supported formats
	switch (mode)
	{
	case DevILMode_Load:
		if (format == ".dds")
			ret_val = IL_DDS;
		else if (format == ".png")
			ret_val = IL_PNG;
		else if (format == ".jpg" || format == ".jpeg" || format == ".jpe")
			ret_val = IL_JPG;
		else if (format == ".bmp")
			ret_val = IL_BMP;
		else if (format == ".tif" || format == ".TIF" || format == ".tiff")
			ret_val = IL_TIF;
		else if (format == ".psd")
			ret_val = IL_PSD;
		else if (format == ".tga")
			ret_val = IL_TGA;
		else if (format == ".gif")
			ret_val = IL_GIF;
		break;

	case DevILMode_Save:
		if (format == ".dds")
			ret_val = IL_DDS;
		else if (format == ".ico")
			ret_val = IL_ICO;
		break;
	}

	return ret_val;
}

bool DevIL::FormatSupported(DevILMode mode, const std::string & format)
{
	switch (mode)
	{
	case DevILMode_Load:
		/*
			Supported Loading types:
			- .bmp	[Windows Bitmap]
			- .cut	[Dr. Halo Cut File]
			- .gif	[Graphics Interchange Format] *
			- .ico, .cur [Icons]
			- .lbm	[Interlaced Bitmap]
			- .lif	[Homeworld File]
			- .lmp	[Doom Walls / Flats]
			- .mdl	[Half-Life Model]
			- .mng	[Mng Animation]
			- .pcd	[PhotoCD]
			- .pcx	[ZSoft PCX]
			- .dds  [DirectDraw Surface]
			- .pic	[PIC]
			- .psd	[Photoshop] *
			- .wal	[Quake2 Texture]
			- .jpg, .jpe, .jpeg *
			- .pal	[Palette]
			- .png	[Portable Network Graphics] *
			- .pbm, .pgm, .ppm, .pnm
			- .raw	[Raw Data]
			- .sgi, .bw, .rgb, .rgba [Silicon Graphics]
			- .tga	[Targa] *
			- .TIF	[.tif, .tiff] *

			BUT for sanity purpose, we are just going to support .dds, .jpeg / .jpg / .jpe, .png, .bmp
		*/
		if (format == ".dds" || format == ".png" ||
			format == ".bmp" || format == ".jpg" ||
			format == ".jpeg" || format == ".jpe"||
			format == ".tga" || format == ".TIF" ||
			format == ".tiff" ||
			format == ".tif" || format == ".gif" ||
			format == ".psd")
			return true;
		break;

	case DevILMode_Save:
		/*
			Supported Saving types:
			- .bmp	[Windows Bitmap]
			- .h	[C-Style Header]
			- .dds  [DirectDraw Surface]
			- .jpg, .jpe, .jpeg
			- .pal	[Palette]
			- .pcx	[ZSoft PCX]
			- .png	[Portable Network Graphics]
			- .pbm, .pgm, .ppm
			- .raw	[Raw Data]
			- .sgi, .bw, .rgb, .rgba [Silicon Graphics]
			- .tga	[Targa]
			- .TIF	[.tif, .tiff]

			BUT for sanity purpose, we are just going to support .dds and .ico
		*/
		if (format == ".dds" || format == ".ico")
			return true;
		break;
	}

	return false;
}

DevIL & DevIL::GetDevIL()
{
	if (!devIL.get())
		devIL = std::make_unique<DevIL>();
	return *devIL;
}

void DevIL::Initialize()
{
	// Initialize the Image Loader
	ilInit();
#ifdef ILU_ENABLED
	iluInit();
#endif 
}

void DevIL::DestroyDevIL()
{
	if (devIL.get())
	{
		// Unload the current image
		DevILUnloadImage();
		// Destroy our singleton
		devIL.reset();
	}
}

bool DevIL::DevILLoadImage(const std::string & filename, ILsizei num_images)
{
	auto& devil = DevIL::GetDevIL();
	// Check our formatting and get the type
	std::string format = TOOLS::GetExtension(filename);
	if (!devil.FormatSupported(DevILMode_Load, format))
	{
		CONSOLE_WARNING("File's format is not supported for load image!");
		return false;
	}
	ILenum type = ConvertFormat(DevILMode_Load, format);

	ILImage devil_image{ num_images, ILuint{}, type, filename };
	devil.image = devil_image;
	// Generate the image name
	ilGenImages(num_images, &devil_image.imagename);

	// Sanity check for the image name
	if (!devil_image.IsBinded())
	{
		TOOLS::Assert(devil_image.IsBinded(), "Should not be 0 for image name!");
		return false;
	}

	// Bind the image
	ilBindImage(devil_image.imagename);
	// Load the image
	ILboolean result = ilLoad(type, (ILconst_string)filename.c_str());
	DevILCheckError(result);
	return (bool)result;
}

bool DevIL::DevILSaveImage(const std::string & out)
{
	auto devil = DevIL::GetDevIL();
	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);

	std::string format = TOOLS::GetExtension(out);
	if (!devil.FormatSupported(DevILMode_Save, format))
	{
		CONSOLE_WARNING("File's format is not supported for load image!");
		return false;
	}
	ILenum type = ConvertFormat(DevILMode_Save, format);
	// Allow overriding of the image file
	ilEnable(IL_FILE_OVERWRITE);
	// Save the image
	ILboolean result = ilSave(type, (ILconst_string)out.c_str());
	DevILCheckError(result);
	return (bool)result;
}

Vector2 DevIL::DevILImageInformation()
{
	auto& devil = DevIL::GetDevIL();
	if (!devil.image.IsBinded())
		return Vector2{ -1, -1 };

	ILuint width = 0, height = 0;
	width = ilGetInteger(IL_IMAGE_WIDTH);
	height = ilGetInteger(IL_IMAGE_HEIGHT);
	return Vector2{ 1.f * width, 1.f * height };
}

bool DevIL::DevILUnloadImage()
{
	auto& devil = DevIL::GetDevIL();
	if (devil.image.IsBinded())
		ilDeleteImages(devil.image.num, &devil.image.imagename);

	return true;
}

ILboolean DevILFilters::Alienify(ILImage &image)
{
	if (!image.IsBinded())	return false;
	return iluAlienify();
}

ILboolean DevILFilters::BlurAvg(ILImage &image, ILuint iter)
{
	if (!image.IsBinded())	return false;
	return iluBlurAvg(iter);
}

ILboolean DevILFilters::BlurGaussian(ILImage &image, ILuint Iter)
{
	if (!image.IsBinded())	return false;
	return iluBlurGaussian(Iter);
}

ILboolean DevILFilters::Contrast(ILImage& image, ILfloat Contrast)
{
	 if (!image.IsBinded()) return false;
	 return iluContrast(Contrast);
}

ILboolean DevILFilters::EdgeDetectE(ILImage& image)
{
	 if (!image.IsBinded()) return false;
	 return iluEdgeDetectE();
}

ILboolean DevILFilters::EdgeDetectP(ILImage& image)
{
	 if (!image.IsBinded()) return false;
	 return iluEdgeDetectP();
}

ILboolean DevILFilters::EdgeDetectS(ILImage& image)
{
	 if (!image.IsBinded()) return false;
	 return iluEdgeDetectS();
}

ILboolean DevILFilters::Emboss(ILImage& image)
{
	 if (!image.IsBinded()) return false;
	 return iluEmboss();
}

ILboolean DevILFilters::Gamma(ILImage& image, ILfloat Gamma)
{
	 if (!image.IsBinded()) return false;
	 return iluGammaCorrect(Gamma);
}

ILboolean DevILFilters::Negative(ILImage& image)
{
	 if (!image.IsBinded()) return false;
	 return iluNegative();
}

ILboolean DevILFilters::Noisify(ILImage& image, ILubyte Factor)
{
	 if (!image.IsBinded()) return false;
	 return iluNoisify(Factor);
}

ILboolean DevILFilters::Pixelize(ILImage& image, ILuint PixSize)
{
	 if (!image.IsBinded()) return false;
	 return iluPixelize(PixSize);
}

ILboolean DevILFilters::Saturate(ILImage& image, ILfloat Saturation)
{
	 if (!image.IsBinded()) return false;
	 return iluSaturate1f(Saturation);
}

ILboolean DevILFilters::Saturate(ILImage& image, ILfloat r, ILfloat g, ILfloat b, ILfloat Saturation)
{
	 if (!image.IsBinded()) return false;
	 return iluSaturate4f(r, g, b, Saturation);
}

ILboolean DevILFilters::ScaleColours(ILImage& image, ILfloat r, ILfloat g, ILfloat b)
{
	 if (!image.IsBinded()) return false;
	 return iluScaleColours(r, g, b);
}

ILboolean DevILFilters::Sharpen(ILImage& image, ILfloat Factor, ILuint Iter)
{
	 if (!image.IsBinded()) return false;
	 return iluSharpen(Factor, Iter);
}
#endif
