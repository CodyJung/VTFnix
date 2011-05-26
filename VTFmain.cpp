#include "VTFnix.cpp"

int main(int argc, char* argv[]){

	if(argc < 3){
		cerr << "Usage:\n";
		cerr << "./VTFnix -i inputFile.ext [-f farImage.ext] [-o outfile.vtf] [-n | --no-alpha]\n";
		cerr << "./VTFnix -i inputDirectory [-f farDirectory] [-o outfile.vtf] [-n | --no-alpha]\n";
		cerr << "Inputs for fading sprays must be identical in size (and number of frames, if an animation)\n";
		return 1;
	} else {
		char *inputFilename = NULL, *farImage = NULL, *outputFilename = NULL;
		bool anim = false;
		bool fade = false;
		bool alpha = true;

		for(int i=1; i<argc; i++){
			if(strcmp(argv[i], "-i") == 0 && i+1 != argc && anim != true){
				inputFilename = argv[i+1];
			} else if(strcmp(argv[i], "-f") == 0 && i+1 != argc && fade != true){
				farImage = argv[i+1];
				fade = true;
			} else if(strcmp(argv[i], "-o") == 0 && i+1 != argc){
				outputFilename = argv[i+1];
			} else if(strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--no-alpha") == 0){
				alpha = false;
			}
		}


		if(inputFilename == NULL){
			cerr << "Must specify input filename.\n";
			cerr << "Usage:\n";
			cerr << "./VTFnix -i inputFile.ext [-f farImage.ext] [-o outfile.vtf] [-n | --no-alpha]\n";
			cerr << "./VTFnix -i inputDirectory [-f farDirectory] [-o outfile.vtf] [-n | --no-alpha]\n";
			cerr << "Inputs for fading sprays must be identical in size (and number of frames, if an animation)\n";
			return 2;
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

		if(fade == true){
			fadingImage(*filename, *far, *out, alpha);
		} else {
			animatedImage(*filename, *out, ALL_MIPMAPS, false, alpha); //inputFilename in this case being a folder name
		}

		return 0;			

	}


}
