module;

#include <string>

export module berkeley_search.html_text_extractor;

export class HtmlTextExtractor {
public:
    std::string extract(const std::string& html) const;
    std::string extractTitle(const std::string& html) const;
};
