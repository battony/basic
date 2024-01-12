#include "basic/coroutine/Coroutine.hxx"
#include "basic/coroutine/impl/CoroutineImpl.hxx"

namespace basic::coroutine {

Coroutine::Coroutine(Func&& fun, size_t stack_size)
        noexcept : m_impl(std::move(fun), stack_size) {}

Coroutine::Coroutine(const Func& fun, size_t stack_size)
        noexcept : m_impl(fun, stack_size) {}

Coroutine::~Coroutine() noexcept = default;

auto Coroutine::Switch(Self& coro) noexcept -> void {
    impl::CoroutineImpl::Switch(*coro.m_impl);
}

auto Coroutine::SwitchBack() noexcept -> void {
    impl::CoroutineImpl::SwitchBack();
}

}
