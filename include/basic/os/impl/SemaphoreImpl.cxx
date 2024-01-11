#include "basic/os/impl/SemaphoreImpl.hxx"

namespace basic::os::impl {

SemaphoreImpl::SemaphoreImpl(size_t size) noexcept {
    static constexpr int not_shared = 0;
    // TODO: check error
    ::sem_init(&m_sem, not_shared, static_cast<unsigned int>(size));
}

SemaphoreImpl::~SemaphoreImpl() noexcept {
    // TODO: check error
    ::sem_destroy(&m_sem);
}

auto SemaphoreImpl::Release() noexcept -> void {
    // TODO: check error
    ::sem_post(&m_sem);
}

auto SemaphoreImpl::Acquire() noexcept -> void {
    // TODO: check error
    ::sem_wait(&m_sem);
}

}
