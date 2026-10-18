// Reuse the existing, hash-gated PE mapping and hardware oracle helpers without
// changing their source. Its renamed entry point is never called here.
#define wmain unused_original_cpu_oracle_main
#include "../../tests/native_cpu_compare.cpp"
#undef wmain
#include "bridge.hpp"

// The production DLL resolves its clock global relative to the process EXE.
// This test EXE reserves real writable BSS covering that RVA, so it exercises
// the shipped DLL without a test-only override or altered production binary.
volatile unsigned char g_clock_fixture_padding[0x200000]{};

namespace {
struct AbiSnapshot {
    std::uint32_t eax_value;
    std::uint32_t stack_pointer;
    std::uint32_t ebx_value;
    std::uint32_t esi_value;
    std::uint32_t edi_value;
    std::uint32_t ebp_value;
};
AbiSnapshot g_probe_result{};
std::uint32_t g_probe_stack_before = 0;
std::uint32_t g_probe_call_mode = 0;

// mode 0: thiscall/no stack argument; 1: thiscall/one callee-cleaned argument;
// mode 2: cdecl/one caller-cleaned argument. The function is invoked with the
// original ABI, even though the DLL uses fastcall and an ignored EDX argument.
__declspec(naked) void __cdecl invoke_probe(void*, void*, std::uint32_t, std::uint32_t) {
    __asm {
        pushfd
        pushad
        mov g_probe_stack_before, esp
        mov eax, dword ptr [esp + 40]
        mov ecx, dword ptr [esp + 44]
        mov edx, dword ptr [esp + 52]
        mov g_probe_call_mode, edx
        cmp edx, 0
        je no_argument
        push dword ptr [esp + 48]
    no_argument:
        mov edx, 0a55aa55ah
        mov ebx, 11223344h
        mov esi, 55667788h
        mov edi, 99aabbcch
        mov ebp, 1234fedch
        call eax
        mov dword ptr [g_probe_result + 0], eax
        cmp g_probe_call_mode, 2
        jne no_caller_cleanup
        add esp, 4
    no_caller_cleanup:
        mov dword ptr [g_probe_result + 4], esp
        mov dword ptr [g_probe_result + 8], ebx
        mov dword ptr [g_probe_result + 12], esi
        mov dword ptr [g_probe_result + 16], edi
        mov dword ptr [g_probe_result + 20], ebp
        mov esp, g_probe_stack_before
        popad
        popfd
        ret
    }
}

struct CheckedCall {
    std::uint32_t result;
    bool abi_ok;
    bool fp_control_ok;
};
CheckedCall checked_call(void* function, void* self, std::uint32_t argument, unsigned mode) {
    FloatingEnvironment::prepare();
    invoke_probe(function, self, argument, mode);
    unsigned short x87_after;
    __asm fnstcw x87_after
    const bool abi = g_probe_result.stack_pointer == g_probe_stack_before
        && g_probe_result.ebx_value == 0x11223344 && g_probe_result.esi_value == 0x55667788
        && g_probe_result.edi_value == 0x99aabbcc && g_probe_result.ebp_value == 0x1234fedc;
    // Floating status flags are allowed to change just as in the original.
    return {g_probe_result.eax_value, abi, (_mm_getcsr() & ~0x3fu) == 0x1f80 && x87_after == 0x037f};
}

constexpr std::array<const char*, 7> names{{
    "th20_lcg_step", "th20_lcg_next", "th20_timer_reset", "th20_timer_mode",
    "th20_timer_set", "th20_timer_add", "th20_timer_tick"
}};
constexpr std::array<std::uint32_t, 7> rvas{{0x22cb0, 0x235f0, 0x23f50, 0x23fe0, 0x23f80, 0x530f0, 0x533b0}};
constexpr std::array<unsigned, 7> modes{{2, 0, 0, 1, 1, 1, 0}};
}

int wmain(int argc, wchar_t** argv) {
    HMODULE bridge = nullptr;
    try {
        if (argc != 4) throw std::runtime_error("Usage: th20_bridge_abi_check VERIFIED_TH20.exe BRIDGE.dll OUTPUT.json");
        const std::filesystem::path input(argv[1]), dll_path(argv[2]), output(argv[3]);
        const auto canonical_output = std::filesystem::weakly_canonical(output);
        if (canonical_output == std::filesystem::weakly_canonical(input)
            || canonical_output == std::filesystem::weakly_canonical(dll_path))
            throw std::runtime_error("Report output must differ from binary inputs");
        const auto bytes = th20::read_file(input);
        const auto digest = sha256(bytes);
        if (digest != expected_sha) throw std::runtime_error("Source SHA-256 differs from the verified specimen");
        const auto image = th20::parse_pe(bytes);
        Mapping original(bytes, image);
        mapped_image_base = original.address();
        const auto host_base = reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr));
        const auto fixture_begin = reinterpret_cast<std::uintptr_t>(&g_clock_fixture_padding[0]);
        const auto clock_slot = host_base + th20_clock_pointer_rva;
        if (clock_slot < fixture_begin || clock_slot + 4 > fixture_begin + sizeof(g_clock_fixture_padding))
            throw std::runtime_error("Host clock RVA is not inside the reserved test fixture BSS");
        *reinterpret_cast<const float**>(clock_slot) = nullptr;
        bridge = LoadLibraryExW(std::filesystem::absolute(dll_path).c_str(), nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
        if (!bridge) throw std::runtime_error("Cannot load actual bridge DLL");
        std::array<void*, 7> functions{};
        for (unsigned i = 0; i < functions.size(); ++i) {
            functions[i] = reinterpret_cast<void*>(GetProcAddress(bridge, names[i]));
            if (!functions[i]) throw std::runtime_error(std::string("Missing export: ") + names[i]);
        }
        auto* stats = reinterpret_cast<Th20BridgeStats*>(GetProcAddress(bridge, "g_th20_bridge_stats"));
        if (!stats || stats->magic != th20_bridge_magic || stats->version != 1 || stats->size != sizeof(*stats)
            || stats->source_status != 1 || stats->main_module_base != host_base
            || stats->clock_pointer_rva != th20_clock_pointer_rva)
            throw std::runtime_error("Bridge statistics header or host base is incorrect");
        FloatingEnvironment env;
        std::array<std::uint32_t, 7> counts{};
        std::uint32_t failure_count = 0, abi_failure_count = 0, fp_failure_count = 0;
        std::vector<std::string> failures;
        auto record = [&](unsigned index, const CheckedCall& a, const CheckedCall& b, bool equal) {
            ++counts[index];
            if (!a.abi_ok || !b.abi_ok) ++abi_failure_count;
            if (!a.fp_control_ok || !b.fp_control_ok) ++fp_failure_count;
            if (!equal || !a.abi_ok || !b.abi_ok || !a.fp_control_ok || !b.fp_control_ok) {
                ++failure_count;
                if (failures.size() < 30) {
                    std::ostringstream msg;
                    msg << names[index] << " case " << counts[index] << " memory/return=" << equal
                        << " original_abi=" << a.abi_ok << " bridge_abi=" << b.abi_ok
                        << " original_fp_control=" << a.fp_control_ok << " bridge_fp_control=" << b.fp_control_ok;
                    failures.push_back(msg.str());
                }
            }
        };
        auto original_function = [&](unsigned index) { return reinterpret_cast<void*>(original.address() + rvas[index]); };
        std::mt19937 rng(0x20b11);
        std::vector<std::uint32_t> seeds{0, 1, 2, 48271, 0x7ffffffe, 0x7fffffff, 0x80000000, 0xffffffff};
        for (unsigned i = 0; i < 4096; ++i) seeds.push_back(rng());
        for (auto seed : seeds) {
            auto oa = checked_call(original_function(0), nullptr, seed, modes[0]);
            auto ca = checked_call(functions[0], nullptr, seed, modes[0]);
            record(0, oa, ca, oa.result == ca.result);
            std::uint32_t os = seed, cs = seed;
            oa = checked_call(original_function(1), &os, 0, modes[1]);
            ca = checked_call(functions[1], &cs, 0, modes[1]);
            record(1, oa, ca, oa.result == ca.result && os == cs);
        }
        auto timer_compare = [&](unsigned index, r::Timer initial, std::uint32_t argument, const float* rate) {
            r::Timer ot = initial, ct = initial;
            *reinterpret_cast<const float**>(original.address() + th20_clock_pointer_rva) = rate;
            *reinterpret_cast<const float**>(clock_slot) = rate;
            const auto oa = checked_call(original_function(index), &ot, argument, modes[index]);
            const auto ca = checked_call(functions[index], &ct, argument, modes[index]);
            const bool returns_equal = index == 6 ? oa.result == ca.result
                : oa.result == reinterpret_cast<std::uintptr_t>(&ot) && ca.result == reinterpret_cast<std::uintptr_t>(&ct);
            record(index, oa, ca, returns_equal && std::memcmp(&ot, &ct, sizeof(ot)) == 0);
        };
        const std::array<std::uint32_t, 21> bits{{0, 0x80000000, 1, 0x80000001, 0x007fffff, 0x00800000,
            0x3f800000, 0x3f7d70a4, 0x3f7d70a3, 0x3f7d70a5, 0x3f8147ae, 0x3f8147ad, 0x3f8147af,
            0x7f7fffff, 0xff7fffff, 0x7f800000, 0xff800000, 0x7f800001, 0x7fa12345, 0x7fc01234, 0xffc01234}};
        for (std::uint32_t flags : {0u, 1u, 2u, 3u, 6u, 7u, 0xffffffffu}) {
            for (std::uint32_t value : {0u, 1u, 0xffffffffu, 0x7fffffffu, 0x80000000u}) {
                r::Timer t{-19, r::signed_bits(value), -123.25f, flags};
                timer_compare(2, t, 0, nullptr);
                for (auto mode : {0u, 1u, 2u, 3u, 4u, 0xffffffffu}) timer_compare(3, t, mode, nullptr);
                timer_compare(4, t, value, nullptr);
            }
            for (auto current : bits) {
                for (std::size_t ri = 0; ri <= bits.size(); ++ri) {
                    float rate = 0;
                    if (ri) std::memcpy(&rate, &bits[ri - 1], 4);
                    const float* rate_ptr = ri ? &rate : nullptr;
                    r::Timer t{-41, 7, 0, flags};
                    std::memcpy(&t.current_f, &current, 4);
                    timer_compare(6, t, 0, rate_ptr);
                    for (auto delta : bits) timer_compare(5, t, delta, rate_ptr);
                }
            }
        }
        for (unsigned i = 0; i < 4096; ++i) {
            const std::uint32_t current = rng(), rate_bits = rng(), delta = rng();
            r::Timer t{r::signed_bits(rng()), r::signed_bits(rng()), 0, rng()};
            float rate = 0;
            std::memcpy(&rate, &rate_bits, 4); std::memcpy(&t.current_f, &current, 4);
            timer_compare(2, t, 0, &rate);
            timer_compare(3, t, rng(), &rate);
            timer_compare(4, t, rng(), &rate);
            timer_compare(5, t, delta, &rate);
            timer_compare(6, t, 0, &rate);
        }
        bool counters_ok = true;
        std::uint32_t total = 0;
        for (unsigned i = 0; i < 7; ++i) {
            total += counts[i];
            counters_ok = counters_ok && static_cast<std::uint32_t>(stats->counts[i]) == counts[i];
        }
        const bool passed = failure_count == 0 && counters_ok;
        std::ofstream out(output, std::ios::binary | std::ios::trunc);
        if (!out) throw std::runtime_error("Cannot create ABI report");
        out << "{\n  \"status\":\"" << (passed ? "passed" : "failed") << "\",\n"
            << "  \"scope\":\"Shipped x86 bridge DLL exports versus verified original CPU code, invoked with original ABI; original game entry point never executed\",\n"
            << "  \"source_sha256\":" << th20::json_string(digest) << ",\n"
            << "  \"bridge_sha256\":" << th20::json_string(sha256(th20::read_file(dll_path))) << ",\n"
            << "  \"native_core_sha256\":\"" << TH20_NATIVE_CORE_SHA256 << "\",\n"
            << "  \"total\":" << total << ",\n  \"failures\":" << failure_count
            << ",\n  \"abi_failures\":" << abi_failure_count << ",\n  \"fp_control_failures\":" << fp_failure_count
            << ",\n  \"counter_matches_calls\":" << (counters_ok ? "true" : "false") << ",\n"
            << "  \"preserved_registers_checked\":[\"EBX\",\"ESI\",\"EDI\",\"EBP\"],\n"
            << "  \"stack_balance_checked\":true,\n  \"timer_self_returns_checked\":true,\n"
            << "  \"floating_control\":{\"mxcsr\":\"0x1f80\",\"x87\":\"0x037f\",\"status_flags_compared\":false},\n"
            << "  \"comparisons\":{";
        for (unsigned i = 0; i < 7; ++i) {
            if (i) out << ',';
            out << '\n' << "    " << th20::json_string(names[i]) << ':' << counts[i];
        }
        out << "\n  },\n  \"failure_examples\":[";
        for (std::size_t i = 0; i < failures.size(); ++i) {
            if (i) out << ',';
            out << th20::json_string(failures[i]);
        }
        out << "]\n}\n";
        if (!out) throw std::runtime_error("Cannot finish ABI report");
        FreeLibrary(bridge); bridge = nullptr;
        std::cout << "Bridge ABI/CPU comparisons: " << total << "; failures: " << failure_count
            << "; counter match: " << counters_ok << '\n';
        return passed ? 0 : 1;
    } catch (const std::exception& error) {
        if (bridge) FreeLibrary(bridge);
        std::cerr << "error: " << error.what() << '\n';
        return 2;
    }
}
