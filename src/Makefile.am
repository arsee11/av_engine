noinst_LIBRARIES=libavengine.a

AM_CPPFLAGS = -I$(top_srcdir)/include/ -I$(top_srcdir)/third_party/include/ 
AM_CPPFLAGS += -g --std=c++11
AM_CPPFLAGS += -DLINUX

libavengine_a_SOURCES = \
	av_audio_encode_filter.cpp \
	av_audio_decode_filter.cpp \
	av_camera.cpp \
	av_video_decode_filter.cpp \
	av_video_encode_filter.cpp \
	av_file_sink.cpp \
	av_file_source.cpp \
	av_frame_scale_filter.cpp \
	av_log.cpp \
	av_microphone.cpp \
	av_resample_filter.cpp \
	codec_specify.cpp \
	h264_rtp_packer.cpp \
	pcma_rtp_packer.cpp \
	rtp_wrapper.cpp \
	h264helper.cpp \
	videofilter.cpp

