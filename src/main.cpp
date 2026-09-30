#include <iostream>
#include <string>
#include <queue>
#include <unordered_set>
#include <vector>

#include <curl/curl.h>

class Crawler {
public:
    Crawler(const std::string& startUrl)
        : startUrl(startUrl) {}

    void crawl(int maxPages) {
        std::queue<std::string> urls;

        // Put starting URL in queue
        urls.push(startUrl);

        // Mark it as discovered immediately
        discovered.insert(startUrl);

        while (!urls.empty() && visited.size() < maxPages) {
            std::string url = urls.front();
            urls.pop();

            std::cout << "\nCrawling: " << url << '\n';

            std::string html = downloadPage(url);

            if (html.empty()) {
                std::cout << "Failed to download page\n";
                continue;
            }

            // This page has now actually been crawled
            visited.insert(url);

            std::vector<std::string> links =
                extractLinks(html);

            for (const std::string& link : links) {

                std::string normalized =
                    normalizeUrl(link);

                if (normalized.empty()) {
                    continue;
                }

                if (!shouldVisit(normalized)) {
                    continue;
                }

                // Already discovered earlier?
                if (discovered.contains(normalized)) {
                    continue;
                }

                // Mark discovered BEFORE putting it in queue
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
    }

private:
    std::string startUrl;

    // URLs we've already found / queued
    std::unordered_set<std::string> discovered;

    // URLs we've successfully downloaded
    std::unordered_set<std::string> visited;

    static size_t writeCallback(
        void* contents,
        size_t size,
        size_t nmemb,
        void* userData
    ) {
        size_t totalBytes = size * nmemb;

        std::string* output =
            static_cast<std::string*>(userData);

        output->append(
            static_cast<char*>(contents),
            totalBytes
        );

        return totalBytes;
    }

    std::string downloadPage(
        const std::string& url
    ) {
        CURL* curl = curl_easy_init();

        if (!curl) {
            return "";
        }

        std::string html;

        curl_easy_setopt(
            curl,
            CURLOPT_URL,
            url.c_str()
        );

        curl_easy_setopt(
            curl,
            CURLOPT_WRITEFUNCTION,
            writeCallback
        );

        curl_easy_setopt(
            curl,
            CURLOPT_WRITEDATA,
            &html
        );

        curl_easy_setopt(
            curl,
            CURLOPT_FOLLOWLOCATION,
            1L
        );

        curl_easy_setopt(
            curl,
            CURLOPT_USERAGENT,
            "BerkeleySearchBot/0.1"
        );

        CURLcode result =
            curl_easy_perform(curl);

        if (result != CURLE_OK) {
            std::cerr
                << "curl error: "
                << curl_easy_strerror(result)
                << '\n';

            curl_easy_cleanup(curl);

            return "";
        }

        curl_easy_cleanup(curl);

        return html;
    }

    std::vector<std::string> extractLinks(
        const std::string& html
    ) {
        std::vector<std::string> links;

        std::string target = "href=\"";
        size_t pos = 0;

        while (
            (pos = html.find(target, pos))
            != std::string::npos
        ) {
            pos += target.length();

            size_t end =
                html.find('"', pos);

            if (end == std::string::npos) {
                break;
            }

            std::string link =
                html.substr(
                    pos,
                    end - pos
                );

            links.push_back(link);

            pos = end + 1;
        }

        return links;
    }

    std::string normalizeUrl(
        const std::string& link
    ) {
        if (link.empty()) {
            return "";
        }

        // Already absolute
        if (
            link.find("https://") == 0 ||
            link.find("http://") == 0
        ) {
            return link;
        }

        // Root-relative URL
        if (link[0] == '/') {
            return
                "https://eecs.berkeley.edu"
                + link;
        }

        return "";
    }

    bool shouldVisit(
        const std::string& url
    ) {
        if (url.empty()) {
            return false;
        }

        // Only crawl EECS Berkeley
        if (
            url.find(
                "https://eecs.berkeley.edu/"
            ) != 0
        ) {
            return false;
        }

        // Ignore URL fragments
        if (
            url.find('#')
            != std::string::npos
        ) {
            return false;
        }

        return true;
    }
};

int main() {
    curl_global_init(
        CURL_GLOBAL_DEFAULT
    );

    Crawler crawler(
        "https://eecs.berkeley.edu/"
    );

    crawler.crawl(10);

    curl_global_cleanup();

    return 0;
}