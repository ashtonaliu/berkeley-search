#include <curl/curl.h>

#include <iostream>
#include <string>

import berkeley_search.url_normalizer;

namespace {

bool expectEqual(
    const std::string& description,
    const std::string& actual,
    const std::string& expected
) {
    if (actual == expected) {
        return true;
    }

    std::cerr
        << description << " failed.\n"
        << "Expected: " << expected << '\n'
        << "Actual:   " << actual << '\n';
    return false;
}

bool expectTrue(const std::string& description, bool condition) {
    if (condition) {
        return true;
    }

    std::cerr << description << " failed.\n";
    return false;
}

} // namespace

int main() {
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) {
        std::cerr << "Could not initialize libcurl.\n";
        return 1;
    }

    const std::string base =
        "https://eecs.berkeley.edu/research/areas/index.html";
    const UrlNormalizer normalizer(base);

    bool passed = true;
    passed &= expectEqual(
        "origin",
        normalizer.origin(),
        "https://eecs.berkeley.edu"
    );
    passed &= expectEqual(
        "root-relative link",
        normalizer.resolve(base, "/people/#faculty"),
        "https://eecs.berkeley.edu/people/"
    );
    passed &= expectEqual(
        "path-relative link",
        normalizer.resolve(base, "labs/systems.html"),
        "https://eecs.berkeley.edu/research/areas/labs/systems.html"
    );
    passed &= expectEqual(
        "parent-relative link",
        normalizer.resolve(base, "../courses/"),
        "https://eecs.berkeley.edu/research/courses/"
    );
    passed &= expectEqual(
        "query-relative link",
        normalizer.resolve(base, "?view=all#top"),
        "https://eecs.berkeley.edu/research/areas/index.html?view=all"
    );
    passed &= expectEqual(
        "fragment removal",
        normalizer.resolve(base, "#overview"),
        base
    );
    passed &= expectTrue(
        "unsupported scheme rejection",
        normalizer.resolve(base, "javascript:alert(1)").empty()
    );
    passed &= expectEqual(
        "default port removal",
        normalizer.resolve(base, "https://eecs.berkeley.edu:443/"),
        "https://eecs.berkeley.edu/"
    );
    passed &= expectTrue(
        "same-origin comparison",
        normalizer.isAllowedOrigin(
            "https://EECS.BERKELEY.EDU/courses/"
        )
    );
    passed &= expectTrue(
        "cross-origin rejection",
        !normalizer.isAllowedOrigin(
            "https://example.com/courses/"
        )
    );
    passed &= expectEqual(
        "path and query extraction",
        normalizer.path(
            "https://eecs.berkeley.edu/courses/?year=2026#fall"
        ),
        "/courses/?year=2026"
    );

    curl_global_cleanup();

    if (!passed) {
        std::cerr << "URL normalization returned an unexpected result.\n";
        return 1;
    }

    std::cout << "All URL normalizer tests passed.\n";
    return 0;
}
