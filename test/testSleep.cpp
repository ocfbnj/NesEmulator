#include <chrono>
#include <iostream>
#include <thread>
#include <type_traits>

#ifdef __APPLE__
#include <unistd.h>
#endif

#ifdef _WIN32
#include <Windows.h>
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

#ifdef __APPLE__
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
#endif

#ifdef _WIN32
void testSleep(int ms) {
    Clock::time_point begin = Clock::now();
    Sleep(ms);
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - begin);

    std::cout << "Windows Sleep() " << time.count() << "ms\n";
}
#endif

int main() {
    using F = void(long long);

#ifdef __APPLE__

    constexpr F* testCases[] = {
        &testStdSleep,
        &testUsleep,
        &testNanosleep,
    };

    std::cout << "-> sleep 1ms tests\n";
    for (auto testCase : testCases) {
        testCase(1'000);
    }

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

#ifdef _WIN32
    std::cout << "-> sleep 10ms tests\n";
    testStdSleep(10'000);
    testSleep(10);

    std::cout << "\n-> sleep 1ms tests\n";
    testStdSleep(1'000);
    testSleep(1);

    std::cout << "\n== call timeBeginPeriod(1) and timeEndPeriod(1) in Windows ==\n";

    std::cout << "\n-> sleep 10ms tests\n";
    timeBeginPeriod(1);
    testStdSleep(10'000);
    timeEndPeriod(1);

    timeBeginPeriod(1);
    testSleep(10);
    timeEndPeriod(1);

    std::cout << "\n-> sleep 1ms tests\n";
    timeBeginPeriod(1);
    testStdSleep(1'000);
    timeEndPeriod(1);

    timeBeginPeriod(1);
    testSleep(1);
    timeEndPeriod(1);

    std::cout << "\n-> sleep 100us tests\n";
    timeBeginPeriod(1);
    testStdSleep(100);
    timeEndPeriod(1);
#endif
}
