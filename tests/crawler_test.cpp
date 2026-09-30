#include <curl/curl.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

import berkeley_search.crawler;

int main() {
    const std::filesystem::path pagePath =
        std::filesystem::temp_directory_path() /
        "berkeley-search-crawler-test.html";

    {
        std::ofstream page(pagePath);

        if (!page) {
            std::cerr << "Could not create the crawler test page.\n";
            return 1;
        }

        page
            << "<html><head><title>Test page</title></head>"
            << "<body><h1>Operating Systems</h1>"
            << "<script>hidden()</script>"
            << "<p>Learn about processes.</p></body></html>";
    }

    const std::string pageUrl = "file://" + pagePath.string();

    if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) {
        std::filesystem::remove(pagePath);
        std::cerr << "Could not initialize libcurl.\n";
        return 1;
    }

    Crawler crawler(pageUrl);
    const std::vector<Document> documents = crawler.crawl(1);

    curl_global_cleanup();
    std::filesystem::remove(pagePath);

    const bool documentMatches =
        documents.size() == 1 &&
        documents[0].id == 0 &&
        documents[0].url == pageUrl &&
        documents[0].title.empty() &&
        documents[0].text ==
            "Test page Operating Systems Learn about processes.";

    if (!documentMatches) {
        std::cerr << "Crawler did not produce the expected document.\n";
        return 1;
    }

    std::cout << "All crawler integration tests passed.\n";
    return 0;
}
