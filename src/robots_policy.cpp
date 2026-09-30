module;

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

module berkeley_search.robots_policy;

namespace {

struct ParsedRule {
    std::string pattern;
    bool allow;
};

struct Group {
    std::vector<std::string> userAgents;
    std::vector<ParsedRule> rules;
};

std::string trim(const std::string& value) {
    std::size_t first = 0;

    while (
        first < value.size() &&
        std::isspace(static_cast<unsigned char>(value[first]))
    ) {
        ++first;
    }

    std::size_t last = value.size();

    while (
        last > first &&
        std::isspace(static_cast<unsigned char>(value[last - 1]))
    ) {
        --last;
    }

    return value.substr(first, last - first);
}

std::string lowercase(std::string value) {
    for (char& character : value) {
        character = static_cast<char>(
            std::tolower(static_cast<unsigned char>(character))
        );
    }

    return value;
}

bool groupMatches(
    const Group& group,
    const std::string& productToken
) {
    for (const std::string& userAgent : group.userAgents) {
        if (userAgent == productToken) {
            return true;
        }
    }

    return false;
}

bool wildcardGroup(const Group& group) {
    for (const std::string& userAgent : group.userAgents) {
        if (userAgent == "*") {
            return true;
        }
    }

    return false;
}

std::size_t ruleSpecificity(const std::string& pattern) {
    std::size_t specificity = 0;

    for (char character : pattern) {
        if (character != '*' && character != '$') {
            ++specificity;
        }
    }

    return specificity;
}

bool wildcardMatch(
    const std::string& value,
    std::string pattern
) {
    const bool endAnchored = !pattern.empty() && pattern.back() == '$';

    if (endAnchored) {
        pattern.pop_back();
    } else {
        pattern.push_back('*');
    }

    std::size_t valueIndex = 0;
    std::size_t patternIndex = 0;
    std::size_t starIndex = std::string::npos;
    std::size_t starValueIndex = 0;

    while (valueIndex < value.size()) {
        if (
            patternIndex < pattern.size() &&
            pattern[patternIndex] == value[valueIndex]
        ) {
            ++patternIndex;
            ++valueIndex;
        } else if (
            patternIndex < pattern.size() &&
            pattern[patternIndex] == '*'
        ) {
            starIndex = patternIndex;
            ++patternIndex;
            starValueIndex = valueIndex;
        } else if (starIndex != std::string::npos) {
            patternIndex = starIndex + 1;
            valueIndex = ++starValueIndex;
        } else {
            return false;
        }
    }

    while (
        patternIndex < pattern.size() &&
        pattern[patternIndex] == '*'
    ) {
        ++patternIndex;
    }

    return patternIndex == pattern.size();
}

} // namespace

void RobotsPolicy::parse(
    const std::string& robotsText,
    const std::string& productToken
) {
    rules.clear();

    std::vector<Group> groups;
    Group currentGroup;
    bool groupHasRules = false;
    std::istringstream lines(robotsText);
    std::string line;

    while (std::getline(lines, line)) {
        const std::size_t comment = line.find('#');

        if (comment != std::string::npos) {
            line.erase(comment);
        }

        line = trim(line);

        if (line.empty()) {
            continue;
        }

        const std::size_t colon = line.find(':');

        if (colon == std::string::npos) {
            continue;
        }

        const std::string field = lowercase(trim(line.substr(0, colon)));
        const std::string value = trim(line.substr(colon + 1));

        if (field == "user-agent") {
            if (groupHasRules) {
                groups.push_back(currentGroup);
                currentGroup = Group{};
                groupHasRules = false;
            }

            if (!value.empty()) {
                currentGroup.userAgents.push_back(lowercase(value));
            }

            continue;
        }

        if (
            currentGroup.userAgents.empty() ||
            (field != "allow" && field != "disallow")
        ) {
            continue;
        }

        groupHasRules = true;

        if (!value.empty()) {
            currentGroup.rules.push_back(ParsedRule{
                value,
                field == "allow"
            });
        }
    }

    if (!currentGroup.userAgents.empty()) {
        groups.push_back(currentGroup);
    }

    const std::string normalizedProductToken = lowercase(productToken);
    bool hasNamedGroup = false;

    for (const Group& group : groups) {
        if (groupMatches(group, normalizedProductToken)) {
            hasNamedGroup = true;
            break;
        }
    }

    for (const Group& group : groups) {
        const bool selected = hasNamedGroup
            ? groupMatches(group, normalizedProductToken)
            : wildcardGroup(group);

        if (!selected) {
            continue;
        }

        for (const ParsedRule& rule : group.rules) {
            rules.push_back(Rule{
                rule.pattern,
                rule.allow,
                ruleSpecificity(rule.pattern)
            });
        }
    }
}

bool RobotsPolicy::allows(const std::string& path) const {
    bool hasMatch = false;
    bool allowed = true;
    std::size_t bestSpecificity = 0;

    for (const Rule& rule : rules) {
        if (!wildcardMatch(path, rule.pattern)) {
            continue;
        }

        if (
            !hasMatch ||
            rule.specificity > bestSpecificity ||
            (rule.specificity == bestSpecificity && rule.allow)
        ) {
            hasMatch = true;
            allowed = rule.allow;
            bestSpecificity = rule.specificity;
        }
    }

    return allowed;
}
