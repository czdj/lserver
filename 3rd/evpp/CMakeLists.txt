
file(GLOB evpp_SRCS *.cc */*.cc *.h */*.h)
file(GLOB evpp_lite_SRCS *.cc *.h httpc/*.*)
list(REMOVE_ITEM evpp_lite_SRCS tcp_server.h tcp_server.cc listener.h listener.cc event_loop_thread.h event_loop_thread.cc event_loop_thread_pool.h event_loop_thread_pool.cc)
file(GLOB evpp_PUBLIC_HEADERS *.h)
file(GLOB evpp_HTTP_PUBLIC_HEADERS http/*.h)
file(GLOB evpp_HTTPC_PUBLIC_HEADERS httpc/*.h)
file(GLOB evpp_UDP_PUBLIC_HEADERS udp/*.h)

message(STATUS "evpp_SRCS : " ${evpp_SRCS})

include_directories(${PROJECT_SOURCE_DIR})

add_library(evpp_static STATIC ${evpp_SRCS})
target_link_libraries(evpp_static ${DEPENDENT_LIBRARIES})

if (HTTPS)
add_library(evpp_https_static STATIC ${evpp_SRCS})
target_compile_definitions(evpp_https_static PRIVATE -DEVPP_HTTP_CLIENT_SUPPORTS_SSL)
target_link_libraries(evpp_https_static ${DEPENDENT_LIBRARIES})
endif (HTTPS)

add_library(evpp_lite_static STATIC ${evpp_lite_SRCS})
target_link_libraries(evpp_lite_static ${DEPENDENT_LIBRARIES})

