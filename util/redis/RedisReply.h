//
// Created by binlv on 2019/8/8.
//

#ifndef LSERVER_REDISREPLY_H
#define LSERVER_REDISREPLY_H

#include <vector>
#include <map>
#include <string>
#include <functional>
#include "hiredis/hiredis.h"

class RedisRawReply {
public:
    RedisRawReply() : RedisRawReply(nullptr) {}

    explicit RedisRawReply(void *reply) : reply_(reply) {};

    operator redisReply *() {
        return (redisReply *) reply_;
    }

    RedisRawReply(RedisRawReply &&o) : reply_(o.reply_) {
    }

    RedisRawReply &operator=(RedisRawReply &&o) {
        if (&o == this) {
            return *this;
        }
        reply_ = o.reply_;
        return *this;
    }

private:
    void *reply_;
};

class RedisCommandPackage {
public:

    RedisCommandPackage(const std::string &cmd,std::function<void(RedisRawReply &)> cb) : cmd(cmd), cb(cb), hasInvoked(false) {}

    void invoke(RedisRawReply &r) {
        hasInvoked = true;
        cb(r);
    }

    ~RedisCommandPackage() {
        if (!hasInvoked) {
            RedisRawReply r;
            cb(r);
        }
    }

    const std::string &getCmd() const {
        return cmd;
    }

private:
    std::string cmd;
    std::function<void(RedisRawReply &)> cb;
    bool hasInvoked;
};

class RedisReplyType {
public:

    explicit RedisReplyType(const redisReply *rep);

public:
    bool hasValue() const;

    bool isReplyOk() const;

    bool isStatusType() const;

    bool isNilType() const;

    bool isIntType() const;

    bool isStringType() const;

    bool isArrayType() const;

private:
    int replyType_;

};

//Util Conv RedisReply to String
extern std::string redisReplyContentConv2String(RedisReplyType &rp, redisReply *it);

using RedisNilType = void;
using RedisIntType = long long int;
using RedisBoolType = bool;
using RedisStringType = std::string;
using RedisStatusType = std::string;
using RedisVecType = std::vector<std::string>;
using RedisMapType = std::map<std::string, std::string>;

class RedisBasicReplyContent {

public:
    RedisBasicReplyContent(RedisReplyType &rt)
            : isHasValue(rt.hasValue()) {
    }

    RedisBasicReplyContent()
            : isHasValue(false) {
    }

    bool hasValue() const {
        return isHasValue;
    }

private:
    bool isHasValue;
};

template<typename T>
class RedisReplyContent {

};

template<>
class RedisReplyContent<RedisBoolType> : public RedisBasicReplyContent {
public:
    RedisReplyContent(RedisReplyType &rt, const redisReply *) :
            RedisBasicReplyContent(rt),
            val(hasValue() ? rt.isReplyOk() : false) {
    }

    const RedisBoolType getValue() const {
        return val;
    }

private:
    RedisBoolType val{false};
};

template<>
class RedisReplyContent<RedisIntType> : public RedisBasicReplyContent {
public:
    RedisReplyContent(RedisReplyType &rt,
                        const redisReply *r) :
            RedisBasicReplyContent(rt),
            val(hasValue() ? r->integer : -1) {
    }

    const RedisIntType getValue() const {
        return val;
    }

private:
    RedisIntType val{-1};
};

template<>
class RedisReplyContent<RedisStringType> : public RedisBasicReplyContent {
public:
    RedisReplyContent(RedisReplyType &rt, const redisReply *r) :
            RedisBasicReplyContent(rt) {
        if (hasValue()) {
            val = std::string(r->str, r->len);
        }
    }

    const RedisStringType &getValue() const {
        return val;
    }

private:
    RedisStringType val{""};
};

template<>
class RedisReplyContent<RedisVecType> : public RedisBasicReplyContent {
public:
    RedisReplyContent(RedisReplyType &rp, const redisReply *r) :
            RedisBasicReplyContent(rp),
            val() {
        if (!hasValue()) {
            return;
        }
        for (size_t i = 0; i < r->elements; ++i) {
            redisReply *tmp = r->element[i];
            val.push_back(redisReplyContentConv2String(rp, tmp));
        }
    }

    const RedisVecType &getValue() const {
        return val;
    }

private:
    RedisVecType val{};
};

template<>
class RedisReplyContent<RedisMapType> : public RedisBasicReplyContent {
public:
    RedisReplyContent(RedisReplyType &rp, const redisReply *r) :
            RedisBasicReplyContent(rp),
            val() {
        if (!hasValue()) {
            return;
        }
        for (size_t i = 0; i < r->elements; i += 2) {
            redisReply *first = r->element[i];
            redisReply *second = r->element[i + 1];
            val.insert(
                    std::make_pair(redisReplyContentConv2String(rp, first),
                                   redisReplyContentConv2String(rp, second))
            );
        }
    }

    const RedisMapType &getValue() const {
        return val;
    }

private:
    RedisMapType val{};
};

#endif //LSERVER_REDISREPLY_H
