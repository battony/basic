#pragma once

#include "basic/concurrency/HazardPointer.hxx"
#include "basic/utils/Atomic.hxx"
#include "basic/utils/DynamicArray.hxx"

#include <optional>

namespace basic::concurrency {

template <typename T>
class HpFaaArrayQueueNode {

private:

    using Self = HpFaaArrayQueueNode<T>;

public:

    HpFaaArrayQueueNode(size_t buffer_size)
        noexcept : m_items(buffer_size, nullptr) {}

    ~HpFaaArrayQueueNode() noexcept {
        auto deq_id = DeqId().LoadRlx();
        auto enq_id = EnqId().LoadRlx();

        auto fixed_enq_id = std::min(enq_id, m_items.Size());
        for (size_t i = deq_id; i < fixed_enq_id; i++) {
            delete m_items[i].LoadRlx();
        }
    }

    HpFaaArrayQueueNode(Self&& rhs)
            noexcept(noexcept(T(std::declval<T>()))) = delete;

    HpFaaArrayQueueNode(const Self& rhs)
            noexcept(noexcept(T(std::declval<const T&>()))) = delete;

    auto operator=(Self&& rhs)
            noexcept(noexcept(std::declval<T&>() = std::declval<T>()))
            -> Self& = delete;

    auto operator=(const Self& rhs)
            noexcept(noexcept(std::declval<T&>() = std::declval<const T&>()))
            -> Self& = delete;

    auto Next() noexcept -> basic::utils::Atomic<Self*>& {
        return m_next;
    }

    auto Next() const noexcept -> const basic::utils::Atomic<Self*>& {
        return m_next;
    }

    auto DeqId() noexcept -> basic::utils::Atomic<size_t>& {
        return m_deq_id;
    }

    auto DeqId() const noexcept -> const basic::utils::Atomic<size_t>& {
        return m_deq_id;
    }

    auto IncreaseDeqId() noexcept -> size_t {
        return m_deq_id.FetchAddRlx(1);
    }

    auto DecreaseDeqId() noexcept -> size_t {
        return m_deq_id.FetchSubRlx(1);
    }

    auto EnqId() noexcept -> basic::utils::Atomic<size_t>& {
        return m_enq_id;
    }

    auto EnqId() const noexcept -> const basic::utils::Atomic<size_t>& {
        return m_enq_id;
    }

    auto IncreaseEnqId() noexcept -> size_t {
        return m_enq_id.FetchAddRlx(1);
    }

    auto DecreaseEnqId() noexcept -> size_t {
        return m_enq_id.FetchSubRlx(1);
    }

    auto Item(size_t id) noexcept -> basic::utils::Atomic<T*>& {
        return m_items[id];
    }

    auto Item(size_t id) const noexcept -> const basic::utils::Atomic<T*>& {
        return m_items[id];
    }

    auto FirstItem() noexcept -> basic::utils::Atomic<T*>& {
        return Item(0);
    }

    auto FirstItem() const noexcept -> const basic::utils::Atomic<T*>& {
        return Item(0);
    }

public:

    basic::utils::Atomic<Self*> m_next = nullptr;
    basic::utils::Atomic<size_t> m_deq_id = 0;
    basic::utils::Atomic<size_t> m_enq_id = 0;
    basic::utils::DynamicArray<basic::utils::Atomic<T*>> m_items;

};

template <typename T>
class HpFaaArrayQueue {

private:

    using Self = HpFaaArrayQueue<T>;
    using Node = HpFaaArrayQueueNode<T>;

    static constexpr size_t default_buffer_size = 4096;
    static constexpr size_t hazard_pointer_per_thread = 1;

public:

    HpFaaArrayQueue(size_t buffer_size = default_buffer_size) noexcept :
            m_buffer_size(buffer_size), m_domain(hazard_pointer_per_thread) {
        auto* node = CreateNode();
        m_head.StoreRlx(node);
        m_tail.StoreRlx(node);
    }

    ~HpFaaArrayQueue() noexcept {
        auto node = m_head.LoadAcq();
        while (node != nullptr) {
            auto* temp = node;
            node = node->Next().LoadRlx();
            delete temp;
        }
    }

    HpFaaArrayQueue(Self&& rhs) noexcept = delete;

    HpFaaArrayQueue(const Self& rhs) noexcept = delete;

    auto operator=(Self&& rhs) noexcept -> Self& = delete;

    auto operator=(const Self& rhs) noexcept -> Self& = delete;

    template <typename... A>
    auto Emplace(A&&... args) noexcept -> void {
        auto* node = new T(std::forward<A>(args)...);
        auto hazptr = m_domain.MakeHazardPointer();

        while (true) {
            auto* tail = hazptr.Protect(m_tail);
            size_t id = tail->IncreaseEnqId();

            if (id >= m_buffer_size) {
                if (tail != m_tail.LoadAcq())
                    continue;

                auto* next = tail->Next().LoadAcq();
                if (next != nullptr) {
                    m_tail.CasStrongRelRlx(tail, next);
                } else {
                    auto* new_node = CreateNode();
                    new_node->FirstItem().StoreRlx(node);
                    new_node->IncreaseEnqId();

                    Node* temp = nullptr;
                    if (tail->Next().CasStrongRelRlx(temp, new_node)) {
                        m_tail.CasStrongRelRlx(tail, new_node);
                        return;
                    }
                    
                    new_node->DecreaseEnqId();
                    delete new_node;
                }

                continue;
            }

            T* temp = nullptr;
            if (tail->Item(id).CasStrongRelRlx(temp, node))
                return;
        }
    }

    auto Push(T&& value) noexcept -> void {
        Emplace(std::move(value));
    }

    auto Push(const T& value) noexcept -> void {
        Emplace(value);
    }

    auto Pop(T& value) noexcept -> bool {
        auto hazptr = m_domain.MakeHazardPointer();

        while (true) {
            auto* head = hazptr.Protect(m_head);

            auto invalid_index =
                head->DeqId().LoadRlx() >=
                head->EnqId().LoadRlx();
            auto no_next_node = head->Next().LoadRlx() == nullptr;

            if (invalid_index && no_next_node)
                return false;

            auto id = head->IncreaseDeqId();
            if (id >= m_buffer_size) {
                auto* next = head->Next().LoadAcq();
                if (next == nullptr) return false;
                if (m_head.CasStrongRelRlx(head, next))
                    m_domain.Retire(head);
                continue;
            }

            auto* non_null_pointer = reinterpret_cast<T*>(this);
            auto* item = head->Item(id).ExchangeAcq(non_null_pointer);
            if (item == nullptr) continue;

            value = std::move(*item);
            delete item;
            return true;
        }

        return false;
    }

private:

    auto CreateNode() noexcept -> Node* {
        return new Node(m_buffer_size);
    }

    size_t m_buffer_size;
    basic::utils::Atomic<Node*> m_head = nullptr;
    basic::utils::Atomic<Node*> m_tail = nullptr;
    HazardPointerDomain<Node> m_domain;

};

}
