#include <nvtt/nvtt.h>
#include <iostream>
#include <fstream>
#include <string.h>

using namespace std;

struct Vtfoutput : public nvtt::OutputHandler {

	char outputF[512]; //This could technically cause a problem, but fuck it

	Vtfoutput(){
		strcpy(outputF, "DEFAULTOUT.vtf");
	}
	
	void beginImage(int size, int width, int height, int depth, int face, int miplevel){
	//	cout << "HERP";
	}
	
	bool writeData(const void * data, int size){
		ofstream output;
		output.open(outputF, ios::out | ios::binary | ios::app);
		output.write(reinterpret_cast<const char *>(data), size);
		output.close();
		return true;
	}

	void setOutputFile(char* outputFile){
		strcpy(outputF, outputFile);
	}

};
