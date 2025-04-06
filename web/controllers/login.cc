#include "login.h"
#include "../models/Users.h"// 引入 Users 模型
#include <Utility/uuid.hpp>
#include <cstdint>
#include <drogon/orm/DbClient.h>
#include <drogon/orm/Mapper.h>
#include <drogon/utils/Utilities.h>
#include <service/qrcode.h>
#include <string>
#include <trantor/utils/Logger.h>
#include <Utility/consts.hpp>
using namespace drogon::orm;
using namespace drogon_model::sqlite3;

// 显示登录页面
void login::LoginPage(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback) {
    // 返回登录页面的 HTML 内容
    HttpViewData data;
    auto resp = HttpResponse::newHttpViewResponse("login", data);
    callback(resp);
}

// 处理登录请求
void login::handleLogin(const HttpRequestPtr &req,
                        std::function<void(const HttpResponsePtr &)> &&callback) {
    LOG_DEBUG << "Handling login request";

    // 从请求中获取 userId 和 password
    auto userId = req->getParameter("userId");
    auto password = req->getParameter("password");

    // 查询数据库验证身份
    auto client = drogon::app().getDbClient();
    Mapper<Users> mapper(client);
    mapper.findBy(
            Criteria(Users::Cols::_username, CompareOperator::EQ, userId) &&
                    Criteria(Users::Cols::_password, CompareOperator::EQ, password),
            [callback](const std::vector<Users> &users) {
                if (!users.empty()) {
                    // 登录成功，生成 token 并重定向到根路径
                    Json::Value ret;
                    ret["result"] = "ok";
                    ret["token"] = drogon::utils::getUuid();

                    // 返回重定向响应
                    auto resp = HttpResponse::newRedirectionResponse("/");
                    callback(resp);
                } else {
                    // 登录失败，返回错误信息
                    Json::Value ret;
                    ret["result"] = "failed";
                    ret["message"] = "Invalid username or password";
                    auto resp = HttpResponse::newHttpJsonResponse(ret);
                    resp->setStatusCode(HttpStatusCode::k401Unauthorized);
                    callback(resp);
                }
            },
            [callback](const drogon::orm::DrogonDbException &e) {
                // 数据库查询异常，返回错误信息
                Json::Value ret;
                ret["result"] = "failed";
                ret["message"] = e.base().what();
                auto resp = HttpResponse::newHttpJsonResponse(ret);
                resp->setStatusCode(HttpStatusCode::k500InternalServerError);
                callback(resp);
            });
}

void login::RegisPage(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback) {
    // 返回注册页面的 HTML 内容
    auto resp = HttpResponse::newFileResponse("./views/templates/regis.html");
    callback(resp);
}


void login::handleRegis(const HttpRequestPtr &req,
                        std::function<void(const HttpResponsePtr &)> &&callback) {
    /*
    // 从请求中获取用户名、密码和其他必要信息
    auto username = req->getParameter("username");
    auto email = req->getParameter("email");
    auto password = req->getParameter("password");
    auto confirmPassword = req->getParameter("confirmPassword");

    if (password != confirmPassword) {
        Json::Value ret;
        ret["result"] = "failed";
        ret["message"] = "Passwords do not match";
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(HttpStatusCode::k400BadRequest);
        callback(resp);
        return;c
    }

    // 检查用户名是否已存在
    auto client = drogon::app().getDbClient();
    Mapper<Users> mapper(client);
    mapper.findBy(
            Criteria(Users::Cols::_username, CompareOperator::EQ, username),
            [callback, username, email, password](const std::vector<Users> &users) {
                if (!users.empty()) {
                    Json::Value ret;
                    ret["result"] = "failed";
                    ret["message"] = "Username already exists";
                    auto resp = HttpResponse::newHttpJsonResponse(ret);
                    resp->setStatusCode(HttpStatusCode::k409Conflict);
                    callback(resp);
                } else {
                    // 插入新用户信息
                    Users newUser;
                    newUser.setUsername(username);
                    //todo
                    //newUser.setEmail(email);
                    newUser.setPassword(password);// 注意：这里应存储哈希后的密码，而不是明文
                    //todo

                    newUser.save(client, [callback](Users &&user) {
                               Json::Value ret;
                               ret["result"] = "ok";
                               ret["message"] = "User registered successfully";
                               auto resp = HttpResponse::newHttpJsonResponse(ret);
                               callback(resp); }, [callback](const std::exception &e) {
                               Json::Value ret;
                               ret["result"] = "failed";
                               ret["message"] = e.what();
                               auto resp = HttpResponse::newHttpJsonResponse(ret);
                               resp->setStatusCode(HttpStatusCode::k500InternalServerError);
                               callback(resp); });
                }
            },
            [callback](const std::exception &e) {
                Json::Value ret;
                ret["result"] = "failed";
                ret["message"] = e.what();
                auto resp = HttpResponse::newHttpJsonResponse(ret);
                resp->setStatusCode(HttpStatusCode::k500InternalServerError);
                callback(resp);
            });
            */
}

void login::redirectVerify(const HttpRequestPtr &req,
                           std::function<void(const HttpResponsePtr &)> &&callback, int userID, std::string password) {
}

static inline int countDigits(int n) {
    if (n == 0) return 1;// 特殊情况：0 的位数为 1
    return static_cast<int>(std::log10(std::abs(n))) + 1;
}

void login::generateQRCode(const HttpRequestPtr &req,
                           std::function<void(const HttpResponsePtr &)> &&callback, int userID) {
    std::string baseName = "./LoginQRCode/qrcode.png";
    auto RCodeName = UUID::generate(16 - countDigits(userID));
    QRCodeGenerator qrCodeGenerator;
    const std::string login_url = base_url+"login/";
    qrCodeGenerator.generate_qr_code_with_filename("./LoginQRCode", RCodeName, login_url + std::to_string(userID), 1, 1, 1);
}

void login::generateVerifyToken(const HttpRequestPtr &req,
                                std::function<void(const HttpResponsePtr &)> &&callback) {
}