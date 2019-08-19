cmake_minimum_required(VERSION 3.10)
project(lserver)

# Check platforms
if (CMAKE_HOST_WIN32)
    message(STATUS  "platform windows")
    set(WINDOWS 1)
elseif(CMAKE_HOST_APPLE)
    message(STATUS  "platform macos")
    set(MACOS 1)
elseif(CMAKE_HOST_UNIX)
    message(STATUS  "platform linux")
    set(LINUX 1)
endif()

#编译选项
set(CMAKE_CXX_STANDARD 11)
#标准库在<algorithm>头中定义了两个模板函数std::min() 和 std::max()，通常用它可以计算某个值对的最小值和最大值。而在Visual C++ 使用会发生名字min和max与<windows.h>中传统的min/max宏定义有冲突的现象。
add_definitions(-D NOMINMAX)

include_directories(3rd)


if (WINDOWS)
    include_directories(3rd/lib/windows/include)
    if (CMAKE_BUILD_TYPE STREQUAL Debug)
        message(STATUS  "debug model")
        SET(DEPENDENT_LIBRARIES hiredis libprotobufd)
        link_directories(3rd/lib/windows/bin/debug)
    else()
        message(STATUS  "release model")
        SET(DEPENDENT_LIBRARIES hiredis libprotobufd libprotobuf)
        link_directories(3rd/lib/windows/bin/release)
    endif()
elseif(LINUX)
    include_directories(3rd/lib/linux/include)
    if (CMAKE_BUILD_TYPE STREQUAL Debug)
        message(STATUS  "debug model")
        SET(DEPENDENT_LIBRARIES  event  glog pthread gflags_debug)
        link_directories(3rd/lib/linux/bin/debug)
    else()
        message(STATUS  "release model")
        SET(DEPENDENT_LIBRARIES  event  glog pthread gflags)
        link_directories(3rd/lib/linux/bin/release)
    endif()
endif()

add_subdirectory(3rd/evpp)
add_subdirectory(util)

add_executable(lserver main.cpp ananas_rpc.pb.cc)
target_link_libraries(lserver  ${DEPENDENT_LIBRARIES} evpp_static)

#编译后处理
if (WINDOWS)
    if (CMAKE_BUILD_TYPE STREQUAL Debug)
        add_custom_command(TARGET lserver POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/3rd/lib/windows/bin/debug/glog.dll" $<TARGET_FILE_DIR:lserver>)
        add_custom_command(TARGET lserver POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/3rd/lib/windows/bin/debug/gflags_debug.dll" $<TARGET_FILE_DIR:lserver>)
        add_custom_command(TARGET lserver POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/3rd/lib/windows/bin/debug/event.dll" $<TARGET_FILE_DIR:lserver>)
        add_custom_command(TARGET lserver POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/3rd/lib/windows/bin/debug/libprotobufd.dll" $<TARGET_FILE_DIR:lserver>)
    else()
        add_custom_command(TARGET lserver POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/3rd/lib/windows/bin/release/glog.dll" $<TARGET_FILE_DIR:lserver>)
        add_custom_command(TARGET lserver POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/3rd/lib/windows/bin/release/gflags.dll" $<TARGET_FILE_DIR:lserver>)
        add_custom_command(TARGET lserver POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/3rd/lib/windows/bin/release/event.dll" $<TARGET_FILE_DIR:lserver>)
        add_custom_command(TARGET lserver POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/3rd/lib/windows/bin/debug/libprotobuf.dll" $<TARGET_FILE_DIR:lserver>)
    endif()

elseif(LINUX)

endif()
