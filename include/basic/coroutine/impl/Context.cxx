#include "basic/coroutine/Context.hxx"
#include "basic/coroutine/impl/ContextImpl.hxx"

namespace basic::coroutine {

Context::Context() noexcept = default;

Context::Context(void (*fun)(), size_t stack_size)
        noexcept : m_impl(fun, stack_size) {}

Context::~Context() noexcept = default;

auto Context::Switch(Self& ctx) noexcept -> void {
    m_impl->Switch(*ctx.m_impl);
}

}
