//av_rtmp_sink.cpp

#include "av_rtmp_sink.h"
#include "av_log.h"

AvRtmpSink::AvRtmpSink(const std::vector<AvStreamInfo>& ss, const std::string& push_url) throw(AvRtmpSinkException)
	:AvFileSink(push_url)
{
	int ret = -1;
	if ((ret = avformat_alloc_output_context2(&_format_ctx, NULL, "flv", _filename.c_str())) < 0)
		throw AvRtmpSinkException(ret, __FILE__, __LINE__);

	try {
		for (auto i : ss)
			addStream(i);

		open();
	}
	catch (AvRtmpSinkException& e)
	{
		avformat_free_context(_format_ctx);
		_format_ctx = NULL;
		throw e;
	}
}

void AvRtmpSink::addStream(const AvStreamInfo& s)throw(AvFileSinkException)
{
	if (s.media_type == MEDIA_VIDEO)
	{
		if (_video_stream != nullptr)
			throw AvFileSinkException("video stream existed", __FILE__, __LINE__);

		_video_stream = avformat_new_stream(_format_ctx, NULL);
		if (_video_stream == NULL)
			throw AvFileSinkException("Failed to allocating output stream", __FILE__, __LINE__);

		_codec_id = s.codecid;
		openVideoEncoder(PixelFormat::FORMAT_YUV420, s.vi.width, s.vi.height, 15);

		avcodec_parameters_from_context(_video_stream->codecpar, _codec_ctx_v);
		_video_stream->id = _format_ctx->nb_streams - 1;

	}
	else if (s.media_type == MEDIA_AUDIO)
	{
		if (_audio_stream != nullptr)
			throw AvFileSinkException("audio stream existed", __FILE__, __LINE__);

		_audio_stream = avformat_new_stream(_format_ctx, NULL);
		if (_audio_stream == NULL)
			throw AvFileSinkException("Failed to allocating output stream", __FILE__, __LINE__);

		//_audio_stream->time_base = AVRational{ 1, 44100 };
		_audio_stream->codecpar->codec_id = _2ffmpeg_id(s.codecid);
		_audio_stream->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
		_audio_stream->codecpar->sample_rate = s.ai.sample_rate;
		_audio_stream->codecpar->channels = s.ai.channel;
		//_audio_stream->codecpar->channel_layout = AV_CH_LAYOUT_STEREO;
		_audio_stream->codecpar->format = _2ffmpeg_format(s.ai.sample_format);

		_audio_stream->id = _format_ctx->nb_streams - 1;
	}
	else
		av_log_error() << "not support media type[" << s.media_type << "]" << end_log();
}

bool AvRtmpSink::openVideoEncoder(PixelFormat f, int width, int height, int framerate)
{
	AVCodec* codec = avcodec_find_encoder(_2ffmpeg_id(_codec_id));
	if (codec == NULL)
	{
		av_log_error() << "encoder[id=" << _codec_id << "] not found" << end_log();
		return false;
	}

	_codec_ctx_v = avcodec_alloc_context3(codec);
	if (_codec_ctx_v == NULL)
		return false;

	_codec_ctx_v->bit_rate = 1024*1024;
	_codec_ctx_v->width = width;
	_codec_ctx_v->height = height;
	AVRational rate;
	rate.num = 1;
	rate.den = framerate;
	_codec_ctx_v->time_base = rate;
	_codec_ctx_v->gop_size = 0;
	_codec_ctx_v->max_b_frames = 0;
	_codec_ctx_v->thread_count = 4;
	_codec_ctx_v->pix_fmt = _2ffmpeg_format(f);
	_codec_ctx_v->flags = AV_CODEC_FLAG_GLOBAL_HEADER;
	AVDictionary* opts = NULL;
	av_dict_set(&opts, "allow_sw", "1", 0); //allows software encoding
	int ret = 0;
	if ((ret = avcodec_open2(_codec_ctx_v, codec, &opts)) < 0)
	{
		char buf[256];
		av_make_error_string(buf, 256, ret);
		av_log_error() << "encoder avcodec_open2 failed:" << buf << end_log();
		avcodec_close(_codec_ctx_v);
		_codec_ctx_v = nullptr;
		return false;
	}
	
	return true;
}

