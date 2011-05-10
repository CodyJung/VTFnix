#include <cmath>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <nvtt/nvtt.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include "VTFheader.h"
#include "VTFoutput.cpp"

using namespace std;

#ifndef VTFNIX_CPP
#define VTFNIX_CPP

bool isPowerOfTwo(int x){
	return (x != 0) && ((x & (x - 1)) == 0);
}

void writeLowResData(int imgSize, char* outputFile){
	
	cout << "Writing low res data\n";

	int size;
	if(imgSize <= 16){
		size = imgSize;
	} else {
		size = 16; //I know my inputs are going to be square
	}

	nvtt::InputOptions inputOptions;
	inputOptions.setTextureLayout(nvtt::TextureType_2D, size, size);
	inputOptions.setMipmapData(ilGetData(), size, size);
	inputOptions.setMipmapGeneration(false);
	
	nvtt::OutputOptions outputOptions;
	Vtfoutput out;
	out.setOutputFile(outputFile);
	outputOptions.setOutputHandler(&out);
	outputOptions.setOutputHeader(false);

	nvtt::CompressionOptions compressionOptions;
	compressionOptions.setFormat(nvtt::Format_DXT1);
	
	nvtt::Compressor compressor;
	compressor.process(inputOptions, compressionOptions, outputOptions);
}

void writeHighResData(int imgSize, char* outputFile){
	
	cout << "Writing high res data for size " << imgSize << "\n";

	int size = imgSize;

	const void* imageData = malloc(ilGetInteger(IL_IMAGE_SIZE_OF_DATA));
	cout << ilGetInteger(IL_IMAGE_SIZE_OF_DATA) << " ";
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

void writeHeader(int imgSize, int frames, char* outputFile){

	// A lot of this information comes from the wonderful VTFLib and VTFCMD
	// See vtfheader.cpp for more info on each of these
	VTFHEADER header;
	header.version[0] = 7;
	header.version[1] = 2;
	header.headerSize = 0x50;
	header.width = imgSize;
	header.height = imgSize;
	header.flags = 0x2200;
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
	output.open(outputFile, ios::out | ios::binary | ios::app);
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

int singleImage(char *filename, char *outputFile, int skipBiggest, bool skipHeaderAndLow){
	ilInit();
	iluInit();

	if(!ilLoadImage(filename)){
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

			if(!skipHeaderAndLow){
				writeHeader(ilGetInteger(IL_IMAGE_HEIGHT), 1, outputFile);

				/* 16x16 image or biggest below that */
				if(numMips >= 5){
					ilActiveMipmap(numMips - 5);
				} else {
					ilActiveMipmap(0);
				}
				writeLowResData(ilGetInteger(IL_IMAGE_HEIGHT), outputFile);
			}
	
			int startingMip = numMips-1;
			int biggestMip = 0;
			if(skipBiggest == 1){
				biggestMip = 1;
			} else if(skipBiggest == 2){
				startingMip = 0;
			}
	
			for(int i=startingMip; i>=biggestMip; i--){
				ilLoadImage(filename);
				iluBuildMipmaps();
				ilActiveMipmap(i);
				writeHighResData(ilGetInteger(IL_IMAGE_HEIGHT), outputFile);
			}
		}
	cout << "Done.";
	return 0;
	}
}

int singleAnimation(char *filename, char *outputFile, int skipBiggest, bool skipHeaderAndLow){
	ilInit();
	iluInit();

	if(!ilLoadImage(filename)){
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
			int numFrames = ilGetInteger(IL_NUM_IMAGES);
			if(numFrames == 0)
				return singleImage(filename, outputFile, skipBiggest, skipHeaderAndLow);
					
			iluBuildMipmaps();
			int numMips = (int)log2(ilGetInteger(IL_IMAGE_HEIGHT)) + 1;
			
			// Non-fading sprays
			if(!skipHeaderAndLow){
				writeHeader(ilGetInteger(IL_IMAGE_HEIGHT), numFrames, outputFile);

				/* 16x16 image or biggest below that */
				if(numMips >= 5){
					ilActiveMipmap(numMips - 5);
				} else {
					ilActiveMipmap(0);
				}
				writeLowResData(ilGetInteger(IL_IMAGE_HEIGHT), outputFile);
			}

			// Setting up the logic for fading sprays
			int startingMip = numMips;
			int biggestMip = 0;
			if(skipBiggest == 1){ // Will skip the largest mipmap
				biggestMip = 1;
			} else if(skipBiggest == 2){ // Will only do the largest mipmap
				startingMip = 1;
			}

			// Rolling it all together
			for(int i=startingMip; i>=biggestMip; i--){
				for(int j=0; j<numFrames; j++){
					ILuint image = iluGenImage();
					ilBindImage(image);	

					ilLoadImage(filename);
					ilActiveImage(j);
					iluScale(ilGetInteger(IL_IMAGE_HEIGHT)*2, ilGetInteger(IL_IMAGE_WIDTH)*2, 1);
					iluBuildMipmaps();
					ilActiveMipmap(i);
					if(i != 0){
						writeHighResData(ilGetInteger(IL_IMAGE_HEIGHT), outputFile);
						cout << "mip" << i << " frame" << j << " size" << ilGetInteger(IL_IMAGE_HEIGHT) << endl;
					}

					ilDeleteImage(image);
				}
			}
		}
	cout << "Done.\n";
	return 0;
	}
}


int fadingImage(char *near, char *far, char *outputFile){
	ilInit();
	iluInit();

	singleAnimation(far, outputFile, 1, false);
	singleAnimation(near, outputFile, 2, true);
}


#endif /*VTFNIX_CPP*/
