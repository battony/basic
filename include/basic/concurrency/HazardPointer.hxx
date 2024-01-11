#pragma once

#include "basic/concurrency/UnsafeTreiberStack.hxx"
#include "basic/utils/Atomic.hxx"

#include <unordered_set>
#include <thread>

namespace basic::concurrency {

template <typename T>
class HazardPointerResourceRecord {

private:

    using Self = HazardPointerResourceRecord<T>;

public:

    HazardPointerResourceRecord() noexcept = default;

    ~HazardPointerResourceRecord() noexcept = default;

    HazardPointerResourceRecord(Self&& rhs) noexcept = delete;

    HazardPointerResourceRecord(const Self& rhs) noexcept = delete;

    auto operator=(Self&& rhs) noexcept -> Self& = delete;

    auto operator=(const Self& rhs) noexcept -> Self& = delete;

    auto TryLock() noexcept -> bool {
        return !m_lock.TestAndSetAcq();
    }

    auto Lock() noexcept -> void {
        while (!TryLock());
    }

    auto Unlock() noexcept -> void {
        m_lock.ClearRel();
    }

    auto Ptr() noexcept -> basic::utils::Atomic<T*>& {
        return m_ptr;
    }

    auto Ptr() const noexcept -> const basic::utils::Atomic<T*>& {
        return m_ptr;
    }

private:

    basic::utils::Atomic<T*> m_ptr = nullptr;
    basic::utils::AtomicFlag m_lock;

};

template <typename T>
class HazardPointerReclaimRecord {

private:

    using Self = HazardPointerReclaimRecord<T>;

public:

    HazardPointerReclaimRecord(T* ptr) noexcept : m_ptr(ptr) {}

    ~HazardPointerReclaimRecord() noexcept {
        delete m_ptr;
    }

    HazardPointerReclaimRecord(Self&& rhs) noexcept = delete;

    HazardPointerReclaimRecord(const Self& rhs) noexcept = delete;

    auto operator=(Self&& rhs) noexcept -> Self& = delete;

    auto operator=(const Self& rhs) noexcept -> Self& = delete;

    auto Ptr() noexcept -> T*& {
        return m_ptr;
    }

    auto Ptr() const noexcept -> const T*& {
        return m_ptr;
    }

private:

    T* m_ptr;

};

template <typename T>
class HazardPointer {

private:

    using Self = HazardPointer<T>;
    using Record = HazardPointerResourceRecord<T>;

public:

    HazardPointer(Record* record) noexcept : m_record(record) {}

    ~HazardPointer() noexcept {
        Unprotect();
        m_record->Unlock();
    }

    HazardPointer(Self&& rhs) noexcept = delete;

    HazardPointer(const Self& rhs) noexcept = delete;

    auto operator=(Self&& rhs) noexcept -> Self& = delete;

    auto operator=(const Self& rhs) noexcept -> Self& = delete;

    auto Protect(const std::atomic<T*>& ptr) noexcept -> T* {
        while (true) {
            auto* val = ptr.load(std::memory_order_relaxed);
            m_record->Ptr().StoreRel(val);
            
            if (val == ptr.load(std::memory_order_acquire))
                return val;
        }
    }

    auto Protect(const basic::utils::Atomic<T*>& ptr) noexcept -> T* {
        return Protect(ptr.Raw());
    }

    auto Unprotect() noexcept -> void {
        m_record->Ptr().StoreRel(nullptr);
    }

private:

    Record* m_record;

};

template <typename T>
class CachedHazardPointers {

private:

    using Self = CachedHazardPointers<T>;
    using Container = std::unordered_set<T*>;

public:

    CachedHazardPointers() noexcept = default;

    ~CachedHazardPointers() noexcept = default;

    CachedHazardPointers(Self&& rhs) noexcept = default;

    CachedHazardPointers(const Self& rhs) noexcept = default;

    auto operator=(Self&& rhs) noexcept -> Self& = default;

    auto operator=(const Self& rhs) noexcept -> Self& = default;

    auto Push(T* ptr) noexcept -> void {
        m_container.emplace(ptr);
    }

    auto Contains(T* ptr) noexcept -> bool {
        return m_container.find(ptr) != m_container.end();
    }

private:

    Container m_container;

};

template <typename T>
class HazardPointerResourcePool {

private:

    using Self = HazardPointerResourcePool<T>;
    using Record = HazardPointerResourceRecord<T>;

public:

    HazardPointerResourcePool(size_t preallocate_records_count) noexcept {
        for (size_t i = 0; i < preallocate_records_count; i++)
            InsertNewUnlockedRecord();
    }

    ~HazardPointerResourcePool() noexcept = default;

    HazardPointerResourcePool(Self&& rhs) noexcept = delete;

    HazardPointerResourcePool(const Self& rhs) noexcept = delete;

    auto operator=(Self&& rhs) noexcept -> Self& = delete;

    auto operator=(const Self& rhs) noexcept -> Self& = delete;

    auto MakeHazardPointer() noexcept -> HazardPointer<T> {
        if (auto* v = GetUnusedRecord(); v)
            return HazardPointer<T>(v);
        return HazardPointer<T>(InsertNewLockedRecord());
    }

    auto BuildCachedHazardPointers() noexcept -> CachedHazardPointers<T> {
        CachedHazardPointers<T> hps;
        for (auto* v = m_stack.Head(); v != nullptr; v = v->Next()) {
            auto* ptr = v->Value().Ptr().LoadAcq();
            if (ptr) hps.Push(ptr);
        }
        return hps;
    }

    auto GetSize() noexcept -> size_t {
        return m_size.LoadRlx();
    }

private:

    auto GetUnusedRecord() noexcept -> Record* {
        for (auto* v = m_stack.Head(); v != nullptr; v = v->Next())
            if (v->Value().TryLock()) return &v->Value();
        return nullptr;
    }

    auto InsertNewLockedRecord() noexcept -> Record* {
        auto* node = new UnsafeTreiberStackNode<Record>();
        node->Value().Lock();

        IncreaseSize();
        return &m_stack.Push(node)->Value(); 
    }

    auto InsertNewUnlockedRecord() noexcept -> Record* {
        IncreaseSize();
        return &m_stack.Push()->Value(); 
    }

    auto IncreaseSize(size_t size = 1) noexcept -> void {
        m_size.FetchAddRlx(size);
    }

    UnsafeTreiberStack<Record> m_stack;
    basic::utils::Atomic<size_t> m_size = 0;

};

template <typename T>
class HazardPointerReclaimPool {

private:

    using Self = HazardPointerReclaimPool<T>;
    using Record = HazardPointerReclaimRecord<T>;
    using ResourcePool = HazardPointerResourcePool<T>;

public:

    HazardPointerReclaimPool() noexcept = default;

    ~HazardPointerReclaimPool() noexcept = default;

    HazardPointerReclaimPool(Self&& rhs) noexcept = delete;

    HazardPointerReclaimPool(const Self& rhs) noexcept = delete;

    auto operator=(Self&& rhs) noexcept -> Self& = delete;

    auto operator=(const Self& rhs) noexcept -> Self& = delete;

    auto Reclaim(T* ptr, ResourcePool& resource_pool) noexcept -> void {
        auto threshold = EstimateThreshold(resource_pool);
        if (Push(ptr) <= threshold) return;
        if (IsSizeBiggerThreshold(threshold)) Reclaim(resource_pool);
    }

private:

    auto Push(T* ptr) noexcept -> size_t {
        m_stack.Push(ptr);
        return IncrementSize(1);
    }

    auto EstimateThreshold(ResourcePool& resource_pool) noexcept -> size_t {
        auto hp_size = resource_pool.GetSize();
        return hp_size + hp_size;
    }

    auto Reclaim(ResourcePool& resource_pool) noexcept -> void {
        auto* head = m_stack.PopAll();
        auto hps = resource_pool.BuildCachedHazardPointers();

        size_t new_size = 0;
        UnsafeTreiberStackNode<Record>* new_head = nullptr;
        UnsafeTreiberStackNode<Record>* new_tail = nullptr;

        while (head != nullptr) {
            auto* next = head->Next();

            if (!hps.Contains(head->Value().Ptr())) {
                delete head;
            } else {
                head->Next() = new_head;
                new_head = head;

                if (new_tail == nullptr)
                    new_tail = head;

                new_size++;
            }

            head = next;
        }

        if (new_head == nullptr)
            return;

        m_stack.Push(new_head, new_tail);
        IncrementSize(new_size);
    }

    auto ExchangeSize(size_t size) noexcept -> size_t {
        return m_size.ExchangeRlx(size);
    }

    auto IncrementSize(size_t size) noexcept -> size_t {
        return m_size.FetchAddRlx(size);
    }

    auto IsSizeBiggerThreshold(size_t threshold) noexcept -> bool {
        auto size = ExchangeSize(0);
        if (size > threshold)
            return true;

        IncrementSize(size);
        return false;
    }

    UnsafeTreiberStack<Record> m_stack;
    basic::utils::Atomic<size_t> m_size = 0;

};

template <typename T>
class HazardPointerDomain {

private:

    using Self = HazardPointerDomain<T>;

public:

    HazardPointerDomain(
            size_t hazard_pointer_per_thread,
            size_t expected_threads = std::thread::hardware_concurrency())
            noexcept :
            m_resource_pool(hazard_pointer_per_thread * expected_threads) {}

    ~HazardPointerDomain() noexcept = default;

    HazardPointerDomain(Self&& rhs) noexcept = delete;

    HazardPointerDomain(const Self& rhs) noexcept = delete;

    auto operator=(Self&& rhs) noexcept -> Self& = delete;

    auto operator=(const Self& rhs) noexcept -> Self& = delete;

    auto MakeHazardPointer() noexcept -> HazardPointer<T> {
        return m_resource_pool.MakeHazardPointer();
    }

    auto Retire(T* ptr) noexcept -> void {
        m_reclaim_pool.Reclaim(ptr, m_resource_pool);
    }

private:

    HazardPointerResourcePool<T> m_resource_pool;
    HazardPointerReclaimPool<T> m_reclaim_pool;

};

}
