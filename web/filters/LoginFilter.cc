#include "LoginFilter.h"
#include <drogon/HttpAppFramework.h>
#include <drogon/orm/DbClient.h>
#include <drogon/orm/Mapper.h>
#include <models/Users.h>
using namespace drogon;
using namespace drogon_model::sqlite3;
using namespace drogon::orm;

void LoginFilter::doFilter(const HttpRequestPtr &req,
                           FilterCallback &&fcb,
                           FilterChainCallback &&fccb) {
    auto session = req->getSession();

    // 检查 session 中是否存在 user_id
    auto userIdOpt = session->getOptional<long>("user_id");
    bool isLoggedIn = userIdOpt.has_value();

    if (isLoggedIn) {
        // 获取 user_id
        auto userId = *userIdOpt;

        // 查询数据库获取用户信息
        auto client = drogon::app().getDbClient();
        Mapper<Users> mapper(client);

        try {
            auto user = mapper.findByPrimaryKey(userId);
            if (!user.getValueOfUsername().empty()) {
                // 用户存在
                req->attributes()->insert("isLoggedIn", true);
                req->attributes()->insert("username", user.getValueOfUsername());
                LOG_INFO<<"User nickname: "<<user.getValueOfUsername();
            } else {
                // 用户不存在，清除 session
                session->erase("user_id");
                isLoggedIn = false;
                LOG_INFO<<"User not found";
            }
        } catch (const DrogonDbException &e) {
            // 查询失败，清除 session
            session->erase("user_id");
            isLoggedIn = false;
            LOG_INFO<<"Query failed: "<<e.base().what();
        }
    }

    if (isLoggedIn) {
        fccb();// 继续执行过滤链
    } else {
        auto res = HttpResponse::newRedirectionResponse("/login");
        LOG_INFO<<"Redirect to login page";
        fcb(res);// 重定向到登录页面
    }
}