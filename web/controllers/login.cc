#include "login.h"
#include "../models/Users.h"// 引入 Users 模型
#include "../service/urlencode.h"
#include "Utility/consts.hpp"
#include <Utility/uuid.hpp>
#include <cstdint>
#include <drogon/orm/DbClient.h>
#include <drogon/orm/Mapper.h>
#include <drogon/utils/Utilities.h>
#include <json/value.h>
#include <service/qrcode.h>
#include <service/smtpsender.h>
#include <string>
#include <trantor/utils/Logger.h>
#include <vector>

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
            [callback, req](const std::vector<Users> &users) {
                if (users.empty()) {
                    // 登录失败，返回错误信息
                    Json::Value ret;
                    ret["result"] = "failed";
                    ret["message"] = "Invalid username or password";
                    auto resp = HttpResponse::newHttpJsonResponse(ret);
                    resp->setStatusCode(HttpStatusCode::k401Unauthorized);
                    callback(resp);
                    return;
                }

                // 确保 users[0] 存在
                const auto &user = users[0];

                // 获取会话对象，并检查是否为空
                auto session = req->getSession();
                if (!session) {
                    LOG_ERROR << "Session is null!";
                    Json::Value ret;
                    ret["result"] = "failed";
                    ret["message"] = "Internal server error";
                    auto resp = HttpResponse::newHttpJsonResponse(ret);
                    resp->setStatusCode(HttpStatusCode::k500InternalServerError);
                    callback(resp);
                    return;
                }

                // 将用户 ID 插入会话
                try {
                    session->insert("user_id", user.getValueOfId());
                    //打印user_id
                    session->insert("username", user.getValueOfUsername());
                    session->insert("role", user.getValueOfRole());
                    // 登录成功重定向到根路径
                    auto resp = HttpResponse::newRedirectionResponse("/");
                    callback(resp);
                } catch (const std::exception &e) {
                    LOG_ERROR << "Error inserting session data: " << e.what();
                    Json::Value ret;
                    ret["result"] = "failed";
                    ret["message"] = "Internal server error";
                    auto resp = HttpResponse::newHttpJsonResponse(ret);
                    resp->setStatusCode(HttpStatusCode::k500InternalServerError);
                    callback(resp);
                }
            },
            [callback](const DrogonDbException &e) {
                // 数据库查询异常，返回错误信息
                Json::Value ret;
                ret["result"] = "failed";
                ret["message"] = e.base().what();
                auto resp = HttpResponse::newHttpJsonResponse(ret);
                resp->setStatusCode(HttpStatusCode::k500InternalServerError);
                callback(resp);
            });
}

// 显示注册页面
void login::RegisPage(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback) {
    // 返回注册页面的 HTML 内容
    auto resp = HttpResponse::newFileResponse("./views/templates/regis.html");
    callback(resp);
}

// 处理注册请求
void login::handleRegis(const HttpRequestPtr &req,
                        std::function<void(const HttpResponsePtr &)> &&callback) {
    LOG_DEBUG << "Handling registration request";

    // 检查请求体是否为空
    auto body = req->getBody();
    if (body.empty()) {
        Json::Value ret;
        ret["result"] = "failed";
        ret["message"] = "Empty request body";
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(HttpStatusCode::k400BadRequest);
        callback(resp);
        return;
    }

    // 尝试解析 JSON 请求体
    auto json = req->getJsonObject();
    if (!json) {
        Json::Value ret;
        ret["result"] = "failed";
        ret["message"] = "Invalid JSON request body";
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(HttpStatusCode::k400BadRequest);
        callback(resp);
        return;
    }

    // 提取参数
    auto username = (*json)["username"].asString();
    auto email = (*json)["email"].asString();
    auto password = (*json)["password"].asString();
    auto confirmPassword = (*json)["confirmPassword"].asString();
    auto verificationCode = (*json)["verificationCode"].asString();

    // 打印接收到的参数
    std::cout << "username: " << username << std::endl;
    std::cout << "email: " << email << std::endl;
    std::cout << "password: " << password << std::endl;
    std::cout << "confirmPassword: " << confirmPassword << std::endl;
    std::cout << "verificationCode: " << verificationCode << std::endl;

    // 检查输入是否为空
    if (username.empty() || password.empty() || confirmPassword.empty() || verificationCode.empty()) {
        Json::Value ret;
        ret["result"] = "failed";
        ret["message"] = "All fields are required";
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(HttpStatusCode::k400BadRequest);
        LOG_WARN << "All fields are required";
        callback(resp);
        return;
    }

    // 检查两次密码是否一致
    if (password != confirmPassword) {
        Json::Value ret;
        ret["result"] = "failed";
        ret["message"] = "Passwords do not match";
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(HttpStatusCode::k400BadRequest);
        LOG_WARN << "Passwords do not match";
        callback(resp);
        return;
    }

    // 验证验证码
    auto session = req->getSession();
    if (!session) {
        LOG_ERROR << "Session is null!";
        Json::Value ret;
        ret["result"] = "failed";
        ret["message"] = "Internal server error";
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(HttpStatusCode::k500InternalServerError);
        callback(resp);
        return;
    }

    auto storedVerificationCode = session->getOptional<std::string>("verification_code");
    if (!storedVerificationCode.has_value() || verificationCode != storedVerificationCode.value()) {
        Json::Value ret;
        ret["result"] = "failed";
        ret["message"] = "Verification code not found";
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(HttpStatusCode::k400BadRequest);
        LOG_ERROR << "Verification code not found";
        callback(resp);
        return;
    }

    // 检查用户名是否已存在
    auto client = drogon::app().getDbClient();
    Mapper<Users> mapper(client);

    mapper.findBy(
            Criteria(Users::Cols::_username, CompareOperator::EQ, username),
            [client, callback, username, email, password](const std::vector<Users> &users) {
                if (!users.empty()) {
                    // 用户名已存在
                    Json::Value ret;
                    ret["result"] = "failed";
                    ret["message"] = "Username already exists";
                    auto resp = HttpResponse::newHttpJsonResponse(ret);
                    resp->setStatusCode(HttpStatusCode::k409Conflict);
                    LOG_ERROR << "Username already exists";
                    callback(resp);
                } else {
                    // 插入新用户信息
                    Users newUser;
                    newUser.setUsername(username);
                    newUser.setEmail(email);      // 设置邮箱
                    newUser.setPassword(password);// 注意：这里应存储哈希后的密码
                    newUser.setRole("guest");     // 默认角色为 guest

                    // 插入新用户到数据库
                    Mapper<Users> mapper(client);
                    mapper.insert(
                            newUser,
                            [callback](const Users &user) {
                                // 注册成功，重定向到 /login
                                auto resp = HttpResponse::newHttpResponse();
                                resp->setStatusCode(HttpStatusCode::k302Found);// 302 重定向
                                resp->addHeader("Location", "/login");         // 设置重定向目标
                                LOG_INFO << "User registered successfully and redirected to /login";
                                callback(resp);
                            },
                            [callback](const DrogonDbException &e) {
                                Json::Value ret;
                                ret["result"] = "failed";
                                ret["message"] = e.base().what();
                                auto resp = HttpResponse::newHttpJsonResponse(ret);
                                resp->setStatusCode(HttpStatusCode::k500InternalServerError);
                                LOG_ERROR << e.base().what();
                                callback(resp);
                            });
                }
            },
            [callback](const DrogonDbException &e) {
                // 数据库查询异常
                Json::Value ret;
                ret["result"] = "failed";
                ret["message"] = e.base().what();
                auto resp = HttpResponse::newHttpJsonResponse(ret);
                resp->setStatusCode(HttpStatusCode::k500InternalServerError);
                LOG_ERROR << e.base().what();
                callback(resp);
            });
}

// 快速幂获取位数
[[nodiscard]] constexpr inline std::uint8_t countDigits(int n) noexcept {
    if (n == 0) return 1;
    n = std::abs(n);
    std::uint8_t count = 0;
    int power = 1;
    while (power <= n) {
        power *= 10;
        ++count;
    }
    return count;
}

// 获取验证码
void login::generateVerifyToken(const HttpRequestPtr &req,
                                std::function<void(const HttpResponsePtr &)> &&callback) {
    auto email = req->getParameter("email");
    if (email.empty()) {
        Json::Value ret;
        ret["result"] = "failed";
        ret["message"] = "Email is required";
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(HttpStatusCode::k400BadRequest);
        callback(resp);
        return;
    }

    // 生成验证码
    std::string verificationCode = UUID::generate_num(6);
    LOG_INFO << "Generated verification code: " << verificationCode;

    // 保存验证码到会话
    auto session = req->getSession();
    if (!session) {
        LOG_ERROR << "Session is null!";
        Json::Value ret;
        ret["result"] = "failed";
        ret["message"] = "Internal server error";
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(HttpStatusCode::k500InternalServerError);
        callback(resp);
        return;
    }
    session->insert("verification_code", verificationCode);

    // 发送验证码邮件
    SMTPSender<RegisSender> regisSender;
    regisSender.sendEmail(email, verificationCode, [callback](const std::string &message) {
        Json::Value ret;
        ret["result"] = "ok";
        ret["message"] = message;
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    });
}

void login::generateQRCode(const HttpRequestPtr &req,
                           std::function<void(const HttpResponsePtr &)> &&callback, int userID) {
    std::string baseName = "./LoginQRCode/qrcode.png";

    auto RCodeName = UUID::generate(16 - countDigits(userID));// 生成唯一名称
    UrlEncode2QRCode(baseName, RCodeName, base_url + "login/" + std::to_string(userID));

    // 返回二维码文件路径
    Json::Value ret;
    ret["result"] = "ok";
    ret["qrcode"] = RCodeName;
    auto resp = HttpResponse::newHttpJsonResponse(ret);
    callback(resp);
}