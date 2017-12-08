//av_file_output_filter.cpp

#include "av_file_sink.h"

AvFileSink::AvFileSink(const std::vector<AvStreamInfo>& ss, const std::string& filename) throw(AvFileSinkException)
	:_filename(filename)
	,_format_ctx(NULL)
{
	int ret = -1;
	if ((ret = avformat_alloc_output_context2(&_format_ctx, NULL, NULL, _filename.c_str())) < 0)
		throw AvFileSinkException(ret, __FILE__, __LINE__);

	try {
		for (auto i : ss)
			addStream(i);

		open();
	}
	catch (AvFileSinkException& e)
	{
		avformat_free_context(_format_ctx);
		_format_ctx = NULL;
		throw e;
	}
}

void AvFileSink::addStream(const AvStreamInfo& s)throw(AvFileSinkException)
{
	AVCodec* codec = avcodec_find_encoder(_2ffmpeg_id(s.codecid));
	if (codec == NULL)
		throw AvFileSinkException("encoder not found", __FILE__, __LINE__);

	if (codec->type == AVMEDIA_TYPE_VIDEO )
	{
		if (_video_stream != nullptr)
			throw AvFileSinkException("video stream existed", __FILE__, __LINE__);
	
		_video_stream = avformat_new_stream(_format_ctx, NULL);
		if (_video_stream == NULL)
			throw AvFileSinkException("Failed to allocating output stream", __FILE__, __LINE__);

		_video_stream->codecpar->codec_id = codec->id;
		_video_stream->codecpar->codec_type = codec->type;
		_video_stream->codecpar->width = s.vi.width;
		_video_stream->codecpar->height = s.vi.height;
		_video_stream->codecpar->bit_rate = 320 * 1024;
		_video_stream->codecpar->format = _2ffmpeg_format(s.vi.pixel_format);

		_video_stream->id = _format_ctx->nb_streams - 1;
	}
	else if (codec->type == AVMEDIA_TYPE_AUDIO)
	{
		if (_audio_stream != nullptr)
			throw AvFileSinkException("audio stream existed", __FILE__, __LINE__);

		_audio_stream = avformat_new_stream(_format_ctx, NULL);
		if (_audio_stream == NULL)
			throw AvFileSinkException("Failed to allocating output stream", __FILE__, __LINE__);

		//_audio_stream->time_base = AVRational{ 1, 44100 };
		_audio_stream->codecpar->codec_id = codec->id;
		_audio_stream->codecpar->codec_type = codec->type;
		_audio_stream->codecpar->sample_rate = s.ai.sample_rate;
		_audio_stream->codecpar->channels = s.ai.channel;
		_audio_stream->codecpar->channel_layout = AV_CH_LAYOUT_STEREO;
		_audio_stream->codecpar->format = _2ffmpeg_format(s.ai.sample_format);
		
		_audio_stream->id = _format_ctx->nb_streams - 1;
	}
	else
		throw AvFileSinkException("Invalid stream type", __FILE__, __LINE__);
}

void AvFileSink::write(AVPacket* packet) throw(AvFileSinkException)
{
	if (_format_ctx == NULL)
		throw AvFileSinkException("didn't open", __FILE__, __LINE__);

	int ret = -1;
	ret = av_interleaved_write_frame(_format_ctx, packet);
	if (ret != 0)
		throw AvFileSinkException(ret, __FILE__, __LINE__);
}

void AvFileSink::put(AVParam* p)
{
	AVPacket *pack = av_packet_alloc();
	av_init_packet(pack);
	pack->data = p->getData();
	pack->size = p->len;
	pack->pts = p->pts;
	pack->dts = p->dts;
	if (p->type == MEDIA_AUDIO)
	{
		av_packet_rescale_ts(pack, AVRational{ 1, p->framerate }, _audio_stream->time_base);
		pack->stream_index = _audio_stream->id;
	}
	else if(p->type == MEDIA_VIDEO)
	{
		av_packet_rescale_ts(pack, AVRational{ 1, p->framerate }, _video_stream->time_base);
		pack->stream_index = _video_stream->id;
	}
	else
		throw AvException("error MediaType", __FILE__, __LINE__);


	try {
		write(pack);
	}
	catch (AvFileSinkException& e){
		return;
	}
	
	return;
	
}

void AvFileSink::open()throw(AvFileSinkException)
{
	int ret=-1;
		
	if (!(_format_ctx->oformat->flags & AVFMT_NOFILE))
	{
		ret = avio_open(&_format_ctx->pb, _filename.c_str(), AVIO_FLAG_WRITE);
		if (ret < 0) 
			throw AvFileSinkException(ret, __FILE__, __LINE__);	
	}

	ret = avformat_write_header(_format_ctx, NULL);
	if (ret < 0) {
		char errstr[255];
		av_make_error_string(errstr, 256, ret);
		throw AvFileSinkException(errstr, __FILE__, __LINE__);
	}
}

void AvFileSink::close()
{
	int ret = av_write_trailer(_format_ctx);
	avformat_free_context(_format_ctx);
	_format_ctx = NULL;
}
