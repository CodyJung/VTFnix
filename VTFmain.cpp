#include "VTFnix.cpp"

int main(int argc, char* argv[]){

	if(argc < 3){
		cerr << "Usage: -in inputFile.ext [-anim] [-f farImage.ext] [-out outfile.vtf]\n";
		return -1;
	} else {
		char *inputFilename = NULL, *farImage = NULL, *outputFilename = NULL;
		bool anim = false;
		bool fade = false;

		for(int i=1; i<argc; i++){
			if(strcmp(argv[i], "-in") == 0 && i+1 != argc){
				inputFilename = argv[i+1];
			} else if(strcmp(argv[i], "-f") == 0 && i+1 != argc){
				farImage = argv[i+1];
				fade = true;
			} else if(strcmp(argv[i], "-out") == 0 && i+1 != argc){
				outputFilename = argv[i+1];
			} else if(strcmp(argv[i], "-anim") == 0){
				anim = true;
			}
		}


		if(inputFilename == NULL){
			cerr << "Must specify input filename.\n";
			cerr << "Usage: ./VTFnix -in inputFile.ext [-anim] [-f farImage.ext] [-out outfile.vtf]\n";
			return -2;
		}


		if(outputFilename == NULL){
			cerr << "No output file specified, outputting to OUTPUT.vtf\n";
			outputFilename = (char *)"OUTPUT.vtf"; // Workaround typecast to avoid a deprecation warning
		}


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
