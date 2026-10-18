#include "th20/binary.hpp"
#include <algorithm>
#include <limits>
#include <ostream>

namespace th20 {
std::size_t PeImage::rva_to_file(std::uint32_t rva, std::size_t count) const {
    if (rva < headers_size && rva < file_size && count <= std::min<std::size_t>(headers_size, file_size) - rva) return rva;
    for (const auto& section : sections) {
        if (rva < section.virtual_address) continue;
        const auto delta = std::uint64_t(rva) - section.virtual_address;
        if (delta >= section.raw_size || count > section.raw_size - delta) continue;
        const auto at = std::uint64_t(section.raw_offset) + delta;
        if (at <= file_size && count <= file_size - at) return static_cast<std::size_t>(at);
    }
    throw FormatError(rva, "RVA has no complete file-backed mapping");
}
PeImage parse_pe(const Bytes& data) {
    const Reader r(data);
    r.require(0, 64, "DOS header");
    if (r.u16(0) != 0x5a4d) throw FormatError(0, "Missing MZ signature");
    PeImage image;
    image.file_size = data.size(); image.pe_offset = r.u32(0x3c);
    const std::size_t pe = image.pe_offset;
    r.require(pe, 24, "PE/COFF header");
    if (r.u32(pe) != 0x00004550) throw FormatError(pe, "Missing PE signature");
    image.machine = r.u16(pe + 4);
    const auto section_count = r.u16(pe + 6);
    image.timestamp = r.u32(pe + 8);
    const auto optional_size = r.u16(pe + 20);
    image.characteristics = r.u16(pe + 22);
    const auto optional = pe + 24;
    r.require(optional, optional_size, "optional header");
    if (optional_size < 2) throw FormatError(optional, "Missing optional header magic");
    const auto signature = r.u16(optional);
    if (signature != 0x10b && signature != 0x20b) throw FormatError(optional, "Unsupported PE optional header");
    image.pe32_plus = signature == 0x20b;
    const std::size_t directories = image.pe32_plus ? 112 : 96;
    if (optional_size < directories) throw FormatError(optional, "Short PE optional header");
    image.entry_rva = r.u32(optional + 16);
    image.image_base = image.pe32_plus ? r.u64(optional + 24) : r.u32(optional + 28);
    image.image_size = r.u32(optional + 56); image.headers_size = r.u32(optional + 60);
    image.subsystem = r.u16(optional + 68);
    const auto directory_count = r.u32(optional + directories - 4);
    if (directory_count > (optional_size - directories) / 8) throw FormatError(optional + directories, "Data directories exceed optional header");
    const auto sections = optional + optional_size;
    r.require(sections, std::size_t(section_count) * 40, "section table");
    for (std::size_t i = 0; i < section_count; ++i) {
        const auto at = sections + i * 40;
        PeSection section;
        for (std::size_t n = 0; n < 8 && r.u8(at + n); ++n) section.name += static_cast<char>(r.u8(at + n));
        section.virtual_size = r.u32(at + 8); section.virtual_address = r.u32(at + 12);
        section.raw_size = r.u32(at + 16); section.raw_offset = r.u32(at + 20); section.characteristics = r.u32(at + 36);
        if (section.raw_size) r.require(section.raw_offset, section.raw_size, "section raw bytes");
        image.sections.push_back(std::move(section));
    }
    if (directory_count < 2) return image;
    const auto imports_rva = r.u32(optional + directories + 8);
    const auto imports_size = r.u32(optional + directories + 12);
    if (!imports_rva && !imports_size) return image;
    if (!imports_rva || imports_size < 20) throw FormatError(optional + directories + 8, "Invalid import directory");
    const auto imports_at = image.rva_to_file(imports_rva, imports_size);
    auto string_at_rva = [&](std::uint32_t rva) {
        auto at = image.rva_to_file(rva);
        const auto start = at;
        auto value = r.cstring(at, std::min<std::size_t>(r.size(), at + 65536));
        image.rva_to_file(rva, at - start);
        return value;
    };
    bool descriptor_terminated = false;
    for (std::size_t delta = 0; delta + 20 <= imports_size; delta += 20) {
        const auto at = imports_at + delta;
        const auto original_thunk = r.u32(at), timestamp = r.u32(at + 4), forwarder = r.u32(at + 8);
        const auto name_rva = r.u32(at + 12), first_thunk = r.u32(at + 16);
        if (!(original_thunk | timestamp | forwarder | name_rva | first_thunk)) { descriptor_terminated = true; break; }
        if (!name_rva || !first_thunk) throw FormatError(at, "Invalid import descriptor");
        const auto module = string_at_rva(name_rva);
        const auto thunk_rva = original_thunk ? original_thunk : first_thunk;
        const std::size_t stride = image.pe32_plus ? 8 : 4;
        const auto ordinal_mask = image.pe32_plus ? 0x8000000000000000ULL : 0x80000000ULL;
        bool thunk_terminated = false;
        for (std::size_t n = 0; n < 1048576; ++n) {
            const auto lookup = std::uint64_t(thunk_rva) + std::uint64_t(n) * stride;
            const auto iat = std::uint64_t(first_thunk) + std::uint64_t(n) * stride;
            if (lookup > 0xffffffffULL || iat > 0xffffffffULL) throw FormatError(at, "Import thunk RVA overflow");
            const auto thunk_at = image.rva_to_file(static_cast<std::uint32_t>(lookup), stride);
            const auto entry = image.pe32_plus ? r.u64(thunk_at) : r.u32(thunk_at);
            if (!entry) { thunk_terminated = true; break; }
            PeImport symbol;
            symbol.module = module; symbol.lookup_rva = static_cast<std::uint32_t>(lookup); symbol.iat_rva = static_cast<std::uint32_t>(iat);
            symbol.by_ordinal = (entry & ordinal_mask) != 0;
            if (symbol.by_ordinal) {
                if (entry & ~(ordinal_mask | 0xffffULL)) throw FormatError(thunk_at, "Invalid ordinal import thunk");
                symbol.ordinal_or_hint = static_cast<std::uint16_t>(entry);
            } else {
                if (entry > 0xfffffffdULL) throw FormatError(thunk_at, "Import name RVA exceeds 32-bit address space");
                const auto hint_at = image.rva_to_file(static_cast<std::uint32_t>(entry), 3);
                symbol.ordinal_or_hint = r.u16(hint_at);
                symbol.name = string_at_rva(static_cast<std::uint32_t>(entry + 2));
            }
            image.imports.push_back(std::move(symbol));
        }
        if (!thunk_terminated) throw FormatError(at, "Import thunk count exceeds inspection limit");
    }
    if (!descriptor_terminated) throw FormatError(imports_at, "Import directory has no null descriptor");
    return image;
}
void write_pe_json(std::ostream& out, const PeImage& image, const std::string& source) {
    out << "{\n  \"schema\":\"th20.pe.binary.v1\",\n  \"source\":" << json_string(source)
        << ",\n  \"file_size\":" << image.file_size << ",\n  \"format\":\"" << (image.pe32_plus ? "PE32+" : "PE32")
        << "\",\n  \"machine\":" << json_string(hex(image.machine, 4)) << ",\n  \"pe_offset\":" << image.pe_offset
        << ",\n  \"timestamp\":" << image.timestamp << ",\n  \"characteristics\":" << image.characteristics
        << ",\n  \"image_base\":" << json_string(hex(image.image_base)) << ",\n  \"entry_rva\":" << json_string(hex(image.entry_rva, 8))
        << ",\n  \"entry_va\":" << json_string(hex(image.image_base + image.entry_rva))
        << ",\n  \"image_size\":" << image.image_size << ",\n  \"headers_size\":" << image.headers_size
        << ",\n  \"subsystem\":" << image.subsystem << ",\n  \"sections\":[";
    for (std::size_t i = 0; i < image.sections.size(); ++i) {
        if (i) out << ',';
        const auto& s = image.sections[i];
        out << "\n    {\"name\":" << json_string(s.name) << ",\"virtual_address\":" << json_string(hex(s.virtual_address, 8))
            << ",\"virtual_size\":" << s.virtual_size << ",\"raw_offset\":" << s.raw_offset
            << ",\"raw_size\":" << s.raw_size << ",\"characteristics\":" << json_string(hex(s.characteristics, 8)) << '}';
    }
    out << "\n  ],\n  \"import_count\":" << image.imports.size() << ",\n  \"imports\":[";
    for (std::size_t i = 0; i < image.imports.size(); ++i) {
        if (i) out << ',';
        const auto& s = image.imports[i];
        out << "\n    {\"module\":" << json_string(s.module) << ",\"name\":" << json_string(s.name)
            << ",\"by_ordinal\":" << (s.by_ordinal ? "true" : "false") << ",\"ordinal_or_hint\":" << s.ordinal_or_hint
            << ",\"lookup_rva\":" << json_string(hex(s.lookup_rva, 8)) << ",\"iat_rva\":" << json_string(hex(s.iat_rva, 8))
            << ",\"iat_va\":" << json_string(hex(image.image_base + s.iat_rva)) << '}';
    }
    out << "\n  ]\n}\n";
}
}
