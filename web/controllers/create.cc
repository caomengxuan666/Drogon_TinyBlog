/**
 * @FilePath     : /tiny_blog/web/controllers/create.cc
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-31 19:46:39
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#include "create.h"
#include "../models/Articles.h" // 引入 Articles 模型
#include <drogon/orm/DbClient.h>
#include <drogon/orm/Mapper.h>
#include <trantor/utils/Logger.h>

// 实现保存功能
void create::save(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    // 获取请求中的 JSON 数据
    auto json = req->getJsonObject();
    if (!json) {
        Json::Value response;
        response["status"] = "error";
        response["message"] = "Invalid JSON data!";
        auto resp = HttpResponse::newHttpJsonResponse(response);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    // 提取标题和内容
    std::string title = (*json)["title"].asString();
    std::string content = (*json)["content"].asString();
    int userid = (*json)["userid"].asInt();

    // 将内容编码为 HTML 或 Base64（这里选择直接存储 HTML）
    std::string encodedContent = content; // 如果需要 Base64 编码，可以使用 drogon::utils::base64Encode(content)

    // 获取数据库客户端
    auto clientPtr = drogon::app().getDbClient("default");
    drogon::orm::Mapper<drogon_model::sqlite3::Articles> mapper(clientPtr);

    // 创建新的文章记录
    drogon_model::sqlite3::Articles article;
    article.setTitle(title);
    article.setAuthorId(userid);
    article.setContent(encodedContent);
    article.setCreatedAt(trantor::Date::now());

    try {
        // 插入数据库
        mapper.insert(article);
        LOG_INFO << "Article saved successfully: " << title;

        // 返回成功响应
        Json::Value response;
        response["status"] = "success";
        response["message"] = "Article saved successfully!";
        auto resp = HttpResponse::newHttpJsonResponse(response);
        callback(resp);
    } catch (const drogon::orm::DrogonDbException &e) {
        LOG_ERROR << "Failed to save article: " << e.base().what();

        // 返回错误响应
        Json::Value response;
        response["status"] = "error";
        response["message"] = "Failed to save article!";
        auto resp = HttpResponse::newHttpJsonResponse(response);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}

// Add definition of your processing function here
void create::get(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, int authorid) {

    // 直接返回 webroot 目录下的 HTML 文件
    auto resp = HttpResponse::newFileResponse("./views/templates/create.html");
    callback(resp);
}
