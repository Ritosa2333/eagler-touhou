#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <vector>

namespace th128::record {

class NativeBstCodec {
    static constexpr std::uint32_t RingSize = 8192;
    static constexpr std::uint32_t RingMask = RingSize - 1;
    static constexpr std::uint32_t Root = RingSize;
    static constexpr std::size_t MaximumBytes = 1048576;

    struct Node {
        std::uint32_t parent = 0;
        std::uint32_t left = 0;
        std::uint32_t right = 0;
    };

    struct Writer {
        std::vector<std::uint8_t> bytes;
        std::uint8_t current = 0;
        std::uint8_t mask = 0x80;

        void put(std::uint32_t value, unsigned width) {
            for (std::uint32_t bit = std::uint32_t{1} << (width - 1); bit; bit >>= 1) {
                if (value & bit) current |= mask;
                mask >>= 1;
                if (!mask) {
                    bytes.push_back(current);
                    current = 0;
                    mask = 0x80;
                }
            }
        }
    };

    std::array<std::uint8_t, RingSize> history_{};
    std::array<Node, RingSize + 1> nodes_{};

    void promote(std::uint32_t node, std::uint32_t child) {
        const auto parent = nodes_[node].parent;
        nodes_[child].parent = parent;
        if (nodes_[parent].right == node) nodes_[parent].right = child;
        else nodes_[parent].left = child;
        nodes_[node].parent = 0;
    }

    void replace(std::uint32_t node, std::uint32_t replacement) {
        const auto parent = nodes_[node].parent;
        if (nodes_[parent].left == node) nodes_[parent].left = replacement;
        else nodes_[parent].right = replacement;
        nodes_[replacement] = nodes_[node];
        nodes_[nodes_[replacement].left].parent = replacement;
        nodes_[nodes_[replacement].right].parent = replacement;
        nodes_[node].parent = 0;
    }

    void erase(std::uint32_t node) {
        if (!nodes_[node].parent) return;
        if (!nodes_[node].right) return promote(node, nodes_[node].left);
        if (!nodes_[node].left) return promote(node, nodes_[node].right);
        auto previous = nodes_[node].left;
        std::uint32_t visits = 0;
        while (nodes_[previous].right) {
            if (++visits > RingSize) throw std::runtime_error("native BST predecessor cycle");
            previous = nodes_[previous].right;
        }
        erase(previous);
        replace(node, previous);
    }

    std::uint32_t insert(std::uint32_t node, std::uint32_t& matchPosition) {
        if (!node) return 0;
        auto current = nodes_[Root].right;
        std::uint32_t best = 0;
        std::uint32_t visits = 0;
        while (true) {
            if (++visits > RingSize || !current)
                throw std::runtime_error("native BST insertion topology");
            std::uint32_t length = 0;
            int difference = 0;
            while (length < 18) {
                difference = int(history_[(node + length) & RingMask])
                    - int(history_[(current + length) & RingMask]);
                if (difference) break;
                ++length;
            }
            if (length >= best) {
                best = length;
                matchPosition = current;
                if (length >= 18) {
                    replace(current, node);
                    return length;
                }
            }
            auto& child = difference < 0 ? nodes_[current].left : nodes_[current].right;
            if (!child) {
                child = node;
                nodes_[node].parent = current;
                nodes_[node].left = 0;
                nodes_[node].right = 0;
                return best;
            }
            current = child;
        }
    }

public:
    static std::vector<std::uint8_t> encode(std::span<const std::uint8_t> input) {
        if (input.empty() || input.size() > MaximumBytes)
            throw std::runtime_error("native BST input size is outside the owned bounded domain");
        NativeBstCodec codec;
        Writer writer;
        writer.bytes.reserve(input.size() * 2 + 2);
        codec.nodes_[Root].right = 1;
        codec.nodes_[1].parent = Root;
        std::size_t source = 0;
        std::uint32_t remaining = 0;
        while (remaining < 18 && source < input.size())
            codec.history_[1 + remaining++] = input[source++];
        std::uint32_t current = 1;
        std::uint32_t matchLength = 0;
        std::uint32_t matchPosition = 0;
        while (remaining) {
            if (matchLength > remaining) matchLength = remaining;
            const auto consumed = matchLength <= 2 ? std::uint32_t{1} : matchLength;
            if (matchLength <= 2) {
                writer.put(1, 1);
                writer.put(codec.history_[current], 8);
            } else {
                writer.put(0, 1);
                writer.put(matchPosition, 13);
                writer.put(matchLength - 3, 4);
            }
            for (std::uint32_t step = 0; step < consumed; ++step) {
                const auto next = (current + 18) & RingMask;
                codec.erase(next);
                if (source < input.size()) codec.history_[next] = input[source++];
                else --remaining;
                current = (current + 1) & RingMask;
                if (remaining) matchLength = codec.insert(current, matchPosition);
            }
        }
        writer.put(0, 14);
        return writer.bytes;
    }

    static std::vector<std::uint8_t> decode(std::span<const std::uint8_t> input,
                                           std::size_t expectedSize) {
        if (input.empty() || input.size() > MaximumBytes * 2 || expectedSize > MaximumBytes)
            throw std::runtime_error("native BST decode size is outside the owned bounded domain");
        std::array<std::uint8_t, RingSize> history{};
        std::vector<std::uint8_t> output;
        output.reserve(expectedSize);
        std::size_t position = 0;
        const auto bits = [&](unsigned width) {
            std::uint32_t value = 0;
            for (unsigned index = 0; index < width; ++index) {
                value <<= 1;
                if (position < input.size() * 8)
                    value |= (input[position / 8] >> (7 - position % 8)) & 1u;
                ++position;
            }
            if (position > input.size() * 8 + 21)
                throw std::runtime_error("native BST input has no bounded terminator");
            return value;
        };
        std::uint32_t destination = 1;
        while (true) {
            if (bits(1)) {
                if (output.size() >= expectedSize)
                    throw std::runtime_error("native BST literal exceeds declared output");
                const auto value = static_cast<std::uint8_t>(bits(8));
                output.push_back(value);
                history[destination] = value;
                destination = (destination + 1) & RingMask;
            } else {
                const auto source = bits(13);
                if (!source) break;
                const auto length = bits(4) + 3;
                if (length > expectedSize - output.size())
                    throw std::runtime_error("native BST match exceeds declared output");
                for (std::uint32_t index = 0; index < length; ++index) {
                    const auto value = history[(source + index) & RingMask];
                    output.push_back(value);
                    history[destination] = value;
                    destination = (destination + 1) & RingMask;
                }
            }
        }
        if (output.size() != expectedSize)
            throw std::runtime_error("native BST output size differs from declared output");
        return output;
    }
};

}
