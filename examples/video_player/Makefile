CXX = g++
CXXFLAGS=-std=c++11 -g #-fsanitize=address

IncDir=-I../../include -I/usr/local/include/SDL2 \
	   -I../../third_party/NvCodec \
	   -I/usr/local/cuda/include
LinkDir=-L../../lib -L../../third_party/NvCodec/Lib/linux/x86_64/
Libs= -lavengine -lnvcodec -lnvcuvid -lcuda \
	  -lavcodec -lavdevice -lswresample -lavformat -lswscale -lavutil -lSDL2 

player:video_player.o
	$(CXX) -o player video_player.o $(LinkDir) $(Libs)

video_player.o: video_player.cpp
	$(CXX) -c $(CXXFLAGS) $(IncDir) video_player.cpp 

.PHONY clean:
clean:
	rm -f *.o player
