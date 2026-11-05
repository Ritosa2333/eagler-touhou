#pragma once
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <iosfwd>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace th20 {
using Bytes = std::vector<std::uint8_t>;
class FormatError : public std::runtime_error {
public:
    FormatError(std::size_t offset, const std::string& message);
    std::size_t offset() const noexcept { return offset_; }
private:
    std::size_t offset_;
};
Bytes read_file(const std::filesystem::path& path);
void write_file(const std::filesystem::path& path, const Bytes& bytes);
std::string hex(std::uint64_t value, unsigned width = 0);
// JSON byte strings use U+0000..U+00FF without guessing the original text encoding.
std::string json_string(const std::string& bytes);
std::string bytes_hex(const Bytes& bytes);

class Reader {
public:
    explicit Reader(const Bytes& data) : data_(data) {}
    std::size_t size() const { return data_.size(); }
    void require(std::size_t at, std::size_t count, const char* what) const;
    std::uint8_t u8(std::size_t at) const;
    std::uint16_t u16(std::size_t at) const;
    std::uint32_t u32(std::size_t at) const;
    std::uint64_t u64(std::size_t at) const;
    void magic(std::size_t at, const char* expected) const;
    std::string cstring(std::size_t& at, std::size_t end) const;
    Bytes slice(std::size_t at, std::size_t count) const;
private:
    const Bytes& data_;
};

struct EclInstruction {
    std::uint32_t file_offset = 0, sub_offset = 0;
    std::uint32_t time_raw = 0;
    std::uint16_t opcode = 0, size = 0, parameter_mask = 0;
    std::uint8_t rank_mask = 0, parameter_count = 0;
    std::uint32_t stack_reference_count = 0;
    Bytes parameters;
};
struct EclSubroutine {
    std::string name;
    std::uint32_t name_offset = 0, file_offset = 0, end_offset = 0;
    std::uint32_t data_offset = 0;
    std::uint32_t reserved[2]{};
    std::vector<EclInstruction> instructions;
};
struct EclDocument {
    std::uint16_t header_marker = 0, include_length = 0;
    std::uint32_t include_offset = 0, offsets_table_offset = 0;
    std::vector<std::string> animations, includes;
    std::vector<EclSubroutine> subroutines;
    // Retains unmodelled header/padding bytes for lossless structural serialization.
    Bytes original;
    std::size_t instruction_count() const;
    std::map<std::uint16_t, std::size_t> opcode_histogram() const;
};
EclDocument parse_ecl(const Bytes& data);
// Re-encodes every parsed instruction header and payload, preserving untouched
// container bytes. This is a structural round trip, not an ECL runtime/compiler.
Bytes serialize_ecl(const EclDocument& document);
void write_ecl_json(std::ostream& out, const EclDocument& doc, const std::string& source,
                    bool instructions = true);

struct PeSection {
    std::string name;
    std::uint32_t virtual_size = 0, virtual_address = 0;
    std::uint32_t raw_size = 0, raw_offset = 0, characteristics = 0;
};
struct PeImport {
    std::string module, name;
    std::uint16_t ordinal_or_hint = 0;
    bool by_ordinal = false;
    std::uint32_t lookup_rva = 0, iat_rva = 0;
};
struct PeImage {
    std::uint16_t machine = 0, characteristics = 0, subsystem = 0;
    bool pe32_plus = false;
    std::uint32_t pe_offset = 0, timestamp = 0, entry_rva = 0;
    std::uint64_t image_base = 0;
    std::uint32_t image_size = 0, headers_size = 0;
    std::vector<PeSection> sections;
    std::vector<PeImport> imports;
    std::size_t file_size = 0;
    std::size_t rva_to_file(std::uint32_t rva, std::size_t size = 1) const;
};
PeImage parse_pe(const Bytes& data);
void write_pe_json(std::ostream& out, const PeImage& image, const std::string& source);
}
