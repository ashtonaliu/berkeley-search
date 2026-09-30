module;

#include <cstddef>
#include <string>
#include <unordered_set>
#include <vector>

export module berkeley_search.crawler;

export class Crawler {
public:
    explicit Crawler(const std::string& startUrl);

    void crawl(std::size_t maxPages);

private:
    std::string startUrl;
    std::unordered_set<std::string> discovered;
    std::unordered_set<std::string> visited;

    static std::size_t writeCallback(
        void* contents,
        std::size_t size,
        std::size_t nmemb,
        void* userData
    );

    std::string downloadPage(const std::string& url);

    std::vector<std::string> extractLinks(
        const std::string& html
    );

    std::string normalizeUrl(const std::string& link);
    bool shouldVisit(const std::string& url);
};
