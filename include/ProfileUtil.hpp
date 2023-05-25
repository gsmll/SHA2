#ifndef PROFILE_UTIL_HPP
#define PROFILE_UTIL_HPP

#include <chrono>
#include <utility>
#include <ctime>
#include <tuple>

template<typename TimeUnit, typename Func, typename... Args>
std::int64_t __attribute__((optimize("O0"))) chrono_time_it(Func&& func, Args&&... args)
{
    using namespace std::chrono;
    auto t0 = high_resolution_clock::now();
    volatile auto res = func(std::forward<Args>(args)...);
    auto t1 = high_resolution_clock::now();
    auto delta_time = duration_cast<TimeUnit>(t1 - t0);
    return delta_time.count();
}

template<typename Func, typename... Args>
std::tuple<long, long, long> __attribute__((optimize("O0"))) high_res_time_it(Func&& func, Args&&... args)
{
    constexpr std::uint64_t nano_in_sec = 1e9;

    timespec rt0, ct0, tt0;
    clock_gettime(CLOCK_REALTIME, &rt0);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ct0);
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tt0);

    func(std::forward<Args>(args)...);

    timespec rt1, ct1, tt1;
    clock_gettime(CLOCK_REALTIME, &rt1);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ct1);
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tt1);

    return { 
        nano_in_sec * (rt1.tv_sec - rt0.tv_sec) + rt1.tv_nsec - rt0.tv_nsec, 
        nano_in_sec * (ct1.tv_sec - ct0.tv_sec) + ct1.tv_nsec - ct0.tv_nsec, 
        nano_in_sec * (tt1.tv_sec - tt0.tv_sec) + tt1.tv_nsec - tt0.tv_nsec 
    };
}

#endif