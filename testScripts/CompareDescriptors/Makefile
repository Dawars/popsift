all: test

test: compareSiftFiles
	sh ./compareWithVLFeat.sh

compareSiftFiles: compareSiftFiles.cpp
	g++ -std=c++11 -o compareSiftFiles compareSiftFiles.cpp

realclean: clean
	rm -f compareSiftFiles

clean:
	rm -f sort*.txt
	rm -f sort*.png
	rm -f *.sift
	rm -f descdist*.txt
	rm -f descdist*.png
	rm -f tmp/*.sift tmp/UML*.txt tmp/sort*.txt tmp/coord*.txt tmp/sum*.txt
	rm -f tmp/descdist*.txt

