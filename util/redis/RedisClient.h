//
// Created by binlv on 2019/8/6.
//

#ifndef LSERVER_REDISCLIENT_H
#define LSERVER_REDISCLIENT_H

#include <queue>
#include "evpp/event_loop.h"
#include "evpp/future/Future.h"
#include "evpp/any.h"
#include <hiredis/hiredis.h>
#include <hiredis/async.h>
#include <hiredis/adapters/libevent.h>
#include "RedisReply.h"

using namespace evpp;

class RedisClient {
    typedef void (*ConnectCallback)(const redisAsyncContext *c, int status) ;
    typedef void (*DisconnectCallback)(const redisAsyncContext *c, int status);
    typedef void (*MessageCallback)(redisAsyncContext *c, void *r, void *privdata);

    template<typename T>
    using RedisReplyContentFuture  =Future<RedisReplyContent<T>>;

    template<typename T>
    using RedisReplyContentPromise = Promise<RedisReplyContent<T>>;

    using CallBackType = std::function<void(RedisRawReply &)>;

public:
    RedisClient(EventLoop *loop):loop_(loop){
    }

    ~RedisClient(){
    }

    void AsyncConnect(std::string serverIp = "127.0.0.1", int serverPort = 6379);

    void SetAsyncConnectCallback(ConnectCallback cb){
        connectCallback_ = cb;
    }

    void SetAsyncDisConnectCallBack(DisconnectCallback cb){
        disconnectCallback_ = cb;
    }

    void SetOnMessageCallback(MessageCallback cb){
        messageCallback_ = cb;
    }

    static void OnMessageCallback(redisAsyncContext *c, void *r, void *privdata);

    RedisReplyContentFuture<RedisBoolType>
    set(const std::string &key, const std::string &value);

    RedisReplyContentFuture<RedisStringType>
    get(const std::string &key);

    RedisReplyContentFuture<RedisBoolType>
    hset(const std::string &domain, const std::string &key, const std::string &value);

    RedisReplyContentFuture<RedisStringType>
    hget(const std::string &domain, const std::string &key);

    RedisReplyContentFuture<RedisBoolType>
    setWithTTL(const std::string &key, const std::string &value, int ttlSeconds);

    RedisReplyContentFuture<RedisBoolType>
    expire(const std::string &key, int ttlSeconds);

    RedisReplyContentFuture<RedisBoolType>
    exists(const std::string &key);

    RedisReplyContentFuture<RedisBoolType>
    del(const std::string &key);

    RedisReplyContentFuture<RedisBoolType>
    hdel(const std::string &domain, const std::string &key);

    RedisReplyContentFuture<RedisMapType>
    hgetall(const std::string &domain);

    RedisReplyContentFuture<RedisIntType>
    incr(const std::string &key);

    RedisReplyContentFuture<RedisIntType>
    incrBy(const std::string &key, int num);

private:
    static void _ConnectCallback(const redisAsyncContext *c, int status) {
        if (status != REDIS_OK) {
            LOG_ERROR<<"Error:" <<c->errstr;
            return;
        }
        LOG_INFO<<"Connected...";
    }

    static void _DisconnectCallback(const redisAsyncContext *c, int status) {
        if (status != REDIS_OK) {
            LOG_ERROR<<"Error:" <<c->errstr;
            return;
        }
        LOG_INFO<<"Disconnected...";
    }
    void _ExecuteAsyncCmd(const std::string &cmd, CallBackType &&cb);

	template<typename RedisType,
		typename F = RedisReplyContentFuture<RedisType>,
		typename P = RedisReplyContentPromise<RedisType>
	>
		F _ExctueRedisCmd(const std::string &cmd);

private:
    EventLoop* loop_;
    ConnectCallback connectCallback_;
    DisconnectCallback disconnectCallback_;
    MessageCallback messageCallback_;
    redisAsyncContext* redisAsyncContext_;
};

#endif //LSERVER_REDISCLIENT_H
