#include <vector>
#include <string>
#include <algorithm>

std::vector<std::string> LookupEnvVarReferences(const std::vector<std::string>& specs, const std::vector<std::string>& environ) {
    std::vector<std::string> result;
    result.reserve(specs.size());

    for (const auto& spec : specs) {
        size_t pos = spec.find('=');
        if (pos != std::string::npos) {
            result.push_back(spec);

        } else if (spec == "*") {
            result.insert(result.end(), environ.begin(), environ.end());

        } else {
            std::string prefix = spec + "=";
            if (spec.back() == '*') {
                prefix.pop_back();
            }

            for (const auto& env : environ) {
                if (env.compare(0, prefix.size(), prefix) == 0) {
                    result.push_back(env);
                }
            }
        }
    }

    return result;
}
