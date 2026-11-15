#pragma once
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace th128::record {

struct NativeReplayInputContext {
    bool sessionPresent;
    bool managerPresent;
    bool messageAuxiliaryLive;
    float fps;
    std::array<std::uint16_t, 3> input;
};

class NativeReplayInput {
public:
    struct Chunk {
        std::array<std::uint8_t, 5400> input{};
        std::array<std::uint8_t, 900> fps{};
        std::uint32_t packets = 0;
        std::uint32_t samples = 0;
    };

    void beginStageBuffers(unsigned stage) {
        if (stage > 16) throw std::runtime_error("replay input stage outside owned native domain");
        chunks_ = std::vector<Chunk>(1);
        stage_ = stage;
        frame_ = 0;
    }

    bool append(const NativeReplayInputContext& context) {
        if (!context.sessionPresent || !context.managerPresent
            || context.messageAuxiliaryLive || frame_ < 0) return false;
        const bool sample = frame_ % 30 == 0;
        std::uint8_t fpsByte = 0;
        if (sample) {
            if (!std::isfinite(context.fps) || context.fps < 0)
                throw std::runtime_error("sampled replay FPS outside finite owned domain");
            const auto rounded = static_cast<double>(context.fps) + .5;
            fpsByte = rounded >= 256 ? 255 : static_cast<std::uint8_t>(rounded);
        }
        auto& chunk = chunks_.back();
        if (chunk.packets >= 900 || (sample && chunk.samples >= 30))
            throw std::runtime_error("native replay chunk ownership is inconsistent");
        if (sample) chunk.fps[chunk.samples++] = fpsByte;
        const auto offset = std::size_t(chunk.packets) * 6;
        for (std::size_t word = 0; word < context.input.size(); ++word) {
            chunk.input[offset + word * 2] = static_cast<std::uint8_t>(context.input[word]);
            chunk.input[offset + word * 2 + 1] = static_cast<std::uint8_t>(context.input[word] >> 8);
        }
        ++chunk.packets;
        if (chunk.packets == 900) chunks_.emplace_back();
        frame_ = std::bit_cast<std::int32_t>(std::bit_cast<std::uint32_t>(frame_) + 1u);
        return true;
    }

    std::int32_t frame() const noexcept { return frame_; }
    unsigned stage() const noexcept { return stage_; }
    const std::vector<Chunk>& chunks() const noexcept { return chunks_; }

private:
    std::int32_t frame_ = -1;
    unsigned stage_ = 0;
    std::vector<Chunk> chunks_{1};
};

}
