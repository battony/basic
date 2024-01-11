#pragma once

#include "basic/concurrency/HazardPointer.hxx"
#include "basic/utils/Atomic.hxx"

#include <optional>

namespace basic::concurrency {

template <typename T>
class HpMichaelScottQueueNode {

private:

    using Self = HpMichaelScottQueueNode<T>;

public:

    HpMichaelScottQueueNode() noexcept = default;

    template <typename... A>
    HpMichaelScottQueueNode(std::in_place_t, A&&... args)
            noexcept(noexcept(T(std::declval<A>()...))) :
            m_value(std::forward<A>(args)...) {}

    ~HpMichaelScottQueueNode() noexcept = default;

    HpMichaelScottQueueNode(Self&& rhs)
            noexcept(noexcept(T(std::declval<T>()))) = delete;

    HpMichaelScottQueueNode(const Self& rhs)
            noexcept(noexcept(T(std::declval<const T&>()))) = delete;

    auto operator=(Self&& rhs)
            noexcept(noexcept(std::declval<T&>() = std::declval<T>()))
            -> Self& = delete;

    auto operator=(const Self& rhs)
            noexcept(noexcept(std::declval<T&>() = std::declval<const T&>()))
            -> Self& = delete;

    auto Value() noexcept -> std::optional<T>& {
        return m_value;
    }

    auto Value() const noexcept -> const std::optional<T>& {
        return m_value;
    }

    auto Next() noexcept -> basic::utils::Atomic<Self*>& {
        return m_next;
    }

    auto Next() const noexcept -> const basic::utils::Atomic<Self*>& {
        return m_next;
    }

public:

    basic::utils::Atomic<Self*> m_next = nullptr;
    std::optional<T> m_value;

};

template <typename T>
class HpMichaelScottQueue {

private:

    using Self = HpMichaelScottQueue<T>;
    using Node = HpMichaelScottQueueNode<T>;

    static constexpr size_t hazard_pointer_per_thread = 2;

public:

    HpMichaelScottQueue() noexcept : m_domain(hazard_pointer_per_thread) {
        auto* node = new Node();
        m_head.StoreRlx(node);
        m_tail.StoreRlx(node);
    }

    ~HpMichaelScottQueue() noexcept {
        auto node = m_head.LoadAcq();
        while (node != nullptr) {
            auto* temp = node;
            node = node->Next().LoadRlx();
            delete temp;
        }
    }

    HpMichaelScottQueue(Self&& rhs) noexcept = delete;

    HpMichaelScottQueue(const Self& rhs) noexcept = delete;

    auto operator=(Self&& rhs) noexcept -> Self& = delete;

    auto operator=(const Self& rhs) noexcept -> Self& = delete;

    template <typename... A>
    auto Emplace(A&&... args) noexcept -> void {
        auto* node = new Node(std::in_place, std::forward<A>(args)...);
        auto hazptr = m_domain.MakeHazardPointer();

        Node* tail = nullptr;
        while (true) {
            tail = hazptr.Protect(m_tail);
            auto* next = tail->Next().LoadAcq();

            if (next != nullptr) {
                m_tail.CasStrongRelRlx(tail, next);
                continue;
            }

            Node* temp = nullptr;
            if (tail->Next().CasStrongRelRlx(temp, node))
                break;
        }

        m_tail.CasStrongArRlx(tail, node);
    }

    auto Push(T&& value) noexcept -> void {
        Emplace(std::move(value));
    }

    auto Push(const T& value) noexcept -> void {
        Emplace(value);
    }

    auto Pop(T& value) noexcept -> bool {
        auto hazptr_head = m_domain.MakeHazardPointer();
        auto hazptr_next = m_domain.MakeHazardPointer();

        Node* head = nullptr;
        while (true) {
            head = hazptr_head.Protect(m_head);
            auto* next = hazptr_next.Protect(head->Next());

            if (head != m_head.LoadRlx())
                continue;

            if (next == nullptr)
                return false;

            auto* tail = m_tail.LoadAcq();
            if (head == tail) {
                m_tail.CasStrongRelRlx(tail, next);
                continue;
            }

            if (m_head.CasStrongRelRlx(head, next))
                break;
        }
        hazptr_next.Unprotect();
        hazptr_head.Unprotect();

        value = std::move(*head->Value());
        m_domain.Retire(head);
        return true;
    }

private:

    basic::utils::Atomic<Node*> m_head = nullptr;
    basic::utils::Atomic<Node*> m_tail = nullptr;
    HazardPointerDomain<Node> m_domain;

};

}
