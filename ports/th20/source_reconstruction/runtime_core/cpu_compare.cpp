// Original routines are used only by this isolated hardware test target.
#define wmain unused_original_oracle_main
#include "../../tests/native_cpu_compare.cpp"
#undef wmain
#include "runtime_core.hpp"
#include "worker.hpp"

namespace rt = th20::source::runtime;
namespace {
LONG WINAPI diagnostic_exception(EXCEPTION_POINTERS* exception) {
    std::fprintf(stderr, "CPU oracle exception %08lx at %08lx (mapped original base %08x)\n",
        exception->ExceptionRecord->ExceptionCode, exception->ContextRecord->Eip, mapped_image_base);
    std::fflush(stderr);
    return EXCEPTION_EXECUTE_HANDLER;
}
template<class Return, class... Args> Return original_call(std::uint32_t rva, void* self, Args... args) {
    using Function = Return(__thiscall*)(void*, Args...);
    return reinterpret_cast<Function>(mapped_image_base + rva)(self, args...);
}
template<class Return, class... Args> Return original_cdecl(std::uint32_t rva, Args... args) {
    using Function = Return(__cdecl*)(Args...);
    return reinterpret_cast<Function>(mapped_image_base + rva)(args...);
}
}
int wmain(int argc, wchar_t** argv) {
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
    SetUnhandledExceptionFilter(diagnostic_exception);
    try {
        if (argc != 3) throw std::runtime_error("Usage: th20_runtime_cpu_compare VERIFIED_TH20.exe OUTPUT.json");
        const std::filesystem::path source(argv[1]), report_path(argv[2]);
        if (std::filesystem::weakly_canonical(source) == std::filesystem::weakly_canonical(report_path))
            throw std::runtime_error("Report must not replace source EXE");
        const auto bytes = th20::read_file(source);
        const auto digest = sha256(bytes);
        if (digest != expected_sha) throw std::runtime_error("Original EXE hash mismatch");
        const auto pe = th20::parse_pe(bytes);
        Mapping mapped(bytes, pe); mapped_image_base = mapped.address();
        // Resolve only documented Win32 imports needed by actual original CRT
        // mutex/heap calls. No original code bytes are replaced or intercepted.
        std::vector<std::string> resolved;
        for (const auto& item : pe.imports) {
            if (item.name == "GetCurrentThreadId" || item.name == "AcquireSRWLockExclusive"
                || item.name == "ReleaseSRWLockExclusive" || item.name == "HeapAlloc"
                || item.name == "HeapFree" || item.name == "GetLastError"
                || item.name == "WaitForSingleObjectEx" || item.name == "CloseHandle"
                || item.name == "GetExitCodeThread") {
                auto address = GetProcAddress(GetModuleHandleW(L"kernel32.dll"), item.name.c_str());
                if (!address) throw std::runtime_error("Cannot resolve original Win32 dependency");
                *reinterpret_cast<std::uintptr_t*>(mapped_image_base + item.iat_rva) = reinterpret_cast<std::uintptr_t>(address);
                resolved.push_back(item.name);
            }
        }
        *reinterpret_cast<HANDLE*>(mapped_image_base + 0x1e5990) = GetProcessHeap();
        original_call<void>(0x52e00, reinterpret_cast<void*>(mapped_image_base + 0x1c0240));
        rt::MemoryResource resource;
        auto* previous_default = rt::set_default_resource(&resource);
        *reinterpret_cast<std::uintptr_t*>(mapped_image_base + 0x1e4d28) = reinterpret_cast<std::uintptr_t>(&resource);
        std::map<std::string, unsigned> counts;
        std::vector<std::string> failures;
        unsigned failed = 0;
        auto check = [&](const char* label, bool condition) {
            ++counts[label]; if (!condition) { ++failed; if (failures.size() < 20) failures.push_back(label); }
        };
        {
            std::array<unsigned char, 0x438> original{};
            original_call<void>(0x52e00, original.data());
            rt::LockRegistry source_registry;
            static_assert(sizeof(rt::LockRegistry) == 0x438 && sizeof(std::recursive_mutex) == 0x30);
            const auto* recovered = reinterpret_cast<const unsigned char*>(&source_registry);
            // Compare actual mutex fields and tracked bytes; skip reserved CRT
            // padding that is not semantically initialized by every toolset.
            bool same = true;
            for (unsigned slot = 0; slot < 22; ++slot) for (unsigned offset : {0u, 8u, 40u, 44u}) {
                same = same && std::memcmp(original.data() + slot * 48 + offset, recovered + slot * 48 + offset, 4) == 0;
            }
            same = same && std::memcmp(original.data() + 0x420, recovered + 0x420, 23) == 0;
            check("registry_constructor_fields", same);
            for (unsigned slot = 0; slot < 22; ++slot) {
                original_call<void>(0x1ccc0, original.data()); source_registry.enable();
                for (unsigned depth = 1; depth <= 3; ++depth) {
                    original_call<void>(0x12550, original.data(), slot); source_registry.enter_tracked(slot);
                    check("registry_tracked_enter", original[0x420 + slot] == source_registry.depth(slot));
                }
                for (unsigned depth = 0; depth < 3; ++depth) {
                    original_call<void>(0x12750, original.data(), slot); source_registry.leave_tracked(slot);
                    check("registry_tracked_leave", original[0x420 + slot] == source_registry.depth(slot));
                }
                original_call<void>(0x1ca30, original.data()); source_registry.disable();
                check("registry_disable", (original[0x436] != 0) == source_registry.enabled());
            }
        }
        for (unsigned n = 0; n < 100; ++n) {
            rt::Log original, recovered;
            // Source-created live pmr string objects have the verified original
            // layout. The original constructor stores our real C++ resource,
            // allowing its allocator virtual calls to use actual source code.
            original_call<void>(0x52dd0, &original);
            check("log_constructor", original.text.empty() && original.error == 0
                && original.text.capacity() == recovered.text.capacity()
                && original.text.get_allocator().resource() == recovered.text.get_allocator().resource());
            for (unsigned append = 0; append < 10; ++append) {
                const std::string text((n * 31 + append * 17) % 600, static_cast<char>('A' + append));
                original_call<void>(0x530a0, &original, text.c_str()); rt::append_log(recovered, text.c_str());
                check("log_append_sso_and_growth", original.text == recovered.text && original.text.capacity() == recovered.text.capacity());
            }
            original.error = recovered.error = 1;
            original_call<void>(0x19be0, &original); rt::clear_log(recovered);
            check("log_clear_preserves_error", original.text == recovered.text && original.text.capacity() == recovered.text.capacity() && original.error == recovered.error);
        }
        for (std::size_t size : {0u, 1u, 7u, 16u, 31u, 1024u, 8193u, 0x100000u}) {
            auto* original = original_call<void*>(0x1f610, nullptr, size, 0u);
            auto* recovered = rt::allocate_bytes(size);
            check("raw_allocation_success", original != nullptr && recovered != nullptr);
            if (size) { std::memset(original, 0x37, size); std::memset(recovered, 0x37, size); }
            original_call<void>(0x1f670, nullptr, original); rt::release_bytes(recovered);
        }
        for (std::size_t alignment : {4u, 8u, 16u, 64u, 256u, 4096u}) for (std::size_t size : {0u, 1u, 13u, 1024u, 8193u}) {
            alignas(4) std::array<std::uint32_t, 2> controller{};
            original_call<void>(0x1f5b0, controller.data());
            auto* original = original_call<void*>(0x1f6d0, controller.data() + 1, size, alignment);
            auto* recovered = resource.allocate(size, alignment);
            check("aligned_allocation_success", controller[0] == 0
                && controller[1] == mapped_image_base + 0x16c920
                && original && recovered && reinterpret_cast<std::uintptr_t>(original) % alignment == 0
                && reinterpret_cast<std::uintptr_t>(recovered) % alignment == 0);
            if (size) { std::memset(original, 0x53, size); std::memset(recovered, 0x53, size); }
            original_call<void>(0x1f760, controller.data() + 1, original, size, alignment);
            resource.deallocate(recovered, size, alignment);
            check("memory_resource_is_equal", original_call<bool>(0x1c9f0, controller.data() + 1, &resource) == resource.is_equal(resource));
            original_call<void>(0x19320, controller.data(), 0u);
            check("allocation_controller_destruct_without_free", controller[0] == 0 && controller[1] == mapped_image_base + 0x16c920);
        }
        #include "worker_cpu_cases.inc"
        rt::set_default_resource(previous_default);
        unsigned total = 0; for (const auto& item : counts) total += item.second;
        std::ofstream out(report_path, std::ios::binary | std::ios::trunc);
        out << "{\n  \"status\":\"" << (failed ? "failed" : "passed") << "\",\n  \"source_sha256\":\"" << digest
            << "\",\n  \"runtime_cpp_sha256\":\"" << TH20_RUNTIME_CPP_SHA << "\",\n  \"runtime_hpp_sha256\":\"" << TH20_RUNTIME_HPP_SHA
            << "\",\n  \"worker_cpp_sha256\":\"" << TH20_WORKER_CPP_SHA << "\",\n  \"worker_hpp_sha256\":\"" << TH20_WORKER_HPP_SHA << "\",\n  \"worker_cases_sha256\":\"" << TH20_WORKER_CASES_SHA
            << "\",\n  \"total\":" << total << ",\n  \"failed\":" << failed
            << ",\n  \"scope\":\"Original CPU constructors, actual recursive mutex operations, byte-string append/clear including heap growth, raw/aligned allocation and release, PMR equality; Worker constructor/destructor idle object bytes including tail padding, idle close/join/detach, and real running source-created ABI-compatible jthreads closed/joined or detached by unchanged original Worker methods\",\n"
            << "  \"limitations\":[\"Original entry point never executed\",\"Source tests cover formatting and renderer callback order; original variadic formatting was not CPU-compared\",\"Allocation failure/new-handler and invalid parameter paths not CPU-compared\",\"Original log MessageBox not invoked by tests\"],\n  \"comparisons\":{";
        bool first = true; for (const auto& item : counts) { if (!first) out << ','; first = false; out << '\n' << th20::json_string(item.first) << ':' << item.second; }
        out << "\n  },\n  \"failure_examples\":[";
        first = true; for (const auto& item : failures) { if (!first) out << ','; first = false; out << th20::json_string(item); }
        out << "]\n}\n";
        if (!out) throw std::runtime_error("Cannot finish CPU report");
        std::cout << "Runtime CPU comparisons: " << total << "; failed: " << failed << '\n';
        for (const auto& item : failures) std::cout << item << '\n';
        return failed ? 1 : 0;
    } catch (const std::exception& error) { std::cerr << error.what() << '\n'; return 2; }
}
