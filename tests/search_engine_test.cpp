#include <iostream>
#include <string>
#include <vector>

import berkeley_search.search_engine;

int main() {
    SearchEngine engine;

    engine.addDocument(Document{
        10,
        "https://example.test/operating-systems",
        "Operating Systems",
        "Operating systems and systems programming"
    });
    engine.addDocument(Document{
        20,
        "https://example.test/distributed-systems",
        "Distributed Systems",
        "Distributed systems research"
    });
    engine.addDocument(Document{
        30,
        "https://example.test/robotics",
        "Robotics",
        "Robotics research"
    });

    const std::vector<SearchResult> ranked =
        engine.search("OPERATING, systems!");
    const std::vector<SearchResult> repeated =
        engine.search("systems systems");
    const std::vector<SearchResult> tied = engine.search("research");

    bool passed = true;
    passed &= engine.documentCount() == 3;
    passed &= engine.termCount() == 7;
    passed &= ranked.size() == 2;
    passed &= ranked[0].documentId == 10 && ranked[0].score == 3;
    passed &= ranked[1].documentId == 20 && ranked[1].score == 1;
    passed &= repeated.size() == 2;
    passed &= repeated[0].score == 2;
    passed &= tied.size() == 2;
    passed &= tied[0].documentId == 20;
    passed &= tied[1].documentId == 30;
    passed &= engine.search("unknown").empty();
    passed &= engine.search("").empty();

    const Document* document = engine.document(10);
    passed &= document != nullptr;
    passed &= document != nullptr && document->title == "Operating Systems";
    passed &= engine.document(999) == nullptr;

    // A repeated ID is ignored by both document storage and the index.
    engine.addDocument(Document{10, "replacement", "Replacement", "robotics"});
    passed &= engine.documentCount() == 3;
    passed &= engine.document(10)->title == "Operating Systems";
    passed &= engine.search("robotics").size() == 1;

    if (!passed) {
        std::cerr << "Search engine returned unexpected results.\n";
        return 1;
    }

    std::cout << "All search engine tests passed.\n";
    return 0;
}
