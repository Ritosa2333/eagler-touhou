// Test-only mapping of hash-verified original routines. No game startup runs.
#define wmain unused_native_oracle_main
#include "../../tests/native_cpu_compare.cpp"
#undef wmain
#include "configuration.hpp"
#include "clock.hpp"
#include <float.h>
#include <limits>
namespace p = th20::source::platform;
namespace {
std::uint64_t sampled_counter;
DWORD sampled_milliseconds;
BOOL WINAPI counter_sample(LARGE_INTEGER* out) { std::memcpy(out, &sampled_counter, 8); return TRUE; }
DWORD WINAPI milliseconds_sample() { return sampled_milliseconds; }
template<class T> T& global(std::uint32_t address) { return *reinterpret_cast<T*>(mapped_image_base + address - 0x400000); }
std::uint64_t bits(double value) { std::uint64_t result; std::memcpy(&result,&value,8); return result; }
double original_conversion(std::uint64_t value) {
    const auto target = mapped_image_base + 0x142e00;
    const auto low_part = std::uint32_t(value), high_part = std::uint32_t(value >> 32);
    double result;
    __asm { mov ecx, low_part }
    __asm { mov edx, high_part }
    __asm { call target }
    __asm { movsd result, xmm0 }
    return result;
}
unsigned short x87_control() {
    unsigned short result;
    __asm { fnstcw result }
    return result;
}
void prepare(unsigned rc = 0) {
    FloatingEnvironment::prepare();
    unsigned x87, sse; __control87_2(rc, _MCW_RC, &x87, &sse);
}
}
int wmain(int argc, wchar_t** argv) {
    try {
        if (argc != 3) throw std::runtime_error("Usage: th20_platform_cpu_compare VERIFIED_TH20.exe OUTPUT.json");
        const std::filesystem::path source(argv[1]), report(argv[2]);
        if (std::filesystem::weakly_canonical(source) == std::filesystem::weakly_canonical(report))
            throw std::runtime_error("Report may not replace original EXE");
        auto bytes = th20::read_file(source); const auto digest = sha256(bytes);
        if (digest != expected_sha) throw std::runtime_error("Original SHA-256 mismatch");
        auto pe = th20::parse_pe(bytes); Mapping mapping(bytes, pe); mapped_image_base = mapping.address();
        FloatingEnvironment restore;
        global<std::uint8_t>(0x5b2610) |= 0x20; // Select the original SSE2 implementation.
        global<std::uint32_t>(0x5e5288) = 1;   // Original CRT detects SSE2 on this x86 CPU.
        auto* mutex = &global<std::uint32_t>(0x5c0240 + 5 * 48);
        std::memset(mutex, 0, 48); mutex[0] = 0x101; mutex[10] = GetCurrentThreadId(); mutex[11] = 1;
        unsigned resolved = 0;
        for (auto& item : pe.imports) {
            std::uintptr_t function = 0;
            if (item.name == "GetCurrentThreadId") function = reinterpret_cast<std::uintptr_t>(&GetCurrentThreadId);
            if (item.name == "QueryPerformanceCounter") function = reinterpret_cast<std::uintptr_t>(&counter_sample);
            if (item.name == "timeGetTime") function = reinterpret_cast<std::uintptr_t>(&milliseconds_sample);
            if (function) { *reinterpret_cast<std::uintptr_t*>(mapped_image_base + item.iat_rva) = function; ++resolved; }
        }
        if (resolved != 3) throw std::runtime_error("Clock oracle imports did not resolve exactly");
        std::map<std::string,std::size_t> counts;
        std::vector<std::string> failures; std::size_t failed = 0;
        auto check = [&](const char* label, bool equal) {
            ++counts[label]; if (!equal) { ++failed; if(failures.size()<24) failures.push_back(std::string(label)+" case "+std::to_string(counts[label])); }
        };
        std::mt19937 random(0x41cb10);
        auto random64 = [&] { return (std::uint64_t(random()) << 32) | random(); };
        for (unsigned i = 0; i < 2048; ++i) {
            p::Configuration a, b;
            for (auto* at = reinterpret_cast<unsigned char*>(&a); at != reinterpret_cast<unsigned char*>(&a)+sizeof(a); ++at) *at = static_cast<unsigned char>(random());
            b = a;
            using Constructor = void*(__thiscall*)(void*);
            auto* returned = reinterpret_cast<Constructor>(mapped_image_base+0xb9c10)(&a);
            p::initialize_configuration(b);
            check("configuration_all_176_bytes", returned == &a && std::memcmp(&a,&b,sizeof(a))==0);
            p::KeyBindings x{}, y{};
            returned = reinterpret_cast<Constructor>(mapped_image_base+0x1fb10)(&x); p::initialize_bindings(y);
            check("key_bindings_all_48_bytes", returned == &x && std::memcmp(&x,&y,sizeof(x))==0);
            std::uint32_t f = random(), g=f;
            returned = reinterpret_cast<Constructor>(mapped_image_base+0xb9b80)(&f); p::initialize_configuration_flags(g);
            check("flags_preserve_high_bits", returned == &f && f==g);
        }
        using Clock = double(__thiscall*)(void*);
        auto original_clock = reinterpret_cast<Clock>(mapped_image_base+0x1cb10);
        for (unsigned rc : {0u,0x100u,0x200u,0x300u}) {
            for (unsigned i=0;i<5000;++i) {
                const auto value = random64(); prepare(rc);
                const auto a = bits(original_conversion(value)); prepare(rc);
                const auto b = bits(p::signed_counter_to_double(value)); check("signed_counter_conversion",a==b);
                sampled_counter = random64(); const auto origin = random64();
                const auto frequency = (i % 2 ? (random64() | 1) : std::uint64_t(10000000));
                double offset = i%3 ? double(std::int32_t(random()))/1000.0 : 0.0;
                global<std::uint64_t>(0x5b67d0)=frequency; global<std::uint64_t>(0x5b67d8)=origin;
                global<double>(0x5b8838)=offset; prepare(rc);
                const auto result_a = bits(original_clock(nullptr)), offset_a=bits(global<double>(0x5b8838));
                prepare(rc); const auto result_b=bits(p::performance_clock(sampled_counter,origin,frequency,offset));
                check("performance_clock_result_and_offset",result_a==result_b && offset_a==bits(offset));
                sampled_milliseconds=random(); offset= i%3 ? double(std::int32_t(random()))/1000.0 : 0.0;
                global<std::uint64_t>(0x5b67d0)=0; global<double>(0x5b8838)=offset; prepare(rc);
                const auto tick_a=bits(original_clock(nullptr)), tick_offset=bits(global<double>(0x5b8838));
                prepare(rc); const auto tick_b=bits(p::multimedia_clock(sampled_milliseconds,offset));
                check("multimedia_clock_result_and_offset",tick_a==tick_b && tick_offset==bits(offset));
            }
        }
        using SetMode = bool(__cdecl*)(std::uint32_t);
        auto original_round = reinterpret_cast<SetMode>(mapped_image_base+0x14a4e0);
        for (unsigned initial : {0u,0x100u,0x200u,0x300u}) for(unsigned mode : {0u,0x100u,0x200u,0x300u,1u,0xffffffffu,0x400u}) {
            for(unsigned daz : {0u,0x40u,0x8000u,0x8040u}) {
                prepare(initial); _mm_setcsr(_mm_getcsr() | daz);
                const auto a=original_round(mode); const auto acw=x87_control(); const auto amx=_mm_getcsr();
                prepare(initial); _mm_setcsr(_mm_getcsr() | daz);
                const auto b=p::set_rounding_mode(mode); const auto bcw=x87_control(); const auto bmx=_mm_getcsr();
                check("rounding_control_registers_and_result",a==b && acw==bcw && amx==bmx);
            }
        }
        prepare();
        auto config=p::default_configuration(); p::Configuration decoded;
        check("configuration_decode_default", p::decode_configuration(decoded,&config,sizeof(config)));
        for(auto mode : {std::numeric_limits<std::int32_t>::min(), -1,0,9,10,255,256,std::numeric_limits<std::int32_t>::max()}) {
            config.saved_display_mode=mode;
            check("configuration_signed_display_bound", p::valid_configuration(config,sizeof(config)) == (mode<10));
        }
        for(std::size_t n : {0u,4u,8u,175u,177u}) check("configuration_truncation_rejected",!p::decode_configuration(decoded,&config,n));
        std::size_t total=0;for(auto& row:counts)total+=row.second;
        std::ofstream out(report,std::ios::binary|std::ios::trunc); if(!out)throw std::runtime_error("Cannot write report");
        out << "{\n  \"status\":\""<<(failed?"failed":"passed")<<"\",\n  \"source_sha256\":\""<<digest
            <<"\",\n  \"configuration_cpp_sha256\":\""<<TH20_SHA_configuration_cpp
            <<"\",\n  \"configuration_hpp_sha256\":\""<<TH20_SHA_configuration_hpp
            <<"\",\n  \"clock_cpp_sha256\":\""<<TH20_SHA_clock_cpp
            <<"\",\n  \"clock_hpp_sha256\":\""<<TH20_SHA_clock_hpp
            <<"\",\n  \"total\":"<<total<<",\n  \"failed\":"<<failed
            <<",\n  \"scope\":\"Original constructors compare all bytes including retained padding; original clock compares 64-bit result and updated offset using deterministic Win32 import samples in all four rounding modes. Original lock is recursively held on the test thread.\",\n"
            <<"  \"limitations\":[\"Clock conversion selects original SSE2 branch; AVX512 and subnormal/NaN offsets not covered\",\"File/path APIs and complete game are not tested by this executable\",\"Configuration validation/truncation cases are source contract tests, not original load function execution\"],\n  \"comparisons\":{";
        bool first=true;for(auto& row:counts){if(!first)out<<',';first=false;out<<'\n'<<th20::json_string(row.first)<<':'<<row.second;}
        out<<"\n  },\n  \"failure_examples\":[";first=true;for(auto& error:failures){if(!first)out<<',';first=false;out<<th20::json_string(error);}
        out<<"]\n}\n";if(!out)throw std::runtime_error("Cannot complete report");
        std::cout<<"Platform comparisons: "<<total<<"; failed: "<<failed<<'\n';for(auto& error:failures)std::cout<<error<<'\n';
        return failed?1:0;
    } catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 2;}
}
