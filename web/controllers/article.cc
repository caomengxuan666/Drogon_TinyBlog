#include "article.h"
#include "../models/Articles.h"
#include "../models/Users.h"
#include "cache.h"
#include <drogon/HttpResponse.h>
#include <drogon/HttpViewData.h>
#include <drogon/orm/DbClient.h>
#include <drogon/orm/Mapper.h>
#include <trantor/utils/Logger.h>

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::sqlite3;

void article::getArticle(const HttpRequestPtr &req,
                         std::function<void(const HttpResponsePtr &)> &&callback,
                         int articleId) {
    // 从缓存中获取文章数据
    auto articles = querycache::getcachedAllArticles();
    auto it = std::find_if(articles.begin(), articles.end(), [articleId](const Articles &article) {
        return article.getValueOfId() == articleId;
    });

    if (it != articles.end()) {
        const Articles &article = *it;
        auto clientPtr = drogon::app().getDbClient("default");

        // 异步获取用户数据
        article.getUser(clientPtr, [this, req, callback, article](const Users &user) mutable {
            try {
                HttpViewData data;

                // 插入文章数据
                data.insert("title", article.getValueOfTitle());
                data.insert("author", user.getValueOfUsername());
                data.insert("date", article.getValueOfCreatedAt().toDbStringLocal());
                data.insert("category", article.getValueOfCategory());
                data.insert("tags", article.getValueOfTag());
                data.insert("content", article.getValueOfContent());

                // 动态生成导航栏数据(缓存)
                auto sidebarLinks = this->getSidebarLinks();
                data.insert("sidebarLinks", sidebarLinks);

                // 生成响应
                auto resp = HttpResponse::newHttpViewResponse("ArticleDetailView", data);
                callback(resp);
            } catch (const std::exception &e) {
                LOG_ERROR << "Error in processing article data: " << e.what();
                this->NotFound(req, std::move(callback));
            } }, [this, req, callback](const DrogonDbException &e) mutable {
            LOG_ERROR << "Failed to get user: " << e.base().what();
            this->NotFound(req, std::move(callback)); });
    } else {
        LOG_ERROR << "Article with ID " << articleId << " not found in cache";
        NotFound(req, std::move(callback));
    }
}

std::vector<std::pair<std::string, std::string>> article::getSidebarLinks() {
    // 导航直接查缓存记录，避免一直查询
    auto articles = querycache::getcachedAllArticles();
    // 构造导航栏数据
    std::vector<std::pair<std::string, std::string>> sidebarLinks;
    for (const auto &article: articles) {
        sidebarLinks.emplace_back(
                "/article/" + std::to_string(article.getValueOfId()),// URL
                article.getValueOfTitle()                            // 标题
        );
    }
    return sidebarLinks;
}

void article::NotFound(const HttpRequestPtr &req,
                       std::function<void(const HttpResponsePtr &)> &&callback) {
    auto resp = HttpResponse::newNotFoundResponse();
    callback(resp);
}