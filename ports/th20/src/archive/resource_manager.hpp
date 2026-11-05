#pragma once
#include "archive.hpp"
#include <memory>
#include <optional>
namespace th20::source::resources {
// Source-owned replacement for the global ArcMngr lifecycle, with a single
// archive and the original process-lifetime LZSS dictionary. The C++ ownership
// API intentionally replaces the old 16-byte class/FILE-vtable ABI.
class Manager {
public:
    bool open(const std::filesystem::path&);    // 0x53a6f0/0x539ed0
    void close() noexcept;                    // 0x53a170, dictionary survives
    std::uint32_t size(std::string_view) const;// 0x53a620
    std::optional<Bytes> read(std::string_view);// 0x53a3c0
    Bytes decode(const Bytes& compressed,std::uint32_t expected_size); //5391f0, same dictionary as archive reads
    void with_dictionary(const std::function<void(std::array<std::uint8_t,8192>&)>&);
    std::size_t entry_count() const noexcept;
    const std::string& error() const noexcept {return error_;}
private:
    LzssDecoder dictionary_;
    std::unique_ptr<Archive> archive_;
    std::filesystem::path path_;
    std::string error_;
};
Manager& manager() noexcept;
bool open(const std::filesystem::path&);
void close() noexcept;
// Original 0x410aa0 selection: loose_only=false searches ONLY the archive;
// there is no filesystem fallback for a missing archive member.
std::optional<Bytes> read(const char* name,bool loose_only=false);
Bytes decode_shared(const Bytes& compressed,std::uint32_t expected_size); //process dictionary under shared file lock2
void with_shared_dictionary(const std::function<void(std::array<std::uint8_t,8192>&)>&); //539550 compressor shares decode storage
std::string_view archive_lookup_name(std::string_view) noexcept;
}
