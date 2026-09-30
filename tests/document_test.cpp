#include <iostream>
#include <string>

import berkeley_search.document;

int main() {
    const Document document{
        7,
        "https://eecs.berkeley.edu/research/",
        "Research",
        "Berkeley EECS research areas"
    };

    const bool fieldsMatch =
        document.id == 7 &&
        document.url == "https://eecs.berkeley.edu/research/" &&
        document.title == "Research" &&
        document.text == "Berkeley EECS research areas";

    if (!fieldsMatch) {
        std::cerr << "Document fields did not retain their values.\n";
        return 1;
    }

    std::cout << "All document tests passed.\n";
    return 0;
}
