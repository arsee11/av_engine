
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib/)
aux_source_directory(./ SOURCES)
add_library(avengine STATIC ${SOURCES})
