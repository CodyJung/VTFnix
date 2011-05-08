#include "vtfcmdnix.cpp"

int main(int argc, char* argv[]){

	if(argc < 3){
		cerr << "Usage: -in inputFile.ext [-anim] [-f farImage.ext] [-out outfile.vtf]\n";
		return -1;
	} else {
		char *inputFilename, *farImage, *outputFilename;
		bool anim = false;
		bool fade = false;

		for(int i=1; i<argc; i++){
			if(argv[i] == "-in" && i+1 != argc){
				inputFilename = argv[i+1];
			} else if(argv[i] == "-f" && i+1 != argc){
				farImage = argv[i+1];
				fade = true;
			} else if(argv[i] == "-out" && i+1 != argc){
				outputFilename = argv[i+1];
			} else if(argv[i] == "-anim"){
				anim = true;
			}
		}


		if(inputFilename == NULL){
			cerr << "Must specify input filename.\n";
			cerr << "Usage: -in inputFile.ext [-anim] [-f farImage.ext] [-out outfile.vtf]\n";
			return -2;
		}


		if(outputFilename == NULL){
//			strcpy(outputFilename, inputFilename);
//			strcat(outputFilename, ".vtf");
		}


		cout << &inputFilename;
		if(anim == true){
			singleAnimation(inputFilename, outputFilename, 0, false);
		} else if(fade == true){
			fadingImage(inputFilename, farImage, outputFilename);
		} else {
			singleImage(inputFilename, outputFilename, 0, false);
		}

		return 0;			

	}


}
