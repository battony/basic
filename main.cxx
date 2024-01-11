#include "basic/concurrency/HpFaaArrayQueue.hxx"
#include "basic/concurrency/HpMichaelScottQueue.hxx"
#include "basic/concurrency/HpTreiberStack.hxx"
#include "basic/os/Semaphore.hxx"

#include <iostream>
#include <array>
#include <vector>
#include <deque>
#include <thread>
#include <chrono>
#include <mutex>
#include <algorithm>

#define K 8
size_t cnt = 8000000;

auto main() -> int {

    auto start = std::chrono::steady_clock::now();

    basic::os::Semaphore sem(5);

    { 
        std::array<basic::concurrency::HpFaaArrayQueue<size_t>, K> ss;
        std::array<std::vector<std::thread>, K> pools;
        std::array<std::atomic<size_t>, K> not_dones;

#ifdef CHECK
        std::array<std::mutex, K> mtxs;
        std::array<std::vector<size_t>, K> values;
#endif

        for (size_t k = 0; k < K; k++)
            not_dones[k] = cnt;

        for (size_t k = 0; k < K; k++) {
	        for (size_t i = 8; i > 0; --i)
	    	    pools[k].emplace_back(
                    [&, k, idx=i - 1] () mutable {
                        for (size_t i = cnt / 8 * idx; i < cnt / 8 * (idx + 1); i++)
                            ss[k].Push(i + 1);
	    	        });

	        for (auto i = 8; i > 0; --i)
	        	pools[k].emplace_back(
                    [&, k] () mutable {
#ifdef CHECK
                        std::vector<size_t> arr;
                        arr.reserve(cnt);
#endif
	        	    	while (not_dones[k].load() > 100) {
                            size_t v;
                            if (!ss[k].Pop(v)) continue;
#ifdef CHECK
                            arr.emplace_back(v);
#endif
                            not_dones[k]--;
                        }

#ifdef CHECK
                        std::lock_guard lock(mtxs[k]);
                        std::copy(arr.begin(), arr.end(), std::back_inserter(values[k]));
#endif
	        	    });
        }

        for (size_t k = 0; k < K; k++)
	        for (auto &thread: pools[k])
	        	if (thread.joinable())
	        		thread.join();

#ifdef CHECK
        for (size_t k = 0; k < K; k++) {
            auto& arr = values[k];
            auto usize = arr.size();
            std::sort(arr.begin(), arr.end());
            auto last = std::unique(arr.begin(), arr.end());
            arr.erase(last, arr.end());
            std::cout << k << ": " << usize << " -> " << arr.size() << " | " << (arr.size() == cnt) << std::endl;
            for (auto v: arr)
                if (v > cnt)
                    std::cout << k << ": error " << v << std::endl;
        }
#endif
    }

    auto end = std::chrono::steady_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::duration<float>>(end - start).count();
    std::cout << (static_cast<float>(cnt * 2 * K) / elapsed_time) / 1e6F << " Mop/s" << std::endl;
    std::cout << (elapsed_time / static_cast<float>(cnt * 2 * K)) * 1e9F << " ns/op" << std::endl;
    return 0;
}
