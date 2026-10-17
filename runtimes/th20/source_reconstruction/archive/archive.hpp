#pragma once
#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <string>
#include <string_view>
#include <vector>

namespace th20::source {
using Bytes = std::vector<std::uint8_t>;
struct CryptParameters {
    std::uint8_t key, step;
    std::uint32_t block, limit;
};

// Original VA 0x004100e0. Operates on the compressed bytes before LZSS.
void decrypt(Bytes& bytes, CryptParameters parameters);
// Original VA 0x00456270 and data table at VA 0x005ae000.
CryptParameters file_crypt_parameters(std::string_view name) noexcept;

class LzssDecoder {
public:
    // Original VA 0x005391f0. Dictionary bytes survive successive calls in
    // the original global at VA 0x005c6b38. Cursor resets to one each stream.
    Bytes decode(const Bytes& compressed, std::uint32_t expected_size);
    void reset() noexcept { dictionary_.fill(0); }
    const auto& dictionary() const noexcept { return dictionary_; }
    // Shared compressor539550 uses this same8192-byte state. Synchronization
    // belongs to the process owner; isolated Archive instances remain local.
    void with_dictionary(const std::function<void(std::array<std::uint8_t,8192>&)>& operation){operation(dictionary_);}
private:
    std::array<std::uint8_t, 8192> dictionary_{};
};

struct ArchiveEntry {
    std::string name;
    std::uint32_t offset = 0, size = 0, stored_size = 0, extra = 0;
};

// An owning, independent C++ API for the archive algorithms recovered from
// 0x00539ed0 / 0x0053a210 / 0x0053a350 / 0x0053a3c0. It deliberately does
// not claim to reproduce the original class/vtable/allocator ABI.
class Archive {
public:
    explicit Archive(const std::filesystem::path& filename);
    explicit Archive(Bytes file);
    Archive(const std::filesystem::path& filename, LzssDecoder& process_dictionary);
    Archive(Bytes file, LzssDecoder& process_dictionary);
    const std::vector<ArchiveEntry>& entries() const noexcept { return entries_; }
    std::uint32_t catalog_offset() const noexcept { return catalog_offset_; }
    std::size_t find(std::string_view name) const;
    Bytes read(std::size_t index);
    Bytes read(std::string_view name) { return read(find(name)); }
private:
    Bytes file_;
    std::ifstream source_;
    std::size_t file_size_ = 0;
    std::vector<ArchiveEntry> entries_;
    LzssDecoder decoder_;
    LzssDecoder* process_dictionary_ = nullptr;
    LzssDecoder& decoder() noexcept { return process_dictionary_ ? *process_dictionary_ : decoder_; }
    std::uint32_t catalog_offset_ = 0;
    void open_file(const std::filesystem::path&);
    Bytes read_range(std::size_t offset, std::size_t size);
    void parse();
};
} // namespace th20::source
