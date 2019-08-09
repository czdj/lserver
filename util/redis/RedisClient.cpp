//
// Created by binlv on 2019/8/6.
//

#include "RedisClient.h"


void RedisClient::AsyncConnect(std::string serverIp, int serverPort)
{
    redisOptions options = {0};
    REDIS_OPTIONS_SET_TCP(&options, serverIp.c_str(), serverPort);
    struct timeval tv = {0};
    tv.tv_sec = 1;
    options.timeout = &tv;

    redisAsyncContext_ = redisAsyncConnectWithOptions(&options);
    if (redisAsyncContext_ == nullptr){
        return ;
    }
    if (redisAsyncContext_->err) {
        LOG_ERROR<<"Error:" <<redisAsyncContext_->errstr;
        return;
    }
    redisAsyncContext_->data = this;

    redisLibeventAttach(redisAsyncContext_,this->loop_->event_base());
    redisAsyncSetConnectCallback(redisAsyncContext_,_ConnectCallback);
    redisAsyncSetDisconnectCallback(redisAsyncContext_,_DisconnectCallback);
}

//连接异常断开，hiredis会在连接断开时调用所有未返回的回调，防止privdata资源泄露
void RedisClient::OnMessageCallback(redisAsyncContext *c, void *reply, void *privdata)
{
    LOG_INFO<<"OnMessageCallback:"<<std::this_thread::get_id();

//    redisReply* reply = (redisReply*)r;
//    if (reply == NULL) {
//        if (c->errstr) {
//            LOG_ERROR<<"redis return error: "<<c->errstr;
//        }
//        return;
//    }

    RedisCommandPackage *cmdPackage = (RedisCommandPackage *) privdata;
    RedisRawReply rawReply(reply);
    cmdPackage->invoke(rawReply);
    delete cmdPackage;
}

template<typename RedisType,
	typename F = RedisClient::RedisReplyContentFuture<RedisType>,
	typename P = RedisClient::RedisReplyContentPromise<RedisType>
>
F RedisClient::_ExctueRedisCmd(const std::string &cmd) {
	std::shared_ptr<P> promise(new P);
	F future = promise->GetFuture();
	//        if (okRedisCore_->isClosed()) {
	//            RedisRawReply nullRpy;
	//            RedisReplyType rpt(nullRpy);
	//            promise->setValue(RedisReplyContent<RedisType>(rpt, nullRpy));
	//            return future;
	//        }
	this->_ExecuteAsyncCmd(
		cmd,
		[promise](RedisRawReply &r)
		mutable {
		RedisReplyType replyType(r);
		RedisReplyContent<RedisType> content = RedisReplyContent<RedisType>(replyType, r);
		promise->SetValue(std::move(content));
	});
	return future;
}

void RedisClient::_ExecuteAsyncCmd(const std::string &cmd, CallBackType &&cb) {
    auto cmdPackage = new RedisCommandPackage(cmd,cb);
    redisAsyncCommand(redisAsyncContext_,&RedisClient::OnMessageCallback,cmdPackage, cmdPackage->getCmd().c_str());
}

RedisClient::RedisReplyContentFuture<RedisBoolType>
RedisClient::set(const std::string &key, const std::string &value) {
    std::ostringstream os;
    os<<"SET "<<key+" "<<value;
    return _ExctueRedisCmd<RedisBoolType>(os.str());
}

RedisClient::RedisReplyContentFuture<RedisStringType>
RedisClient::get(const std::string &key) {
    std::ostringstream os;
    os<<"GET "<<key;
    return _ExctueRedisCmd<RedisStringType>(os.str());
}

RedisClient::RedisReplyContentFuture<RedisBoolType>
RedisClient::hset(const std::string &domain, const std::string &key, const std::string &value) {
    std::ostringstream os;
    os<<"HSET "<<domain+" "<<key+" "<<value;
    return _ExctueRedisCmd<RedisBoolType>(os.str());
}

RedisClient::RedisReplyContentFuture<RedisStringType>
RedisClient::hget(const std::string &domain, const std::string &key) {
    std::ostringstream os;
    os<<"HGET "<<domain+" "<<key;
    return _ExctueRedisCmd<RedisStringType>(os.str());
}

RedisClient::RedisReplyContentFuture<RedisBoolType>
RedisClient::setWithTTL(const std::string &key, const std::string &value, int ttlSeconds) {
    std::ostringstream os;
    os<<"SETEX "<<key+" "<<value+" "<<ttlSeconds;
    return _ExctueRedisCmd<RedisBoolType>(os.str());
}

RedisClient::RedisReplyContentFuture<RedisBoolType>
RedisClient::expire(const std::string &key, int ttlSeconds) {
    std::ostringstream os;
    os<<"EXPIRE "<<key+" "<<ttlSeconds;
    return _ExctueRedisCmd<RedisBoolType>(os.str());
}

RedisClient::RedisReplyContentFuture<RedisBoolType>
RedisClient::exists(const std::string &key) {
    std::ostringstream os;
    os<<"EXISTS "<<key;
    return _ExctueRedisCmd<RedisBoolType>(os.str());
}

RedisClient::RedisReplyContentFuture<RedisBoolType>
RedisClient::del(const std::string &key) {
    std::ostringstream os;
    os<<"DEL "<<key;
    return _ExctueRedisCmd<RedisBoolType>(os.str());
}

RedisClient::RedisReplyContentFuture<RedisBoolType>
RedisClient::hdel(const std::string &domain, const std::string &key) {
    std::ostringstream os;
    os<<"HDEL "<<domain+" "<<key;
    return _ExctueRedisCmd<RedisBoolType>(os.str());
}

RedisClient::RedisReplyContentFuture<RedisMapType>
RedisClient::hgetall(const std::string &domain) {
    std::ostringstream os;
    os<<"HGETALL "<<domain;
    return _ExctueRedisCmd<RedisMapType>(os.str());

}

RedisClient::RedisReplyContentFuture<RedisIntType>
RedisClient::incr(const std::string &key) {
    std::ostringstream os;
    os<<"INCR "<<key;
    return _ExctueRedisCmd<RedisIntType>(os.str());
}

RedisClient::RedisReplyContentFuture<RedisIntType>
RedisClient::incrBy(const std::string &key, int num) {
    std::ostringstream os;
    os<<"INCRBY "<<key+" "<<num;
    return _ExctueRedisCmd<RedisIntType>(os.str());
}