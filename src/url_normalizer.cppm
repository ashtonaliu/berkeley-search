module;

#include <string>

export module berkeley_search.url_normalizer;

export class UrlNormalizer {
public:
    explicit UrlNormalizer(const std::string& startUrl);

    std::string resolve(
        const std::string& baseUrl,
        const std::string& link
    ) const;

    bool isAllowedOrigin(const std::string& url) const;
    std::string path(const std::string& url) const;
    const std::string& origin() const;

private:
    std::string allowedOrigin;
};
