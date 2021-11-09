all:
	g++ -g -std=c++17 -O2 -I/usr/include/hdf5/serial/ -Iinclude/ ncc.cc model.cc -o ncc -lhdf5_serial -lboost_system -lboost_filesystem
clean:
	rm -rf ncc
json:
	g++ -g -std=c++17 -I/usr/include/hdf5/serial/ -Iinclude/ readjson.cc -o readjson -lhdf5_serial -lboost_system -lboost_filesystem
