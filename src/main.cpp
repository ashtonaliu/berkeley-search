#include <curl/curl.h>
#include <iostream>

import berkeley_search.crawler;
import berkeley_search.search_index;

int main() {
    curl_global_init(CURL_GLOBAL_DEFAULT);

    Crawler crawler("https://eecs.berkeley.edu/");
    const auto documents = crawler.crawl(10);

    SearchIndex searchIndex;

    for (const Document& document : documents) {
        searchIndex.addDocument(document);
    }

    std::cout
        << "Documents collected: "
        << documents.size()
        << '\n';
    std::cout
        << "Unique terms indexed: "
        << searchIndex.termCount()
        << '\n';

    curl_global_cleanup();

    return 0;
}
