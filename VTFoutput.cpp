#include <nvtt/nvtt.h>
#include <iostream>
#include <fstream>
#include <string.h>

using namespace std;

struct Vtfoutput : public nvtt::OutputHandler {

	string outputFilename;

	Vtfoutput(){
		outputFilename = "OUTPUT.vtf";
	}
	
	void beginImage(int size, int width, int height, int depth, int face, int miplevel){
		// We don't need anything here, I don't *think*
	}
	
	bool writeData(const void * data, int size){
		ofstream output;
		output.open(outputFilename.c_str(), ios::out | ios::binary | ios::app);
		output.write(reinterpret_cast<const char *>(data), size);
		output.close();
		return true;
	}

	void setOutputFile(string outputFile){
		if(!outputFile.empty())
			outputFilename = outputFile;
	}

};
