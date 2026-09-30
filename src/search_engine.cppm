module;

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

export module berkeley_search.search_engine;

export import berkeley_search.search_index;

export using SearchScore = std::size_t;

export struct SearchResult {
    DocumentId documentId;
    SearchScore score;
};

export class SearchEngine {
public:
    void addDocument(const Document& document);

    std::vector<SearchResult> search(
        const std::string& query
    ) const;

    const Document* document(DocumentId documentId) const;

    std::size_t documentCount() const;
    std::size_t termCount() const;

private:
    SearchIndex index;
    std::unordered_map<DocumentId, Document> documents;
};
