CC=g++
CPPFLAGS=--std=c++11 -g -DLINUX
TARGET=rtp_server
OBJS=rtp_server.o

ffmpeg_link= -lavcodec -lavdevice -lswresample -lavformat -lswscale -lavutil 
rtp_link=-ljrtp
avengine_h_dir=-I../../include
avengine_lib_dir=-L../../src

$(TARGET):$(OBJS) 
	$(CC) -o $(TARGET) $(OBJS) $(avengine_lib_dir) -lavengine $(ffmpeg_link) $(rtp_link) 

rtp_server.o: rtp_server.cpp
	$(CC) -c $(CPPFLAGS) $(avengine_h_dir) rtp_server.cpp

clean:
	rm -rf $(TARGET) $(OBJS)
