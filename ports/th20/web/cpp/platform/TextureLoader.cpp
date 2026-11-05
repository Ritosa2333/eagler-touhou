#include "TextureLoader.hpp"
#include <cstring>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#define STBI_ONLY_PNG
#define STBI_ONLY_TGA
#define STBI_ONLY_BMP
#define STBI_ONLY_JPEG
#include "third_party/stb_image.h"

namespace th20::web {
namespace {
constexpr std::uint32_t filter_none = 1;
constexpr std::uint32_t filter_triangle_mirror = 0x30004;

int mirror(int index, int size) {
    while (index < 0 || index >= size) index = index < 0 ? -index - 1 : 2 * size - 1 - index;
    return index;
}

// Separable triangle (tent) resample from (sw,sh) to (dw,dh), BGRA channels,
// with mirror addressing at the source image boundary.
void resample_triangle(const std::uint8_t* src, int sw, int sh, int sx0, int sy0, int sx1, int sy1,
                       std::uint8_t* dst, int dw, int dh, int dst_pitch) {
    const int rw = sx1 - sx0, rh = sy1 - sy0;
    std::vector<float> weights;
    std::vector<int> indices;
    weights.resize(std::size_t(dw) * 4);
    indices.resize(std::size_t(dw) * 4);
    std::vector<std::uint8_t> temp(std::size_t(dw) * rh * 4);
    // horizontal pass: rw -> dw for every source row
    for (int x = 0; x < dw; ++x) {
        const float center = (x + 0.5f) * rw / dw - 0.5f;
        const float radius = rw > dw ? float(rw) / dw : 1.0f;
        float total = 0;
        int count = 0;
        const int first = int(center - radius) , last = int(center + radius) + 1;
        for (int j = first; j <= last; ++j) {
            const float w = 1.0f - (j - center < 0 ? center - j : j - center) / radius;
            if (w <= 0) continue;
            indices[x * 4 + count] = j;
            weights[x * 4 + count] = w;
            total += w;
            ++count;
            if (count == 4) break;
        }
        for (int k = count; k < 4; ++k) { indices[x * 4 + k] = first; weights[x * 4 + k] = 0; }
        for (int k = 0; k < 4; ++k) weights[x * 4 + k] /= total;
    }
    for (int y = 0; y < rh; ++y) {
        for (int x = 0; x < dw; ++x) {
            float acc[4]{};
            for (int k = 0; k < 4; ++k) {
                const int j = mirror(indices[x * 4 + k], rw);
                const std::uint8_t* p = src + ((sy0 + y) * sw + sx0 + j) * 4;
                for (int c = 0; c < 4; ++c) acc[c] += p[c] * weights[x * 4 + k];
            }
            std::uint8_t* out = temp.data() + (y * dw + x) * 4;
            for (int c = 0; c < 4; ++c) out[c] = std::uint8_t(acc[c] + 0.5f);
        }
    }
    // vertical pass: rh -> dh for every output row
    for (int y = 0; y < dh; ++y) {
        const float center = (y + 0.5f) * rh / dh - 0.5f;
        const float radius = rh > dh ? float(rh) / dh : 1.0f;
        float total = 0, w[4]{};
        int at[4]{}, count = 0;
        const int first = int(center - radius), last = int(center + radius) + 1;
        for (int j = first; j <= last; ++j) {
            const float weight = 1.0f - (j - center < 0 ? center - j : j - center) / radius;
            if (weight <= 0) continue;
            at[count] = j; w[count] = weight; total += weight;
            if (++count == 4) break;
        }
        for (int k = count; k < 4; ++k) { at[k] = first; w[k] = 0; }
        for (int k = 0; k < 4; ++k) w[k] /= total;
        std::uint8_t* out = dst + y * dst_pitch;
        for (int x = 0; x < dw; ++x) {
            float acc[4]{};
            for (int k = 0; k < 4; ++k) {
                const int j = mirror(at[k], rh);
                const std::uint8_t* p = temp.data() + (j * dw + x) * 4;
                for (int c = 0; c < 4; ++c) acc[c] += p[c] * w[k];
            }
            for (int c = 0; c < 4; ++c) out[x * 4 + c] = std::uint8_t(acc[c] + 0.5f);
        }
    }
}
}

bool decode_image(const std::uint8_t* bytes, std::uint32_t size, DecodedImage& out) {
    int w = 0, h = 0, channels = 0;
    auto* rgba = stbi_load_from_memory(bytes, int(size), &w, &h, &channels, 4);
    if (!rgba || w <= 0 || h <= 0) return false;
    out.width = std::uint32_t(w);
    out.height = std::uint32_t(h);
    out.pixels.resize(std::size_t(w) * h * 4);
    for (std::size_t i = 0, n = out.pixels.size(); i < n; i += 4) {
        out.pixels[i] = rgba[i + 2];     // B
        out.pixels[i + 1] = rgba[i + 1]; // G
        out.pixels[i + 2] = rgba[i];     // R
        out.pixels[i + 3] = rgba[i + 3]; // A
    }
    stbi_image_free(rgba);
    return true;
}

bool load_surface_from_memory(GraphicsDevice& device, Texture* texture,
    const std::int32_t* destination_rect, const std::uint8_t* bytes, std::uint32_t size,
    const std::int32_t* source_rect, std::uint32_t filter) {
    if (!texture || !bytes || !size) return false;
    DecodedImage image;
    if (!decode_image(bytes, size, image)) return false;
    std::int32_t full_source[]{0, 0, std::int32_t(image.width), std::int32_t(image.height)};
    if (!source_rect) source_rect = full_source;
    const auto tw = std::int32_t(device.texture_width(texture));
    const auto th = std::int32_t(device.texture_height(texture));
    // A null destination rectangle is bounded by the surface, not by the
    // encoded PNG. Manual PNGs contain a 1024-wide padded atlas, while their
    // dynamic surface is only 768 wide. FILTER_NONE copies the visible crop.
    std::int32_t full_destination[]{0, 0, tw, th};
    if (!destination_rect) destination_rect = full_destination;
    if (source_rect[0] < 0 || source_rect[1] < 0 ||
        source_rect[2] > std::int32_t(image.width) || source_rect[3] > std::int32_t(image.height) ||
        source_rect[2] <= source_rect[0] || source_rect[3] <= source_rect[1] ||
        destination_rect[0] < 0 || destination_rect[1] < 0 ||
        destination_rect[2] > tw || destination_rect[3] > th ||
        destination_rect[2] <= destination_rect[0] || destination_rect[3] <= destination_rect[1]) return false;
    const auto lock = device.lock(texture);
    if (!lock.pixels) return false;
    bool done = false;
    if (filter == filter_none) {
        const int width = std::min(destination_rect[2] - destination_rect[0], source_rect[2] - source_rect[0]);
        const int height = std::min(destination_rect[3] - destination_rect[1], source_rect[3] - source_rect[1]);
        if (width > 0 && height > 0 &&
            source_rect[0] >= 0 && source_rect[1] >= 0 && source_rect[2] <= (std::int32_t)image.width &&
            source_rect[3] <= (std::int32_t)image.height) {
            for (int y = 0; y < height; ++y) {
                auto* out = static_cast<std::uint8_t*>(lock.pixels) + (destination_rect[1] + y) * lock.pitch + destination_rect[0] * 4;
                const auto* in = image.pixels.data() + ((source_rect[1] + y) * image.width + source_rect[0]) * 4;
                std::memcpy(out, in, width * 4);
            }
            done = true;
        }
    } else if (filter == filter_triangle_mirror) {
        resample_triangle(image.pixels.data(), int(image.width), int(image.height),
                          source_rect[0], source_rect[1], source_rect[2], source_rect[3],
                          static_cast<std::uint8_t*>(lock.pixels) + destination_rect[1] * lock.pitch + destination_rect[0] * 4,
                          destination_rect[2] - destination_rect[0], destination_rect[3] - destination_rect[1],
                          lock.pitch);
        done = true;
    }
    device.unlock(texture);
    return done;
}
}
