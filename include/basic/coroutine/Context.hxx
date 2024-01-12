#pragma once

#include "basic/coroutine/impl/ContextImplFwd.hxx"
#include "basic/utils/FastPimpl.hxx"

namespace basic::coroutine {

class Context {

private:

    using Self = Context;

public:

    Context() noexcept;

    Context(void (*fun)(), size_t stack_size) noexcept;

    ~Context() noexcept;

    Context(Self&& rhs) noexcept = delete;

    Context(const Self& rhs) noexcept = delete;

    auto operator=(Self&& rhs) noexcept -> Self& = delete;

    auto operator=(const Self& rhs) noexcept -> Self& = delete;

    auto Switch(Self& ctx) noexcept -> void;

private:

    basic::utils::FastPimpl<impl::ContextImpl, 16, 8> m_impl;

};

}
