#include <iostream>
#include <string>

import berkeley_search.search_index;

namespace {

bool hasFrequency(
    const PostingList& postings,
    DocumentId documentId,
    TermFrequency expected
) {
    const auto match = postings.find(documentId);
    return match != postings.end() && match->second == expected;
}

} // namespace

int main() {
    SearchIndex index;

    const Document systemsDocument{
        1,
        "https://example.test/systems",
        "Operating Systems",
        "Operating systems, systems programming!"
    };
    const Document distributedDocument{
        2,
        "https://example.test/distributed",
        "Distributed Systems",
        "Distributed systems research"
    };

    index.addDocument(systemsDocument);
    index.addDocument(distributedDocument);

    const PostingList& systems = index.postings("systems");
    const PostingList& operating = index.postings("operating");
    const PostingList& missing = index.postings("robotics");

    bool passed = true;
    passed &= index.documentCount() == 2;
    passed &= index.termCount() == 5;
    passed &= systems.size() == 2;
    passed &= hasFrequency(systems, 1, 2);
    passed &= hasFrequency(systems, 2, 1);
    passed &= hasFrequency(operating, 1, 1);
    passed &= missing.empty();

    // Adding the same document ID again must not double its frequencies.
    index.addDocument(systemsDocument);
    passed &= index.documentCount() == 2;
    passed &= hasFrequency(index.postings("systems"), 1, 2);

    if (!passed) {
        std::cerr << "Search index did not contain the expected postings.\n";
        return 1;
    }

    std::cout << "All search index tests passed.\n";
    return 0;
}
