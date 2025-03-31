#include "../models/Articles.h"
#include "HomePageCtrl.h"
#include "RestfulArticlesCtrl.h"
#include "RestfulUsersCtrl.h"
#include <drogon/HttpResponse.h>
#include <drogon/HttpViewData.h>
#include <drogon/orm/BaseBuilder.h>
#include <drogon/orm/DbClient.h>
#include <drogon/orm/Mapper.h>
#include <drogon/utils/Utilities.h> // 用于类型转换
#include <trantor/utils/Logger.h>   // 用于日志记录
#include "cache.h" // 引入缓存头文件

using namespace drogon;
using namespace drogon::orm;

void IndexCtrl::asyncHandleHttpRequest(
        const HttpRequestPtr &req,
        std::function<void(const HttpResponsePtr &)> &&callback) {
    drogon::HttpViewData data;

    // 所有基础类型数据必须转换为字符串
    data.insert("site_name", "Tiny Blog");

    // 从缓存中获取文章数据
    std::vector<Articles> articles = querycache::getcachedAllArticles();

    data.insert("articles", articles);

    // 分类数据需要转换数值为字符串
    std::map<std::string, std::string> strCategories;
    for (const auto &[cat, count]: std::map<std::string, size_t>{{"技术", 2}, {"生活", 1}}) {
        strCategories[cat] = drogon::utils::formattedString("%zu", count);
    }
    data.insert("categories", strCategories);

    // 热门文章处理
    std::vector<Articles> hot_articles = articles; // 假设热门文章就是所有文章
    for (auto &article: hot_articles) {
        article.setId(article.getValueOfId());
    }
    data.insert("hot_articles", hot_articles);

    // 标签数据（假设从文章中提取）
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

    auto resp = HttpResponse::newHttpViewResponse("HomePageView", data);
    callback(resp);
}