// 极简单测框架：容器内 g++ 编译，无外部依赖。
// 用法：TEST("名称") { CHECK(cond); CHECK_EQ(a, b); return; }
// 输出 TAP 风格 ok/not ok 行，任一失败使进程以非零码退出。
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

}  // namespace th13test

#define TEST(name)                                                                     \
    void th13test_body_##name();                                                       \
    static ::th13test::Registrar th13test_reg_##name(#name, &th13test_body_##name);    \
    void th13test_body_##name()

#define CHECK(condition)                                                                   \
    do {                                                                                   \
        if (!(condition)) {                                                                \
            std::printf("  失败 %s:%d: %s\n", __FILE__, __LINE__, #condition);             \
            ::th13test::current_failed() = true;                                           \
        }                                                                                  \
    } while (0)

namespace th13test {
template <class A, class E>
inline void report_mismatch(const char* file, int line, const char* a_text, const char* e_text, const A& a, const E& e) {
    std::printf("  失败 %s:%d: %s == %s\n", file, line, a_text, e_text);
    if constexpr (std::is_arithmetic_v<A>)
        std::printf("    实际 %lld 期望 %lld\n", static_cast<long long>(a), static_cast<long long>(e));
}
}  // namespace th13test

#define CHECK_EQ(actual, expected)                                                                     \
    do {                                                                                               \
        const auto& a_value = (actual);                                                                \
        const auto& e_value = (expected);                                                              \
        if (!(a_value == e_value)) {                                                                   \
            ::th13test::report_mismatch(__FILE__, __LINE__, #actual, #expected, a_value, e_value);     \
            ::th13test::current_failed() = true;                                                       \
        }                                                                                              \
    } while (0)

#define CHECK_NEAR(actual, expected, tolerance)                                                         \
    do {                                                                                                \
        const double a_value = double(actual);                                                          \
        const double e_value = double(expected);                                                        \
        if (!(a_value > double(expected) - double(tolerance) && a_value < double(expected) + double(tolerance))) { \
            std::printf("  失败 %s:%d: %s ≈ %s（±%s）\n", __FILE__, __LINE__, #actual, #expected, #tolerance); \
            std::printf("    实际 %.9g 期望 %.9g\n", a_value, e_value);                                 \
            ::th13test::current_failed() = true;                                                       \
        }                                                                                               \
    } while (0)
