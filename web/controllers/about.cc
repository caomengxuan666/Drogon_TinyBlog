#include "about.h"
#include <drogon/HttpResponse.h>

void about::asyncHandleHttpRequest(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    auto resp = HttpResponse::newFileResponse("./views/templates/about.html");
    callback(resp);
}
