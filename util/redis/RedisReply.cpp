//
// Created by binlv on 2019/8/8.
//

#include <sstream>
#include "RedisReply.h"

RedisReplyType::RedisReplyType(const redisReply *rep)
        : replyType_(rep == nullptr ? -1 : rep->type) {
}

bool RedisReplyType::isReplyOk() const {
    return replyType_ != REDIS_REPLY_ERROR;
}

bool RedisReplyType::isStatusType() const {
    return replyType_ == REDIS_REPLY_STATUS;
}

bool RedisReplyType::isIntType() const {
    return replyType_ == REDIS_REPLY_INTEGER;
}

bool RedisReplyType::isNilType() const {
    return replyType_ == REDIS_REPLY_NIL;
}

bool RedisReplyType::isStringType() const {
    return replyType_ == REDIS_REPLY_STRING;
}

bool RedisReplyType::isArrayType() const {
    return replyType_ == REDIS_REPLY_ARRAY;
}

bool RedisReplyType::hasValue() const {
    return replyType_ != -1;
}

std::string redisReplyContentConv2String(RedisReplyType &rp, redisReply *it) {
    if (!rp.hasValue()) {
        return "";
    }
    std::stringstream ss;
    if (rp.isIntType()) {
        ss << it->integer;
    } else {
        ss << std::string(it->str, it->len);
    }
    return ss.str();
}

