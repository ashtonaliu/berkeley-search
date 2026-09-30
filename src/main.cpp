#include <curl/curl.h>
#include <iostream>

import berkeley_search.crawler;

int main() {
    curl_global_init(CURL_GLOBAL_DEFAULT);

    Crawler crawler("https://eecs.berkeley.edu/");
    const auto documents = crawler.crawl(10);

    std::cout
        << "Documents collected: "
        << documents.size()
        << '\n';

    curl_global_cleanup();

    return 0;
}
