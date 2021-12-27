#include <chrono>
#include <iostream>
#include <thread>
#include <type_traits>

#ifdef __APPLE__
#include <unistd.h>
#endif

using Clock = std::conditional_t<std::chrono::high_resolution_clock::is_steady,
                                 std::chrono::high_resolution_clock,
                                 std::chrono::steady_clock>;

void testStdSleep(long long us) {
    Clock::time_point begin = Clock::now();
    std::this_thread::sleep_for(std::chrono::microseconds{us});
    auto time = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - begin);

    std::cout << "std::this_thread::sleep_for() " << time.count() << "us\n";
}

void testUsleep(long long us) {
    Clock::time_point begin = Clock::now();
    usleep(us);
    auto time = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - begin);

    std::cout << "unix usleep() " << time.count() << "us\n";
}

void testNanosleep(long long us) {
    timespec req{
        .tv_sec = 0,
        .tv_nsec = us * 1'000,
    };

    Clock::time_point begin = Clock::now();
    nanosleep(&req, nullptr);
    auto time = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - begin);

    std::cout << "unix nanosleep() " << time.count() << "us\n";
}

int main() {
    using F = void(long long);

    constexpr F* testCases[] = {
        &testStdSleep,
#ifdef __APPLE__
        &testUsleep,
        &testNanosleep,
#endif
    };

    std::cout << "-> sleep 1ms tests\n";
    for (auto testCase : testCases) {
        testCase(1'000);
    }

#ifdef __APPLE__
    std::cout << "\n-> sleep 100us tests\n";
    for (auto testCase : testCases) {
        testCase(100);
    }

    std::cout << "\n-> sleep 10us tests\n";
    for (auto testCase : testCases) {
        testCase(10);
    }

    std::cout << "\n-> sleep 1us tests\n";
    for (auto testCase : testCases) {
        testCase(1);
    }
#endif
}
