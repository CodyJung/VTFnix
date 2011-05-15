#include <cmath>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <errno.h>
#include <dirent.h>
#include <nvtt/nvtt.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include "VTFheader.h"
#include "VTFoutput.cpp"

using namespace std;

#ifndef VTFNIX_CPP
#define VTFNIX_CPP

////////////////////////
/// Global constants ///
////////////////////////

const int ALL_MIPMAPS = 0;
const int SKIP_LARGEST_MIPMAP = 1;
const int ONLY_LARGEST_MIPMAP = 2;


////////////////////////
/// Helper functions ///
////////////////////////

bool isPowerOfTwo(int x){
	return (x != 0) && ((x & (x - 1)) == 0);
}

// Taken from the example at:
// http://www.linuxquestions.org/questions/programming-9/c-list-files-in-directory-379323/
int getdir (string dir, vector<string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        cerr << "Error(" << errno << ") opening " << dir << ": " << strerror(errno) << endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL) {
    	string fileName = string(dirp->d_name);
    	if(fileName.compare("..") != 0 && fileName.compare(".") != 0)
    		files.push_back(dir + "/" + fileName);
    }

    closedir(dp);
    return 0;
}

// Returns the size of the image (for writing the animation header)
int imageSize(string filename){
	ilInit();

	ILuint image = iluGenImage();
	ilBindImage(image);

	if(!ilLoadImage(filename.c_str())){
		cerr << "Error loading input file\n";
		return -1;
	}

	return ilGetInteger(IL_IMAGE_WIDTH);

	ilDeleteImage(image);
}

/////////////////
/// Functions ///
/////////////////

void writeLowResData(int imgSize, string outputFile){
	
	cout << "Writing low res data\n";

	int size;
	if(imgSize <= 16){
		size = imgSize;
	} else {
		size = 16; //I know my inputs are going to be square
	}

	const void* imageData = malloc(ilGetInteger(IL_IMAGE_SIZE_OF_DATA));
	cout << " data_size: " << ilGetInteger(IL_IMAGE_SIZE_OF_DATA) << endl;
	imageData = 0;
	imageData = ilGetData();

	nvtt::InputOptions inputOptions;
	inputOptions.setTextureLayout(nvtt::TextureType_2D, size, size);
	inputOptions.setMipmapData(imageData, size, size);
	inputOptions.setMipmapGeneration(false);
	
	nvtt::OutputOptions outputOptions;
	Vtfoutput out;
	out.setOutputFile(outputFile);
	outputOptions.setOutputHandler(&out);
	outputOptions.setOutputHeader(false);

	nvtt::CompressionOptions compressionOptions;
	compressionOptions.setFormat(nvtt::Format_DXT1);
	compressionOptions.setQuality(nvtt::Quality_Fastest);
	
	nvtt::Compressor compressor;
	compressor.process(inputOptions, compressionOptions, outputOptions);
}

void writeHighResData(int imgSize, string outputFile){
	
	cout << "Writing high res data for size " << imgSize;

	int size = imgSize;

	const void* imageData = malloc(ilGetInteger(IL_IMAGE_SIZE_OF_DATA));
	cout << " data_size: " << ilGetInteger(IL_IMAGE_SIZE_OF_DATA) << endl;
	imageData = 0;
	imageData = ilGetData();

	nvtt::InputOptions inputOptions;
	inputOptions.setTextureLayout(nvtt::TextureType_2D, size, size);
	inputOptions.setMipmapData(imageData, size, size);
	inputOptions.setMipmapGeneration(false);

	nvtt::OutputOptions outputOptions;
	Vtfoutput out;
	out.setOutputFile(outputFile);
	outputOptions.setOutputHandler(&out);
	outputOptions.setOutputHeader(false);

	nvtt::CompressionOptions compressionOptions;
	compressionOptions.setFormat(nvtt::Format_DXT5);
	compressionOptions.setQuality(nvtt::Quality_Fastest);
	
	nvtt::Compressor compressor;
	compressor.process(inputOptions, compressionOptions, outputOptions);
}

void writeHeader(int imgSize, int frames, string outputFile){

	// A lot of this information comes from the wonderful VTFLib and VTFCMD
	// See vtfheader.cpp for more info on each of these
	VTFHEADER header;
	header.version[0] = 7;
	header.version[1] = 2;
	header.headerSize = 0x50;
	header.width = imgSize;
	header.height = imgSize;
	header.flags = 0x2000;
	header.frames = frames;
	header.firstFrame = 0;
	header.padding0[0] = 0;
	header.padding0[1] = 0;
	header.padding0[2] = 0;
	header.padding0[3] = 0;
	header.reflectivity[0] = 1; //TODO Figure out how to calculate reflectivity
	header.reflectivity[1] = 1;
	header.reflectivity[2] = 1;
	header.padding1[0] = 0;
	header.padding1[1] = 0;
	header.padding1[2] = 0;
	header.padding1[3] = 0;
	header.bumpmapScale = 1;
	header.highResImageFormat = 0xF; //DXT5
	header.mipmapCount = (char)(log2(imgSize) + 1);
	header.lowResImageFormat = 0xD; //DXT1
	if(imgSize <= 16){
		header.lowResImageWidth = imgSize;
		header.lowResImageHeight = imgSize;
	} else {
		header.lowResImageWidth = 16; //I know my inputs are going to be square
		header.lowResImageHeight = 16; //So I don't need to bother with scaling stuff
	}
	header.depth = 1;
	header.padding3[0] = 0;
	header.padding3[1] = 0;
	header.padding3[2] = 0;
	header.resourceCount = 0;
	
	//cout << "Header size:" + sizeof(tagVTFHEADER);

	// This is the important stuff
	cout << "Writing Header\n";
	ofstream output;
	output.open(outputFile.c_str(), ios::out | ios::binary);
	output.write(reinterpret_cast<char *>(&header.signature), 4*sizeof(char));
	output.write(reinterpret_cast<char *>(&header.version), 2*sizeof(int));
	output.write(reinterpret_cast<char *>(&header.headerSize), sizeof(int));
	output.write(reinterpret_cast<char *>(&header.width), sizeof(short));
	output.write(reinterpret_cast<char *>(&header.height), sizeof(short));
	output.write(reinterpret_cast<char *>(&header.flags), sizeof(int));
	output.write(reinterpret_cast<char *>(&header.frames), sizeof(short));
	output.write(reinterpret_cast<char *>(&header.firstFrame), sizeof(short));
	output.write(reinterpret_cast<char *>(&header.padding0), 4*sizeof(char));
	output.write(reinterpret_cast<char *>(&header.reflectivity), 3*sizeof(float));
	output.write(reinterpret_cast<char *>(&header.padding1), 4*sizeof(char));
	output.write(reinterpret_cast<char *>(&header.bumpmapScale), sizeof(float));
	output.write(reinterpret_cast<char *>(&header.highResImageFormat), sizeof(int));
	output.write(reinterpret_cast<char *>(&header.mipmapCount), sizeof(char));
	output.write(reinterpret_cast<char *>(&header.lowResImageFormat), sizeof(int));
	output.write(reinterpret_cast<char *>(&header.lowResImageWidth), sizeof(char));
	output.write(reinterpret_cast<char *>(&header.lowResImageHeight), sizeof(char));
	output.write(reinterpret_cast<char *>(&header.depth), sizeof(short));
	output.write("000000000000000", 15*sizeof(char));
	//output.write(reinterpret_cast<char *>(&header.padding3), 3*sizeof(char));
	//output.write(reinterpret_cast<char *>(&header.resourceCount), sizeof(int));
	output.close();
	
}

int singleImage(string filename, string outputFile, int mipmapOptions, bool onlyHighResData){
	ilInit();
	iluInit();

	if(!ilLoadImage(filename.c_str())){
		cerr << "Error loading input file\n";
		return -1;
	} else if(!isPowerOfTwo(ilGetInteger(IL_IMAGE_WIDTH)) || !isPowerOfTwo(ilGetInteger(IL_IMAGE_HEIGHT))){
		cerr << "Width and height must be powers of two\n";
		return -2;
	} else if(ilGetInteger(IL_IMAGE_WIDTH) != ilGetInteger(IL_IMAGE_HEIGHT)){
		cerr << "Width and height must be equal\n";
		return -3;
	} else {
		
		if(!ilConvertImage(IL_BGRA, IL_UNSIGNED_BYTE)){
			cerr << "Error converting image to BGRA format.\n";
			return -4;
		} else {
		
			ilHint(IL_MEM_SPEED_HINT, IL_LESS_MEM);	
			iluBuildMipmaps();
			int numMips = (int)log2(ilGetInteger(IL_IMAGE_HEIGHT)) + 1;

			if(!onlyHighResData){
				writeHeader(ilGetInteger(IL_IMAGE_HEIGHT), 1, outputFile);

				/* 16x16 image or biggest below that */
				if(numMips >= 5){
					ilActiveMipmap(numMips - 5);
				} else {
					ilActiveMipmap(0);
				}
				writeLowResData(ilGetInteger(IL_IMAGE_HEIGHT), outputFile);
			} else {
				/* Don't ask me why we need this. I don't know.
				I figured since we're regenerating the mips later, it's not an issue.
				But for some reason, if I don't do this, it freaks out and only gives me
				part of my data */
				if(numMips >= 5){
					ilActiveMipmap(numMips - 5);
				} else {
					ilActiveMipmap(0);
				}
			}

			int startingMip; int biggestMip;
			if(mipmapOptions == ALL_MIPMAPS){
				startingMip = numMips-1;
				biggestMip = 0;
			} else if(mipmapOptions == SKIP_LARGEST_MIPMAP){
				startingMip = numMips-1;
				biggestMip = 1;
			} else if(mipmapOptions == ONLY_LARGEST_MIPMAP){
				startingMip = 0;
				biggestMip = 0;
			}
	
			for(int i=startingMip; i>=biggestMip; i--){
				ilLoadImage(filename.c_str());
				ilConvertImage(IL_BGRA, IL_UNSIGNED_BYTE);
				iluBuildMipmaps();
				ilActiveMipmap(i);
				writeHighResData(ilGetInteger(IL_IMAGE_HEIGHT), outputFile);
			}
		}
	cout << "Done.";
	return 0;
	}
}

int animatedImage(string folder, string outputFile, int mipmapOptions, bool onlyHighResData) {
	vector<string> files;

	if (getdir(folder, files) != 0) {
		return -5;
	}

	if (files.size() == 1) {
		return singleImage(files.at(0), outputFile, mipmapOptions, onlyHighResData);
	}

	int imageSz = imageSize(files.at(0));
	int numMips = (int) log2(imageSz) + 1; //Base our mipmaps on the first image
	int startingMip; int biggestMip;
	if (mipmapOptions == ALL_MIPMAPS) {
		startingMip = numMips - 1;
		biggestMip = 0;
	} else if (mipmapOptions == SKIP_LARGEST_MIPMAP) {
		startingMip = numMips - 1;
		biggestMip = 1;
	} else if (mipmapOptions == ONLY_LARGEST_MIPMAP) {
		startingMip = 0;
		biggestMip = 0;
	}

	for (int i = startingMip; i >= biggestMip; i--) {

		for (int j = 0; j < files.size(); j++) {

			string filename = files.at(j);

			ilInit();
			iluInit();

			if (!ilLoadImage(filename.c_str())) {
				cerr << "Error loading input file\n";
				return -1;
			} else if (!isPowerOfTwo(ilGetInteger(IL_IMAGE_WIDTH)) || !isPowerOfTwo(ilGetInteger(IL_IMAGE_HEIGHT))) {
				cerr << "Width and height must be powers of two\n";
				return -2;
			} else if (ilGetInteger(IL_IMAGE_WIDTH) != ilGetInteger(IL_IMAGE_HEIGHT)) {
				cerr << "Width and height must be equal\n";
				return -3;
			} else {

				if (!ilConvertImage(IL_BGRA, IL_UNSIGNED_BYTE)) {
					cerr << "Error converting image to BGRA format.\n";
					return -4;
				} else {

					ilHint(IL_MEM_SPEED_HINT, IL_LESS_MEM);
					iluBuildMipmaps();
					if (i == startingMip && j == 0) {
						writeHeader(imageSz, files.size(), outputFile);

						/* 16x16 image or biggest below that */
						if (numMips >= 5) {
							ilActiveMipmap(numMips - 5);
						} else {
							ilActiveMipmap(0);
						}
						writeLowResData(ilGetInteger(IL_IMAGE_HEIGHT), outputFile);
					} else {
						/* Don't ask me why we need this. I don't know.
						 I figured since we're regenerating the mips later, it's not an issue.
						 But for some reason, if I don't do this, it freaks out and only gives me
						 part of my data */
						if (numMips >= 5) {
							ilActiveMipmap(numMips - 5);
						} else {
							ilActiveMipmap(0);
						}
					}

					ILuint img = iluGenImage();
					ilBindImage(img);
					ilLoadImage(filename.c_str());
					ilConvertImage(IL_BGRA, IL_UNSIGNED_BYTE);
					iluBuildMipmaps();
					ilActiveMipmap(i);
					writeHighResData(ilGetInteger(IL_IMAGE_HEIGHT), outputFile);
					ilDeleteImage(img);
				}
			}
		}

	}

	cout << "Done.\n";
	return 0;
}


int fadingImage(string near, string far, string outputFile){
	ilInit();
	iluInit();

	singleImage(far, outputFile, SKIP_LARGEST_MIPMAP, false); //Output the smallest mipmaps (the far images)
	singleImage(near, outputFile, ONLY_LARGEST_MIPMAP, true); //Output the close image (the large mipmap
}


#endif /*VTFNIX_CPP*/
