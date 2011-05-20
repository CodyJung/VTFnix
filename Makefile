all:VTFnix

VTFnix:VTFmain.o VTFnix.o VTFheader.h VTFoutput.o
	g++ VTFmain.o -O2 -L/usr/local/lib/ -lnvtt -lIL -lILU -o"VTFnix"

VTFmain.o:VTFmain.cpp
	g++ -c -O2 VTFmain.cpp

VTFnix.o:VTFnix.cpp
	g++ -c -O2 VTFnix.cpp

VTFoutput.o:VTFoutput.cpp
	g++ -c -O2 VTFoutput.cpp

clean:
	rm -rf *.o
