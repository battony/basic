#pragma once

#include <cstddef>
#include <cstdint>
#include <new>
#include <utility>

namespace basic::utils {

template <typename T, size_t Size, size_t Alignment>
class FastPimpl final {

public:

    template <typename... A>
    FastPimpl(A&&... args)
            noexcept(noexcept(T(std::declval<A>()...))) {
        new (Ptr()) T(std::forward<A>(args)...);
    }

    ~FastPimpl() noexcept {
        Validate<sizeof(T), alignof(T)>();
        Ptr()->~T();
    }

    FastPimpl(FastPimpl&& rhs)
            noexcept(noexcept(T(std::declval<T>()))) :
            FastPimpl(std::move(*rhs)) {}

    FastPimpl(const FastPimpl& rhs)
            noexcept(noexcept(T(std::declval<const T&>()))) :
            FastPimpl(*rhs) {}

    auto operator=(FastPimpl&& rhs)
            noexcept(noexcept(std::declval<T&>() = std::declval<T>()))
            -> FastPimpl& {
        *Ptr() = std::move(*rhs);
        return *this;
    }

    auto operator=(const FastPimpl& rhs)
            noexcept(noexcept(std::declval<T&>() = std::declval<const T&>()))
            -> FastPimpl& {
        *Ptr() = *rhs;
        return *this;
    }

    auto operator*() noexcept -> T& {
        return *Ptr();
    }

    auto operator*() const noexcept -> const T& {
        return *Ptr();
    }

    auto operator->() noexcept -> T* {
        return Ptr();
    }

    auto operator->() const noexcept -> const T* {
        return Ptr();
    }

private:

    auto Ptr() noexcept -> T* {
        return reinterpret_cast<T*>(&m_storage);
    }

    auto Ptr() const noexcept -> const T* {
        return reinterpret_cast<const T*>(&m_storage);
    }

    template <size_t ActualSize, size_t ActualAlignment>
    static auto Validate() noexcept -> void {
        static_assert(
            Size == ActualSize,
            "Ivalid size: Size != sizeof(T)"
        );
        static_assert(
            Alignment == ActualAlignment,
            "Ivalid alignment: Alignment != alignof(T)"
        );
    }

    alignas(Alignment) uint8_t m_storage[Size];

};

}
