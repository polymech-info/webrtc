#ifndef ABSL_STRINGS_HAS_ABSL_STRINGIFY_H_
#define ABSL_STRINGS_HAS_ABSL_STRINGIFY_H_

#include <type_traits>

namespace absl {

template <typename T>
struct HasAbslStringify : std::false_type {};

}  // namespace absl

#endif  // ABSL_STRINGS_HAS_ABSL_STRINGIFY_H_
