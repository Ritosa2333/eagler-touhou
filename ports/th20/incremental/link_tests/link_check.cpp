// Executes the final on-disk patched PE entries through their relocated FF25
// jumps and resolved import slots. No original or rebuilt process entry is run.
#define NOMINMAX
#include <Windows.h>
#include <bcrypt.h>
#include <intrin.h>
#include "th20/binary.hpp"
#include "native_core.hpp"
#include "../native_bridge/bridge.hpp"
#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <sstream>

static_assert(sizeof(void*) == 4);
namespace {
using th20::Bytes;
using Timer = th20::recovered::Timer;
constexpr std::uint32_t preferred_base = 0x400000;
constexpr std::uint32_t original_test_base = 0x30000000;
constexpr std::array<std::uint32_t, 7> rvas{{0x22cb0, 0x235f0, 0x23f50, 0x23fe0, 0x23f80, 0x530f0, 0x533b0}};
constexpr std::array<const char*, 7> names{{"th20_lcg_step", "th20_lcg_next", "th20_timer_reset", "th20_timer_mode", "th20_timer_set", "th20_timer_add", "th20_timer_tick"}};
constexpr const char* source_sha = "a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897";

std::string sha256(const Bytes& bytes) {
    BCRYPT_ALG_HANDLE algorithm = nullptr;
    BCRYPT_HASH_HANDLE hash = nullptr;
    if (BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM, nullptr, 0) < 0)
        throw std::runtime_error("Cannot open SHA-256 provider");
    ULONG size = 0, written = 0;
    const auto queried = BCryptGetProperty(algorithm, BCRYPT_OBJECT_LENGTH, reinterpret_cast<PUCHAR>(&size), sizeof(size), &written, 0);
    Bytes object(size), result(32);
    const auto created = queried < 0 ? queried : BCryptCreateHash(algorithm, &hash, object.data(), size, nullptr, 0, 0);
    const auto updated = created < 0 ? created : BCryptHashData(hash, const_cast<PUCHAR>(bytes.data()), static_cast<ULONG>(bytes.size()), 0);
    const auto finished = updated < 0 ? updated : BCryptFinishHash(hash, result.data(), static_cast<ULONG>(result.size()), 0);
    if (hash) BCryptDestroyHash(hash);
    BCryptCloseAlgorithmProvider(algorithm, 0);
    if (finished < 0) throw std::runtime_error("Cannot compute SHA-256");
    return th20::bytes_hex(result);
}

struct Mapping {
    void* base = nullptr;
    std::uint32_t size = 0;
    std::size_t relocation_count = 0, import_count = 0;
    std::map<std::string, std::uint32_t> bridge_iats;
    std::uint32_t address() const { return reinterpret_cast<std::uint32_t>(base); }
    ~Mapping() { if (base) VirtualFree(base, 0, MEM_RELEASE); }
    Mapping(const Mapping&) = delete;
    Mapping& operator=(const Mapping&) = delete;
    Mapping(const Bytes& bytes, const th20::PeImage& pe, std::uint32_t requested, HMODULE bridge = nullptr) {
        if (pe.pe32_plus || pe.machine != 0x14c || pe.image_base != preferred_base)
            throw std::runtime_error("Expected the PE32 game at preferred base 0x400000");
        size = pe.image_size;
        base = VirtualAlloc(reinterpret_cast<void*>(requested), size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        if (!base || address() != requested) {
            MEMORY_BASIC_INFORMATION info{};
            VirtualQuery(reinterpret_cast<void*>(requested), &info, sizeof(info));
            throw std::runtime_error("Cannot allocate requested test base " + th20::hex(requested)
                + "; allocation=" + th20::hex(reinterpret_cast<std::uint32_t>(info.AllocationBase))
                + "; state=" + th20::hex(info.State) + "; type=" + th20::hex(info.Type));
        }
        try {
            if (pe.headers_size > bytes.size() || pe.headers_size > size) throw std::runtime_error("Invalid mapped header size");
            std::memcpy(base, bytes.data(), pe.headers_size);
            for (const auto& section : pe.sections) {
                if (std::uint64_t(section.virtual_address) + section.raw_size > size) throw std::runtime_error("Section exceeds image");
                if (section.raw_size) std::memcpy(static_cast<unsigned char*>(base) + section.virtual_address, bytes.data() + section.raw_offset, section.raw_size);
            }
            if (address() != preferred_base) {
                const th20::Reader reader(bytes);
                const auto directory = std::size_t(pe.pe_offset) + 24 + 96 + 5 * 8;
                const auto reloc_rva = reader.u32(directory), reloc_size = reader.u32(directory + 4);
                if (!reloc_rva || !reloc_size) throw std::runtime_error("Missing base relocations");
                auto at = pe.rva_to_file(reloc_rva, reloc_size);
                const auto end = at + reloc_size;
                const auto delta = address() - preferred_base;
                while (at < end) {
                    if (end - at < 8) throw std::runtime_error("Truncated relocation block");
                    const auto page = reader.u32(at), block_size = reader.u32(at + 4);
                    if (block_size < 8 || block_size > end - at || block_size % 4) throw std::runtime_error("Invalid relocation block");
                    for (auto entry = at + 8; entry < at + block_size; entry += 2) {
                        const auto word = reader.u16(entry);
                        const auto type = word >> 12;
                        if (!type) continue;
                        if (type != IMAGE_REL_BASED_HIGHLOW) throw std::runtime_error("Unsupported relocation type");
                        const auto target = std::uint64_t(page) + (word & 0xfff);
                        if (target + 4 > size) throw std::runtime_error("Relocation exceeds image");
                        auto* slot = static_cast<unsigned char*>(base) + static_cast<std::size_t>(target);
                        std::uint32_t value; std::memcpy(&value, slot, 4); value += delta; std::memcpy(slot, &value, 4);
                        ++relocation_count;
                    }
                    at += block_size;
                }
            }
            if (bridge) {
                for (const auto& symbol : pe.imports) {
                    if (_stricmp(symbol.module.c_str(), "th20_native_bridge.dll") != 0) continue;
                    const auto target = GetProcAddress(bridge, symbol.by_ordinal ? MAKEINTRESOURCEA(symbol.ordinal_or_hint) : symbol.name.c_str());
                    if (!target || std::uint64_t(symbol.iat_rva) + 4 > size) throw std::runtime_error("Cannot resolve bridge import " + symbol.name);
                    const auto value = reinterpret_cast<std::uint32_t>(target);
                    std::memcpy(static_cast<unsigned char*>(base) + symbol.iat_rva, &value, 4);
                    if (!bridge_iats.emplace(symbol.name, symbol.iat_rva).second) throw std::runtime_error("Duplicate bridge import");
                    ++import_count;
                }
                if (import_count != 7) throw std::runtime_error("Expected exactly seven final PE bridge imports");
                for (std::size_t i = 0; i < rvas.size(); ++i) {
                    const auto* code = static_cast<const unsigned char*>(base) + rvas[i];
                    std::uint32_t operand; std::memcpy(&operand, code + 2, 4);
                    if (code[0] != 0xff || code[1] != 0x25 || operand != address() + bridge_iats.at(names[i]))
                        throw std::runtime_error("Final patched FF25 operand or relocation is wrong: " + std::string(names[i]));
                }
            }
            DWORD previous = 0;
            if (!VirtualProtect(base, size, PAGE_EXECUTE_READWRITE, &previous)) throw std::runtime_error("Cannot enable isolated machine code");
            FlushInstructionCache(GetCurrentProcess(), base, size);
        } catch (...) { VirtualFree(base, 0, MEM_RELEASE); base = nullptr; throw; }
    }
};

struct FloatingEnvironment {
    unsigned int mxcsr = _mm_getcsr();
    unsigned short x87_control = 0;
    FloatingEnvironment() {
        unsigned short saved;
        __asm fnstcw saved
        x87_control = saved;
    }
    ~FloatingEnvironment() {
        const auto saved = x87_control;
        __asm fldcw saved
        _mm_setcsr(mxcsr);
    }
    static void prepare() {
        __asm fninit
        _mm_setcsr(0x1f80);
    }
};
struct Invocation { std::uint32_t eax, before, after, ebx, esi, edi; };
// kind 0: cdecl stack argument; 1: thiscall without stack args;
// kind 2: thiscall with one four-byte stack argument. Raw float bits stay raw.
__declspec(noinline) Invocation invoke(std::uint32_t target, std::uint32_t self, std::uint32_t argument, std::uint32_t kind) {
    std::uint32_t saved_bx, saved_si, saved_di, before, after, actual_bx, actual_si, actual_di, result;
    __asm {
        mov saved_bx, ebx
        mov saved_si, esi
        mov saved_di, edi
        mov before, esp
        mov ebx, 013579bdfh
        mov esi, 02468ace0h
        mov edi, 00badc0deh
        mov ecx, self
        mov eax, target
        cmp kind, 0
        je cdecl_case
        cmp kind, 2
        jne no_argument
        push argument
    no_argument:
        call eax
        jmp completed_call
    cdecl_case:
        push argument
        call eax
        add esp, 4
    completed_call:
        mov result, eax
        mov after, esp
        mov actual_bx, ebx
        mov actual_si, esi
        mov actual_di, edi
        mov esp, before
        mov ebx, saved_bx
        mov esi, saved_si
        mov edi, saved_di
    }
    return {result, before, after, actual_bx, actual_si, actual_di};
}
bool abi_ok(const Invocation& v) { return v.before == v.after && v.ebx == 0x13579bdf && v.esi == 0x2468ace0 && v.edi == 0x0badc0de; }
struct GuardedTimer { std::uint32_t prefix[4]; Timer value; std::uint32_t suffix[4]; };
std::string object_hex(const GuardedTimer& value) {
    const auto* p = reinterpret_cast<const std::uint8_t*>(&value);
    return th20::bytes_hex(Bytes(p, p + sizeof(value)));
}
struct Stats {
    std::map<std::string, std::size_t> counts;
    std::size_t failed = 0;
    std::vector<std::string> failures;
    void record(std::size_t function, bool equal, std::uint32_t base, const Invocation& original, const Invocation& rebuilt,
                std::uint32_t argument, std::uint32_t rate, const std::string& original_object, const std::string& rebuilt_object) {
        ++counts[names[function]];
        if (equal) return;
        ++failed;
        if (failures.size() >= 32) return;
        std::ostringstream out;
        out << "{\"function\":" << th20::json_string(names[function]) << ",\"rebuilt_base\":" << base
            << ",\"argument_bits\":" << argument << ",\"rate_bits\":" << rate
            << ",\"original_eax\":" << original.eax << ",\"rebuilt_eax\":" << rebuilt.eax
            << ",\"original_abi_ok\":" << (abi_ok(original) ? "true" : "false")
            << ",\"rebuilt_abi_ok\":" << (abi_ok(rebuilt) ? "true" : "false")
            << ",\"original_object\":" << th20::json_string(original_object) << ",\"rebuilt_object\":" << th20::json_string(rebuilt_object) << '}';
        failures.push_back(out.str());
    }
};

void compare_mapping(Mapping& original, Mapping& rebuilt, Stats& stats) {
    std::mt19937 random(0x20e11);
    auto compare_rng = [&](std::size_t index, std::uint32_t seed) {
        std::array<std::uint32_t, 3> a{{0xfeedcafe, seed, 0xcafebeef}}, b = a;
        FloatingEnvironment::prepare();
        const auto ra = invoke(original.address() + rvas[index], reinterpret_cast<std::uint32_t>(&a[1]), seed, index ? 1 : 0);
        FloatingEnvironment::prepare();
        const auto rb = invoke(rebuilt.address() + rvas[index], reinterpret_cast<std::uint32_t>(&b[1]), seed, index ? 1 : 0);
        const bool equal = a == b && ra.eax == rb.eax && abi_ok(ra) && abi_ok(rb);
        stats.record(index, equal, rebuilt.address(), ra, rb, seed, 0, th20::hex(a[1]), th20::hex(b[1]));
    };
    const std::array<std::uint32_t, 8> seeds{{0, 1, 2, 48271, 0x7ffffffe, 0x7fffffff, 0x80000000, 0xffffffff}};
    for (auto seed : seeds) { compare_rng(0, seed); compare_rng(1, seed); }
    for (std::size_t i = 0; i < 10000; ++i) { const auto seed = random(); compare_rng(0, seed); compare_rng(1, seed); }
    auto compare_timer = [&](std::size_t index, std::uint32_t current, std::uint32_t flags, std::uint32_t argument,
                             bool null_rate, std::uint32_t rate_bits) {
        GuardedTimer a{}, b{};
        for (auto& value : a.prefix) value = 0x12345678;
        for (auto& value : a.suffix) value = 0x87654321;
        a.value.previous = -123; a.value.current = th20::recovered::signed_bits(current ^ 0x12345678u);
        std::memcpy(&a.value.current_f, &current, 4); a.value.flags = flags; b = a;
        float rate; std::memcpy(&rate, &rate_bits, 4);
        const auto pointer = null_rate ? 0u : reinterpret_cast<std::uint32_t>(&rate);
        std::memcpy(static_cast<unsigned char*>(original.base) + th20_clock_pointer_rva, &pointer, 4);
        std::memcpy(static_cast<unsigned char*>(rebuilt.base) + th20_clock_pointer_rva, &pointer, 4);
        const auto a_self = reinterpret_cast<std::uint32_t>(&a.value), b_self = reinterpret_cast<std::uint32_t>(&b.value);
        const auto kind = index == 2 || index == 6 ? 1u : 2u;
        FloatingEnvironment::prepare();
        const auto ra = invoke(original.address() + rvas[index], a_self, argument, kind);
        FloatingEnvironment::prepare();
        const auto rb = invoke(rebuilt.address() + rvas[index], b_self, argument, kind);
        const bool returns_equal = index == 6 ? ra.eax == rb.eax : ra.eax == a_self && rb.eax == b_self;
        const bool equal = std::memcmp(&a, &b, sizeof(a)) == 0 && returns_equal && abi_ok(ra) && abi_ok(rb);
        stats.record(index, equal, rebuilt.address(), ra, rb, argument, rate_bits, object_hex(a), object_hex(b));
    };
    const std::array<std::uint32_t, 17> patterns{{0, 0x80000000, 1, 0x80000001, 0x007fffff, 0x00800000,
        0x3f800000, 0x3f000000, 0x40000000, 0x7f7fffff, 0xff7fffff, 0x7f800000, 0xff800000,
        0x7f800001, 0x7fa12345, 0x7fc01234, 0xffc01234}};
    const std::array<std::uint32_t, 8> flags{{0, 1, 2, 3, 6, 7, 0xfffffffe, 0xffffffff}};
    for (auto current : patterns) for (auto flag : flags) {
        compare_timer(2, current, flag, 0, true, 0);
        for (auto arg : seeds) { compare_timer(3, current, flag, arg, true, 0); compare_timer(4, current, flag, arg, true, 0); }
        for (std::size_t ri = 0; ri <= patterns.size(); ++ri) {
            const bool null_rate = ri == 0; const auto rate = null_rate ? 0 : patterns[ri - 1];
            compare_timer(6, current, flag, 0, null_rate, rate);
            for (auto delta : patterns) compare_timer(5, current, flag, delta, null_rate, rate);
        }
    }
    for (std::size_t i = 0; i < 3000; ++i) {
        const auto current = random(), flag = random(), argument = random(), rate = random();
        for (std::size_t index = 2; index < 7; ++index) compare_timer(index, current, flag, argument, false, rate);
    }
}
struct BridgeModule {
    HMODULE value = nullptr;
    Th20BridgeStats* stats = nullptr;
    std::uint32_t saved_base = 0;
    explicit BridgeModule(const std::filesystem::path& path) {
        value = LoadLibraryW(path.c_str());
        if (!value) throw std::runtime_error("Cannot load native bridge, Win32 error " + std::to_string(GetLastError()));
        stats = reinterpret_cast<Th20BridgeStats*>(GetProcAddress(value, "g_th20_bridge_stats"));
        if (!stats || stats->magic != th20_bridge_magic || stats->size < sizeof(Th20BridgeStats)) {
            FreeLibrary(value); value = nullptr; throw std::runtime_error("Unexpected bridge stats export");
        }
        saved_base = stats->main_module_base;
    }
    ~BridgeModule() { if (value) { stats->main_module_base = saved_base; FreeLibrary(value); } }
};
struct PreferredReservation {
    void* value = VirtualAlloc(reinterpret_cast<void*>(preferred_base), 0x400000, MEM_RESERVE, PAGE_NOACCESS);
    std::string unavailable_reason;
    PreferredReservation() {
        if (!value) {
            using NameFunction = DWORD(WINAPI*)(HANDLE, LPVOID, LPWSTR, DWORD);
            auto get_name = reinterpret_cast<NameFunction>(GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "K32GetMappedFileNameW"));
            wchar_t name[1024]{};
            if (get_name) get_name(GetCurrentProcess(), reinterpret_cast<void*>(preferred_base), name, 1024);
            std::string narrow;
            for (const auto c : std::wstring(name)) narrow += c <= 0x7f ? static_cast<char>(c) : '?';
            unavailable_reason = "Preferred image region is occupied before test initialization: " + narrow;
        }
    }
    void release() { if (value) { VirtualFree(value, 0, MEM_RELEASE); value = nullptr; } }
    ~PreferredReservation() { release(); }
};
}

int wmain(int argc, wchar_t** argv) {
    try {
        PreferredReservation reservation;
        if (argc != 5) throw std::runtime_error("Usage: th20_playable_link_check ORIGINAL.exe REBUILT.exe BRIDGE.dll OUTPUT.json");
        const auto original_path = std::filesystem::absolute(argv[1]);
        const auto rebuilt_path = std::filesystem::absolute(argv[2]);
        const auto bridge_path = std::filesystem::absolute(argv[3]);
        const auto report_path = std::filesystem::absolute(argv[4]);
        for (const auto& input : {original_path, rebuilt_path, bridge_path})
            if (std::filesystem::weakly_canonical(input) == std::filesystem::weakly_canonical(report_path)
                || (std::filesystem::exists(report_path) && std::filesystem::equivalent(input, report_path)))
                throw std::runtime_error("Report path must differ from all inputs");
        const auto original_bytes = th20::read_file(original_path), rebuilt_bytes = th20::read_file(rebuilt_path), bridge_bytes = th20::read_file(bridge_path);
        const auto digest = sha256(original_bytes);
        if (digest != source_sha) throw std::runtime_error("Original SHA-256 does not match the analyzed specimen");
        const auto original_pe = th20::parse_pe(original_bytes), rebuilt_pe = th20::parse_pe(rebuilt_bytes);
        Mapping original(original_bytes, original_pe, original_test_base);
        BridgeModule bridge(bridge_path);
        FloatingEnvironment environment;
        Stats stats;
        std::vector<std::string> mappings;
        const bool preferred_available = reservation.value != nullptr;
        const std::array<std::uint32_t, 2> test_bases{{preferred_available ? preferred_base : 0x20000000u, 0x24000000u}};
        for (auto test_base : test_bases) {
            if (test_base == preferred_base) reservation.release();
            Mapping rebuilt(rebuilt_bytes, rebuilt_pe, test_base, bridge.value);
            bridge.stats->main_module_base = rebuilt.address();
            const auto failed_before = stats.failed;
            compare_mapping(original, rebuilt, stats);
            std::ostringstream row;
            row << "{\"base\":" << rebuilt.address() << ",\"preferred\":" << (test_base == preferred_base ? "true" : "false")
                << ",\"highlow_relocations_applied\":" << rebuilt.relocation_count << ",\"resolved_bridge_imports\":" << rebuilt.import_count
                << ",\"failures\":" << stats.failed - failed_before << '}';
            mappings.push_back(row.str());
        }
        std::size_t total = 0; for (const auto& item : stats.counts) total += item.second;
        std::ofstream out(report_path, std::ios::binary | std::ios::trunc);
        if (!out) throw std::runtime_error("Cannot create final linkage report");
        out << "{\n  \"schema\":\"th20.playable.link.validation.v1\",\n  \"status\":\"" << (stats.failed ? "failed" : "passed")
            << "\",\n  \"scope\":\"Final on-disk PE seven patched function entries via FF25 and native DLL IAT, original hardware oracle; no process entry point, gameplay, or Windows loader validation\","
            << "\n  \"original_sha256\":" << th20::json_string(digest) << ",\n  \"rebuilt_sha256\":" << th20::json_string(sha256(rebuilt_bytes))
            << ",\n  \"bridge_sha256\":" << th20::json_string(sha256(bridge_bytes))
            << ",\n  \"original_mapped_base\":" << original.address() << ",\n  \"original_highlow_relocations_applied\":" << original.relocation_count
            << ",\n  \"msvc_full_ver\":" << _MSC_FULL_VER << ",\n  \"floating_environment\":{\"mxcsr\":8064,\"x87_control\":895,\"status_flags_compared\":false},"
            << "\n  \"checks\":[\"complete object bytes and surrounding guards\",\"EAX value or self pointer\",\"ESP stack cleanup\",\"EBX/ESI/EDI preservation\",\"seven final FF25 IAT operands\",\"two distinct rebuilt image bases\"],"
            << "\n  \"preferred_base_attempt\":{\"base\":" << preferred_base << ",\"status\":\"" << (preferred_available ? "tested" : "skipped")
            << "\",\"reason\":" << th20::json_string(reservation.unavailable_reason) << "},"
            << "\n  \"mappings\":[";
        for (std::size_t i = 0; i < mappings.size(); ++i) { if (i) out << ','; out << mappings[i]; }
        out << "],\n  \"comparisons\":{";
        bool first = true;
        for (const auto& item : stats.counts) { if (!first) out << ','; first = false; out << th20::json_string(item.first) << ':' << item.second; }
        out << "},\n  \"total\":" << total << ",\n  \"failed\":" << stats.failed << ",\n  \"bridge_call_counters\":[";
        for (std::size_t i = 0; i < rvas.size(); ++i) { if (i) out << ','; out << bridge.stats->counts[i]; }
        out << "],\n  \"failures_first_32\":[";
        for (std::size_t i = 0; i < stats.failures.size(); ++i) { if (i) out << ','; out << stats.failures[i]; }
        out << "]\n}\n";
        if (!out) throw std::runtime_error("Cannot finish final linkage report");
        std::cout << "Final PE linkage comparisons: " << total << "; failures: " << stats.failed << '\n';
        return stats.failed ? 1 : 0;
    } catch (const std::exception& error) { std::cerr << "error: " << error.what() << '\n'; return 2; }
}
