#include "basic/coroutine/impl/CoroutineImpl.hxx"

namespace basic::coroutine::impl {

CoroutineImpl::CoroutineImpl() noexcept = default;

CoroutineImpl::CoroutineImpl(Func&& fun, size_t stack_size) noexcept : 
        m_function(std::move(fun)),
        m_context(CoroutineImpl::CoroutineMain, stack_size) {}

CoroutineImpl::CoroutineImpl(const Func& fun, size_t stack_size) noexcept :
        m_function(fun),
        m_context(CoroutineImpl::CoroutineMain, stack_size) {}

CoroutineImpl::~CoroutineImpl() noexcept = default;

auto CoroutineImpl::Switch(Self& coro) noexcept -> void {
    auto* curr = Current();
    auto* prev = Previous();

    Current() = &coro;
    Previous() = curr;
    curr->SwitchContext(coro);
    Current() = curr;
    Previous() = prev;
}

auto CoroutineImpl::SwitchBack() noexcept -> void {
    Switch(*Previous());
}

auto CoroutineImpl::RunMain() noexcept -> void {
    m_function();
}

auto CoroutineImpl::SwitchContext(Self& coro) noexcept -> void {
    m_context.Switch(coro.m_context);
}

auto CoroutineImpl::Current() noexcept -> Self*& {
    static thread_local CoroutineImpl ctx;
    static thread_local CoroutineImpl* ctx_ptr = &ctx;
    return ctx_ptr;
}

auto CoroutineImpl::Previous() noexcept -> Self*& {
    static thread_local CoroutineImpl* ctx_ptr = nullptr;
    return ctx_ptr;
}

auto CoroutineImpl::CoroutineMain() noexcept -> void {
    Current()->RunMain();
    SwitchBack();
}

}
