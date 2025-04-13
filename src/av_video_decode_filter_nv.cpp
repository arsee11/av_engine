#include "av_video_decode_filter_nv.h"

#include <iostream>
#include "av_log.h"

#define _DUMP 0

#define nv_ck(r) \
    if(r != CUDA_SUCCESS){ \
        const char* err; \
        cuGetErrorString(r, &err); \
        av_log_error()<< "AvVideoDecodeFilterNv open failed:"<<err<<end_log(); \
        return false; \
    }

PixelFormat nv2format(int f)
{
    switch (f) {
        case cudaVideoSurfaceFormat_NV12: return FORMAT_NV12;

        default:
            return FORMAT_NONE;
    }
}
#if _DUMP

static void dump_frame(AVParam* p, NvDecoder* dec)
{
    static std::ofstream fpOut("out.decode", std::ios::out | std::ios::binary);
    uint8_t* pFrame = p->data_ptr();
    if (dec->GetWidth() == dec->GetDecodeWidth())
    {
        fpOut.write(reinterpret_cast<char*>(pFrame), p->size());
    }
    else
    {
        // 4:2:0/4:2:2 output width is 2 byte aligned. If decoded width is odd , luma has 1 pixel padding
        // Remove padding from luma while dumping it to disk
        // dump luma
        for (auto i = 0; i < dec->GetHeight(); i++)
        {
            fpOut.write(reinterpret_cast<char*>(pFrame), dec->GetDecodeWidth() * dec->GetBPP());
            pFrame += dec->GetWidth() * dec->GetBPP();
        }
        // dump Chroma
        fpOut.write(reinterpret_cast<char*>(pFrame), dec->GetChromaPlaneSize());
    }
}
#endif 

bool AvVideoDecodeFilterNv::transform(AVParam* p)
{
    if(p->type != MEDIA_VIDEO
       ||p->data_ptr() == nullptr 
       ||p->size() == 0
    ){
        return false;
    }

    int nframe_returned=0, nframe=0;
    nframe_returned = _nvdecoder->Decode(p->data_ptr(), p->size(), 0, p->pts);
    if (nframe_returned == 0) {
        return false;
    }

    if (nframe_returned > 1) {
        av_log_info() << "nvdecoder return " << nframe_returned << " frames."<<end_log();
    }

    for( int i=0; i<nframe_returned; i++){
        uint8_t* frame_data = _nvdecoder->GetFrame();
        int frame_size = _nvdecoder->GetFrameSize();
        _param.data(frame_data, frame_size);
        _param.format = nv2format(_nvdecoder->GetOutputFormat());
        _param.w = _nvdecoder->GetWidth();
        _param.h = _nvdecoder->GetHeight();
#if _DUMP
        dump_frame(&_param, _nvdecoder.get());
#endif

    }
    
    return true;
}

/**
*   @brief  Utility function to create CUDA context
*   @param  cuContext - Pointer to CUcontext. Updated by this function.
*   @param  iGpu      - Device number to get handle for
*/
static bool create_cuda_context(CUcontext* cuContext, int iGpu, unsigned int flags)
{
    CUdevice cuDevice = 0;
    cuDeviceGet(&cuDevice, iGpu);
    char szDeviceName[80];
    cuDeviceGetName(szDeviceName, sizeof(szDeviceName), cuDevice);
    av_log_info()<< "GPU in use: " << szDeviceName << end_log();
    nv_ck( cuCtxCreate(cuContext, flags, cuDevice) );
    return true;
}

cudaVideoCodec _2nvcodec_id(CodecID cid)
{
    switch(cid)
    {
        case H264: return cudaVideoCodec_H264;
        case H265: return cudaVideoCodec_HEVC;
        case VP9: return cudaVideoCodec_VP9;
        case AV1: return cudaVideoCodec_AV1;

        default: return cudaVideoCodec_NumCodecs;
    }
}

bool AvVideoDecodeFilterNv::open(CodecID cid, int w, int h)
{
    nv_ck( cuInit(0) );
    int nGpu = 0;
    nv_ck( cuDeviceGetCount(&nGpu) );
    av_log_info() << "number of GPU:"<<nGpu<<end_log();
    int iGpu = 0;
    CUcontext cu_context = NULL;
    if( !create_cuda_context(&cu_context, iGpu, 0) )
        return false;

    Rect crop_rect = {};
    Dim resize_dim = {};
	bool extract_user_SEI_message=false;
	unsigned int decsurf=0;
	bool extstream=false;
	CUstream custream = NULL;
    
    if(extstream)
    {
        ck(cuCtxPushCurrent(cu_context));
        ck(cuStreamCreate(&custream, CU_STREAM_DEFAULT));
    }
					
	 _nvdecoder.reset( new NvDecoder (cu_context, 
							false, 
							_2nvcodec_id(cid),
							false, 
							false, 
							&crop_rect, 
							&resize_dim, 
							extract_user_SEI_message, 
							0, 
							0, 
							1000, 
							false, 
							decsurf, 
							custream
                          )
                    );

    return true;
}

bool AvVideoDecodeFilterNv::open(const CodecInfo& ci)
{
    _codec_id = ci.codec;
    _param.fps = ci.fps;
    _param.w = ci.w;
    _param.h= ci.h;
    _param.format = ci.pix_format;
    _param.type = MediaType::MEDIA_VIDEO;
    return open(ci.codec, ci.w, ci.h);
}

void AvVideoDecodeFilterNv::close()
{
    _nvdecoder.reset();
}
