#

#include <drogon/HttpController.h>

using namespace drogon;

/**
 * @brief Controller class for handling article-related HTTP requests.
 * 
 * This class provides methods to handle various article-related operations such as retrieving an article by ID.
 */
class article : public drogon::HttpController<article> {
public:
    METHOD_LIST_BEGIN
    /**
     * @brief Method to retrieve an article by its ID.
     * 
     * This method handles GET requests to the path /{1}, where {1} is the article ID.
     * It retrieves the article data and renders it using the ArticleDetailView template.
     * 
     * Example usage:
     * @code
     * // In article.cc
     * void article::getArticle(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, int articleId) {
     *     // ... existing code ...
     * }
     * @endcode
     */
    METHOD_ADD(article::getArticle, "/{1}", Get);
    METHOD_LIST_END

    /**
     * @brief Retrieves an article by its ID and renders it using the ArticleDetailView template.
     * 
     * @param req The HTTP request object.
     * @param callback The callback function to return the HTTP response.
     * @param articleId The ID of the article to retrieve.
     */
    void getArticle(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, int articleId);

    void NotFound(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback);

    std::vector<std::pair<std::string, std::string>> getSidebarLinks();
};