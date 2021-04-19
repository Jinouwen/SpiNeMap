all:
	g++ -std=c++17 -I/usr/include/hdf5/serial/ ncc.cc model.cc -o ncc -lhdf5 -lboost_system -lboost_filesystem
clean:
	rm -rf ncc
