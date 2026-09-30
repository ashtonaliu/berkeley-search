module;

#include <cctype>
#include <string>
#include <vector>

module berkeley_search.tokenizer;

std::vector<std::string> Tokenizer::tokenize(
    const std::string& text
) const {
    std::vector<std::string> tokens;
    std::string currentToken;

    for (char character : text) {
        const auto byte = static_cast<unsigned char>(character);

        if (std::isalnum(byte)) {
            currentToken.push_back(
                static_cast<char>(std::tolower(byte))
            );
        } else if (!currentToken.empty()) {
            tokens.push_back(currentToken);
            currentToken.clear();
        }
    }

    if (!currentToken.empty()) {
        tokens.push_back(currentToken);
    }

    return tokens;
}
