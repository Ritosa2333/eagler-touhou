#include "th20/binary.hpp"
#include <functional>
#include <iostream>
#include <sstream>

namespace {
int failures = 0;
void check(bool ok, const char* what) {
    if (!ok) { std::cerr << "FAIL: " << what << '\n'; ++failures; }
}
void rejects(const std::function<void()>& action, const char* what) {
    try { action(); check(false, what); } catch (const th20::FormatError&) {}
}
void put(th20::Bytes& out, std::size_t at, std::uint64_t value, unsigned width = 4) {
    for (unsigned i = 0; i < width; ++i) out.at(at + i) = static_cast<std::uint8_t>(value >> (i * 8));
}
void str(th20::Bytes& out, std::size_t at, const std::string& value) {
    for (auto ch : value) out.at(at++) = static_cast<std::uint8_t>(ch);
}
th20::Bytes ecl_fixture() {
    th20::Bytes b(116);
    str(b, 0, "SCPT"); put(b, 4, 1, 2); put(b, 6, 16, 2); put(b, 8, 36); put(b, 16, 1);
    str(b, 36, "ANIM"); str(b, 44, "ECLI"); put(b, 52, 64); str(b, 56, "Main");
    str(b, 64, "ECLH"); put(b, 68, 16);
    put(b, 80, 0xffffffff); put(b, 84, 65530, 2); put(b, 86, 20, 2);
    put(b, 88, 1, 2); b[90] = 0xf2; b[91] = 1; put(b, 92, 1); put(b, 96, 0x3f800000);
    put(b, 100, 60); put(b, 104, 10, 2); put(b, 106, 16, 2); b[110] = 0xff;
    return b;
}
th20::Bytes pe_fixture(bool plus) {
    th20::Bytes b(1024);
    str(b, 0, "MZ"); put(b, 0x3c, 0x80); str(b, 0x80, "PE");
    put(b, 0x84, plus ? 0x8664 : 0x14c, 2); put(b, 0x86, 1, 2);
    put(b, 0x94, plus ? 240 : 224, 2); put(b, 0x96, 0x102, 2);
    const std::size_t opt = 0x98;
    put(b, opt, plus ? 0x20b : 0x10b, 2); put(b, opt + 16, 0x1000);
    put(b, opt + (plus ? 24 : 28), plus ? 0x140000000ULL : 0x400000ULL, plus ? 8 : 4);
    put(b, opt + 56, 0x2000); put(b, opt + 60, 0x200); put(b, opt + 68, 2, 2);
    const auto dirs = opt + (plus ? 112 : 96);
    put(b, dirs - 4, 16); put(b, dirs + 8, 0x1020); put(b, dirs + 12, 40);
    const auto section = opt + (plus ? 240 : 224);
    str(b, section, ".text"); put(b, section + 8, 0x400); put(b, section + 12, 0x1000);
    put(b, section + 16, 0x200); put(b, section + 20, 0x200); put(b, section + 36, 0x60000020);
    put(b, 0x220, 0x1060); put(b, 0x22c, 0x1090); put(b, 0x230, 0x1080);
    put(b, 0x260, 0x10a0, plus ? 8 : 4);
    put(b, plus ? 0x268 : 0x264, plus ? 0x8000000000000007ULL : 0x80000007ULL, plus ? 8 : 4);
    str(b, 0x290, "TEST.dll"); put(b, 0x2a0, 3, 2); str(b, 0x2a2, "Example");
    return b;
}
}
int main() {
    auto bytes = ecl_fixture();
    auto ecl = th20::parse_ecl(bytes);
    check(ecl.subroutines.size() == 1 && ecl.subroutines[0].name == "Main", "ECL name table");
    check(ecl.instruction_count() == 2, "ECL instruction boundary walk");
    const auto& ins = ecl.subroutines[0].instructions[0];
    check(ins.file_offset == 80 && ins.sub_offset == 16, "ECL absolute and relative addresses");
    check(ins.opcode == 65530 && ins.time_raw == 0xffffffff && ins.parameter_mask == 1 && ins.rank_mask == 0xf2, "Unknown ECL opcode and raw fields preserved");
    check(th20::bytes_hex(ins.parameters) == "0000803f", "ECL raw payload ordering");
    check(th20::serialize_ecl(ecl) == bytes, "ECL structural round trip");
    auto edited = ecl;
    edited.subroutines[0].instructions[0].parameters[2] = 0;
    const auto edited_bytes = th20::serialize_ecl(edited);
    check(edited_bytes != bytes && edited_bytes[98] == 0, "Serializer uses parsed instruction payload");
    edited = ecl;
    edited.subroutines[0].instructions[0].time_raw = 17;
    check(th20::parse_ecl(th20::serialize_ecl(edited)).subroutines[0].instructions[0].time_raw == 17, "Serializer uses parsed instruction header");
    std::ostringstream json;
    th20::write_ecl_json(json, ecl, "test.ecl");
    check(json.str().find("\"time_as_i32\":-1") != std::string::npos, "ECL signed time view");
    check(json.str().find("\"parameters_hex\":\"0000803f\"") != std::string::npos, "ECL JSON payload");
    rejects([&] { th20::parse_ecl({}); }, "Empty ECL rejected");
    auto corrupt = bytes; put(corrupt, 86, 0, 2);
    rejects([&] { th20::parse_ecl(corrupt); }, "Zero-sized ECL instruction rejected");
    corrupt = bytes; put(corrupt, 86, 1000, 2);
    rejects([&] { th20::parse_ecl(corrupt); }, "Crossing ECL instruction rejected");
    corrupt = bytes; put(corrupt, 16, 0xffffffff);
    rejects([&] { th20::parse_ecl(corrupt); }, "Overflowing ECL offset table rejected");
    corrupt = bytes; put(corrupt, 52, 4);
    rejects([&] { th20::parse_ecl(corrupt); }, "Backwards ECL subroutine rejected");
    corrupt = bytes; put(corrupt, 68, 12);
    rejects([&] { th20::parse_ecl(corrupt); }, "Unsupported ECLH header rejected");
    corrupt = bytes; for (std::size_t n = 56; n < 64; ++n) corrupt[n] = 'A';
    rejects([&] { th20::parse_ecl(corrupt); }, "Unterminated ECL name rejected");
    // Every single-bit change either fails safely or remains losslessly serializable.
    for (std::size_t n = 0; n < bytes.size(); ++n) for (unsigned bit = 0; bit < 8; ++bit) {
        corrupt = bytes; corrupt[n] ^= static_cast<std::uint8_t>(1U << bit);
        try { const auto mutated = th20::parse_ecl(corrupt); check(th20::serialize_ecl(mutated) == corrupt, "Mutated ECL lossless parse"); }
        catch (const th20::FormatError&) {}
    }
    for (const bool plus : {false, true}) {
        auto pebytes = pe_fixture(plus);
        const auto pe = th20::parse_pe(pebytes);
        check(pe.pe32_plus == plus && pe.sections.size() == 1, "PE32/PE32+ header parsing");
        check(pe.rva_to_file(0x1000) == 0x200 && pe.rva_to_file(0x1080) == 0x280, "PE file-backed RVA mapping");
        check(pe.imports.size() == 2 && pe.imports[0].module == "TEST.dll" && pe.imports[0].name == "Example", "PE named import");
        check(pe.imports[1].by_ordinal && pe.imports[1].ordinal_or_hint == 7, "PE ordinal import");
        rejects([&] { pe.rva_to_file(0x1200); }, "Virtual-only PE bytes rejected");
        rejects([&] { pe.rva_to_file(0x11ff, 2); }, "Cross-section PE range rejected");
        pebytes.pop_back(); rejects([&] { th20::parse_pe(pebytes); }, "Truncated PE section rejected");
        pebytes = pe_fixture(plus); pebytes[0x82] = 1;
        rejects([&] { th20::parse_pe(pebytes); }, "PE signature nulls validated");
    }
    check(th20::json_string(std::string("\"\\\n\xff", 4)) == "\"\\\"\\\\\\u000a\\u00ff\"", "Byte-preserving JSON escape");
    if (!failures) std::cout << "All binary parser checks passed (including 928 ECL bit mutations).\n";
    return failures ? 1 : 0;
}
