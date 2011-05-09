all:VTFnix

VTFnix:VTFmain.o VTFnix.o VTFheader.h VTFoutput.o
	g++ -g VTFmain.o -L/usr/local/lib/ -lnvtt -lIL -lILU -o"VTFnix"

VTFmain.o:VTFmain.cpp
	g++ -c -g VTFmain.cpp

VTFnix.o:VTFnix.cpp
	g++ -c -g VTFnix.cpp

VTFoutput.o:VTFoutput.cpp
	g++ -c -g VTFoutput.cpp

clean:
	rm -rf *.o
