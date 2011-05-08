all:a.out clean

a.out:vtfmain.o vtfcmdnix.o vtfheader.o vtfoutput.o
	g++ -g vtfmain.o -L/usr/local/lib -lnvtt -lIL -lILU

vtfmain.o:vtfmain.cpp
	g++ -c -g vtfmain.cpp

vtfcmdnix.o:vtfcmdnix.cpp
	g++ -c -g vtfcmdnix.cpp

vtfheader.o:vtfheader.cpp
	g++ -c -g vtfheader.cpp

vtfoutput.o:vtfoutput.cpp
	g++ -c -g vtfoutput.cpp

clean:
	rm -rf *.o
