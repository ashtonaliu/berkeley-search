module;

#include <cctype>
#include <string>

#include <curl/curl.h>

module berkeley_search.url_normalizer;

namespace {

std::string lowercase(std::string value) {
    for (char& character : value) {
        character = static_cast<char>(
            std::tolower(static_cast<unsigned char>(character))
        );
    }

    return value;
}

std::string getPart(
    CURLU* url,
    CURLUPart part,
    unsigned int flags = 0
) {
    char* value = nullptr;

    if (curl_url_get(url, part, &value, flags) != CURLUE_OK) {
        return "";
    }

    const std::string result = value;
    curl_free(value);
    return result;
}

std::string canonicalOrigin(const std::string& value) {
    CURLU* url = curl_url();

    if (url == nullptr) {
        return "";
    }

    const CURLUcode parsed = curl_url_set(
        url,
        CURLUPART_URL,
        value.c_str(),
        CURLU_DISALLOW_USER
    );

    if (parsed != CURLUE_OK) {
        curl_url_cleanup(url);
        return "";
    }

    const std::string scheme = lowercase(
        getPart(url, CURLUPART_SCHEME)
    );
    const std::string host = lowercase(
        getPart(url, CURLUPART_HOST)
    );
    const std::string port = getPart(
        url,
        CURLUPART_PORT,
        CURLU_NO_DEFAULT_PORT
    );

    curl_url_cleanup(url);

    if (
        (scheme != "http" && scheme != "https") ||
        host.empty()
    ) {
        return "";
    }

    std::string origin = scheme + "://" + host;

    if (!port.empty()) {
        origin += ":" + port;
    }

    return origin;
}

} // namespace

UrlNormalizer::UrlNormalizer(const std::string& startUrl)
    : allowedOrigin(canonicalOrigin(startUrl)) {}

std::string UrlNormalizer::resolve(
    const std::string& baseUrl,
    const std::string& link
) const {
    if (baseUrl.empty() || link.empty()) {
        return "";
    }

    CURLU* url = curl_url();

    if (url == nullptr) {
        return "";
    }

    std::string baseWithoutFragment = baseUrl;
    const std::size_t baseFragmentPosition =
        baseWithoutFragment.find('#');

    if (baseFragmentPosition != std::string::npos) {
        baseWithoutFragment.erase(baseFragmentPosition);
    }

    std::string linkWithoutFragment = link;
    const std::size_t fragmentPosition =
        linkWithoutFragment.find('#');

    if (fragmentPosition != std::string::npos) {
        linkWithoutFragment.erase(fragmentPosition);
    }

    CURLUcode result = curl_url_set(
        url,
        CURLUPART_URL,
        baseWithoutFragment.c_str(),
        CURLU_DISALLOW_USER
    );

    if (result == CURLUE_OK && !linkWithoutFragment.empty()) {
        result = curl_url_set(
            url,
            CURLUPART_URL,
            linkWithoutFragment.c_str(),
            CURLU_DISALLOW_USER
        );
    }

    const std::string scheme = lowercase(
        getPart(url, CURLUPART_SCHEME)
    );

    if (
        result != CURLUE_OK ||
        (scheme != "http" && scheme != "https")
    ) {
        curl_url_cleanup(url);
        return "";
    }

    const std::string resolved = getPart(
        url,
        CURLUPART_URL,
        CURLU_NO_DEFAULT_PORT
    );

    curl_url_cleanup(url);
    return resolved;
}

bool UrlNormalizer::isAllowedOrigin(const std::string& url) const {
    return
        !allowedOrigin.empty() &&
        canonicalOrigin(url) == allowedOrigin;
}

std::string UrlNormalizer::path(const std::string& value) const {
    CURLU* url = curl_url();

    if (url == nullptr) {
        return "/";
    }

    const CURLUcode parsed = curl_url_set(
        url,
        CURLUPART_URL,
        value.c_str(),
        CURLU_DISALLOW_USER
    );

    if (parsed != CURLUE_OK) {
        curl_url_cleanup(url);
        return "/";
    }

    std::string path = getPart(url, CURLUPART_PATH);
    const std::string query = getPart(url, CURLUPART_QUERY);

    curl_url_cleanup(url);

    if (path.empty()) {
        path = "/";
    }

    if (!query.empty()) {
        path += "?" + query;
    }

    return path;
}

const std::string& UrlNormalizer::origin() const {
    return allowedOrigin;
}
