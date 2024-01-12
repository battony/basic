#pragma once

#include "basic/coroutine/Context.hxx"

#include <functional>

namespace basic::coroutine::impl {

class CoroutineImpl {

private:

    using Self = CoroutineImpl;
    using Func = std::function<void ()>;

public:

    CoroutineImpl(Func&& fun, size_t stack_size) noexcept;

    CoroutineImpl(const Func& fun, size_t stack_size) noexcept;

    ~CoroutineImpl() noexcept;

    CoroutineImpl(Self&& rhs) noexcept = delete;

    CoroutineImpl(const Self& rhs) noexcept = delete;

    auto operator=(Self&& rhs) noexcept -> Self& = delete;

    auto operator=(const Self& rhs) noexcept -> Self& = delete;

    static auto Switch(Self& coro) noexcept -> void;

    static auto SwitchBack() noexcept -> void;

private:

    CoroutineImpl() noexcept;

    auto RunMain() noexcept -> void;

    auto SwitchContext(Self& coro) noexcept -> void;

    static auto Current() noexcept -> Self*&;

    static auto Previous() noexcept -> Self*&;

    static auto CoroutineMain() noexcept -> void;

    std::function<void ()> m_function;
    basic::coroutine::Context m_context;

};

}
