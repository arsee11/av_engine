///  av_video_encode_filter.cpp
#include "av_video_encode_filter_nv.h"

#include "codec_specify.h"
#include "av_log.h"

#define _DUMP 0

#if _DUMP

static void dump_frame(AVParam* p)
{
	static std::ofstream fpOut("out.h264", std::ios::out | std::ios::binary);
	uint8_t* pFrame = p->data_ptr();
	fpOut.write(reinterpret_cast<char*>(pFrame), p->size());	
}
#endif


bool AvVideoEncodeFilterNV::transform(AVParam* p)
{
	static int m = 0, n = 0;
	av_log_info() << "enc req:" <<m<< end_log();
	m++;
	if (_nvencoder == nullptr){
		if (!open((PixelFormat)p->format, p->w, p->h, _framerate))
			return false;
	}
		   
	bool isok = false;
	AVPixelFormat f = _2ffmpeg_format((PixelFormat)p->format);
	AVFrame *frame = av_frame_alloc();	
	frame->width = p->w;
	frame->height= p->h;
	frame->format = f;
	frame->pts = _frame_count++;
	
	int frame_size = _nvencoder->GetFrameSize();
	if (frame_size != p->size()) {
		av_log_error() << "[VENC_NV] Invaild frame size: req=" << frame_size << ", given=" << p->size() << end_log();
		return false;
	}

	uint32_t enableMVHEVC = _nvencoder->IsMVHEVC();

	int viewID = 0;
	std::streamsize nRead = 0;

	const NvEncInputFrame* encoderInputFrame = _nvencoder->GetNextInputFrame();
	NvEncoderCuda::CopyToDeviceFrame(_cu_context,
		p->data_ptr() + viewID * frame_size, 0,
		(CUdeviceptr)encoderInputFrame->inputPtr,
		(int)encoderInputFrame->pitch,
		_nvencoder->GetEncodeWidth(),
		_nvencoder->GetEncodeHeight(),
		CU_MEMORYTYPE_HOST,
		encoderInputFrame->bufferFormat,
		encoderInputFrame->chromaOffsets,
		encoderInputFrame->numChromaPlanes
	);
   
	_nvencoder->EncodeFrame(_nv_packet);

	for (const NvEncOutputFrame &packet : _nv_packet)
	{
		_param.data(const_cast<uint8_t*>(packet.frame.data()), packet.frame.size());
		isok = true;
		av_log_info() << (int)_nv_packet.size()<<","<<(int)_param.size() << " enc frame:" << n << end_log();
		n+=_param.size();
#if _DUMP
		dump_frame(&_param);
#endif
	}
   
	if (enableMVHEVC)
	{
		viewID = viewID ^ 1;
	}
	return isok;    
}

//NV encoder input buffer format
NV_ENC_BUFFER_FORMAT format2nv(PixelFormat f)
{
    switch (f) {
        case FORMAT_NV12: return NV_ENC_BUFFER_FORMAT_NV12;
		case FORMAT_YUV420P: return NV_ENC_BUFFER_FORMAT_IYUV;
		case FORMAT_YUV444P: return NV_ENC_BUFFER_FORMAT_YUV444;
		case FORMAT_ARGB: return NV_ENC_BUFFER_FORMAT_ARGB;

        default:
            return NV_ENC_BUFFER_FORMAT_UNDEFINED;
    }
}

GUID _2encoder_GUID(CodecID id)
{
	switch(id)
    {
        case H264: return NV_ENC_CODEC_H264_GUID;
        case H265: return NV_ENC_CODEC_HEVC_GUID;
        case VP9: return GUID();
        case AV1: return NV_ENC_CODEC_AV1_GUID;
		default: return GUID();
	}
}

#define nv_ck(r) \
    if(r != CUDA_SUCCESS){ \
        const char* err; \
        cuGetErrorString(r, &err); \
        av_log_error()<< "AvVideoDecodeFilterNv open failed:"<<err<<end_log(); \
        return false; \
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
	av_log_info() << "GPU in use: " << szDeviceName << end_log();
	nv_ck(cuCtxCreate(cuContext, flags, cuDevice));
	return true;
}

bool AvVideoEncodeFilterNV::open(PixelFormat f, int width, int height, int framerate)
{
	nv_ck(cuInit(0));
    int nGpu = 0;
	nv_ck(cuDeviceGetCount(&nGpu));
    av_log_info() << "number of GPU:"<<nGpu<<end_log();
    int iGpu = 0;
    if( !create_cuda_context(&_cu_context, iGpu, 0) )
        return false;

	NV_ENC_BUFFER_FORMAT nv_format = format2nv(f);
	_nvencoder.reset( new NvEncoderCuda(_cu_context, width, height, nv_format) );

	NV_ENC_INITIALIZE_PARAMS initializeParams = { NV_ENC_INITIALIZE_PARAMS_VER };
	NV_ENC_CONFIG encodeConfig = { NV_ENC_CONFIG_VER };
	encodeConfig.gopLength = _gop;

	initializeParams.encodeConfig = &encodeConfig;
	GUID guid_codec = _2encoder_GUID(_codec_id);
    GUID guid_preset = NV_ENC_PRESET_P3_GUID;
	NV_ENC_TUNING_INFO tuning_info = NV_ENC_TUNING_INFO_HIGH_QUALITY;

	_nvencoder->CreateDefaultEncoderParams(&initializeParams, guid_codec, guid_preset, tuning_info);
	_nvencoder->CreateEncoder(&initializeParams);
       
	_framerate = framerate;
	_param.format = f;
	_param.fps = _framerate;
	_param.w= width;
	_param.h= height;
	_param.codecid = _codec_id;
	_param.type = MEDIA_VIDEO;

	return true;
}

void AvVideoEncodeFilterNV::close()
{
	if(_nvencoder != nullptr){
		_nvencoder->DestroyEncoder();
	}
}
