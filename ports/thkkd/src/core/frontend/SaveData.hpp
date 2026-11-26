// 依据 0x005e50e0、0x005e5930、0x005e5ae0 恢复的存档布局与调用顺序。
#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace kaikeidou {
struct SaveData {
    std::array<std::uint32_t, 9> bindings{};
    std::array<std::uint32_t, 5> options{};
    std::array<std::uint32_t, 5 * 3 * 3 * 7> progress{};
    std::array<std::uint32_t, 5 * 3 * 3> scores{};
    std::array<std::uint32_t, 3 * 3 * 40> extra_scores{};
    std::array<std::uint32_t, 3 * 3 * 160 * 2> spells{};
    std::array<std::uint32_t, 3 * 3 * 40 * 2> extra_spells{};
    static constexpr std::size_t byte_count = 17336;
};

inline constexpr std::array<std::string_view, 5> save_names{{
    "th_kaikeidou01.dat", "th_kaikeidou_score00.dat", "th_kaikeidou_score10.dat",
    "th_kaikeidou_spell01.dat", "th_kaikeidou_spell10.dat"
}};

// 平台层负责文件句柄及音量应用；核心保持逐个四字节元素读写的原顺序。
struct SaveIO {
    virtual ~SaveIO() = default;
    virtual bool open(std::string_view name, bool writing) = 0;
    virtual std::size_t read(std::uint8_t* destination, std::size_t count) = 0;
    virtual void write(const std::uint8_t* source, std::size_t count) = 0;
    virtual void close() = 0;
};
struct SaveVolumes {
    virtual ~SaveVolumes() = default;
    virtual void bgm(std::uint32_t value) = 0;
    virtual void sound(std::uint32_t value) = 0;
};

namespace savedata_detail {
template<std::size_t N> void read_words(SaveIO& io, std::array<std::uint32_t, N>& words) {
    for (auto& word : words) {
        std::array<std::uint8_t, 4> bytes{};
        const auto count = io.read(bytes.data(), bytes.size());
        // 原程序忽略 fread 的返回值，短读覆盖收到的字节并保留其余旧值。
        for (std::size_t i = 0; i < count && i < 4; ++i) {
            const auto shift = static_cast<unsigned>(i * 8);
            word = (word & ~(0xffu << shift)) | (std::uint32_t(bytes[i]) << shift);
        }
    }
}
template<std::size_t N> void write_words(SaveIO& io, const std::array<std::uint32_t, N>& words) {
    for (auto word : words) {
        std::array<std::uint8_t, 4> bytes{};
        for (unsigned i = 0; i < 4; ++i) bytes[i] = static_cast<std::uint8_t>(word >> (i * 8));
        io.write(bytes.data(), bytes.size());
    }
}
template<std::size_t N> void load_array(SaveIO& io, std::string_view name,
    std::array<std::uint32_t, N>& words, std::uint32_t default_value) {
    if (io.open(name, false)) {
        read_words(io, words);
        io.close();
    } else words.fill(default_value);
}
template<std::size_t N> void store_array(SaveIO& io, std::string_view name,
    const std::array<std::uint32_t, N>& words) {
    if (io.open(name, true)) {
        write_words(io, words);
        io.close();
    }
}
}

inline void load_savedata(SaveData& state, SaveIO& io, SaveVolumes& volumes) {
    using namespace savedata_detail;
    if (io.open(save_names[0], false)) {
        read_words(io, state.bindings);
        read_words(io, state.options);
        read_words(io, state.progress);
        io.close();
    } else {
        state.bindings = {{1, 2, 3, 0, 5, 4, 11, 13, 6}};
        state.options = {{0, 100, 80, 1, 0}};
        state.progress.fill(0);
    }
    load_array(io, save_names[1], state.scores, 10000000);
    load_array(io, save_names[2], state.extra_scores, 10000);
    load_array(io, save_names[3], state.spells, 0);
    load_array(io, save_names[4], state.extra_spells, 0);
    volumes.bgm(state.options[1]);
    volumes.sound(state.options[2]);
}

inline void store_spell_records(const SaveData& state, SaveIO& io) {
    savedata_detail::store_array(io, save_names[3], state.spells);
    savedata_detail::store_array(io, save_names[4], state.extra_spells);
}
inline void store_scores_and_settings(const SaveData& state, SaveIO& io) {
    using namespace savedata_detail;
    if (io.open(save_names[0], true)) {
        write_words(io, state.bindings);
        write_words(io, state.options);
        write_words(io, state.progress);
        io.close();
    }
    store_array(io, save_names[1], state.scores);
}
}
