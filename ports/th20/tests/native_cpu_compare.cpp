// Hardware oracle for explicitly selected isolated routines. The original PE's
// entry point is never called and its imports are neither loaded nor resolved.
// This executable is linked at 0x10000000, leaving 0x400000 for the verified PE.
#define NOMINMAX
#include <Windows.h>
#include <bcrypt.h>
#include <intrin.h>
#include "native_core.hpp"
#include "th20/binary.hpp"
#include <array>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <sstream>

namespace r = th20::recovered;
static_assert(sizeof(void*) == 4, "Original code must execute in a 32-bit process");
namespace {
const char* expected_sha = "a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897";
std::uint32_t mapped_image_base = 0x400000;

std::string sha256(const th20::Bytes& bytes) {
    BCRYPT_ALG_HANDLE algorithm = nullptr;
    BCRYPT_HASH_HANDLE hash = nullptr;
    if (BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM, nullptr, 0) < 0)
        throw std::runtime_error("Cannot open SHA-256 provider");
    ULONG size = 0, written = 0;
    if (BCryptGetProperty(algorithm, BCRYPT_OBJECT_LENGTH, reinterpret_cast<PUCHAR>(&size), sizeof(size), &written, 0) < 0) {
        BCryptCloseAlgorithmProvider(algorithm, 0); throw std::runtime_error("Cannot query SHA-256 object");
    }
    th20::Bytes object(size), result(32);
    const auto created = BCryptCreateHash(algorithm, &hash, object.data(), size, nullptr, 0, 0);
    const auto updated = created < 0 ? created : BCryptHashData(hash, const_cast<PUCHAR>(bytes.data()), static_cast<ULONG>(bytes.size()), 0);
    const auto finished = updated < 0 ? updated : BCryptFinishHash(hash, result.data(), static_cast<ULONG>(result.size()), 0);
    if (hash) BCryptDestroyHash(hash);
    BCryptCloseAlgorithmProvider(algorithm, 0);
    if (finished < 0) throw std::runtime_error("Cannot compute SHA-256");
    return th20::bytes_hex(result);
}

struct Mapping {
    void* base = nullptr;
    std::size_t relocation_count = 0;
    std::uint32_t address() const { return reinterpret_cast<std::uint32_t>(base); }
    ~Mapping() { if (base) VirtualFree(base, 0, MEM_RELEASE); }
    Mapping(const th20::Bytes& bytes, const th20::PeImage& pe) {
        if (pe.image_base != 0x400000 || pe.machine != 0x14c || pe.pe32_plus)
            throw std::runtime_error("Expected verified i386 PE at 0x400000");
        base = VirtualAlloc(reinterpret_cast<void*>(0x400000), pe.image_size,
            MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        if (!base) base = VirtualAlloc(nullptr, pe.image_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        if (!base) throw std::runtime_error("Cannot allocate original PE test image");
        auto fail = [&](const char* message) {
            VirtualFree(base, 0, MEM_RELEASE); base = nullptr;
            throw std::runtime_error(message);
        };
        if (pe.headers_size > bytes.size() || pe.headers_size > pe.image_size)
            fail("Invalid original headers mapping size");
        std::memcpy(base, bytes.data(), pe.headers_size);
        for (const auto& section : pe.sections) {
            if (std::uint64_t(section.virtual_address) + section.raw_size > pe.image_size)
                fail("Section exceeds mapped image");
            std::memcpy(static_cast<unsigned char*>(base) + section.virtual_address,
                bytes.data() + section.raw_offset, section.raw_size);
        }
        if (address() != 0x400000) {
            const th20::Reader reader(bytes);
            const auto directory = std::size_t(pe.pe_offset) + 24 + 96 + 5 * 8;
            const auto reloc_rva = reader.u32(directory), reloc_size = reader.u32(directory + 4);
            if (!reloc_rva || !reloc_size) fail("Preferred base unavailable and PE has no relocation table");
            auto at = pe.rva_to_file(reloc_rva, reloc_size);
            const auto end = at + reloc_size;
            const auto delta = address() - 0x400000u;
            while (at < end) {
                if (end - at < 8) fail("Truncated relocation block");
                const auto page = reader.u32(at), block_size = reader.u32(at + 4);
                if (block_size < 8 || block_size > end - at || block_size % 2) fail("Invalid relocation block size");
                for (std::size_t entry = at + 8; entry < at + block_size; entry += 2) {
                    const auto word = reader.u16(entry);
                    const auto type = word >> 12;
                    if (!type) continue;
                    if (type != IMAGE_REL_BASED_HIGHLOW) fail("Unsupported original PE relocation type");
                    const auto target = std::uint64_t(page) + (word & 0xfff);
                    if (target + 4 > pe.image_size) fail("Relocation target exceeds image");
                    auto* slot = static_cast<unsigned char*>(base) + static_cast<std::size_t>(target);
                    std::uint32_t value;
                    std::memcpy(&value, slot, 4); value += delta; std::memcpy(slot, &value, 4);
                    ++relocation_count;
                }
                at += block_size;
            }
        }
        DWORD previous = 0;
        if (!VirtualProtect(base, pe.image_size, PAGE_EXECUTE_READWRITE, &previous))
            fail("Cannot enable mapped selected machine code");
        FlushInstructionCache(GetCurrentProcess(), base, pe.image_size);
    }
    Mapping(const Mapping&) = delete;
    Mapping& operator=(const Mapping&) = delete;
};

struct FloatingEnvironment {
    unsigned int mxcsr;
    unsigned short x87_control;
    FloatingEnvironment() : mxcsr(_mm_getcsr()) {
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
        // Empty x87 stack and masked exceptions, round-to-nearest, extended
        // precision. Status flags are intentionally outside the comparison.
        __asm fninit
        _mm_setcsr(0x1f80);
    }
};

std::uint32_t original_step(std::uint32_t seed) {
    using Function = std::uint32_t(__cdecl*)(std::uint32_t);
    return reinterpret_cast<Function>(mapped_image_base + 0x22cb0)(seed);
}
std::uint32_t original_next(std::uint32_t& seed) {
    using Function = std::uint32_t(__thiscall*)(std::uint32_t*);
    return reinterpret_cast<Function>(mapped_image_base + 0x235f0)(&seed);
}
void original_add(r::Timer& timer, std::uint32_t delta_bits) {
    // The uint32 argument has exactly the original float's stack bytes, without
    // using a C++ floating conversion or an x87 float-return adapter.
    using Function = void(__thiscall*)(r::Timer*, std::uint32_t);
    reinterpret_cast<Function>(mapped_image_base + 0x530f0)(&timer, delta_bits);
}
std::uint32_t original_tick(r::Timer& timer) {
    using Function = std::uint32_t(__thiscall*)(r::Timer*);
    return reinterpret_cast<Function>(mapped_image_base + 0x533b0)(&timer);
}
bool nan_bits(std::uint32_t bits) { return (bits & 0x7f800000) == 0x7f800000 && (bits & 0x007fffff); }
std::uint32_t float_bits(const float& value) { std::uint32_t bits; std::memcpy(&bits, &value, 4); return bits; }
std::string timer_hex(const r::Timer& t) {
    const auto* begin = reinterpret_cast<const std::uint8_t*>(&t);
    return th20::bytes_hex(th20::Bytes(begin, begin + sizeof(t)));
}
std::string cpu_vendor() {
    int data[4]; __cpuid(data, 0);
    char name[13]{};
    std::memcpy(name, &data[1], 4); std::memcpy(name + 4, &data[3], 4); std::memcpy(name + 8, &data[2], 4);
    return name;
}

struct Record {
    std::string function;
    std::uint32_t current_bits = 0, rate_bits = 0, delta_bits = 0;
    bool null_rate = false, tick = false;
    r::Timer original{}, recovered{};
    std::uint32_t original_return = 0, recovered_return = 0;
    bool nan_only = false;
};
void record_json(std::ostream& out, const Record& v) {
    out << "{\"function\":" << th20::json_string(v.function)
        << ",\"current_bits\":" << th20::json_string(th20::hex(v.current_bits))
        << ",\"rate_bits\":" << (v.null_rate ? "null" : th20::json_string(th20::hex(v.rate_bits)))
        << ",\"delta_bits\":" << (v.tick ? "null" : th20::json_string(th20::hex(v.delta_bits)))
        << ",\"original_object\":" << th20::json_string(timer_hex(v.original))
        << ",\"cpp_object\":" << th20::json_string(timer_hex(v.recovered))
        << ",\"original_float_bits\":" << th20::json_string(th20::hex(float_bits(v.original.current_f)))
        << ",\"cpp_float_bits\":" << th20::json_string(th20::hex(float_bits(v.recovered.current_f)))
        << ",\"original_return_bits\":" << v.original_return << ",\"cpp_return_bits\":" << v.recovered_return
        << ",\"nan_payload_only\":" << (v.nan_only ? "true" : "false") << '}';
}
}

int wmain(int argc, wchar_t** argv) {
    try {
        if (argc != 3) throw std::runtime_error("Usage: th20_native_cpu_compare VERIFIED_TH20.exe OUTPUT.json");
        const std::filesystem::path input(argv[1]), output(argv[2]);
        if (std::filesystem::weakly_canonical(input) == std::filesystem::weakly_canonical(output)
            || (std::filesystem::exists(output) && std::filesystem::equivalent(input, output)))
            throw std::runtime_error("Report output must differ from original input");
        const auto bytes = th20::read_file(input);
        const auto digest = sha256(bytes);
        if (digest != expected_sha) throw std::runtime_error("EXE SHA-256 does not match the analyzed specimen");
        const auto image = th20::parse_pe(bytes);
        Mapping mapped(bytes, image);
        mapped_image_base = mapped.address();
        FloatingEnvironment env;
        std::map<std::string, std::size_t> counts;
        std::vector<Record> failures, probes;
        std::size_t failed = 0, nan_only_count = 0, rng_failed = 0;
        auto compare = [&](bool tick, std::uint32_t current, bool null_rate, std::uint32_t rate, std::uint32_t delta) {
            Record v;
            v.function = tick ? "timer_tick" : "timer_add";
            v.current_bits = current; v.rate_bits = rate; v.delta_bits = delta;
            v.tick = tick; v.null_rate = null_rate;
            v.original.previous = -1; v.original.current = 7; v.original.flags = 1;
            std::memcpy(&v.original.current_f, &current, 4); v.recovered = v.original;
            float rate_storage, delta_storage;
            std::memcpy(&rate_storage, &rate, 4); std::memcpy(&delta_storage, &delta, 4);
            const float* rate_pointer = null_rate ? nullptr : &rate_storage;
            *reinterpret_cast<const float**>(mapped_image_base + 0x1aefe0) = rate_pointer;
            FloatingEnvironment::prepare();
            if (tick) v.original_return = original_tick(v.original);
            else original_add(v.original, delta);
            FloatingEnvironment::prepare();
            if (tick) v.recovered_return = static_cast<std::uint32_t>(r::timer_tick(v.recovered, rate_pointer));
            else r::timer_add(v.recovered, delta_storage, rate_pointer);
            ++counts[v.function];
            const bool equal = std::memcmp(&v.original, &v.recovered, 16) == 0 && v.original_return == v.recovered_return;
            if (!equal) {
                ++failed;
                v.nan_only = v.original.previous == v.recovered.previous && v.original.current == v.recovered.current
                    && v.original.flags == v.recovered.flags && v.original_return == v.recovered_return
                    && nan_bits(float_bits(v.original.current_f)) && nan_bits(float_bits(v.recovered.current_f));
                nan_only_count += v.nan_only;
                if (failures.size() < 100) failures.push_back(v);
            }
            // Keep every Timer result so the independent oracle join cannot
            // lose a disputed input through a floating-point classification.
            probes.push_back(v);
        };
        const std::array<std::uint32_t, 15> patterns{{0, 0x80000000, 1, 0x80000001, 0x007fffff, 0x00800000,
            0x3f800000, 0x7f7fffff, 0xff7fffff, 0x7f800000, 0xff800000,
            0x7f800001, 0x7fa12345, 0x7fc01234, 0xffc01234}};
        for (auto current : patterns) for (std::size_t ri = 0; ri <= patterns.size(); ++ri) {
            const bool null_rate = ri == 0;
            const auto rate = null_rate ? 0 : patterns[ri - 1];
            compare(true, current, null_rate, rate, 0);
            for (auto delta : patterns) compare(false, current, null_rate, rate, delta);
        }
        std::mt19937 random(0x20e11);
        for (std::size_t n = 0; n < 3000; ++n) {
            const auto current = random(), rate = random(), delta = random();
            compare(false, current, false, rate, delta); compare(true, current, false, rate, 0);
        }
        std::vector<std::uint32_t> seeds{0, 1, 2, 48271, 0x7ffffffe, 0x7fffffff, 0x80000000, 0xffffffff};
        for (std::size_t n = 0; n < 10000; ++n) seeds.push_back(random());
        for (auto seed : seeds) {
            ++counts["lcg_step"];
            if (original_step(seed) != r::lcg_step(seed)) ++rng_failed;
            auto actual_state = seed, recovered_state = seed;
            ++counts["lcg_next"];
            if (original_next(actual_state) != r::lcg_next(recovered_state) || actual_state != recovered_state) ++rng_failed;
        }
        std::ofstream out(output, std::ios::binary | std::ios::trunc);
        if (!out) throw std::runtime_error("Cannot open report output");
        std::size_t total = 0; for (const auto& pair : counts) total += pair.second;
        out << "{\n  \"status\":\"" << (failed || rng_failed ? "failed" : "passed")
            << "\",\n  \"oracle\":\"Original PE32 machine code executed on local x86 CPU in a dedicated 32-bit test process\","
            << "\n  \"scope\":\"Only selected Timer/RNG functions; original entry point never called; imports not resolved; no gameplay\","
            << "\n  \"source_sha256\":" << th20::json_string(digest) << ",\n  \"native_core_sha256\":\"" << TH20_NATIVE_CORE_SHA256
            << "\",\n  \"cpu_vendor\":" << th20::json_string(cpu_vendor()) << ",\n  \"msvc_full_ver\":" << _MSC_FULL_VER
            << ",\n  \"mapped_original_base\":" << th20::json_string(th20::hex(mapped.address()))
            << ",\n  \"highlow_relocations_applied\":" << mapped.relocation_count
            << ",\n  \"test_preferred_base\":\"0x10000000\","
            << "\n  \"fp_environment\":{\"mxcsr_before_each_call\":\"0x1F80\",\"x87_before_each_call\":\"0x037F\","
            << "\"mxcsr_saved_and_restored\":" << th20::json_string(th20::hex(env.mxcsr))
            << ",\"x87_control_saved_and_restored\":" << th20::json_string(th20::hex(env.x87_control))
            << ",\"status_flags_compared\":false},\n  \"comparisons\":{";
        bool first = true;
        for (const auto& pair : counts) { if (!first) out << ','; first = false; out << th20::json_string(pair.first) << ':' << pair.second; }
        out << "},\n  \"total\":" << total << ",\n  \"timer_failures\":" << failed
            << ",\n  \"timer_nan_payload_only_failures\":" << nan_only_count << ",\n  \"rng_failures\":" << rng_failed
            << ",\n  \"failures_first_100\":[";
        first = true; for (const auto& row : failures) { if (!first) out << ','; first = false; out << '\n'; record_json(out, row); }
        out << "\n  ],\n  \"hardware_probes\":[";
        first = true; for (const auto& row : probes) { if (!first) out << ','; first = false; out << '\n'; record_json(out, row); }
        out << "\n  ]\n}\n";
        if (!out) throw std::runtime_error("Cannot finish report");
        std::cout << "Hardware comparisons: " << total << "; Timer failures: " << failed << "; RNG failures: " << rng_failed << '\n';
        return failed || rng_failed ? 1 : 0;
    } catch (const std::exception& e) { std::cerr << "error: " << e.what() << '\n'; return 2; }
}
