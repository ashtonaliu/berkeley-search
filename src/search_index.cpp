module;

#include <string>
#include <vector>

module berkeley_search.search_index;

import berkeley_search.tokenizer;

void SearchIndex::addDocument(const Document& document) {
    const bool inserted = documentIds.insert(document.id).second;

    if (!inserted) {
        return;
    }

    const Tokenizer tokenizer;
    const std::vector<std::string> tokens =
        tokenizer.tokenize(document.text);

    for (const std::string& token : tokens) {
        ++index[token][document.id];
    }
}

const PostingList& SearchIndex::postings(
    const std::string& term
) const {
    static const PostingList emptyPostings;

    const auto match = index.find(term);

    if (match == index.end()) {
        return emptyPostings;
    }

    return match->second;
}

std::size_t SearchIndex::documentCount() const {
    return documentIds.size();
}

std::size_t SearchIndex::termCount() const {
    return index.size();
}
