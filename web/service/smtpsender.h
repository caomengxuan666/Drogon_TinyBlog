/**
 * @FilePath     : /tiny_blog/web/service/smtpsender.h
 * @Description  :  进一步封装smtp类作为外部控制器的接口
                    降低代码复杂度和耦合程度
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-04-08 17:12:11
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once
#include "smtp.h"
#include <format>
#include <functional>
#include <string>

using refstring = const std::string &;

template<class ConcreteEmail>
class SMTPSender {
public:
    void sendEmail(refstring recv, refstring verificationCode, std::function<void(const std::string &)> callback) {
        // CRTP,调用子类的方法，并在发送完成后调用回调函数
        static_cast<ConcreteEmail *>(this)->sendEmailImpl(recv, verificationCode, callback);
    }

protected:
    EmailWrapper _email_sender_ctx;
};

/**
 * @author       : caomengxuan
 * @brief        : 注册邮件，给用户第一次注册用,发送注册验证码
 * @note         : 这里的注册邮件模板采用硬编码，在这里手动格式化字符串，增加用户的具体信息
 * @return        {*}
**/
class RegisSender : public SMTPSender<RegisSender> {
public:
    void sendEmailImpl(refstring recv, refstring verificationCode, std::function<void(const std::string &)> callback) {
        std::string title = "您有一个来自tinyblog的邮件，请查收您的注册验证码";
        std::string html_content = std::format(R"(
            <!DOCTYPE html>
            <html lang="en">
            <head>
                <meta charset="UTF-8">
                <title>来自tinyBlog的注册验证码</title>
            </head>
            <body>
                <h1>Hello,</h1>
                <p>感谢你注册tinyblog!这是你的验证码!<b>{}</b></p>
                <p>如果这不是你主动发送的邮件,请勿泄露您的验证码.</p>
               <p>If you did not register, please ignore this email.</p>
            </body>
            </html>)",
                                                       verificationCode);

        _email_sender_ctx.send_email(recv, html_content, title);
        callback("Verification code sent successfully");
    }
};

/**
 * @author       : caomengxuan
 * @brief        : 重置密码邮件，给用户重置密码使用，发送重置验证码
 * @return        {*}
**/
class ResetPwdSender : public SMTPSender<ResetPwdSender> {
public:
    void sendEmailImpl(refstring recv, refstring verificationCode, std::function<void(const std::string &)> callback) {
        std::string title = "您有一个来自tinyblog的邮件，请查收您的重置密码验证码";
        std::string html_content = std::format(R"(
            <!DOCTYPE html>
            <html lang="en">
            <head>
                <meta charset="UTF-8">
                <title>来自tinyBlog的重置密码验证码</title>
            </head>
            <body>
                <h1>Hello,</h1>
                <p>您正在重置密码，以下是您的验证码<b>{}</b></p>
                <p>如果这不是你主动发送的邮件,请勿泄露您的验证码.</p>
                <p>If you did not register, please ignore this email.</p>
            </body>
            </html>)",
                                                       verificationCode);

        _email_sender_ctx.send_email(recv, html_content, title);
        callback("Verification code sent successfully");
    }
};

/**
 * @author       : caomengxuan
 * @brief        : 给用户邮箱推荐每日比较火爆的文章,发送我们做的一个HTML文档
 * @return        {*}
**/
//todo
class AdviseSender : public SMTPSender<AdviseSender> {
public:
    [[maybe_unused]]
    void sendEmailImpl(refstring recv, refstring /*verificationCode*/, std::function<void(const std::string &)> callback) {
        std::string title = "每日推荐文章";
        std::string html_content = R"(
            <!DOCTYPE html>
            <html lang="en">
            <head>
                <meta charset="UTF-8">
                <title>tinyBlog 每日推荐</title>
            </head>
            <body>
                <h1>每日推荐文章</h1>
                <p>以下是今日推荐的文章列表：</p>
                <ul>
                    <li><a href="http://example.com/article1">文章1</a></li>
                    <li><a href="http://example.com/article2">文章2</a></li>
                    <li><a href="http://example.com/article3">文章3</a></li>
                </ul>
            </body>
            </html>)";
        _email_sender_ctx.send_email(recv, html_content, title);
        callback("Daily recommendations sent successfully");
    }
};