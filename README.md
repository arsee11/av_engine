av_engine
====

Audio and video util codes. 

Depends on these thirdparty libs below:<br>
[ffmpeg](https://www.ffmpeg.org) <br>
[jrtplib](http://research.edm.uhasselt.be/jori/page/Main/HomePage.html)

examples/rtp_client and example/width_sdl_player project depends on [SDL](http://www.libsdl.org/) libs.

Requires c++11 support.

Had compiled & run with g++ 4.8 VS2015 XCode 8.


cmake -G "Visual Studio 17 2022" -A x64  -DFFMPEG_DIR="D:\libs\ffmpeg-7.1.1-dev" -DCUDA_DIR="C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.8" ../