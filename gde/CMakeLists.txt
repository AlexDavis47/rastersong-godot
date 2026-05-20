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

# Read the platform/arch/suffix from godot-cpp's target properties
get_target_property(GODOT_ARCH   godot-cpp GODOTCPP_ARCH)
get_target_property(GODOT_TARGET godot-cpp GODOTCPP_TARGET)
string(TOLOWER "${CMAKE_SYSTEM_NAME}" GODOT_PLATFORM)


set(BUILD_PATH "$<1:${CMAKE_SOURCE_DIR}/godot/addons/rastersong/bin/${GODOT_PLATFORM}>")

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c*")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h*")

add_library(rastersong SHARED ${SOURCES} ${HEADERS})




set_target_properties(rastersong PROPERTIES
    PREFIX ""
    SUFFIX ".${GODOT_PLATFORM}.${GODOT_TARGET}.${GODOT_ARCH}.dll"
    RUNTIME_OUTPUT_DIRECTORY "$<1:${CMAKE_SOURCE_DIR}/godot/addons/rastersong/bin/${GODOT_PLATFORM}>"
    LIBRARY_OUTPUT_DIRECTORY "$<1:${CMAKE_SOURCE_DIR}/godot/addons/rastersong/bin/${GODOT_PLATFORM}>"
    ARCHIVE_OUTPUT_DIRECTORY "$<1:${CMAKE_SOURCE_DIR}/godot/addons/rastersong/bin/${GODOT_PLATFORM}>"
)

target_link_libraries(rastersong PRIVATE godot-cpp rastersong_core)

target_include_directories(rastersong
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# FFmpeg DLLs
set(FFMPEG_DLLS avcodec-62.dll avformat-62.dll avutil-60.dll swresample-6.dll swscale-9.dll)
foreach(dll ${FFMPEG_DLLS})
    add_custom_command(TARGET rastersong POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${FFMPEG_BIN_DIR}/${dll}"
            "${BUILD_PATH}/"
        COMMENT "Copying ${dll}"
    )
endforeach()

# .gdextension file goes in the addon root, not bin
add_custom_command(TARGET rastersong POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "${CMAKE_SOURCE_DIR}/godot/addons/rastersong"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/rastersong.gdextension"
        "${CMAKE_SOURCE_DIR}/godot/addons/rastersong/"
    COMMENT "Copying rastersong.gdextension"
)