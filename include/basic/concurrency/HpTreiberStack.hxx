#pragma once

#include "basic/concurrency/HazardPointer.hxx"
#include "basic/utils/Atomic.hxx"

namespace basic::concurrency {

template <typename T>
class HpTreiberStackNode {

private:

    using Self = HpTreiberStackNode<T>;

public:

    template <typename... A>
    HpTreiberStackNode(A&&... args)
            noexcept(noexcept(T(std::declval<A>()...))) :
            m_value(std::forward<A>(args)...) {}

    ~HpTreiberStackNode() noexcept = default;

    HpTreiberStackNode(Self&& rhs)
            noexcept(noexcept(T(std::declval<T>()))) = delete;

    HpTreiberStackNode(const Self& rhs)
            noexcept(noexcept(T(std::declval<const T&>()))) = delete;

    auto operator=(Self&& rhs)
            noexcept(noexcept(std::declval<T&>() = std::declval<T>()))
            -> Self& = delete;

    auto operator=(const Self& rhs)
            noexcept(noexcept(std::declval<T&>() = std::declval<const T&>()))
            -> Self& = delete;

    auto Value() noexcept -> T& {
        return m_value;
    }

    auto Value() const noexcept -> const T& {
        return m_value;
    }

    auto Next() noexcept -> Self*& {
        return m_next;
    }

    auto Next() const noexcept -> const Self*& {
        return m_next;
    }

public:

    Self* m_next = nullptr;
    T m_value;

};

template <typename T>
class HpTreiberStack {

private:

    using Self = HpTreiberStack<T>;
    using Node = HpTreiberStackNode<T>;

    static constexpr size_t hazard_pointer_per_thread = 1;

public:

    HpTreiberStack() noexcept : m_domain(hazard_pointer_per_thread) {}

    ~HpTreiberStack() noexcept {
        auto node = m_head.LoadAcq();
        while (node != nullptr) {
            auto* temp = node;
            node = node->Next();
            delete temp;
        }
    }

    HpTreiberStack(Self&& rhs) noexcept = delete;

    HpTreiberStack(const Self& rhs) noexcept = delete;

    auto operator=(Self&& rhs) noexcept -> Self& = delete;

    auto operator=(const Self& rhs) noexcept -> Self& = delete;

    template <typename... A>
    auto Emplace(A&&... args) noexcept -> void {
        auto* node = new Node(std::forward<A>(args)...);
        node->Next() = m_head.LoadRlx();
        while (!m_head.CasWeakRelRlx(node->Next(), node));
    }

    auto Push(T&& value) noexcept -> void {
        Emplace(std::move(value));
    }

    auto Push(const T& value) noexcept -> void {
        Emplace(value);
    }

    auto Pop(T& value) noexcept -> bool {
        Node* node = nullptr;
        auto hazptr = m_domain.MakeHazardPointer();

        do {
            node = hazptr.Protect(m_head);
            if (node == nullptr) return false;
        } while (!m_head.CasWeakRelRlx(node, node->Next()));
        hazptr.Unprotect();

        value = std::move(node->Value());
        m_domain.Retire(node);
        return true;
    }

private:

    basic::utils::Atomic<Node*> m_head = nullptr;
    HazardPointerDomain<Node> m_domain;

};

}
