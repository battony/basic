#pragma once

#include "basic/utils/Atomic.hxx"

namespace basic::concurrency {

template <typename T>
class UnsafeTreiberStackNode {

private:

    using Self = UnsafeTreiberStackNode<T>;

public:

    template <typename... A>
    UnsafeTreiberStackNode(A&&... args)
            noexcept(noexcept(T(std::declval<A>()...))) :
            m_value(std::forward<A>(args)...) {}

    ~UnsafeTreiberStackNode() noexcept = default;

    UnsafeTreiberStackNode(Self&& rhs)
            noexcept(noexcept(T(std::declval<T>()))) = delete;

    UnsafeTreiberStackNode(const Self& rhs)
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

    Self* m_next;
    T m_value;

};

template <typename T>
class UnsafeTreiberStack {

private:

    using Self = UnsafeTreiberStack<T>;
    using Node = UnsafeTreiberStackNode<T>;

public:

    UnsafeTreiberStack() noexcept = default;

    ~UnsafeTreiberStack() noexcept {
        auto node = m_head.LoadAcq();
        while (node != nullptr) {
            auto* temp = node;
            node = node->Next();
            delete temp;
        }
    }

    UnsafeTreiberStack(Self&& rhs) noexcept = delete;

    UnsafeTreiberStack(const Self& rhs) noexcept = delete;

    auto operator=(Self&& rhs) noexcept -> Self& = delete;

    auto operator=(const Self& rhs) noexcept -> Self& = delete;

    template <typename... A>
    auto Push(A&&... args) noexcept -> Node* {
        return Push(new Node(std::forward<A>(args)...));
    }

    auto Push(Node* node) noexcept -> Node* {
        node->Next() = m_head.LoadRlx();
        while (!m_head.CasWeakRelRlx(node->Next(), node));
        return node;
    }

    auto Push(Node* head, Node* tail) noexcept -> Node* {
        tail->Next() = m_head.LoadRlx();
        while (!m_head.CasWeakRelRlx(tail->Next(), head));
        return head;
    }

    auto PopAll() noexcept -> Node* {
        return m_head.ExchangeAcq(nullptr);
    }

    auto Head() noexcept -> Node* {
        return m_head.LoadAcq();
    }

private:

    basic::utils::Atomic<Node*> m_head = nullptr;

};

}
