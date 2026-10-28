#ifndef ABSL_STRINGS_STR_CAT_H_
#define ABSL_STRINGS_STR_CAT_H_

#include <sstream>
#include <string>
#include <utility>

namespace absl {

template <typename... Args>
std::string StrCat(Args&&... args)
{
    std::ostringstream out;
    (out << ... << std::forward<Args>(args));
    return out.str();
}

}  // namespace absl

#endif  // ABSL_STRINGS_STR_CAT_H_
