module;

#include <cstddef>
#include <string>
#include <vector>

export module berkeley_search.robots_policy;

export class RobotsPolicy {
public:
    void parse(
        const std::string& robotsText,
        const std::string& productToken
    );

    bool allows(const std::string& path) const;

private:
    struct Rule {
        std::string pattern;
        bool allow;
        std::size_t specificity;
    };

    std::vector<Rule> rules;
};
