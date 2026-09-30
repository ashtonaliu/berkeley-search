#include <iostream>
#include <string>
#include <vector>

import berkeley_search.tokenizer;

namespace {

bool expectTokens(
    const Tokenizer& tokenizer,
    const std::string& input,
    const std::vector<std::string>& expected
) {
    const std::vector<std::string> actual = tokenizer.tokenize(input);

    if (actual == expected) {
        return true;
    }

    std::cerr << "Tokenization failed for: " << input << '\n';
    return false;
}

} // namespace

int main() {
    const Tokenizer tokenizer;
    bool passed = true;

    passed &= expectTokens(
        tokenizer,
        "Berkeley EECS teaches Operating Systems!",
        {"berkeley", "eecs", "teaches", "operating", "systems"}
    );
    passed &= expectTokens(
        tokenizer,
        "CS162 has 3 projects.",
        {"cs162", "has", "3", "projects"}
    );
    passed &= expectTokens(
        tokenizer,
        "  commas, dashes---and tabs\tseparate words  ",
        {"commas", "dashes", "and", "tabs", "separate", "words"}
    );
    passed &= expectTokens(tokenizer, "", {});
    passed &= expectTokens(tokenizer, "...!", {});

    if (!passed) {
        return 1;
    }

    std::cout << "All tokenizer tests passed.\n";
    return 0;
}
