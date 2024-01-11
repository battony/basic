#pragma once

#include <atomic>

namespace basic::utils {

template <typename T>
class Atomic {

private:

    using Self = Atomic<T>;

public:

    Atomic() noexcept = default;

    Atomic(T value) noexcept : m_atomic(value) {}

    ~Atomic() noexcept = default;

    Atomic(Self&& rhs) noexcept = delete;

    Atomic(const Self& rhs) noexcept = delete;

    auto operator=(Self&& rhs) noexcept -> Self& = delete;

    auto operator=(const Self& rhs) noexcept -> Self& = delete;

    auto Raw() noexcept -> std::atomic<T>& {
        return m_atomic;
    }

    auto Raw() const noexcept -> const std::atomic<T>& {
        return m_atomic;
    }

    auto Load(std::memory_order order) const noexcept -> T {
        return m_atomic.load(order);
    }

    auto LoadRlx() const noexcept -> T {
        return Load(std::memory_order_relaxed);
    }

    auto LoadAcq() const noexcept -> T {
        return Load(std::memory_order_acquire);
    }

    auto LoadSc() const noexcept -> T {
        return Load(std::memory_order_seq_cst);
    }

    auto Store(T value, std::memory_order order) noexcept -> void {
        m_atomic.store(value, order);
    }

    auto StoreRlx(T value) noexcept -> void {
        Store(value, std::memory_order_relaxed);
    }

    auto StoreRel(T value) noexcept -> void {
        Store(value, std::memory_order_release);
    }

    auto StoreSc(T value) noexcept -> void {
        Store(value, std::memory_order_seq_cst);
    }

    auto Exchange(T value, std::memory_order order) noexcept -> T {
        return m_atomic.exchange(value, order);
    }

    auto ExchangeRlx(T value) noexcept -> T {
        return Exchange(value, std::memory_order_relaxed);
    }

    auto ExchangeAcq(T value) noexcept -> T {
        return Exchange(value, std::memory_order_acquire);
    }

    auto ExchangeRel(T value) noexcept -> T {
        return Exchange(value, std::memory_order_release);
    }

    auto ExchangeAr(T value) noexcept -> T {
        return Exchange(value, std::memory_order_acq_rel);
    }

    auto ExchangeSc(T value) noexcept -> T {
        return Exchange(value, std::memory_order_seq_cst);
    }

    auto FetchAdd(T value, std::memory_order order) noexcept -> T {
        return m_atomic.fetch_add(value, order);
    }

    auto FetchAddRlx(T value) noexcept -> T {
        return FetchAdd(value, std::memory_order_relaxed);
    }

    auto FetchAddAcq(T value) noexcept -> T {
        return FetchAdd(value, std::memory_order_acquire);
    }

    auto FetchAddRel(T value) noexcept -> T {
        return FetchAdd(value, std::memory_order_release);
    }

    auto FetchAddAr(T value) noexcept -> T {
        return FetchAdd(value, std::memory_order_acq_rel);
    }

    auto FetchAddSc(T value) noexcept -> T {
        return FetchAdd(value, std::memory_order_seq_cst);
    }

    auto FetchSub(T value, std::memory_order order) noexcept -> T {
        return m_atomic.fetch_sub(value, order);
    }

    auto FetchSubRlx(T value) noexcept -> T {
        return FetchSub(value, std::memory_order_relaxed);
    }

    auto FetchSubAcq(T value) noexcept -> T {
        return FetchSub(value, std::memory_order_acquire);
    }

    auto FetchSubRel(T value) noexcept -> T {
        return FetchSub(value, std::memory_order_release);
    }

    auto FetchSubAr(T value) noexcept -> T {
        return FetchSub(value, std::memory_order_acq_rel);
    }

    auto FetchSubSc(T value) noexcept -> T {
        return FetchSub(value, std::memory_order_seq_cst);
    }

    auto FetchOr(T value, std::memory_order order) noexcept -> T {
        return m_atomic.fetch_or(value, order);
    }

    auto FetchOrRlx(T value) noexcept -> T {
        return FetchOr(value, std::memory_order_relaxed);
    }

    auto FetchOrAcq(T value) noexcept -> T {
        return FetchOr(value, std::memory_order_acquire);
    }

    auto FetchOrRel(T value) noexcept -> T {
        return FetchOr(value, std::memory_order_release);
    }

    auto FetchOrAr(T value) noexcept -> T {
        return FetchOr(value, std::memory_order_acq_rel);
    }

    auto FetchOrSc(T value) noexcept -> T {
        return FetchOr(value, std::memory_order_seq_cst);
    }

    auto FetchAnd(T value, std::memory_order order) noexcept -> T {
        return m_atomic.fetch_and(value, order);
    }

    auto FetchAndRlx(T value) noexcept -> T {
        return FetchAnd(value, std::memory_order_relaxed);
    }

    auto FetchAndAcq(T value) noexcept -> T {
        return FetchAnd(value, std::memory_order_acquire);
    }

    auto FetchAndRel(T value) noexcept -> T {
        return FetchAnd(value, std::memory_order_release);
    }

    auto FetchAndAr(T value) noexcept -> T {
        return FetchAnd(value, std::memory_order_acq_rel);
    }

    auto FetchAndSc(T value) noexcept -> T {
        return FetchAnd(value, std::memory_order_seq_cst);
    }

    auto FetchXor(T value, std::memory_order order) noexcept -> T {
        return m_atomic.fetch_xor(value, order);
    }

    auto FetchXorRlx(T value) noexcept -> T {
        return FetchXor(value, std::memory_order_relaxed);
    }

    auto FetchXorAcq(T value) noexcept -> T {
        return FetchXor(value, std::memory_order_acquire);
    }

    auto FetchXorRel(T value) noexcept -> T {
        return FetchXor(value, std::memory_order_release);
    }

    auto FetchXorAr(T value) noexcept -> T {
        return FetchXor(value, std::memory_order_acq_rel);
    }

    auto FetchXorSc(T value) noexcept -> T {
        return FetchXor(value, std::memory_order_seq_cst);
    }

    auto CasWeak(
            T& cmp, T value,
            std::memory_order success,
            std::memory_order failure)
            noexcept -> bool {
        return m_atomic.compare_exchange_weak(cmp, value, success, failure);
    }

    auto CasWeakRlxRlx(T& cmp, T value) noexcept -> bool {
        return CasWeak(
                cmp, value,
                std::memory_order_relaxed,
                std::memory_order_relaxed);
    }

    auto CasWeakRelRlx(T& cmp, T value) noexcept -> bool {
        return CasWeak(
                cmp, value,
                std::memory_order_release,
                std::memory_order_relaxed);
    }

    auto CasWeakRelAcq(T& cmp, T value) noexcept -> bool {
        return CasWeak(
                cmp, value,
                std::memory_order_release,
                std::memory_order_acquire);
    }

    auto CasWeakAcqRlx(T& cmp, T value) noexcept -> bool {
        return CasWeak(
                cmp, value,
                std::memory_order_acquire,
                std::memory_order_relaxed);
    }

    auto CasWeakAcqAcq(T& cmp, T value) noexcept -> bool {
        return CasWeak(
                cmp, value,
                std::memory_order_acquire,
                std::memory_order_acquire);
    }

    auto CasWeakArRlx(T& cmp, T value) noexcept -> bool {
        return CasWeak(
                cmp, value,
                std::memory_order_acq_rel,
                std::memory_order_relaxed);
    }

    auto CasWeakArAcq(T& cmp, T value) noexcept -> bool {
        return CasWeak(
                cmp, value,
                std::memory_order_acq_rel,
                std::memory_order_acquire);
    }

    auto CasWeakScRlx(T& cmp, T value) noexcept -> bool {
        return CasWeak(
                cmp, value,
                std::memory_order_seq_cst,
                std::memory_order_relaxed);
    }

    auto CasWeakScAcq(T& cmp, T value) noexcept -> bool {
        return CasWeak(
                cmp, value,
                std::memory_order_seq_cst,
                std::memory_order_acquire);
    }

    auto CasWeakScSc(T& cmp, T value) noexcept -> bool {
        return CasWeak(
                cmp, value,
                std::memory_order_seq_cst,
                std::memory_order_seq_cst);
    }

    auto CasStrong(
            T& cmp, T value,
            std::memory_order success,
            std::memory_order failure)
            noexcept -> bool {
        return m_atomic.compare_exchange_strong(cmp, value, success, failure);
    }

    auto CasStrongRlxRlx(T& cmp, T value) noexcept -> bool {
        return CasStrong(
                cmp, value,
                std::memory_order_relaxed,
                std::memory_order_relaxed);
    }

    auto CasStrongRelRlx(T& cmp, T value) noexcept -> bool {
        return CasStrong(
                cmp, value,
                std::memory_order_release,
                std::memory_order_relaxed);
    }

    auto CasStrongRelAcq(T& cmp, T value) noexcept -> bool {
        return CasStrong(
                cmp, value,
                std::memory_order_release,
                std::memory_order_acquire);
    }

    auto CasStrongAcqRlx(T& cmp, T value) noexcept -> bool {
        return CasStrong(
                cmp, value,
                std::memory_order_acquire,
                std::memory_order_relaxed);
    }

    auto CasStrongAcqAcq(T& cmp, T value) noexcept -> bool {
        return CasStrong(
                cmp, value,
                std::memory_order_acquire,
                std::memory_order_acquire);
    }

    auto CasStrongArRlx(T& cmp, T value) noexcept -> bool {
        return CasStrong(
                cmp, value,
                std::memory_order_acq_rel,
                std::memory_order_relaxed);
    }

    auto CasStrongArAcq(T& cmp, T value) noexcept -> bool {
        return CasStrong(
                cmp, value,
                std::memory_order_acq_rel,
                std::memory_order_acquire);
    }

    auto CasStrongScRlx(T& cmp, T value) noexcept -> bool {
        return CasStrong(
                cmp, value,
                std::memory_order_seq_cst,
                std::memory_order_relaxed);
    }

    auto CasStrongScAcq(T& cmp, T value) noexcept -> bool {
        return CasStrong(
                cmp, value,
                std::memory_order_seq_cst,
                std::memory_order_acquire);
    }

    auto CasStrongScSc(T& cmp, T value) noexcept -> bool {
        return CasStrong(
                cmp, value,
                std::memory_order_seq_cst,
                std::memory_order_seq_cst);
    }

private:

    std::atomic<T> m_atomic;

};

class AtomicFlag {

private:

    using Self = AtomicFlag;

public:

    AtomicFlag() noexcept = default;

    ~AtomicFlag() noexcept = default;

    AtomicFlag(Self&& rhs) noexcept = delete;

    AtomicFlag(const Self& rhs) noexcept = delete;

    auto operator=(Self&& rhs) noexcept -> Self& = delete;

    auto operator=(const Self& rhs) noexcept -> Self& = delete;

    auto Raw() noexcept -> std::atomic_flag& {
        return m_atomic_flag;
    }

    auto Raw() const noexcept -> const std::atomic_flag& {
        return m_atomic_flag;
    }

    auto Clear(std::memory_order order) noexcept -> void {
        m_atomic_flag.clear(order);
    }

    auto ClearRlx() noexcept -> void {
        Clear(std::memory_order_relaxed);
    }

    auto ClearRel() noexcept -> void {
        Clear(std::memory_order_release);
    }

    auto ClearSc() noexcept -> void {
        Clear(std::memory_order_seq_cst);
    }

    auto TestAndSet(std::memory_order order) noexcept -> bool {
        return m_atomic_flag.test_and_set(order);
    }

    auto TestAndSetRlx() noexcept -> bool {
        return TestAndSet(std::memory_order_relaxed);
    }

    auto TestAndSetAcq() noexcept -> bool {
        return TestAndSet(std::memory_order_acquire);
    }

    auto TestAndSetRel() noexcept -> bool {
        return TestAndSet(std::memory_order_release);
    }

    auto TestAndSetAr() noexcept -> bool {
        return TestAndSet(std::memory_order_acq_rel);
    }

    auto TestAndSetSc() noexcept -> bool {
        return TestAndSet(std::memory_order_seq_cst);
    }

private:

    std::atomic_flag m_atomic_flag = ATOMIC_FLAG_INIT;

};

}
