// Independent literal FNV reference for the interpolation digest wire order.
#include "multiplayer/WorldState.hpp"
#include "multiplayer/DigestSink.hpp"
#include "multiplayer/WorldDigestValues.hpp"
#include "../../harness/test_framework.hpp"

#include <cstdint>
#include <cstring>

using namespace th13;
using namespace th13::multiplayer;

namespace {
struct LiteralFnv {
    uint64_t hash = 0xcbf29ce484222325ull;
    void byte(uint8_t value) { hash = (hash ^ value) * 0x100000001b3ull; }
    void word(uint32_t value) {
        for (unsigned i = 0; i < 4; ++i) byte(static_cast<uint8_t>(value >> (i * 8)));
    }
    void length(uint64_t value) {
        for (unsigned i = 0; i < 8; ++i) byte(static_cast<uint8_t>(value >> (i * 8)));
    }
    template<class T> void field(const T& value) {
        static_assert(sizeof(T) == sizeof(uint32_t));
        uint32_t bits;
        std::memcpy(&bits, &value, sizeof bits);
        word(bits);
    }
    void matrix(const Matrix4& value) {
        length(16);
        for (const auto& element : value.m) field(element);
    }
    template<class T> void interpolation(const T& value) {
        const auto array = [&](const auto& values) {
            length(values.size());
            for (const auto& item : values) field(item);
        };
        array(value.start); array(value.end); array(value.control1);
        array(value.control2); array(value.current);
        field(value.timer.previous); field(value.timer.current); field(value.timer.fractional);
        field(value.timer.rate_index); field(value.timer.flags); field(value.duration); field(value.mode);
    }
};

uint32_t random_word(uint32_t& state) {
    state ^= state << 13; state ^= state >> 17; state ^= state << 5;
    return state;
}

template<class T, class F> void each_field(T& value, F apply) {
    for (auto* array : {&value.start, &value.end, &value.control1, &value.control2, &value.current})
        for (auto& field : *array) apply(&field);
    apply(&value.timer.previous); apply(&value.timer.current); apply(&value.timer.fractional);
    apply(&value.timer.rate_index); apply(&value.timer.flags); apply(&value.duration); apply(&value.mode);
}

template<class T> bool agrees(const T& value, uint32_t& random) {
    DigestSink sink; LiteralFnv ref;
    for (unsigned i = 0; i < 19; ++i) {
        const auto byte = static_cast<uint8_t>(random_word(random));
        sink.u8(byte); ref.byte(byte);
    }
    digest_value(sink, value); ref.interpolation(value);
    if (sink.value() != ref.hash) return false;
    sink.u8(91); ref.byte(91);
    return sink.value() == ref.hash;
}

template<class T> void fields_and_random() {
    uint32_t random = 17;
    T value{};
    CHECK(agrees(value, random));
    LiteralFnv original; original.interpolation(value);
    // Each serialized field is separately altered; bit patterns include -0,
    // NaN, infinities and nonzero integer representations.
    for (const uint32_t bits : {1u, 0x80000000u, 0x7fc00001u, 0x7f800000u, 0xffffffffu, 0x3f800000u}) {
        each_field(value, [&](auto* field) {
            std::memcpy(field, &bits, sizeof bits);
            LiteralFnv changed; changed.interpolation(value);
            CHECK(changed.hash != original.hash);
            CHECK(agrees(value, random));
            const uint32_t zero = 0;
            std::memcpy(field, &zero, sizeof zero);
        });
    }
    for (int i = 0; i < 10000; ++i) {
        each_field(value, [&](auto* field) {
            const uint32_t bits = random_word(random);
            std::memcpy(field, &bits, sizeof bits);
        });
        CHECK(agrees(value, random));
    }
}

template<class T> void prefix_and_sequence() {
    for (unsigned pending : {0u, 1u, 3u, 4u, 7u, 8u, 1023u, 1024u, 1025u, 2047u, 2048u}) {
        for (uint8_t prefix : {uint8_t{0}, uint8_t{1}, uint8_t{255}}) {
            DigestSink sink; LiteralFnv ref;
            sink.u8(prefix); ref.byte(prefix);
            for (unsigned i = 0; i < pending; ++i) { sink.u8(0); ref.byte(0); }
            T zero{}, changed{};
            changed.current.back() = 1;
            for (int i = 0; i < 7; ++i) {
                const T& value = i == 3 ? changed : zero;
                digest_value(sink, value); ref.interpolation(value);
                CHECK_EQ(sink.value(), ref.hash);
            }
            sink.u8(255); ref.byte(255);
            CHECK_EQ(sink.value(), ref.hash);
        }
    }
}
}

TEST(digest_interpolation_exact) {
    fields_and_random<FloatInterpolation<1, false>>();
    fields_and_random<FloatInterpolation<1, true>>();
    fields_and_random<FloatInterpolation<2, false>>();
    fields_and_random<FloatInterpolation<3, false>>();
    fields_and_random<FloatInterpolation<4, false>>();
    fields_and_random<IntegerInterpolation<1>>();
    fields_and_random<IntegerInterpolation<2>>();
    fields_and_random<IntegerInterpolation<3>>();
    fields_and_random<IntegerInterpolation<4>>();
    prefix_and_sequence<FloatInterpolation<1, false>>();
    prefix_and_sequence<FloatInterpolation<1, true>>();
    prefix_and_sequence<FloatInterpolation<2, false>>();
    prefix_and_sequence<FloatInterpolation<3, false>>();
    prefix_and_sequence<FloatInterpolation<4, false>>();
    prefix_and_sequence<IntegerInterpolation<1>>();
    prefix_and_sequence<IntegerInterpolation<2>>();
    prefix_and_sequence<IntegerInterpolation<3>>();
    prefix_and_sequence<IntegerInterpolation<4>>();
}

TEST(digest_matrix_exact) {
    const auto compare = [](const Matrix4& matrix, unsigned pending) {
        DigestSink sink;
        LiteralFnv literal;
        sink.u8(37); literal.byte(37);
        for (unsigned i = 0; i < pending; ++i) { sink.u8(0); literal.byte(0); }
        digest_value(sink, matrix); literal.matrix(matrix);
        CHECK_EQ(sink.value(), literal.hash);
        sink.u8(0); literal.byte(0);
        sink.u8(93); literal.byte(93);
        CHECK_EQ(sink.value(), literal.hash);
    };
    Matrix4 value{};
    for (unsigned pending : {0u, 1u, 7u, 1023u, 1024u, 1025u, 2048u}) compare(value, pending);
    LiteralFnv zero; zero.matrix(value);
    // Every position, including each diagonal, must retain arbitrary bits.
    for (unsigned index = 0; index < 16; ++index) {
        for (const uint32_t bits : {1u, 0x80000000u, 0x7fc00001u, 0x7f800000u,
                                    0xff800000u, 0xffffffffu, 0x3f800000u}) {
            std::memcpy(&value.m[index], &bits, sizeof bits);
            LiteralFnv altered; altered.matrix(value);
            CHECK(altered.hash != zero.hash);
            compare(value, index);
            const uint32_t positive_zero = 0;
            std::memcpy(&value.m[index], &positive_zero, sizeof positive_zero);
        }
    }
    LiteralFnv wrong_length; wrong_length.length(15);
    for (unsigned i = 0; i < 16; ++i) wrong_length.word(0);
    CHECK(wrong_length.hash != zero.hash);
    uint32_t random = 17;
    for (unsigned iteration = 0; iteration < 10000; ++iteration) {
        for (auto& element : value.m) {
            const uint32_t bits = random_word(random);
            std::memcpy(&element, &bits, sizeof bits);
        }
        compare(value, iteration % 19);
        // Exercise arbitrary diagonal values through the sparse path too.
        for (unsigned index : {1u, 2u, 3u, 4u, 6u, 7u, 8u, 9u, 11u, 12u, 13u, 14u}) {
            const uint32_t positive_zero = 0;
            std::memcpy(&value.m[index], &positive_zero, sizeof positive_zero);
        }
        compare(value, iteration % 19);
    }
    DigestSink sequence; LiteralFnv literal;
    Matrix4 diagonal{};
    diagonal.m[0] = diagonal.m[5] = diagonal.m[10] = diagonal.m[15] = 1;
    for (unsigned i = 0; i < 9; ++i) {
        const auto& matrix = i % 2 ? value : diagonal;
        digest_value(sequence, matrix); literal.matrix(matrix);
        CHECK_EQ(sequence.value(), literal.hash);
    }
}

int main() { return th13test::run_all("m6.digest_interpolation_exact"); }
