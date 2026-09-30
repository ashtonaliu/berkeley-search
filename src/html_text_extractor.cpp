module;

#include <cctype>
#include <string>

module berkeley_search.html_text_extractor;

namespace {

struct Tag {
    std::string name;
    bool isClosing = false;
};

void appendSpace(std::string& text) {
    if (!text.empty() && text.back() != ' ') {
        text.push_back(' ');
    }
}

std::size_t findTagEnd(
    const std::string& html,
    std::size_t tagStart
) {
    char quote = '\0';

    for (std::size_t index = tagStart + 1; index < html.size(); ++index) {
        const char character = html[index];

        if (quote != '\0') {
            if (character == quote) {
                quote = '\0';
            }
        } else if (character == '\'' || character == '"') {
            quote = character;
        } else if (character == '>') {
            return index;
        }
    }

    return std::string::npos;
}

Tag parseTag(
    const std::string& html,
    std::size_t tagStart,
    std::size_t tagEnd
) {
    Tag tag;
    std::size_t index = tagStart + 1;

    while (
        index < tagEnd &&
        std::isspace(static_cast<unsigned char>(html[index]))
    ) {
        ++index;
    }

    if (index < tagEnd && html[index] == '/') {
        tag.isClosing = true;
        ++index;
    }

    while (
        index < tagEnd &&
        std::isspace(static_cast<unsigned char>(html[index]))
    ) {
        ++index;
    }

    while (index < tagEnd) {
        const auto byte = static_cast<unsigned char>(html[index]);

        if (!std::isalnum(byte) && html[index] != '-') {
            break;
        }

        tag.name.push_back(static_cast<char>(std::tolower(byte)));
        ++index;
    }

    return tag;
}

bool matchesIgnoringCase(
    const std::string& text,
    std::size_t position,
    const std::string& expected
) {
    if (position + expected.size() > text.size()) {
        return false;
    }

    for (std::size_t index = 0; index < expected.size(); ++index) {
        const auto actualByte =
            static_cast<unsigned char>(text[position + index]);
        const auto expectedByte =
            static_cast<unsigned char>(expected[index]);

        if (std::tolower(actualByte) != std::tolower(expectedByte)) {
            return false;
        }
    }

    return true;
}

std::size_t findClosingTag(
    const std::string& html,
    std::size_t position,
    const std::string& tagName
) {
    const std::string prefix = "</" + tagName;

    while ((position = html.find('<', position)) != std::string::npos) {
        if (matchesIgnoringCase(html, position, prefix)) {
            const std::size_t boundary = position + prefix.size();

            if (
                boundary < html.size() &&
                (html[boundary] == '>' ||
                 std::isspace(static_cast<unsigned char>(html[boundary])))
            ) {
                return position;
            }
        }

        ++position;
    }

    return std::string::npos;
}

} // namespace

std::string HtmlTextExtractor::extract(const std::string& html) const {
    std::string text;
    std::string hiddenElement;
    std::size_t index = 0;

    while (index < html.size()) {
        if (!hiddenElement.empty()) {
            const std::size_t tagStart =
                findClosingTag(html, index, hiddenElement);

            if (tagStart == std::string::npos) {
                break;
            }

            const std::size_t tagEnd = findTagEnd(html, tagStart);

            if (tagEnd == std::string::npos) {
                break;
            }

            hiddenElement.clear();
            appendSpace(text);
            index = tagEnd + 1;
            continue;
        }

        if (html.compare(index, 4, "<!--") == 0) {
            const std::size_t commentEnd = html.find("-->", index + 4);

            if (commentEnd == std::string::npos) {
                break;
            }

            appendSpace(text);
            index = commentEnd + 3;
            continue;
        }

        if (html[index] == '<') {
            const std::size_t tagEnd = findTagEnd(html, index);

            if (tagEnd == std::string::npos) {
                break;
            }

            const Tag tag = parseTag(html, index, tagEnd);

            if (
                !tag.isClosing &&
                (tag.name == "script" || tag.name == "style")
            ) {
                hiddenElement = tag.name;
            }

            appendSpace(text);
            index = tagEnd + 1;
            continue;
        }

        const auto byte = static_cast<unsigned char>(html[index]);

        if (std::isspace(byte)) {
            appendSpace(text);
        } else {
            text.push_back(html[index]);
        }

        ++index;
    }

    if (!text.empty() && text.back() == ' ') {
        text.pop_back();
    }

    return text;
}

std::string HtmlTextExtractor::extractTitle(
    const std::string& html
) const {
    std::size_t index = 0;

    while (index < html.size()) {
        const std::size_t tagStart = html.find('<', index);

        if (tagStart == std::string::npos) {
            return "";
        }

        if (html.compare(tagStart, 4, "<!--") == 0) {
            const std::size_t commentEnd = html.find("-->", tagStart + 4);

            if (commentEnd == std::string::npos) {
                return "";
            }

            index = commentEnd + 3;
            continue;
        }

        const std::size_t tagEnd = findTagEnd(html, tagStart);

        if (tagEnd == std::string::npos) {
            return "";
        }

        const Tag tag = parseTag(html, tagStart, tagEnd);

        if (
            !tag.isClosing &&
            (tag.name == "script" || tag.name == "style")
        ) {
            const std::size_t closingTag =
                findClosingTag(html, tagEnd + 1, tag.name);

            if (closingTag == std::string::npos) {
                return "";
            }

            const std::size_t closingTagEnd =
                findTagEnd(html, closingTag);

            if (closingTagEnd == std::string::npos) {
                return "";
            }

            index = closingTagEnd + 1;
            continue;
        }

        if (!tag.isClosing && tag.name == "title") {
            const std::size_t closingTag =
                findClosingTag(html, tagEnd + 1, tag.name);

            if (closingTag == std::string::npos) {
                return "";
            }

            return extract(
                html.substr(
                    tagEnd + 1,
                    closingTag - (tagEnd + 1)
                )
            );
        }

        index = tagEnd + 1;
    }

    return "";
}
