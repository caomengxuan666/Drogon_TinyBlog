#include "../models/Articles.h"
#include "../models/Users.h"
#include "HomePageCtrl.h"
#include "RestfulArticlesCtrl.h"
#include "RestfulUsersCtrl.h"
#include "cache.h"
#include <drogon/HttpResponse.h>
#include <drogon/HttpViewData.h>
#include <drogon/orm/BaseBuilder.h>
#include <drogon/orm/DbClient.h>
#include <drogon/orm/Mapper.h>
#include <drogon/utils/Utilities.h>
#include <iomanip>
#include <sstream>
#include <trantor/utils/Logger.h>

using namespace drogon;
using namespace drogon::orm;

// 日期格式化函数
inline static std::string formatDateTime(const trantor::Date &date) {
    std::string rawDate = date.toFormattedString(false);// 20250331 11:32:46.7

    if (rawDate.size() < 17) {
        return rawDate;// 避免异常情况
    }

    // 手动格式化：YYYYMMDD -> YYYY-MM-DD
    std::string formattedDate;
    formattedDate += rawDate.substr(0, 4) + "-";// YYYY-
    formattedDate += rawDate.substr(4, 2) + "-";// MM-
    formattedDate += rawDate.substr(6, 2) + " ";// DD

    // 只保留 HH:MM
    formattedDate += rawDate.substr(9, 5);// HH:MM

    return formattedDate;
}


void IndexCtrl::asyncHandleHttpRequest(
        const HttpRequestPtr &req,
        std::function<void(const HttpResponsePtr &)> &&callback) {
    drogon::HttpViewData data;
    data.insert("site_name", "Tiny Blog");

    // 获取所有文章
    auto articles = querycache::getcachedAllArticles();

    // 收集所有唯一的 authorId
    std::unordered_set<int64_t> authorIds;
    for (const auto &article: articles) {
        authorIds.insert(article.getValueOfAuthorId());
    }

    // 构造 SQL 查询语句
    std::ostringstream oss;
    for (const auto &id: authorIds) {
        if (!oss.str().empty()) {
            oss << ",";
        }
        oss << id;
    }
    std::string idsStr = oss.str();

    // 批量查询用户信息
    auto client = app().getDbClient();
    auto sql = "SELECT id, username FROM users WHERE id IN (" + idsStr + ");";
    auto result = client->execSqlSync(sql);

    std::unordered_map<int64_t, std::string> userMap;
    for (const auto &row: result) {
        int64_t userId = row["id"].template as<int64_t>();
        std::string username = row["username"].template as<std::string>();
        userMap[userId] = username;
    }

    // 创建包含作者名称和格式化日期的文章列表
    std::vector<std::tuple<Articles, std::string, std::string>> articlesWithAuthorsAndDates;
    for (const auto &article: articles) {
        auto it = userMap.find(article.getValueOfAuthorId());
        if (it != userMap.end()) {
            // 格式化日期
            std::string formattedDate = formatDateTime(article.getValueOfCreatedAt());
            articlesWithAuthorsAndDates.emplace_back(article, it->second, formattedDate);
        } else {
            LOG_ERROR << "未找到作者ID: " << article.getValueOfAuthorId();
            std::string formattedDate = formatDateTime(article.getValueOfCreatedAt());
            articlesWithAuthorsAndDates.emplace_back(article, "未知作者", formattedDate);
        }
    }

    // 插入处理后的数据
    data.insert("articles", articlesWithAuthorsAndDates);


    // 分类数据处理（保持原有逻辑）
    std::map<std::string, std::string> strCategories;
    for (const auto &[cat, count]: std::map<std::string, size_t>{{"技术", 2}, {"生活", 1}}) {
        strCategories[cat] = drogon::utils::formattedString("%zu", count);
    }
    data.insert("categories", strCategories);
    // 热门文章处理（保持原有逻辑）
    std::vector<Articles> hot_articles = articles;
    for (auto &article: hot_articles) {
        article.setId(article.getValueOfId());
    }
    data.insert("hot_articles", hot_articles);

    // 标签处理（保持原有逻辑）
    std::set<std::string> tagsSet;
    for (const auto &article: articles) {
        std::istringstream iss(article.getValueOfTag());
        std::string tag;
        while (std::getline(iss, tag, ',')) {
            tagsSet.insert(tag);
        }
    }
    std::vector<std::string> tags(tagsSet.begin(), tagsSet.end());
    data.insert("tags", tags);


    auto session = req->getSession();
    // 检查 session 中是否存在 user_id
    auto userIdOpt = session->getOptional<long>("user_id");
    bool isLoggedIn = userIdOpt.has_value();
    LOG_INFO << "isLoggedIn: " << isLoggedIn;

    auto userNameOpt = session->getOptional<std::string>("username");

    // 获取用户昵称，默认值为 "游客"
    auto username = userNameOpt.has_value() ? userNameOpt.value() : "游客";

    auto userAvatarOpt = session->getOptional<std::string>("userAvatar");

    auto userAvatar = userAvatarOpt.has_value() ? userAvatarOpt.value() : "/static/images/default_avatar.png";

    // 将值插入视图数据
    data.insert("isLoggedIn", isLoggedIn);
    data.insert("username", username);
    data.insert("userAvatar", userAvatar);

    auto resp = HttpResponse::newHttpViewResponse("HomePageView", data);

    // 添加 CORS 头部
    resp->addHeader("Access-Control-Allow-Origin", "*");
    resp->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    resp->addHeader("Access-Control-Allow-Headers", "Content-Type");

    callback(resp);
}