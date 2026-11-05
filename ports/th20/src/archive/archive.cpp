#include "archive.hpp"
#include <algorithm>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <utility>

namespace th20::source {
namespace {
[[noreturn]] void invalid(const char* message) { throw std::runtime_error(message); }
void require(bool condition, const char* message) { if (!condition) invalid(message); }
std::uint32_t u32(const Bytes& data, std::size_t at) {
    require(at <= data.size() && data.size() - at >= 4, "Truncated archive integer");
    return data[at] | std::uint32_t(data[at + 1]) << 8 |
           std::uint32_t(data[at + 2]) << 16 | std::uint32_t(data[at + 3]) << 24;
}
Bytes slice(const Bytes& bytes, std::size_t at, std::size_t size) {
    require(at <= bytes.size() && size <= bytes.size() - at, "Archive slice is out of bounds");
    return Bytes(bytes.begin() + at, bytes.begin() + at + size);
}
std::uint8_t lower_ascii(std::uint8_t c) noexcept {
    return c >= 'A' && c <= 'Z' ? static_cast<std::uint8_t>(c + 32) : c;
}
bool equal_name(std::string_view a, std::string_view b) noexcept {
    if (a.size() != b.size()) return false;
    for (std::size_t i = 0; i < a.size(); ++i)
        if (lower_ascii(static_cast<std::uint8_t>(a[i])) != lower_ascii(static_cast<std::uint8_t>(b[i])))
            return false;
    return true;
}
class Bits {
public:
    explicit Bits(const Bytes& bytes) : bytes_(bytes) {}
    std::uint32_t get(unsigned count) {
        std::uint32_t value = 0;
        while (count--) {
            value <<= 1;
            if (position_ / 8 < bytes_.size())
                value |= (bytes_[position_ / 8] >> (7 - position_ % 8)) & 1;
            // The original supplies zero bits on exhaustion. These naturally
            // produce the zero-distance terminator, then size is checked here.
            ++position_;
        }
        return value;
    }
private:
    const Bytes& bytes_;
    std::size_t position_ = 0;
};
}

void decrypt(Bytes& bytes, CryptParameters parameters) {
    require(parameters.block > 0 && parameters.block <= 0x7fffffff &&
            parameters.limit <= 0x7fffffff && bytes.size() <= 0x7fffffff,
            "Unsupported archive crypt parameters");
    const auto block = static_cast<std::int64_t>(parameters.block);
    auto remaining = static_cast<std::int64_t>(bytes.size());
    const auto remainder = remaining % block;
    remaining -= (remaining & 1) + (remainder < block / 4 ? remainder : 0);
    auto limit = static_cast<std::int64_t>(parameters.limit);
    // All archive parameter limits are block aligned or encompass the input.
    // Reject the original's possible temporary-buffer overread for other limits.
    require(limit >= static_cast<std::int64_t>(bytes.size()) || limit % block == 0,
            "Non-aligned partial crypt limit is outside the verified domain");
    if (remaining <= 0 || limit <= 0) return;
    const auto source = bytes;
    std::size_t cursor = 0;
    auto key = parameters.key;
    while (remaining > 0 && limit > 0) {
        const auto count = std::min(remaining, block);
        auto input = cursor;
        for (auto output = count - 1; output >= 0; output -= 2) {
            bytes[cursor + static_cast<std::size_t>(output)] = source[input++] ^ key;
            key = static_cast<std::uint8_t>(key + parameters.step);
        }
        for (auto output = count - 2; output >= 0; output -= 2) {
            bytes[cursor + static_cast<std::size_t>(output)] = source[input++] ^ key;
            key = static_cast<std::uint8_t>(key + parameters.step);
        }
        remaining -= count;
        limit -= count;
        cursor += static_cast<std::size_t>(count);
    }
}

CryptParameters file_crypt_parameters(std::string_view name) noexcept {
    // Directly decoded from this specimen's eight 12-byte data records at
    // VA 0x005ae000. Constants are data, not executable machine code.
    static constexpr CryptParameters table[] = {
        {0x1b, 0x73, 0x100, 0x3800}, {0x12, 0x43, 0x200, 0x3e00},
        {0x35, 0x79, 0x400, 0x3c00}, {0x03, 0x91, 0x080, 0x6400},
        {0xab, 0xdc, 0x080, 0x7000}, {0x51, 0x9e, 0x100, 0x4000},
        {0xc1, 0x15, 0x400, 0x2c00}, {0x99, 0x7d, 0x080, 0x4400},
    };
    std::uint8_t sum = 0;
    for (const auto c : name) sum = static_cast<std::uint8_t>(sum + static_cast<std::uint8_t>(c));
    return table[sum & 7];
}

Bytes LzssDecoder::decode(const Bytes& compressed, std::uint32_t expected_size) {
    require(expected_size <= 0x40000000, "Decoded stream exceeds supported size");
    Bytes output;
    output.reserve(expected_size);
    Bits bits(compressed);
    std::uint32_t cursor = 1;
    const auto emit = [&](std::uint8_t byte) {
        require(output.size() < expected_size, "LZSS expands beyond declared size");
        output.push_back(byte);
        dictionary_[cursor] = byte;
        cursor = (cursor + 1) & 0x1fff;
    };
    while (true) {
        if (bits.get(1)) {
            emit(static_cast<std::uint8_t>(bits.get(8)));
        } else {
            const auto offset = bits.get(13);
            if (offset == 0) break;
            const auto count = bits.get(4) + 3;
            for (std::uint32_t i = 0; i < count; ++i) emit(dictionary_[(offset + i) & 0x1fff]);
        }
    }
    require(output.size() == expected_size, "LZSS terminates before declared size");
    return output;
}

Archive::Archive(const std::filesystem::path& filename) { open_file(filename); parse(); }
Archive::Archive(Bytes file) : file_(std::move(file)), file_size_(file_.size()) { parse(); }
Archive::Archive(const std::filesystem::path& filename, LzssDecoder& dictionary) : process_dictionary_(&dictionary) { open_file(filename); parse(); }
Archive::Archive(Bytes file, LzssDecoder& dictionary) : file_(std::move(file)), file_size_(file_.size()), process_dictionary_(&dictionary) { parse(); }
void Archive::open_file(const std::filesystem::path& filename) {
    source_.open(filename, std::ios::binary | std::ios::ate);
    require(bool(source_), "Cannot open archive");
    const auto size = source_.tellg();
    require(size >= 16 && std::uint64_t(size) <= 0x7fffffff, "Unsupported archive size");
    file_size_ = static_cast<std::size_t>(size);
}
Bytes Archive::read_range(std::size_t offset, std::size_t size) {
    require(offset <= file_size_ && size <= file_size_ - offset, "Archive slice is out of bounds");
    if (!source_.is_open()) return slice(file_, offset, size);
    Bytes result(size);
    source_.clear();
    source_.seekg(static_cast<std::streamoff>(offset));
    require(bool(source_.read(reinterpret_cast<char*>(result.data()), static_cast<std::streamsize>(size))), "Cannot read archive member");
    return result;
}
void Archive::parse() {
    require(file_size_ >= 16 && file_size_ <= 0x7fffffff, "Invalid archive size");
    auto header = read_range(0, 16);
    decrypt(header, {0x1b, 0x37, 16, 16});
    require(u32(header, 0) == 0x31414854, "Archive does not have THA1 magic");
    const auto catalog_size = u32(header, 4) - std::uint32_t(123456789);
    const auto stored_catalog_size = u32(header, 8) - std::uint32_t(987654321);
    const auto count = u32(header, 12) - std::uint32_t(135792468);
    require(stored_catalog_size <= file_size_ - 16, "Catalog extends before archive header");
    require(count <= 1000000 && catalog_size <= 0x40000000 &&
            std::uint64_t(count) * 16 <= catalog_size, "Invalid catalog count or size");
    catalog_offset_ = static_cast<std::uint32_t>(file_size_ - stored_catalog_size);
    auto compressed = read_range(catalog_offset_, stored_catalog_size);
    decrypt(compressed, {0x3e, 0x9b, 0x80, stored_catalog_size});
    const auto catalog = decoder().decode(compressed, catalog_size);
    std::size_t cursor = 0;
    for (std::uint32_t i = 0; i < count; ++i) {
        require(cursor < catalog.size(), "Missing catalog entry name");
        const auto begin = cursor;
        while (cursor < catalog.size() && catalog[cursor] != 0) ++cursor;
        require(cursor < catalog.size(), "Unterminated catalog entry name");
        ArchiveEntry entry;
        entry.name.assign(catalog.begin() + begin, catalog.begin() + cursor);
        cursor = (cursor + 4) & ~std::size_t(3);
        entry.offset = u32(catalog, cursor);
        entry.size = u32(catalog, cursor + 4);
        entry.extra = u32(catalog, cursor + 8);
        cursor += 12;
        require(entry.offset >= 16 && entry.offset <= catalog_offset_, "Archive member offset is out of bounds");
        require(entry.size <= 0x40000000, "Archive member exceeds supported decoded size");
        if (!entries_.empty()) {
            require(entry.offset >= entries_.back().offset, "Archive members are not in offset order");
            entries_.back().stored_size = entry.offset - entries_.back().offset;
        }
        entries_.push_back(std::move(entry));
    }
    if (!entries_.empty()) entries_.back().stored_size = catalog_offset_ - entries_.back().offset;
}
std::size_t Archive::find(std::string_view name) const {
    for (std::size_t i = 0; i < entries_.size(); ++i)
        if (equal_name(name, entries_[i].name)) return i;
    throw std::out_of_range("Archive member not found: " + std::string(name));
}
Bytes Archive::read(std::size_t index) {
    const auto& entry = entries_.at(index);
    auto data = read_range(entry.offset, entry.stored_size);
    decrypt(data, file_crypt_parameters(entry.name));
    if (entry.stored_size == entry.size) return data;
    return decoder().decode(data, entry.size);
}
} // namespace th20::source
