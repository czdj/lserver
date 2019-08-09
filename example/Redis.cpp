//
// Created by binlv on 2019/8/9.
//

#include <evpp/tcp_server.h>
#include <evpp/buffer.h>
#include <evpp/tcp_conn.h>
#include "evpp/logging.h"
#include "evpp/future/Future.h"
#include "evpp/util/ThreadPool.h"
#include <hiredis/hiredis.h>
#include <hiredis/async.h>
#include <hiredis/adapters/libevent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory>
#include "util/redis/RedisClient.h"

int main(int argc, char* argv[]) {
    std::string addr = "0.0.0.0:9099";
    int thread_num = 4;
    evpp::EventLoop loop;
//    evpp::TCPServer server(&loop, addr, "TCPEchoServer", thread_num);
//    server.SetMessageCallback([](const evpp::TCPConnPtr& conn,
//                                 evpp::Buffer* msg) {
//        conn->Send(msg);
//    });
//    server.SetConnectionCallback([](const evpp::TCPConnPtr& conn) {
//        if (conn->IsConnected()) {
//            LOG_INFO << "A new connection from " << conn->remote_addr();
//        } else {
//            LOG_INFO << "Lost the connection from " << conn->remote_addr();
//        }
//    });
//    server.Init();
//    server.Start();
    std::shared_ptr<EventLoopThread> l = std::make_shared<EventLoopThread>();
    std::cout<<"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx:"<<std::this_thread::get_id()<<std::endl;
    auto client = new RedisClient(l->loop());
    client->AsyncConnect();
    l->Start();
    client->set("a","c").Then(&loop,[](RedisReplyContent<RedisBoolType> value){
        LOG_INFO<<"THEN:"<<std::this_thread::get_id();
        LOG_INFO<<value.getValue();
    });
    client->get("a").Then([](RedisReplyContent<RedisStringType> value){
        LOG_INFO<<"key:"<<value.getValue();
        return std::string("x");
    }).Then([client](std::string value){
        client->set("a","d");
        return std::string("x1");
    }).Then([client](std::string value){
        client->get("a").Then([](RedisReplyContent<RedisStringType> value){
            LOG_INFO<<"key:"<<value.getValue();
        });
        return std::string("x2");
    });


    loop.Run();
    return 0;
}

#include "winmain-inl.h"

