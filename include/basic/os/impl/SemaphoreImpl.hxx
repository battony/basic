#pragma once

#include <semaphore.h>

namespace basic::os::impl {

class SemaphoreImpl {

private:

    using Self = SemaphoreImpl;

public:

    SemaphoreImpl(size_t size) noexcept;

    ~SemaphoreImpl() noexcept;

    SemaphoreImpl(Self&& rhs) noexcept = delete;

    SemaphoreImpl(const Self& rhs) noexcept = delete;

    auto operator=(Self&& rhs) noexcept -> Self& = delete;

    auto operator=(const Self& rhs) noexcept -> Self& = delete;

    auto Release() noexcept -> void;

    auto Acquire() noexcept -> void;

    auto TryAcquire() noexcept -> bool;

private:

    sem_t m_semaphore;

};

}
