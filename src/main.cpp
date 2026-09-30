#include <curl/curl.h>
#include <iostream>
#include <string>

import berkeley_search.crawler;
import berkeley_search.search_engine;

int main() {
    curl_global_init(CURL_GLOBAL_DEFAULT);

    Crawler crawler("https://eecs.berkeley.edu/");
    const auto documents = crawler.crawl(10);

    SearchEngine searchEngine;

    for (const Document& document : documents) {
        searchEngine.addDocument(document);
    }

    std::cout
        << "Documents collected: "
        << documents.size()
        << '\n';
    std::cout
        << "Unique terms indexed: "
        << searchEngine.termCount()
        << '\n';

    curl_global_cleanup();

    std::string query;

    while (true) {
        std::cout << "\nSearch (empty to quit): ";

        if (!std::getline(std::cin, query) || query.empty()) {
            break;
        }

        const auto results = searchEngine.search(query);

        if (results.empty()) {
            std::cout << "No matching documents.\n";
            continue;
        }

        std::size_t displayed = 0;

        for (const SearchResult& result : results) {
            const Document* document =
                searchEngine.document(result.documentId);

            if (document == nullptr) {
                continue;
            }

            const std::string& label = document->title.empty()
                ? document->url
                : document->title;

            std::cout
                << displayed + 1 << ". " << label << '\n'
                << "   " << document->url << '\n'
                << "   Score: " << result.score << '\n';

            ++displayed;

            if (displayed == 10) {
                break;
            }
        }
    }

    return 0;
}
