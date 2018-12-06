#pragma once
#ifdef EDITOR
#include <il.h>
#include <ilu.h>
#include <ilut.h>

/*
	We would need ILU just because of iluErrorString() function...
	So make it possible for both with and without ILU!
*/
#ifdef ILU_ENABLED
#define PRINT_ERROR_MACRO(err)	CONSOLE_ERROR("DevIL Error: ", err, " - ", (const char*)iluErrorString(err));
#else	// not ILU_ENABLED
#define PRINT_ERROR_MACRO(err)	CONSOLE_ERROR("DevIL Error: 0x", (unsigned int)(err));
#endif	// not ILU_ENABLED

namespace
{
	struct ILImage
	{
		ILsizei num = 1;			// number of images to load
		ILuint imagename = 0;		// the unique ID for IL
		ILenum format;				// the current type of the format image
		std::string filename;		// the filename of the image

		bool IsBinded() const		{ return imagename; }
	};

	enum DevILMode { DevILMode_Save = 0, DevILMode_Load };

	void DevILCheckError(ILboolean result)
	{
		if (!result)
		{
			ILenum iError;
			while ((iError = ilGetError()) != IL_NO_ERROR)
				PRINT_ERROR_MACRO(iError)
		}
	}
}

struct DevILFilters
{
	static ILboolean	Alienify(ILImage&);
	static ILboolean	BlurAvg(ILImage&, ILuint iter);
	static ILboolean	BlurGaussian(ILImage&, ILuint Iter);
	static ILboolean	Contrast(ILImage&, ILfloat Contrast);
	static ILboolean	EdgeDetectE(ILImage&);
	static ILboolean	EdgeDetectP(ILImage&);
	static ILboolean	EdgeDetectS(ILImage&);
	static ILboolean	Emboss(ILImage&);
	static ILboolean	Gamma(ILImage&, ILfloat Gamma);
	static ILboolean	Negative(ILImage&);
	static ILboolean	Noisify(ILImage&, ILubyte Factor);
	static ILboolean	Pixelize(ILImage&, ILuint PixSize);
	static ILboolean	Saturate(ILImage&, ILfloat Saturation);
	static ILboolean	Saturate(ILImage&, ILfloat r, ILfloat g, ILfloat b, ILfloat Saturation);
	static ILboolean	ScaleColours(ILImage&, ILfloat r, ILfloat g, ILfloat b);
	static ILboolean	Sharpen(ILImage&, ILfloat Factor, ILuint Iter);
};

// Wrapper class for Dev IL - Singleton class
class DevIL final
{
	static std::unique_ptr<DevIL> devIL;
	ILImage image;

	ILenum static ConvertFormat(DevILMode mode, const std::string& format);
	bool static FormatSupported(DevILMode mode, const std::string& format);

public:
	static DevIL& GetDevIL();
	void Initialize();
	void static DestroyDevIL();

	// Loads an image into Dev IL (to perform other actions - this must be the first to call before anything else)
	bool static DevILLoadImage(const std::string& filename, ILsizei num_images = 1);
	// Outputs the image file sa given by the parameter (out)
	bool static DevILSaveImage(const std::string& out);
	// Retrieve the image's information (width and height) [Returns {-1, -1} if there's no image loaded]
	Vector2 static DevILImageInformation();
	// Unload the image from DevIL
	bool static DevILUnloadImage();
};
#endif