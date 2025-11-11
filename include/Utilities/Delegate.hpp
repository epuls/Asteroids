#pragma once
#include <vector>
#include <algorithm>
#include <cstdint>

/**
 * \class Delegate
 * \brief Attempt at recreating c# delegates, allowing for lightweight event-style function subscription
 * @tparam Args
 */
template<class... Args>
class Delegate {
public:
    using Stub = void(*)(void*, Args&&...);
    Delegate() = default;
    template<auto Fn> static Delegate FromFn() {
        return Delegate{ nullptr, [](void*, Args&&... a){ (*Fn)(std::forward<Args>(a)...); } };
    }
    template<auto MemFn, class T> static Delegate FromMethod(T* obj) {
        return Delegate{ obj, [](void* self, Args&&... a){ (static_cast<T*>(self)->*MemFn)(std::forward<Args>(a)...); } };
    }
    void operator()(Args... a) const { stub_(ctx_, std::forward<Args>(a)...); }
    explicit operator bool() const { return stub_ != nullptr; }

    bool operator==(const Delegate& rhs) const noexcept {
        return ctx_ == rhs.ctx_ && stub_ == rhs.stub_;
    }
    bool operator!=(const Delegate& rhs) const noexcept { return !(*this == rhs); }

private:
    void* ctx_{}; Stub stub_{};
    Delegate(void* c, Stub s): ctx_(c), stub_(s) {}
};