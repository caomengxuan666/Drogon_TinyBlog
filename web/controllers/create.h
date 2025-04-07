#pragma once

#include <drogon/HttpController.h>
#include <drogon/HttpTypes.h>
#include <filters/LoginFilter.h>

using namespace drogon;

class create : public drogon::HttpController<create> {
public:
    METHOD_LIST_BEGIN
    METHOD_ADD(create::get, "/{1}", Get,"LoginFilter");
    METHOD_ADD(create::save, "/save", Post);
    METHOD_LIST_END

    void get(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, int authorid);

    void modify(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback,int authorid);

    void save(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
};