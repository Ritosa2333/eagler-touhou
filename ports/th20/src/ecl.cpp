#include "th20/binary.hpp"
#include <algorithm>
#include <limits>
#include <ostream>

namespace th20 {
namespace {
std::size_t align4(std::size_t at) { return (at + 3) & ~std::size_t(3); }
std::vector<std::string> names(const Reader& r, std::size_t& at, std::size_t end, const char* signature) {
    r.magic(at, signature);
    const auto count = r.u32(at + 4);
    at += 8;
    if (at > end || count > end - at) throw FormatError(at, "String count exceeds available bytes");
    std::vector<std::string> result;
    for (std::uint32_t i = 0; i < count; ++i) result.push_back(r.cstring(at, end));
    at = align4(at);
    if (at > end) throw FormatError(at, "Include alignment exceeds declared length");
    return result;
}
void put16(Bytes& out, std::size_t at, std::uint16_t v) {
    out.at(at) = static_cast<std::uint8_t>(v); out.at(at + 1) = static_cast<std::uint8_t>(v >> 8);
}
void put32(Bytes& out, std::size_t at, std::uint32_t v) {
    put16(out, at, static_cast<std::uint16_t>(v)); put16(out, at + 2, static_cast<std::uint16_t>(v >> 16));
}
void string_array(std::ostream& out, const std::vector<std::string>& values) {
    out << '[';
    for (std::size_t i = 0; i < values.size(); ++i) { if (i) out << ','; out << json_string(values[i]); }
    out << ']';
}
}
std::size_t EclDocument::instruction_count() const {
    std::size_t n = 0; for (const auto& sub : subroutines) n += sub.instructions.size(); return n;
}
std::map<std::uint16_t, std::size_t> EclDocument::opcode_histogram() const {
    std::map<std::uint16_t, std::size_t> result;
    for (const auto& sub : subroutines) for (const auto& ins : sub.instructions) ++result[ins.opcode];
    return result;
}
EclDocument parse_ecl(const Bytes& data) {
    const Reader r(data);
    r.require(0, 36, "SCPT header");
    r.magic(0, "SCPT");
    if (data.size() > std::numeric_limits<std::uint32_t>::max()) throw FormatError(0, "ECL exceeds 32-bit address space");
    EclDocument doc;
    doc.header_marker = r.u16(4);
    if (doc.header_marker != 1) throw FormatError(4, "Unsupported SCPT header marker");
    doc.include_length = r.u16(6); doc.include_offset = r.u32(8);
    const auto count = r.u32(16);
    if (doc.include_offset < 36 || doc.include_offset % 4 != 0)
        throw FormatError(8, "Invalid include offset");
    r.require(doc.include_offset, doc.include_length, "include region");
    const std::size_t include_end = std::size_t(doc.include_offset) + doc.include_length;
    std::size_t at = doc.include_offset;
    doc.animations = names(r, at, include_end, "ANIM");
    doc.includes = names(r, at, include_end, "ECLI");
    if (at != include_end) throw FormatError(at, "Include length does not match ANIM/ECLI lists");
    doc.offsets_table_offset = static_cast<std::uint32_t>(at);
    if (count > (r.size() - at) / 4) throw FormatError(at, "Subroutine offset table exceeds file");
    std::vector<std::uint32_t> offsets;
    for (std::uint32_t i = 0; i < count; ++i) offsets.push_back(r.u32(at + std::size_t(i) * 4));
    at += std::size_t(count) * 4;
    const std::size_t names_end = offsets.empty() ? r.size() : offsets[0];
    if (names_end < at || names_end > r.size()) throw FormatError(at, "Invalid subroutine name table boundary");
    for (std::size_t i = 0; i < offsets.size(); ++i) {
        EclSubroutine sub;
        sub.file_offset = offsets[i];
        sub.end_offset = i + 1 < offsets.size() ? offsets[i + 1] : static_cast<std::uint32_t>(r.size());
        if (sub.file_offset % 4 || sub.file_offset >= sub.end_offset || sub.end_offset > r.size())
            throw FormatError(sub.file_offset, "Invalid or unordered subroutine boundaries");
        sub.name_offset = static_cast<std::uint32_t>(at);
        sub.name = r.cstring(at, names_end);
        if (sub.end_offset - sub.file_offset < 16) throw FormatError(sub.file_offset, "Truncated ECLH header");
        r.magic(sub.file_offset, "ECLH");
        sub.data_offset = r.u32(std::size_t(sub.file_offset) + 4);
        if (sub.data_offset != 16) throw FormatError(std::size_t(sub.file_offset) + 4, "Unsupported ECLH data offset");
        sub.reserved[0] = r.u32(std::size_t(sub.file_offset) + 8);
        sub.reserved[1] = r.u32(std::size_t(sub.file_offset) + 12);
        std::size_t ip = std::size_t(sub.file_offset) + sub.data_offset;
        while (ip < sub.end_offset) {
            if (sub.end_offset - ip < 16) throw FormatError(ip, "Truncated ECL instruction header");
            EclInstruction ins;
            ins.file_offset = static_cast<std::uint32_t>(ip);
            ins.sub_offset = static_cast<std::uint32_t>(ip - sub.file_offset);
            ins.time_raw = r.u32(ip); ins.opcode = r.u16(ip + 4); ins.size = r.u16(ip + 6);
            ins.parameter_mask = r.u16(ip + 8); ins.rank_mask = r.u8(ip + 10); ins.parameter_count = r.u8(ip + 11);
            ins.stack_reference_count = r.u32(ip + 12);
            if (ins.size < 16 || ins.size > sub.end_offset - ip)
                throw FormatError(ip + 6, "Instruction size is zero, too short, or crosses subroutine boundary");
            ins.parameters = r.slice(ip + 16, ins.size - 16);
            sub.instructions.push_back(std::move(ins));
            ip += sub.instructions.back().size;
        }
        doc.subroutines.push_back(std::move(sub));
    }
    if (align4(at) != names_end) throw FormatError(at, "Unexpected data between name table and first subroutine");
    doc.original = data;
    return doc;
}
Bytes serialize_ecl(const EclDocument& doc) {
    // Validate the retained envelope, then rebuild its modelled instruction bytes.
    const auto envelope = parse_ecl(doc.original);
    if (envelope.subroutines.size() != doc.subroutines.size()) throw std::runtime_error("Cannot resize subroutine table during structural serialization");
    Bytes result = doc.original;
    for (std::size_t n = 0; n < doc.subroutines.size(); ++n) {
        const auto& sub = doc.subroutines[n];
        const auto& expected = envelope.subroutines[n];
        if (sub.file_offset != expected.file_offset || sub.end_offset != expected.end_offset || sub.name != expected.name)
            throw std::runtime_error("Cannot relocate or rename subroutines during structural serialization");
        std::size_t ip = std::size_t(sub.file_offset) + 16;
        for (const auto& ins : sub.instructions) {
            if (ins.file_offset != ip || ins.sub_offset != ip - sub.file_offset || ins.size < 16
                || ins.size > sub.end_offset - ip || ins.parameters.size() != std::size_t(ins.size - 16))
                throw FormatError(ip, "Inconsistent instruction model during serialization");
            put32(result, ip, ins.time_raw); put16(result, ip + 4, ins.opcode); put16(result, ip + 6, ins.size);
            put16(result, ip + 8, ins.parameter_mask); result.at(ip + 10) = ins.rank_mask; result.at(ip + 11) = ins.parameter_count;
            put32(result, ip + 12, ins.stack_reference_count);
            std::copy(ins.parameters.begin(), ins.parameters.end(), result.begin() + ip + 16);
            ip += ins.size;
        }
        if (ip != sub.end_offset) throw FormatError(ip, "Instruction sequence does not fill subroutine");
    }
    return result;
}
void write_ecl_json(std::ostream& out, const EclDocument& doc, const std::string& source, bool instructions) {
    out << "{\n  \"schema\":\"th20.ecl.binary.v1\",\n  \"source\":" << json_string(source)
        << ",\n  \"format\":\"SCPT/ECLH (TH10+ container)\",\n  \"semantics\":\"unknown; raw instructions only; not executable C++\","
        << "\n  \"byte_string_encoding\":\"U+0000..U+00FF represents the original byte value\","
        << "\n  \"file_size\":" << doc.original.size() << ",\n  \"header_marker\":" << doc.header_marker
        << ",\n  \"include_offset\":" << doc.include_offset << ",\n  \"include_length\":" << doc.include_length
        << ",\n  \"offsets_table_offset\":" << doc.offsets_table_offset << ",\n  \"animations\":";
    string_array(out, doc.animations); out << ",\n  \"includes\":"; string_array(out, doc.includes);
    out << ",\n  \"subroutine_count\":" << doc.subroutines.size() << ",\n  \"instruction_count\":" << doc.instruction_count()
        << ",\n  \"opcode_histogram\":{";
    bool first = true;
    for (const auto& pair : doc.opcode_histogram()) { if (!first) out << ','; first = false; out << '"' << pair.first << "\":" << pair.second; }
    out << "},\n  \"subroutines\":[";
    for (std::size_t s = 0; s < doc.subroutines.size(); ++s) {
        const auto& sub = doc.subroutines[s];
        if (s) out << ',';
        out << "\n    {\"name\":" << json_string(sub.name) << ",\"name_offset\":" << sub.name_offset
            << ",\"file_offset\":" << sub.file_offset << ",\"file_offset_hex\":" << json_string(hex(sub.file_offset, 8))
            << ",\"end_offset\":" << sub.end_offset << ",\"data_offset\":" << sub.data_offset
            << ",\"reserved\":[" << sub.reserved[0] << ',' << sub.reserved[1] << "]"
            << ",\"instruction_count\":" << sub.instructions.size();
        if (instructions) {
            out << ",\"instructions\":[";
            for (std::size_t i = 0; i < sub.instructions.size(); ++i) {
                const auto& ins = sub.instructions[i];
                if (i) out << ',';
                const auto time_signed = ins.time_raw <= 0x7fffffffU ? std::int64_t(ins.time_raw) : std::int64_t(ins.time_raw) - 0x100000000LL;
                out << "\n      {\"index\":" << i << ",\"file_offset\":" << ins.file_offset
                    << ",\"file_offset_hex\":" << json_string(hex(ins.file_offset, 8)) << ",\"sub_offset\":" << ins.sub_offset
                    << ",\"time_raw_u32\":" << ins.time_raw << ",\"time_as_i32\":" << time_signed
                    << ",\"opcode\":" << ins.opcode << ",\"size\":" << ins.size
                    << ",\"parameter_mask\":" << ins.parameter_mask << ",\"rank_mask\":" << unsigned(ins.rank_mask)
                    << ",\"parameter_count\":" << unsigned(ins.parameter_count)
                    << ",\"stack_reference_count\":" << ins.stack_reference_count
                    << ",\"parameters_file_offset\":" << ins.file_offset + 16
                    << ",\"parameters_hex\":" << json_string(bytes_hex(ins.parameters)) << '}';
            }
            out << ']';
        }
        out << '}';
    }
    out << "\n  ]\n}\n";
}
}
