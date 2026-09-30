module;

#include <cstddef>
#include <string>
#include <unordered_map>
#include <unordered_set>

export module berkeley_search.search_index;

export import berkeley_search.document;

export using DocumentId = std::size_t;
export using TermFrequency = std::size_t;
export using PostingList =
    std::unordered_map<DocumentId, TermFrequency>;

export class SearchIndex {
public:
    void addDocument(const Document& document);

    const PostingList& postings(const std::string& term) const;

    std::size_t documentCount() const;
    std::size_t termCount() const;

private:
    std::unordered_map<std::string, PostingList> index;
    std::unordered_set<DocumentId> documentIds;
};
