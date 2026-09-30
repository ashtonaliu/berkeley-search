#include "crawler.h"

#include <curl/curl.h>

int main() {
    curl_global_init(CURL_GLOBAL_DEFAULT);

    Crawler crawler("https://eecs.berkeley.edu/");
    crawler.crawl(10);

    curl_global_cleanup();

    return 0;
}
