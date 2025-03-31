/**
 * @FilePath     : /tiny_blog/web/controllers/cache.h
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-31 20:40:04
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once
#include "../models/Articles.h"
#include <vector>
namespace querycache {
    using namespace drogon;
    using namespace drogon::orm;
    using namespace drogon_model::sqlite3;

    inline std::vector<Articles> cachedarticles;
    inline static void cachedAllArticles(const std::vector<Articles> &articles) {
        cachedarticles = articles;
    }

    inline static auto &getcachedAllArticles() {
        return cachedarticles;
    }
}// namespace querycache

// namespace querycache