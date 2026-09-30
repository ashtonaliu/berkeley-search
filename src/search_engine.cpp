module;

#include <algorithm>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

module berkeley_search.search_engine;

import berkeley_search.tokenizer;

void SearchEngine::addDocument(const Document& document) {
    const auto [position, inserted] =
        documents.emplace(document.id, document);

    if (inserted) {
        index.addDocument(position->second);
    }
}

std::vector<SearchResult> SearchEngine::search(
    const std::string& query
) const {
    const Tokenizer tokenizer;
    const std::vector<std::string> tokens = tokenizer.tokenize(query);
    const std::unordered_set<std::string> queryTerms(
        tokens.begin(),
        tokens.end()
    );

    std::unordered_map<DocumentId, SearchScore> scores;

    for (const std::string& term : queryTerms) {
        for (const auto& [documentId, frequency] : index.postings(term)) {
            scores[documentId] += frequency;
        }
    }

    std::vector<SearchResult> results;
    results.reserve(scores.size());

    for (const auto& [documentId, score] : scores) {
        results.push_back(SearchResult{documentId, score});
    }

    std::sort(
        results.begin(),
        results.end(),
        [](const SearchResult& left, const SearchResult& right) {
            if (left.score != right.score) {
                return left.score > right.score;
            }

            return left.documentId < right.documentId;
        }
    );

    return results;
}

const Document* SearchEngine::document(DocumentId documentId) const {
    const auto match = documents.find(documentId);

    if (match == documents.end()) {
        return nullptr;
    }

    return &match->second;
}

std::size_t SearchEngine::documentCount() const {
    return documents.size();
}

std::size_t SearchEngine::termCount() const {
    return index.termCount();
}
