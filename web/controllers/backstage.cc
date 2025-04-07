/**
 * @FilePath     : /tiny_blog/web/controllers/backstage.cc
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-04-07 22:07:47
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#include "backstage.h"
#include <drogon/HttpResponse.h>
#include <drogon/HttpViewData.h>


void backstage::get(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    HttpViewData data;
    auto resp=HttpResponse::newHttpViewResponse("BackStage",data);
    callback(resp);
}