#ifndef ABSL_FUNCTIONAL_ANY_INVOCABLE_H_
#define ABSL_FUNCTIONAL_ANY_INVOCABLE_H_

#include <functional>
#include <utility>

namespace absl {

template <typename Signature>
class AnyInvocable;

template <typename R, typename... Args>
class AnyInvocable<R(Args...) &&> {
public:
    AnyInvocable() = default;

    template <typename F>
    AnyInvocable(F&& f) : fn_(std::forward<F>(f)) {}

    R operator()(Args... args) && { return fn_(std::forward<Args>(args)...); }
    explicit operator bool() const { return static_cast<bool>(fn_); }

private:
    std::function<R(Args...)> fn_;
};

}  // namespace absl

#endif  // ABSL_FUNCTIONAL_ANY_INVOCABLE_H_
