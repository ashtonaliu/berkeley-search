#include <curl/curl.h>

import berkeley_search.crawler;

int main() {
    curl_global_init(CURL_GLOBAL_DEFAULT);

    Crawler crawler("https://eecs.berkeley.edu/");
    crawler.crawl(10);

    curl_global_cleanup();

    return 0;
}
