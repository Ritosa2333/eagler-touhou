#pragma once

#include <cstdio>
#include <cstdlib>
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
inline int& block_assertions() {
    static int count = 0;
    return count;
}

struct Registrar {
    Registrar(const char* name, void (*body)()) { registry().push_back({name, body}); }
};

inline bool case_filter_active() {
    const char* filter = std::getenv("TH13_CASE_FILTER");
    return filter && filter[0] != '\0';
}

inline bool case_filter_allows(const char* name) {
    const char* filter = std::getenv("TH13_CASE_FILTER");
    if (!filter || filter[0] == '\0') return true;
    const std::string value(filter);
    size_t begin = 0;
    while (begin <= value.size()) {
        size_t end = value.find(',', begin);
        if (end == std::string::npos) end = value.size();
        if (end > begin && value.compare(begin, end - begin, name) == 0) return true;
        begin = end + 1;
    }
    return false;
}

inline int run_all(const char* suite) {
    std::printf("# %s\n", suite);
    if (case_filter_active()) {
        std::printf("# case-filter: %s\n", std::getenv("TH13_CASE_FILTER"));
    }
    int index = 0;
    for (const auto& test : registry()) {
        if (!case_filter_allows(test.name)) continue;
        ++index;
        current_failed() = false;
        block_assertions() = 0;
        test.body();
        const int assertions = block_assertions();
        if (current_failed()) {
            ++failure_count();
            std::printf("not ok %d - %s\n", index, test.name);
        } else {
            std::printf("ok %d - %s\n", index, test.name);
        }
        std::printf("# assertions: %d\n", assertions);
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
        ++::th13test::block_assertions();                                                                              \
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

template <class A, class E>
inline void check_eq(const char* file, int line, const char* a_text, const char* e_text, const A& a, const E& e) {
    if (!(a == e)) {
        report_mismatch(file, line, a_text, e_text, a, e);
        current_failed() = true;
    }
}
}

#define CHECK_EQ(actual, expected)                                                                                     \
    do {                                                                                                               \
        ++::th13test::block_assertions();                                                                              \
        ::th13test::check_eq(__FILE__, __LINE__, #actual, #expected, (actual), (expected));                            \
    } while (0)

#define CHECK_NEAR(actual, expected, tolerance)                                                                        \
    do {                                                                                                               \
        ++::th13test::block_assertions();                                                                              \
        const double a_value = double(actual);                                                                         \
        const double e_value = double(expected);                                                                       \
        if (!(a_value > double(expected) - double(tolerance) && a_value < double(expected) + double(tolerance))) {     \
            std::printf("  失败 %s:%d: %s ≈ %s（±%s）\n", __FILE__, __LINE__, #actual, #expected, #tolerance);         \
            std::printf("    实际 %.9g 期望 %.9g\n", a_value, e_value);                                                \
            ::th13test::current_failed() = true;                                                                       \
        }                                                                                                              \
    } while (0)
