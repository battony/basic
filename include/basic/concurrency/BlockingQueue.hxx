#pragma once

#include "basic/os/Semaphore.hxx"

#include <utility>

namespace basic::concurrency {

template <typename T, template <typename> typename C>
class BlockingQueue {

private:

    using Self = BlockingQueue<T, C>;
    using Queue = C<T>;

public:

    template <typename... A>
    BlockingQueue(A&&... args)
            noexcept(noexcept(T(std::declval<A>()...))) :
            m_queue(std::forward<A>(args)...) {}

    ~BlockingQueue() noexcept = default;

    BlockingQueue(Self&& rhs) noexcept = delete;

    BlockingQueue(const Self& rhs) noexcept = delete;

    auto operator=(Self&& rhs) noexcept -> Self& = delete;

    auto operator=(const Self& rhs) noexcept -> Self& = delete;

    template <typename... A>
    auto Emplace(A&&... args) noexcept -> void {
        m_queue.Emplace(std::forward<A>(args)...);
        m_semaphore.Release();
    }

    auto Push(T&& value) noexcept -> void {
        m_queue.Push(std::move(value));
        m_semaphore.Release();
    }

    auto Push(const T& value) noexcept -> void {
        m_queue.Push(value);
        m_semaphore.Release();
    }

    auto Pop(T& value) noexcept -> bool {
        m_semaphore.Acquire();
        return m_queue.Pop(value);
    }

private:

    Queue m_queue;
    basic::os::Semaphore m_semaphore;

};

}
