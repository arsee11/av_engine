#include "av_video_decode_filter_nv.h"

#include <iostream>
#include "av_log.h"

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
bool AvVideoDecodeFilterNv::transform(AVParam* p)
{
    if(p->type != MEDIA_VIDEO){
        return false;
    }

    int nframe_returned=0, nframe=0;
    nframe_returned = _nvdecoder->Decode(p->data_ptr(), p->size());
    if(nframe != 0 && nframe_returned>0){
        av_log_info()<<_nvdecoder->GetVideoInfo();
    }
    for( int i=0; i<nframe_returned; i++){
        uint8_t* frame_data = _nvdecoder->GetFrame();
        int frame_size = _nvdecoder->GetFrameSize();
        _param.data(frame_data, frame_size);
        _param.format = nv2format(_nvdecoder->GetOutputFormat());
    }
    nframe += nframe_returned;
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
    _nvdecoder.reset( new NvDecoder (
                            cu_context, 
                            false, 
                            _2nvcodec_id(cid), 
                            false, 
                            false, 
                            &crop_rect, 
                            &resize_dim, 
                            false/*bExtractUserSEIMessage*/
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
