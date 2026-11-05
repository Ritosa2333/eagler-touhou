#include "PngWriter.hpp"
#include <cstdio>
#include <cstring>
#include <zlib.h>

namespace th20::web {
namespace {
void store32(std::uint8_t* out, std::uint32_t value) {
    out[0] = std::uint8_t(value >> 24);
    out[1] = std::uint8_t(value >> 16);
    out[2] = std::uint8_t(value >> 8);
    out[3] = std::uint8_t(value);
}
void chunk(std::FILE* file, const char* type, const std::uint8_t* data, std::uint32_t size) {
    std::uint8_t header[8];
    store32(header, size);
    std::memcpy(header + 4, type, 4);
    std::fwrite(header, 1, 8, file);
    if (size) std::fwrite(data, 1, size, file);
    uLong crc = crc32(0L, Z_NULL, 0);
    crc = crc32(crc, reinterpret_cast<const Bytef*>(type), 4);
    if (size) crc = crc32(crc, data, size);
    std::uint8_t tail[4];
    store32(tail, std::uint32_t(crc));
    std::fwrite(tail, 1, 4, file);
}
}

bool stream_png(std::FILE* file, const std::uint8_t* rgba, std::uint32_t width, std::uint32_t height) {
    std::vector<std::uint8_t> raw(std::size_t(height) * (width * 4 + 1));
    for (std::uint32_t y = 0; y < height; ++y) {
        raw[std::size_t(y) * (width * 4 + 1)] = 0;
        std::memcpy(raw.data() + std::size_t(y) * (width * 4 + 1) + 1, rgba + std::size_t(y) * width * 4, width * 4);
    }
    uLongf compressed_size = compressBound(uLong(raw.size()));
    std::vector<std::uint8_t> compressed(compressed_size);
    if (compress2(compressed.data(), &compressed_size, raw.data(), uLong(raw.size()), Z_DEFAULT_COMPRESSION) != Z_OK)
        return false;
    static const std::uint8_t signature[8]{0x89, 'P', 'N', 'G', 0x0d, 0x0a, 0x1a, 0x0a};
    std::fwrite(signature, 1, 8, file);
    std::uint8_t ihdr[13];
    store32(ihdr, width);
    store32(ihdr + 4, height);
    ihdr[8] = 8;  // bit depth
    ihdr[9] = 6;  // RGBA
    ihdr[10] = 0; // deflate
    ihdr[11] = 0; // adaptive filters
    ihdr[12] = 0; // no interlace
    chunk(file, "IHDR", ihdr, 13);
    chunk(file, "IDAT", compressed.data(), std::uint32_t(compressed_size));
    chunk(file, "IEND", nullptr, 0);
    return std::ferror(file) == 0;
}

bool write_png(const char* path, const std::uint8_t* rgba, std::uint32_t width, std::uint32_t height) {
    if (!path || !rgba || !width || !height) return false;
    auto* file = std::fopen(path, "wb");
    if (!file) return false;
    const bool done = stream_png(file, rgba, width, height);
    return std::fclose(file) == 0 && done;
}

bool encode_png(std::vector<std::uint8_t>& out, const std::uint8_t* rgba, std::uint32_t width, std::uint32_t height) {
    out.clear();
    if (!rgba || !width || !height) return false;
    auto* file = std::tmpfile();
    if (!file) return false;
    const bool done = stream_png(file, rgba, width, height);
    if (done) {
        std::fseek(file, 0, SEEK_END);
        const auto size = std::ftell(file);
        std::fseek(file, 0, SEEK_SET);
        out.resize(size > 0 ? std::size_t(size) : 0);
        if (!out.empty()) std::fread(out.data(), 1, out.size(), file);
    }
    std::fclose(file);
    return done && !out.empty();
}
}
