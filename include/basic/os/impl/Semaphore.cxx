#include "basic/os/Semaphore.hxx"
#include "basic/os/impl/SemaphoreImpl.hxx"

namespace basic::os {

Semaphore::Semaphore(size_t size) noexcept : m_impl(size) {}

Semaphore::~Semaphore() noexcept {}

auto Semaphore::Release() noexcept -> void {
    m_impl->Release();
}

auto Semaphore::Acquire() noexcept -> void {
    m_impl->Acquire();
}

auto Semaphore::TryAcquire() noexcept -> bool {
    return m_impl->TryAcquire();
}

}
