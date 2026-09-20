#pragma once

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

namespace HostTest {

struct TestCase {
    std::string name;
    void (*func)();
};

class TestRegistry {
public:
    static TestRegistry& instance() {
        static TestRegistry reg;
        return reg;
    }

    void registerTest(const std::string& name, void (*func)()) {
        tests_.push_back({name, func});
    }

    void countAssertion() {
        total_assertions_++;
    }

    void recordFailure(const char* file, int line, const std::string& msg) {
        total_failures_++;
        current_test_failed_ = true;
        std::printf("  \033[31;1m[FAIL]\033[0m %s:%d: %s\n", file, line, msg.c_str());
    }

    int runAll(const char* suite_name) {
        std::printf("\n\033[34;1m=== Running Test Suite: %s ===\033[0m\n", suite_name);
        int passed = 0;
        total_assertions_ = 0;
        total_failures_ = 0;

        for (const auto& t : tests_) {
            current_test_failed_ = false;
            std::printf("  \033[36mRUN\033[0m %s\n", t.name.c_str());
            t.func();
            if (current_test_failed_) {
                std::printf("  \033[31;1mFAIL\033[0m %s\n", t.name.c_str());
            } else {
                std::printf("  \033[32mPASS\033[0m %s\n", t.name.c_str());
                passed++;
            }
        }

        std::printf("\033[34;1m=== Results for %s: %d/%d passed, %d assertions, %d failures ===\033[0m\n\n",
                    suite_name, passed, (int)tests_.size(), total_assertions_, total_failures_);

        return (total_failures_ == 0) ? 0 : 1;
    }

    int totalAssertions() const { return total_assertions_; }
    int totalFailures() const { return total_failures_; }

private:
    std::vector<TestCase> tests_;
    int total_assertions_ = 0;
    int total_failures_ = 0;
    bool current_test_failed_ = false;
};

struct TestRegistrar {
    TestRegistrar(const std::string& name, void (*func)()) {
        TestRegistry::instance().registerTest(name, func);
    }
};

} // namespace HostTest

#define TEST_CASE(name) \
    static void test_##name(); \
    static ::HostTest::TestRegistrar reg_##name(#name, test_##name); \
    static void test_##name()

#define ASSERT_TRUE(cond) do { \
    ::HostTest::TestRegistry::instance().countAssertion(); \
    if (!(cond)) { \
        ::HostTest::TestRegistry::instance().recordFailure(__FILE__, __LINE__, \
            std::string("Expected true: ") + #cond); \
    } \
} while(0)

#define ASSERT_FALSE(cond) do { \
    ::HostTest::TestRegistry::instance().countAssertion(); \
    if (cond) { \
        ::HostTest::TestRegistry::instance().recordFailure(__FILE__, __LINE__, \
            std::string("Expected false: ") + #cond); \
    } \
} while(0)

#define ASSERT_EQ(actual, expected) do { \
    ::HostTest::TestRegistry::instance().countAssertion(); \
    auto _a = (actual); \
    auto _e = (expected); \
    if (_a != _e) { \
        char _buf[256]; \
        std::snprintf(_buf, sizeof(_buf), "%s == %s (actual %lld vs expected %lld)", \
                      #actual, #expected, (long long)_a, (long long)_e); \
        ::HostTest::TestRegistry::instance().recordFailure(__FILE__, __LINE__, _buf); \
    } \
} while(0)

#define ASSERT_NE(actual, expected) do { \
    ::HostTest::TestRegistry::instance().countAssertion(); \
    auto _a = (actual); \
    auto _e = (expected); \
    if (_a == _e) { \
        char _buf[256]; \
        std::snprintf(_buf, sizeof(_buf), "%s != %s (both equal %lld)", \
                      #actual, #expected, (long long)_a); \
        ::HostTest::TestRegistry::instance().recordFailure(__FILE__, __LINE__, _buf); \
    } \
} while(0)

#define ASSERT_LT(a, b) do { \
    ::HostTest::TestRegistry::instance().countAssertion(); \
    if (!((a) < (b))) { \
        char _buf[256]; \
        std::snprintf(_buf, sizeof(_buf), "%s < %s failed", #a, #b); \
        ::HostTest::TestRegistry::instance().recordFailure(__FILE__, __LINE__, _buf); \
    } \
} while(0)

#define ASSERT_LE(a, b) do { \
    ::HostTest::TestRegistry::instance().countAssertion(); \
    if (!((a) <= (b))) { \
        char _buf[256]; \
        std::snprintf(_buf, sizeof(_buf), "%s <= %s failed", #a, #b); \
        ::HostTest::TestRegistry::instance().recordFailure(__FILE__, __LINE__, _buf); \
    } \
} while(0)

#define ASSERT_GT(a, b) do { \
    ::HostTest::TestRegistry::instance().countAssertion(); \
    if (!((a) > (b))) { \
        char _buf[256]; \
        std::snprintf(_buf, sizeof(_buf), "%s > %s failed", #a, #b); \
        ::HostTest::TestRegistry::instance().recordFailure(__FILE__, __LINE__, _buf); \
    } \
} while(0)

#define ASSERT_GE(a, b) do { \
    ::HostTest::TestRegistry::instance().countAssertion(); \
    if (!((a) >= (b))) { \
        char _buf[256]; \
        std::snprintf(_buf, sizeof(_buf), "%s >= %s failed", #a, #b); \
        ::HostTest::TestRegistry::instance().recordFailure(__FILE__, __LINE__, _buf); \
    } \
} while(0)

#define ASSERT_NEAR(actual, expected, tol) do { \
    ::HostTest::TestRegistry::instance().countAssertion(); \
    double _act = static_cast<double>(actual); \
    double _exp = static_cast<double>(expected); \
    double _tol = static_cast<double>(tol); \
    double _diff = std::fabs(_act - _exp); \
    if (std::isnan(_act) || std::isnan(_exp) || _diff > _tol) { \
        char _buf[256]; \
        std::snprintf(_buf, sizeof(_buf), "%s ~= %s (actual %.8g, expected %.8g, diff %.8g > tol %.8g)", \
                      #actual, #expected, _act, _exp, _diff, _tol); \
        ::HostTest::TestRegistry::instance().recordFailure(__FILE__, __LINE__, _buf); \
    } \
} while(0)

#define ASSERT_NAN(val) do { \
    ::HostTest::TestRegistry::instance().countAssertion(); \
    if (!std::isnan(val)) { \
        char _buf[256]; \
        std::snprintf(_buf, sizeof(_buf), "%s expected NaN, got %.8g", #val, (double)(val)); \
        ::HostTest::TestRegistry::instance().recordFailure(__FILE__, __LINE__, _buf); \
    } \
} while(0)

#define ASSERT_INF(val) do { \
    ::HostTest::TestRegistry::instance().countAssertion(); \
    if (!std::isinf(val)) { \
        char _buf[256]; \
        std::snprintf(_buf, sizeof(_buf), "%s expected Inf, got %.8g", #val, (double)(val)); \
        ::HostTest::TestRegistry::instance().recordFailure(__FILE__, __LINE__, _buf); \
    } \
} while(0)

#define ASSERT_FINITE(val) do { \
    ::HostTest::TestRegistry::instance().countAssertion(); \
    if (!std::isfinite(val)) { \
        char _buf[256]; \
        std::snprintf(_buf, sizeof(_buf), "%s expected finite, got %.8g", #val, (double)(val)); \
        ::HostTest::TestRegistry::instance().recordFailure(__FILE__, __LINE__, _buf); \
    } \
} while(0)
