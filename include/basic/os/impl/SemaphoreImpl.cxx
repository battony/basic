#include "basic/os/impl/SemaphoreImpl.hxx"

namespace basic::os::impl {

SemaphoreImpl::SemaphoreImpl(size_t size) noexcept {
    static constexpr int not_shared = 0;
    // TODO: check error
    ::sem_init(&m_semaphore, not_shared, static_cast<unsigned int>(size));
}

SemaphoreImpl::~SemaphoreImpl() noexcept {
    // TODO: check error
    ::sem_destroy(&m_semaphore);
}

auto SemaphoreImpl::Release() noexcept -> void {
    // TODO: check error
    ::sem_post(&m_semaphore);
}

auto SemaphoreImpl::Acquire() noexcept -> void {
    // TODO: check error
    ::sem_wait(&m_semaphore);
}

auto SemaphoreImpl::TryAcquire() noexcept -> bool {
    // TODO: check error
    return ::sem_trywait(&m_semaphore) == 0;
}

}
