cmake_minimum_required(VERSION 3.17)
project(rastersong LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
    godot-cpp
    GIT_REPOSITORY https://github.com/godotengine/godot-cpp.git
    GIT_TAG        master
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(godot-cpp)

get_target_property(GODOT_ARCH   godot-cpp GODOTCPP_ARCH)
get_target_property(GODOT_TARGET godot-cpp GODOTCPP_TARGET)
string(TOLOWER "${CMAKE_SYSTEM_NAME}" GODOT_PLATFORM)

set(ADDON_PATH "${CMAKE_SOURCE_DIR}/godot/addons/rastersong")
set(BUILD_PATH "$<1:${ADDON_PATH}/bin/${GODOT_PLATFORM}>")

set(FFMPEG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/ffmpeg")

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
    add_library(ffmpeg::${component} SHARED IMPORTED)
    set_target_properties(ffmpeg::${component} PROPERTIES
        IMPORTED_LOCATION             "${FFMPEG_DIR}/bin/${versioned}.dll"
        IMPORTED_IMPLIB               "${FFMPEG_DIR}/lib/${versioned}.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${FFMPEG_DIR}/include"
    )
endforeach()

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c*")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h*")


add_library(rastersong SHARED ${SOURCES} ${HEADERS})

set_target_properties(rastersong PROPERTIES
    PREFIX ""
    SUFFIX ".${GODOT_PLATFORM}.${GODOT_TARGET}.${GODOT_ARCH}.dll"
    RUNTIME_OUTPUT_DIRECTORY "${BUILD_PATH}"
    LIBRARY_OUTPUT_DIRECTORY "${BUILD_PATH}"
    ARCHIVE_OUTPUT_DIRECTORY "${BUILD_PATH}"
)

target_link_libraries(rastersong PRIVATE
    godot-cpp
    ffmpeg::avcodec
    ffmpeg::avformat
    ffmpeg::avutil
    ffmpeg::swresample
    ffmpeg::swscale
)

target_include_directories(rastersong
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# Copy FFmpeg DLLs
set(FFMPEG_DLLS avcodec-62.dll avformat-62.dll avutil-60.dll swresample-6.dll swscale-9.dll)
foreach(dll ${FFMPEG_DLLS})
    add_custom_command(TARGET rastersong POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${FFMPEG_DIR}/bin/${dll}"
            "${BUILD_PATH}/"
        COMMENT "Copying ${dll}"
    )
endforeach()

# Copy .gdextension
add_custom_command(TARGET rastersong POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${ADDON_PATH}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/rastersong.gdextension"
        "${ADDON_PATH}/"
    COMMENT "Copying rastersong.gdextension"
)