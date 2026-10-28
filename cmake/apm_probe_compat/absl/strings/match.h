#ifndef ABSL_STRINGS_MATCH_H_
#define ABSL_STRINGS_MATCH_H_

#include "absl/strings/string_view.h"

namespace absl {

inline bool StartsWith(string_view text, string_view prefix)
{
    return text.size() >= prefix.size() && text.substr(0, prefix.size()) == prefix;
}

}  // namespace absl

#endif  // ABSL_STRINGS_MATCH_H_
