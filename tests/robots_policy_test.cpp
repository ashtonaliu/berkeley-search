#include <iostream>
#include <string>

import berkeley_search.robots_policy;

int main() {
    const std::string robotsText = R"(
        Disallow: /ignored-before-a-group/

        User-agent: *
        Disallow:
        Disallow: /private/
        Allow: /private/public/
        Disallow: /*.pdf$

        User-agent: OtherBot
        Disallow: /

        User-agent: BerkeleySearchLearningBot
        Disallow: /drafts/
        Allow: /drafts/public/
        Disallow: /same # allow wins an equally specific tie
        Allow: /same

        User-agent: berkeleysearchlearningbot
        Disallow: /second-group/
    )";

    RobotsPolicy namedPolicy;
    namedPolicy.parse(robotsText, "BerkeleySearchLearningBot");

    bool passed = true;
    passed &= namedPolicy.allows("/");
    passed &= namedPolicy.allows("/private/page");
    passed &= !namedPolicy.allows("/drafts/page");
    passed &= namedPolicy.allows("/drafts/public/page");
    passed &= namedPolicy.allows("/same");
    passed &= !namedPolicy.allows("/second-group/page");

    RobotsPolicy wildcardPolicy;
    wildcardPolicy.parse(robotsText, "UnknownBot");

    passed &= !wildcardPolicy.allows("/private/page");
    passed &= wildcardPolicy.allows("/private/public/page");
    passed &= !wildcardPolicy.allows("/files/report.pdf");
    passed &= wildcardPolicy.allows("/files/report.pdf?download=1");
    passed &= wildcardPolicy.allows("/ordinary/page");

    if (!passed) {
        std::cerr << "Robots policy returned an unexpected decision.\n";
        return 1;
    }

    std::cout << "All robots policy tests passed.\n";
    return 0;
}
