#include "login.h"


// 显示登录页面
void login::LoginPage(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback) {
    // 返回登录页面的 HTML 内容
    auto resp = HttpResponse::newFileResponse("./views/templates/login.html");
    callback(resp);
}

// 处理登录请求
void login::handleLogin(const HttpRequestPtr &req,
                        std::function<void(const HttpResponsePtr &)> &&callback) {
    LOG_DEBUG << "Handling login request";

    // 从请求中获取 userId 和 password
    auto userId = req->getParameter("userId");
    auto password = req->getParameter("password");

    // 认证算法，读数据库，验证身份等...
    // 这里假设认证成功
    Json::Value ret;
    ret["result"] = "ok";
    ret["token"] = drogon::utils::getUuid();
    auto resp = HttpResponse::newHttpJsonResponse(ret);
    callback(resp);
}

void login::RegisPage(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback) {
    // 返回登录页面的 HTML 内容
    auto resp = HttpResponse::newFileResponse("./views/templates/regis.html");
    callback(resp);
}

void login::handleRegis(const HttpRequestPtr &req,
                        std::function<void(const HttpResponsePtr &)> &&callback) {
}