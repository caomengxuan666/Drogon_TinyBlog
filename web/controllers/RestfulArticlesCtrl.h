/**
 *
 *  RestfulArticlesCtrl.h
 *  This file is generated by drogon_ctl
 *
 */

#pragma once

#include <drogon/HttpController.h>
#include "RestfulArticlesCtrlBase.h"

#include "Articles.h"
using namespace drogon;
using namespace drogon_model::sqlite3;
/**
 * @brief this class is created by the drogon_ctl command.
 * this class is a restful API controller for reading and writing the articles table.
 */

class RestfulArticlesCtrl: public drogon::HttpController<RestfulArticlesCtrl>, public RestfulArticlesCtrlBase
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(RestfulArticlesCtrl::getOne,"/articles/{1}",Get,Options);
    ADD_METHOD_TO(RestfulArticlesCtrl::updateOne,"/articles/{1}",Put,Options);
    ADD_METHOD_TO(RestfulArticlesCtrl::deleteOne,"/articles/{1}",Delete,Options);
    ADD_METHOD_TO(RestfulArticlesCtrl::get,"/articles",Get,Options);
    ADD_METHOD_TO(RestfulArticlesCtrl::create,"/articles",Post,Options);
    //ADD_METHOD_TO(RestfulArticlesCtrl::update,"/articles",Put,Options);
    METHOD_LIST_END
     
    void getOne(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback,
                Articles::PrimaryKeyType &&id);
    void updateOne(const HttpRequestPtr &req,
                   std::function<void(const HttpResponsePtr &)> &&callback,
                   Articles::PrimaryKeyType &&id);
    void deleteOne(const HttpRequestPtr &req,
                   std::function<void(const HttpResponsePtr &)> &&callback,
                   Articles::PrimaryKeyType &&id);
    void get(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback);
    void create(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback);

};
