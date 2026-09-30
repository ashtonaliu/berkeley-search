#include <iostream>
#include <string>

import berkeley_search.html_text_extractor;

namespace {

bool expectText(
    const HtmlTextExtractor& extractor,
    const std::string& html,
    const std::string& expected
) {
    const std::string actual = extractor.extract(html);

    if (actual == expected) {
        return true;
    }

    std::cerr
        << "HTML extraction failed.\n"
        << "Expected: " << expected << '\n'
        << "Actual:   " << actual << '\n';
    return false;
}

} // namespace

int main() {
    const HtmlTextExtractor extractor;
    bool passed = true;

    passed &= expectText(extractor, "", "");
    passed &= expectText(extractor, "Plain text", "Plain text");
    passed &= expectText(
        extractor,
        "<h1>Operating Systems</h1><p>Learn about processes.</p>",
        "Operating Systems Learn about processes."
    );
    passed &= expectText(
        extractor,
        "<main>  Berkeley\n <strong>EECS</strong>\t research </main>",
        "Berkeley EECS research"
    );
    passed &= expectText(
        extractor,
        "<a href=\"https://example.com?q=1>0\" title=\"Ignored\">Course page</a>",
        "Course page"
    );
    passed &= expectText(
        extractor,
        "Visible<!-- navigation label --> text",
        "Visible text"
    );
    passed &= expectText(
        extractor,
        "Before<script>const example = '<p>hidden</p>';</script>After",
        "Before After"
    );
    passed &= expectText(
        extractor,
        "Before<script>if (a < b) { run(); }</script>After",
        "Before After"
    );
    passed &= expectText(
        extractor,
        "<STYLE>body { display: none; }</STYLE><p>Visible</p>",
        "Visible"
    );

    if (!passed) {
        return 1;
    }

    std::cout << "All HTML text extractor tests passed.\n";
    return 0;
}
