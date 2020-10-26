CXX = g++
CXXFLAGS=-std=c++11 -g

IncDir=-I../../include -I /usr/local/include/SDL2
LinkDir=-L../../lib
Libs= -lavengine -lavcodec -lavdevice -lswresample -lavformat -lswscale -lavutil -lSDL2

player:audio_player.o
	$(CXX) -o player audio_player.o $(LinkDir) $(Libs)

audio_player.o: audio_player.cpp
	$(CXX) -c $(CXXFLAGS) $(IncDir) audio_player.cpp 

.PHONY clean:
clean:
	rm -f *.o player
