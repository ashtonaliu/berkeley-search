module;

#include <iostream>
#include <queue>
#include <string>
#include <unordered_set>
#include <vector>

#include <curl/curl.h>

module berkeley_search.crawler;

import berkeley_search.html_text_extractor;

Crawler::Crawler(const std::string& startUrl)
    : startUrl(startUrl) {}

std::vector<Document> Crawler::crawl(std::size_t maxPages) {
    std::queue<std::string> urls;
    std::vector<Document> documents;
    const HtmlTextExtractor textExtractor;

    // Put starting URL in queue.
    urls.push(startUrl);

    // Mark it as discovered immediately.
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

std::string Crawler::downloadPage(const std::string& url) {
    CURL* curl = curl_easy_init();

    if (!curl) {
        return "";
    }

    std::string html;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &html);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "BerkeleySearchBot/0.1");

    CURLcode result = curl_easy_perform(curl);

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
