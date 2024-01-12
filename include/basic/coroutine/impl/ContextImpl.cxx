#include "basic/coroutine/impl/ContextImpl.hxx"
#include "basic/coroutine/impl/ContextAsm.hxx"

namespace basic::coroutine::impl {

ContextImpl::ContextImpl() noexcept = default;

ContextImpl::ContextImpl(void (*fun)(), size_t stack_size) noexcept :
        m_memory(std::make_unique<uint8_t[]>(stack_size)),
        m_stack_pointer(m_memory.get() + stack_size - 1) {
    ::MakeContext(&m_stack_pointer, fun);
}

ContextImpl::~ContextImpl() noexcept = default;

auto ContextImpl::Switch(Self& ctx) noexcept -> void {
    ::SwitchContext(&m_stack_pointer, &ctx.m_stack_pointer);
}

}
