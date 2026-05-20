cmake_minimum_required(VERSION 3.30)
project(rastersong_core LANGUAGES C CXX)

set(FFMPEG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/ffmpeg)

set(FFMPEG_COMPONENTS
    avcodec    avcodec-62
    avformat   avformat-62
    avutil     avutil-60
    swresample swresample-6
    swscale    swscale-9
)

list(LENGTH FFMPEG_COMPONENTS FFMPEG_COUNT)
math(EXPR FFMPEG_LAST "${FFMPEG_COUNT} - 1")

foreach(i RANGE 0 ${FFMPEG_LAST} 2)
    math(EXPR j "${i} + 1")
    list(GET FFMPEG_COMPONENTS ${i} component)
    list(GET FFMPEG_COMPONENTS ${j} versioned)

    add_library(ffmpeg::${component} SHARED IMPORTED GLOBAL)
    set_target_properties(ffmpeg::${component} PROPERTIES
        IMPORTED_LOCATION "${FFMPEG_DIR}/bin/${versioned}.dll"
        IMPORTED_IMPLIB   "${FFMPEG_DIR}/lib/${versioned}.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${FFMPEG_DIR}/include"
    )
endforeach()

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c*")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h*")

add_library(rastersong_core STATIC ${SOURCES} ${HEADERS})

target_link_libraries(rastersong_core PUBLIC
    ffmpeg::avcodec
    ffmpeg::avformat
    ffmpeg::avutil
    ffmpeg::swresample
    ffmpeg::swscale
)

target_include_directories(rastersong_core
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Expose the FFmpeg bin dir so gde can copy DLLs at the right moment
set(FFMPEG_BIN_DIR "${FFMPEG_DIR}/bin" CACHE INTERNAL "FFmpeg DLL directory")