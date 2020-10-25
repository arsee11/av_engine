CXX = g++
CXXFLAGS=-std=c++11 -g

IncDir=-I../../include
LinkDir=-L../../lib
Libs= -lavengine -lavcodec -lavdevice -lswresample -lavformat -lswscale -lavutil -lSDL2

player:with_sdl_player.o
	$(CXX) -o player with_sdl_player.o $(LinkDir) $(Libs)

with_sdl_player.o: with_sdl_player.cpp
	$(CXX) -c $(CXXFLAGS) $(IncDir) with_sdl_player.cpp 

.PHONY clean:
clean:
	rm -f *.o player
