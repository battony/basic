#pragma once

#include "basic/coroutine/impl/CoroutineImplFwd.hxx"
#include "basic/utils/FastPimpl.hxx"

#include <functional>

namespace basic::coroutine {

class Coroutine {

private:

    using Self = Coroutine;
    using Func = std::function<void ()>;

public:

    Coroutine() noexcept;

    Coroutine(Func&& fun, size_t stack_size) noexcept;

    Coroutine(const Func& fun, size_t stack_size) noexcept;

    ~Coroutine() noexcept;

    Coroutine(Self&& rhs) noexcept = delete;

    Coroutine(const Self& rhs) noexcept = delete;

    auto operator=(Self&& rhs) noexcept -> Self& = delete;

    auto operator=(const Self& rhs) noexcept -> Self& = delete;

    static auto Switch(Self& coro) noexcept -> void;

    static auto SwitchBack() noexcept -> void;

private:

    basic::utils::FastPimpl<impl::CoroutineImpl, 48, 8> m_impl;

};

}
