//
// Created by binlv on 2019/8/9.
//

#include "ananas_rpc.pb.h"

int main(int argc, char* argv[]) {

    evpp::EventLoop loop;
    ananas::rpc::Endpoint e;
    e.set_ip("aaa");
    e.set_port(22);
    LOG_INFO<<"xxxxxxxxxxxxxxxxxxxxxxxxxxx";

    loop.Run();
    return 0;
}

#include "winmain-inl.h"

