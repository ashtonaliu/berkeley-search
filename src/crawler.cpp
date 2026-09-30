module;

#include <algorithm>
#include <chrono>
#include <cctype>
#include <iostream>
#include <queue>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

#include <curl/curl.h>

module berkeley_search.crawler;

import berkeley_search.html_text_extractor;
import berkeley_search.robots_policy;

namespace {

bool isHttpUrl(const std::string& url) {
    return
        url.find("http://") == 0 ||
        url.find("https://") == 0;
}

std::string normalizedContentType(const char* contentType) {
    if (contentType == nullptr) {
        return "";
    }

    std::string normalized = contentType;
    std::transform(
        normalized.begin(),
        normalized.end(),
        normalized.begin(),
        [](unsigned char character) {
            return static_cast<char>(std::tolower(character));
        }
    );

    return normalized;
}

bool isHtmlContentType(const char* contentType) {
    const std::string normalized = normalizedContentType(contentType);

    return
        normalized.find("text/html") == 0 ||
        normalized.find("application/xhtml+xml") == 0;
}

bool isRobotsContentType(const char* contentType) {
    return normalizedContentType(contentType).find("text/plain") == 0;
}

std::string originFromUrl(const std::string& url) {
    const std::size_t schemeEnd = url.find("://");

    if (schemeEnd == std::string::npos) {
        return "";
    }

    const std::size_t authorityEnd =
        url.find_first_of("/?#", schemeEnd + 3);

    if (authorityEnd == std::string::npos) {
        return url;
    }

    return url.substr(0, authorityEnd);
}

std::string pathFromUrl(const std::string& url) {
    const std::size_t schemeEnd = url.find("://");

    if (schemeEnd == std::string::npos) {
        return "/";
    }

    const std::size_t pathStart =
        url.find_first_of("/?#", schemeEnd + 3);

    if (pathStart == std::string::npos || url[pathStart] == '#') {
        return "/";
    }

    const std::size_t fragment = url.find('#', pathStart);
    std::string path = url.substr(pathStart, fragment - pathStart);

    if (!path.empty() && path.front() == '?') {
        path.insert(path.begin(), '/');
    }

    return path.empty() ? "/" : path;
}

} // namespace

Crawler::Crawler(
    const std::string& startUrl,
    CrawlerOptions options
)
    : startUrl(startUrl), options(options) {}

std::vector<Document> Crawler::crawl(std::size_t maxPages) {
    std::queue<std::string> urls;
    std::vector<Document> documents;
    const HtmlTextExtractor textExtractor;
    RobotsPolicy robotsPolicy;
    bool hasMadeHttpRequest = false;

    if (isHttpUrl(startUrl)) {
        const std::string robotsUrl =
            originFromUrl(startUrl) + "/robots.txt";

        std::cout << "Checking robots policy: " << robotsUrl << '\n';

        const DownloadResult robots =
            download(robotsUrl, ResourceType::Robots);
        hasMadeHttpRequest = true;

        if (!robots.transportSucceeded) {
            std::cerr
                << "Could not retrieve robots.txt; stopping crawl.\n";
            return documents;
        }

        if (robots.statusCode >= 200 && robots.statusCode < 300) {
            if (!robots.contentTypeAccepted) {
                std::cerr
                    << "robots.txt was not served as text/plain; "
                    << "stopping crawl.\n";
                return documents;
            }

            robotsPolicy.parse(robots.body, options.productToken);
        } else if (
            robots.statusCode < 400 ||
            robots.statusCode >= 500
        ) {
            std::cerr
                << "robots.txt returned HTTP status "
                << robots.statusCode
                << "; stopping crawl.\n";
            return documents;
        } else {
            std::cout
                << "No robots policy published; continuing crawl.\n";
        }
    }

    // Put starting URL in queue.
    urls.push(startUrl);

    // Mark it as discovered immediately.
    discovered.insert(startUrl);

    while (!urls.empty() && visited.size() < maxPages) {
        std::string url = urls.front();
        urls.pop();

        if (isHttpUrl(url)) {
            if (!robotsPolicy.allows(pathFromUrl(url))) {
                std::cout << "Blocked by robots.txt: " << url << '\n';
                continue;
            }

            if (
                hasMadeHttpRequest &&
                options.requestDelay.count() > 0
            ) {
                std::this_thread::sleep_for(options.requestDelay);
            }

            hasMadeHttpRequest = true;
        }

        std::cout << "\nCrawling: " << url << '\n';

        const DownloadResult page = download(url, ResourceType::Html);

        if (!page.transportSucceeded || page.body.empty()) {
            std::cout << "Failed to download page\n";
            continue;
        }

        if (
            isHttpUrl(url) &&
            (page.statusCode < 200 || page.statusCode >= 300)
        ) {
            std::cout
                << "Skipping HTTP status "
                << page.statusCode
                << '\n';
            continue;
        }

        if (!page.contentTypeAccepted) {
            std::cout << "Skipping non-HTML content\n";
            continue;
        }

        const std::string& html = page.body;

        // This page has now actually been crawled.
        visited.insert(url);

        documents.push_back(Document{
            documents.size(),
            url,
            textExtractor.extractTitle(html),
            textExtractor.extract(html)
        });

        std::vector<std::string> links = extractLinks(html);

        for (const std::string& link : links) {
            std::string normalized = normalizeUrl(link);

            if (normalized.empty()) {
                continue;
            }

            if (!shouldVisit(normalized)) {
                continue;
            }

            if (
                isHttpUrl(normalized) &&
                !robotsPolicy.allows(pathFromUrl(normalized))
            ) {
                continue;
            }

            // Already discovered earlier?
            if (discovered.contains(normalized)) {
                continue;
            }

            // Mark discovered before putting it in the queue.
            discovered.insert(normalized);

            std::cout
                << "Adding to queue: "
                << normalized
                << '\n';

            urls.push(normalized);
        }
    }

    std::cout << "\nFinished crawling.\n";
    std::cout
        << "Pages visited: "
        << visited.size()
        << '\n';

    return documents;
}

std::size_t Crawler::writeCallback(
    void* contents,
    std::size_t size,
    std::size_t nmemb,
    void* userData
) {
    std::size_t totalBytes = size * nmemb;

    std::string* output = static_cast<std::string*>(userData);
    output->append(static_cast<char*>(contents), totalBytes);

    return totalBytes;
}

Crawler::DownloadResult Crawler::download(
    const std::string& url,
    ResourceType resourceType
) {
    DownloadResult downloadResult;
    CURL* curl = curl_easy_init();

    if (!curl) {
        return downloadResult;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &downloadResult.body);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
    curl_easy_setopt(
        curl,
        CURLOPT_USERAGENT,
        options.userAgent.c_str()
    );

    if (options.requestTimeout.count() > 0) {
        curl_easy_setopt(
            curl,
            CURLOPT_TIMEOUT_MS,
            static_cast<long>(options.requestTimeout.count())
        );
    }

    if (options.connectTimeout.count() > 0) {
        curl_easy_setopt(
            curl,
            CURLOPT_CONNECTTIMEOUT_MS,
            static_cast<long>(options.connectTimeout.count())
        );
    }

    const CURLcode transferResult = curl_easy_perform(curl);

    if (transferResult != CURLE_OK) {
        std::cerr
            << "curl error: "
            << curl_easy_strerror(transferResult)
            << '\n';

        curl_easy_cleanup(curl);
        return downloadResult;
    }

    downloadResult.transportSucceeded = true;

    if (isHttpUrl(url)) {
        char* contentType = nullptr;

        curl_easy_getinfo(
            curl,
            CURLINFO_RESPONSE_CODE,
            &downloadResult.statusCode
        );
        curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &contentType);

        downloadResult.contentTypeAccepted =
            resourceType == ResourceType::Html
                ? isHtmlContentType(contentType)
                : isRobotsContentType(contentType);
    } else {
        downloadResult.contentTypeAccepted = true;
    }

    curl_easy_cleanup(curl);
    return downloadResult;
}

std::vector<std::string> Crawler::extractLinks(
    const std::string& html
) {
    std::vector<std::string> links;

    std::string target = "href=\"";
    std::size_t pos = 0;

    while ((pos = html.find(target, pos)) != std::string::npos) {
        pos += target.length();

        std::size_t end = html.find('"', pos);

        if (end == std::string::npos) {
            break;
        }

        std::string link = html.substr(pos, end - pos);
        links.push_back(link);
        pos = end + 1;
    }

    return links;
}

std::string Crawler::normalizeUrl(const std::string& link) {
    if (link.empty()) {
        return "";
    }

    // Already absolute.
    if (
        link.find("https://") == 0 ||
        link.find("http://") == 0
    ) {
        return link;
    }

    // Root-relative URL.
    if (link[0] == '/') {
        return "https://eecs.berkeley.edu" + link;
    }

    return "";
}

bool Crawler::shouldVisit(const std::string& url) {
    if (url.empty()) {
        return false;
    }

    // Only crawl EECS Berkeley.
    if (url.find("https://eecs.berkeley.edu/") != 0) {
        return false;
    }

    // Ignore URL fragments.
    if (url.find('#') != std::string::npos) {
        return false;
    }

    return true;
}
