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

set(CMAKE_CXX_STANDARD 11)

include_directories(3rd)


if (WINDOWS)
    SET(DEPENDENT_LIBRARIES hiredis)
    include_directories(3rd/lib/windows/include)
    if (CMAKE_BUILD_TYPE STREQUAL Debug)
        message(STATUS  "debug model")
        link_directories(3rd/lib/windows/bin/debug)
    else()
        message(STATUS  "release model")
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

add_executable(lserver main.cpp util/redis/RedisClient.cpp util/redis/RedisClient.h util/redis/RedisReply.cpp util/redis/RedisReply.h)
target_link_libraries(lserver  ${DEPENDENT_LIBRARIES} evpp_static)

#编译后处理
if (WINDOWS)
    if (CMAKE_BUILD_TYPE STREQUAL Debug)
        add_custom_command(TARGET lserver POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/3rd/lib/windows/bin/debug/glog.dll" $<TARGET_FILE_DIR:lserver>)
        add_custom_command(TARGET lserver POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/3rd/lib/windows/bin/debug/gflags_debug.dll" $<TARGET_FILE_DIR:lserver>)
        add_custom_command(TARGET lserver POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/3rd/lib/windows/bin/debug/event.dll" $<TARGET_FILE_DIR:lserver>)
    else()
        add_custom_command(TARGET lserver POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/3rd/lib/windows/bin/release/glog.dll" $<TARGET_FILE_DIR:lserver>)
        add_custom_command(TARGET lserver POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/3rd/lib/windows/bin/release/gflags.dll" $<TARGET_FILE_DIR:lserver>)
        add_custom_command(TARGET lserver POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/3rd/lib/windows/bin/release/event.dll" $<TARGET_FILE_DIR:lserver>)
    endif()

elseif(LINUX)

endif()
