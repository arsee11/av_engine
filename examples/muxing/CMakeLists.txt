project(muxing)

include_directories(${SDL_INC_DIR})
link_directories(${LIB_DIR})
link_directories(${NVCODEC_LIB_DIR})
link_directories(${FFMPEG_LIB_DIR})
link_directories(${CUDA_LIB_DIR})

set(EXECUTABLE_OUTPUT_PATH ./)
aux_source_directory(./ SOURCES)
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} avengine nvcodec
	nvencodeapi
	cuda
	avutil
	avcodec
	avformat
	swscale
	avdevice)
