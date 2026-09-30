module;

#include <cstddef>
#include <string>

export module berkeley_search.document;

export struct Document {
    std::size_t id;
    std::string url;
    std::string title;
    std::string text;
};
