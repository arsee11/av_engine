

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib/)
add_library(nvcodec STATIC 
	NvCodec/NvDecoder/NvDecoder.cpp
	NvCodec/NvEncoder/NvEncoder.cpp
	NvCodec/NvEncoder/NvEncoderCuda.cpp
) 
