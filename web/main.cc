#include "controllers/cache.h"
#include <drogon/drogon.h>
#include <drogon/orm/Mapper.h>

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::sqlite3;

// 缓存函数
static void cacheArticles() {
    auto clientPtr = drogon::app().getDbClient("default");
    Mapper<Articles> mapper(clientPtr);
    std::vector<Articles> articles = mapper.findAll();
    // 使用 std::move 避免拷贝开销
    querycache::cachedAllArticles(std::move(articles));
    // 打印当地时间
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::tm *now_tm = std::localtime(&now_c);
    char buf[100];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", now_tm);
    LOG_INFO << "Cache articles at " << buf;
}


int main() {
    drogon::app().addListener("0.0.0.0", 5555);
    drogon::app().loadConfigFile("../config.json");

    // 第一次启动一秒后调用一次cachedArticles
    drogon::app().getLoop()->runAfter(1.0, []() {
        cacheArticles();
    });

    // 每 30 秒调用一次 cacheArticles，后面可以做成通知模型
    drogon::app().getLoop()->runEvery(30.0, []() {
        cacheArticles();
    });

    drogon::app().run();
    return 0;
}