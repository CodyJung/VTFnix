#include "VTFnix.cpp"

int main(int argc, char* argv[]){

	if(argc < 3){
		cerr << "Usage: [-in inputFile.ext | -anim inputDirectory] [-f farImage.ext | -fanim farDirectory] [-out outfile.vtf]\n";
		return -1;
	} else {
		char *inputFilename = NULL, *farImage = NULL, *outputFilename = NULL;
		bool anim = false;
		bool fade = false;

		for(int i=1; i<argc; i++){
			if(strcmp(argv[i], "-in") == 0 && i+1 != argc && anim != true){
				inputFilename = argv[i+1];
			} else if(strcmp(argv[i], "-f") == 0 && i+1 != argc && fade != true){
				farImage = argv[i+1];
				fade = true;
			} else if(strcmp(argv[i], "-out") == 0 && i+1 != argc){
				outputFilename = argv[i+1];
			} else if(strcmp(argv[i], "-anim") == 0 && i+1 != argc){
				anim = true;
				inputFilename = argv[i+1];
			} else if(strcmp(argv[i], "-fanim") == 0 && i+1 != argc){
				fade = true;
				farImage = argv[i+1];
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


		// Convert everything to strings because why not

		string *filename = new string(inputFilename);
		string *out = new string(outputFilename);
		string *far;
		if(farImage != NULL)
			far = new string(farImage);

		if(anim == true){
			animatedImage(*filename, *out, ALL_MIPMAPS, false); //inputFilename in this case being a folder name
		} else if(fade == true){
			fadingImage(*filename, *far, *out);
		} else {
			singleImage(*filename, *out, ALL_MIPMAPS, false);
		}

		return 0;			

	}


}
