#pragma once

#include <cstddef>
#include <cstdlib>

namespace basic::utils {

template <typename T>
class DynamicArray {

private:

    using Self = DynamicArray<T>;

public:

    template <typename... A>
    DynamicArray(size_t size, const A&... args)
            noexcept(noexcept(T(std::declval<const A&>()...))) :
            m_size(size) {
        m_array = new (AlignT()) uint8_t[SizeT() * m_size];
        for (size_t i = 0; i < m_size; i++)
            new (Ptr() + i) T(args...);
    }

    ~DynamicArray() noexcept {
        for (size_t i = 0; i < m_size; i++)
            (Ptr() + i)->~T();
        operator delete[](m_array, SizeT() * m_size, AlignT());
    }

    auto Size() const noexcept -> size_t {
        return m_size;
    }

    auto operator[](size_t i) noexcept -> T& {
        return *(Ptr() + i);
    }

    auto operator[](size_t i) const noexcept -> const T& {
        return *(Ptr() + i);
    }

private:

    static constexpr auto AlignT() noexcept -> std::align_val_t {
        return static_cast<std::align_val_t>(alignof(T));
    }

    static constexpr auto SizeT() noexcept -> size_t {
        return sizeof(T);
    }

    auto Ptr() noexcept -> T* {
        return reinterpret_cast<T*>(m_array);
    }

    auto Ptr() const noexcept -> const T* {
        return reinterpret_cast<const T*>(m_array);
    }

    size_t m_size;
    uint8_t* m_array;

};

}
