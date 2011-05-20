#include <string.h>


#ifndef VTFHEADER_H
#define VTFHEADER_H
typedef struct tagVTFHEADER
{
	char		signature[4];		// File signature ("VTF\0").
	unsigned int	version[2];		// version[0].version[1] (currently 7.2).
	unsigned int	headerSize;		// Size of the header struct (16 byte aligned; currently 80 bytes).
	unsigned short	width;			// Width of the largest mipmap in pixels. Must be a power of 2.
	unsigned short	height;			// Height of the largest mipmap in pixels. Must be a power of 2.
	unsigned int	flags;			// VTF flags.
	unsigned short	frames;			// Number of frames, if animated (1 for no animation).
	unsigned short	firstFrame;		// First frame in animation (0 based).
	unsigned char	padding0[4];		// reflectivity padding (16 byte alignment).
	float		reflectivity[3];	// reflectivity vector.
	unsigned char	padding1[4];		// reflectivity padding (8 byte packing).
	float		bumpmapScale;		// Bumpmap scale.
	unsigned int	highResImageFormat;	// High resolution image format.
	unsigned char	mipmapCount;		// Number of mipmaps.
	unsigned int	lowResImageFormat;	// Low resolution image format (always DXT1).
	unsigned char	lowResImageWidth;	// Low resolution image width.
	unsigned char	lowResImageHeight;	// Low resolution image height.
	unsigned short	depth;			// Depth of the largest mipmap in pixels.
						// Must be a power of 2. Can be 0 or 1 for a 2D texture (v7.2 only).
	unsigned char 	padding3[3];
	unsigned int 	resourceCount;
	tagVTFHEADER(){
		strcpy(signature, "VTF");
	}
} VTFHEADER;

#endif /*VTFHEADER_H*/
