#pragma once

#include "basic/os/impl/SemaphoreImplFwd.hxx"
#include "basic/utils/FastPimpl.hxx"

namespace basic::os {

class Semaphore {

private:

    using Self = Semaphore;

public:

    Semaphore(size_t size) noexcept;

    ~Semaphore() noexcept;

    Semaphore(Self&& rhs) noexcept = delete;

    Semaphore(const Self& rhs) noexcept = delete;

    auto operator=(Self&& rhs) noexcept -> Self& = delete;

    auto operator=(const Self& rhs) noexcept -> Self& = delete;

    auto Release() noexcept -> void;

    auto Acquire() noexcept -> void;

private:

    basic::utils::FastPimpl<impl::SemaphoreImpl, 32, 8> m_impl;

};

}
