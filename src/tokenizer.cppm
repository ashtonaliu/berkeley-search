module;

#include <string>
#include <vector>

export module berkeley_search.tokenizer;

export class Tokenizer {
public:
    std::vector<std::string> tokenize(
        const std::string& text
    ) const;
};
