#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>

namespace basic::coroutine::impl {

class ContextImpl {

private:

    using Self = ContextImpl;

public:

    ContextImpl() noexcept;

    ContextImpl(void (*fun)(), size_t stack_size) noexcept;

    ~ContextImpl() noexcept;

    ContextImpl(Self&& rhs) noexcept = delete;

    ContextImpl(const Self& rhs) noexcept = delete;

    auto operator=(Self&& rhs) noexcept -> Self& = delete;

    auto operator=(const Self& rhs) noexcept -> Self& = delete;

    auto Switch(Self& rhs) noexcept -> void;

private:

    std::unique_ptr<uint8_t[]> m_memory = nullptr;
    void* m_stack_pointer = nullptr;

};

}
