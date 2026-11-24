#pragma once

#include <cstdio>
#include <string>
#include <type_traits>
#include <vector>

namespace th13test {

struct TestCase {
    const char* name;
    void (*body)();
};

inline std::vector<TestCase>& registry() {
    static std::vector<TestCase> tests;
    return tests;
}
inline int& failure_count() {
    static int count = 0;
    return count;
}
inline bool& current_failed() {
    static bool failed = false;
    return failed;
}

struct Registrar {
    Registrar(const char* name, void (*body)()) { registry().push_back({name, body}); }
};

inline int run_all(const char* suite) {
    std::printf("# %s\n", suite);
    int index = 0;
    for (const auto& test : registry()) {
        ++index;
        current_failed() = false;
        test.body();
        if (current_failed()) {
            ++failure_count();
            std::printf("not ok %d - %s\n", index, test.name);
        } else {
            std::printf("ok %d - %s\n", index, test.name);
        }
    }
    std::printf("# %d..%d%s\n", 1, index, failure_count() ? " 有失败" : " 全部通过");
    return failure_count() ? 1 : 0;
}

}

#define TEST(name)                                                                                                     \
    void th13test_body_##name();                                                                                       \
    static ::th13test::Registrar th13test_reg_##name(#name, &th13test_body_##name);                                    \
    void th13test_body_##name()

#define CHECK(condition)                                                                                               \
    do {                                                                                                               \
        if (!(condition)) {                                                                                            \
            std::printf("  失败 %s:%d: %s\n", __FILE__, __LINE__, #condition);                                         \
            ::th13test::current_failed() = true;                                                                       \
        }                                                                                                              \
    } while (0)

namespace th13test {
template <class A, class E>
inline void report_mismatch(const char* file, int line, const char* a_text, const char* e_text, const A& a,
                            const E& e) {
    std::printf("  失败 %s:%d: %s == %s\n", file, line, a_text, e_text);
    if constexpr (std::is_arithmetic_v<A>)
        std::printf("    实际 %lld 期望 %lld\n", static_cast<long long>(a), static_cast<long long>(e));
}

template <typename A, typename E>
inline void check_eq(const char* file, int line, const char* a_text, const char* e_text, const A& a, const E& e) {
    if (!(a == e)) {
        report_mismatch(file, line, a_text, e_text, a, e);
        current_failed() = true;
    }
}
}

#define CHECK_EQ(actual, expected)                                                                                     \
    do {                                                                                                               \
        ::th13test::check_eq(__FILE__, __LINE__, #actual, #expected, (actual), (expected));                            \
    } while (0)

#define CHECK_NEAR(actual, expected, tolerance)                                                                        \
    do {                                                                                                               \
        const double a_value = double(actual);                                                                         \
        const double e_value = double(expected);                                                                       \
        if (!(a_value > double(expected) - double(tolerance) && a_value < double(expected) + double(tolerance))) {     \
            std::printf("  失败 %s:%d: %s ≈ %s（±%s）\n", __FILE__, __LINE__, #actual, #expected, #tolerance);         \
            std::printf("    实际 %.9g 期望 %.9g\n", a_value, e_value);                                                \
            ::th13test::current_failed() = true;                                                                       \
        }                                                                                                              \
    } while (0)
