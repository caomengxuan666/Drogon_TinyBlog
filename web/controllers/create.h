#pragma once

#include <drogon/HttpController.h>
#include <drogon/HttpTypes.h>

using namespace drogon;

class create : public drogon::HttpController<create> {
public:
    METHOD_LIST_BEGIN
    METHOD_ADD(create::get, "/{1}", Get);
    METHOD_ADD(create::save, "/save", Post); // 修改：移除路由参数 {1}
    METHOD_LIST_END

    void get(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, int authorid);
    void save(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
};