module;

#include <chrono>
#include <cstddef>
#include <string>
#include <unordered_set>
#include <vector>

export module berkeley_search.crawler;

export import berkeley_search.document;

export struct CrawlerOptions {
    std::chrono::milliseconds requestDelay{1000};
    std::chrono::milliseconds requestTimeout{15000};
    std::chrono::milliseconds connectTimeout{5000};
    std::string productToken{"BerkeleySearchLearningBot"};
    std::string userAgent{"BerkeleySearchLearningBot/0.1"};
};

export class Crawler {
public:
    explicit Crawler(
        const std::string& startUrl,
        CrawlerOptions options = {}
    );

    std::vector<Document> crawl(std::size_t maxPages);

private:
    enum class ResourceType {
        Html,
        Robots
    };

    struct DownloadResult {
        std::string body;
        long statusCode = 0;
        bool transportSucceeded = false;
        bool contentTypeAccepted = false;
    };

    std::string startUrl;
    CrawlerOptions options;
    std::unordered_set<std::string> discovered;
    std::unordered_set<std::string> visited;

    static std::size_t writeCallback(
        void* contents,
        std::size_t size,
        std::size_t nmemb,
        void* userData
    );

    DownloadResult download(
        const std::string& url,
        ResourceType resourceType
    );

    std::vector<std::string> extractLinks(
        const std::string& html
    );

    std::string normalizeUrl(const std::string& link);
    bool shouldVisit(const std::string& url);
};
