///h264_codec.h

#ifndef H264_CODEC_H
#define H264_CODEC_H


const char h264_start4[4] = {0,0,0,1};
const char h264_start3[3] = {0,0,1};


struct RTPH264Header
{
    char F:1;
    char NRI:2;
    char type:5;
};



#endif /*H264_CODEC_H*/
