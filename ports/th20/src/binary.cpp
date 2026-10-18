#include "th20/binary.hpp"
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>

namespace th20 {
namespace {
std::string path_utf8(const std::filesystem::path& path) {
    const auto encoded = path.u8string();
    return {reinterpret_cast<const char*>(encoded.data()), encoded.size()};
}
}
std::string hex(std::uint64_t value, unsigned width) {
    std::ostringstream out;
    out << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(width) << value;
    return out.str();
}
FormatError::FormatError(std::size_t offset, const std::string& message)
    : std::runtime_error(message + " at " + hex(offset)), offset_(offset) {}
Bytes read_file(const std::filesystem::path& path) {
    std::ifstream in(path, std::ios::binary | std::ios::ate);
    if (!in) throw std::runtime_error("Cannot open input: " + path_utf8(path));
    const auto end = in.tellg();
    if (end < 0 || static_cast<std::uint64_t>(end) > 1024ULL * 1024 * 1024)
        throw std::runtime_error("Input exceeds the 1 GiB inspection limit");
    Bytes data(static_cast<std::size_t>(end));
    in.seekg(0);
    if (!data.empty() && !in.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(data.size())))
        throw std::runtime_error("Cannot read complete input: " + path_utf8(path));
    return data;
}
void write_file(const std::filesystem::path& path, const Bytes& bytes) {
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (!out || (!bytes.empty() && !out.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()))))
        throw std::runtime_error("Cannot write output: " + path_utf8(path));
}
std::string json_string(const std::string& bytes) {
    const char digits[] = "0123456789abcdef";
    std::string result = "\"";
    for (const unsigned char ch : bytes) {
        if (ch == '\\' || ch == '"') { result += '\\'; result += static_cast<char>(ch); }
        else if (ch < 0x20 || ch >= 0x7f) {
            result += "\\u00"; result += digits[ch >> 4]; result += digits[ch & 15];
        } else result += static_cast<char>(ch);
    }
    return result + '"';
}
std::string bytes_hex(const Bytes& bytes) {
    const char digits[] = "0123456789abcdef";
    std::string result;
    result.reserve(bytes.size() * 2);
    for (auto ch : bytes) { result += digits[ch >> 4]; result += digits[ch & 15]; }
    return result;
}
void Reader::require(std::size_t at, std::size_t count, const char* what) const {
    if (at > data_.size() || count > data_.size() - at) throw FormatError(at, std::string("Truncated ") + what);
}
std::uint8_t Reader::u8(std::size_t at) const { require(at, 1, "u8"); return data_[at]; }
std::uint16_t Reader::u16(std::size_t at) const {
    require(at, 2, "u16");
    return static_cast<std::uint16_t>(data_[at] | (std::uint16_t(data_[at + 1]) << 8));
}
std::uint32_t Reader::u32(std::size_t at) const {
    require(at, 4, "u32");
    return std::uint32_t(data_[at]) | (std::uint32_t(data_[at + 1]) << 8)
        | (std::uint32_t(data_[at + 2]) << 16) | (std::uint32_t(data_[at + 3]) << 24);
}
std::uint64_t Reader::u64(std::size_t at) const { return u32(at) | (std::uint64_t(u32(at + 4)) << 32); }
void Reader::magic(std::size_t at, const char* expected) const {
    for (std::size_t i = 0; expected[i]; ++i)
        if (u8(at + i) != static_cast<unsigned char>(expected[i]))
            throw FormatError(at, std::string("Missing signature ") + expected);
}
std::string Reader::cstring(std::size_t& at, std::size_t end) const {
    if (end > size() || at >= end) throw FormatError(at, "Missing string");
    const auto begin = at;
    while (at < end && data_[at] != 0) ++at;
    if (at == end) throw FormatError(begin, "Unterminated string");
    std::string result(data_.begin() + begin, data_.begin() + at);
    ++at;
    return result;
}
Bytes Reader::slice(std::size_t at, std::size_t count) const {
    require(at, count, "byte range");
    return Bytes(data_.begin() + at, data_.begin() + at + count);
}
}
