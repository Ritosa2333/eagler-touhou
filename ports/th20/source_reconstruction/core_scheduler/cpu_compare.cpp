// Test-only hardware oracle. The static scheduler library has no PE reader,
// no original-code call, and no original-code bytes. Reuse the existing gated
// mapping/hash helpers; its renamed entry point is never executed.
#define wmain unused_original_oracle_main
#include "../../tests/native_cpu_compare.cpp"
#undef wmain
#include "scheduler.hpp"
#include <algorithm>
#include <limits>

namespace s = th20::source::scheduler;
namespace {
template<class Return, class... Args> Return original_call(std::uint32_t rva, void* self, Args... args) {
    using Function = Return(__thiscall*)(void*, Args...);
    return reinterpret_cast<Function>(mapped_image_base + rva)(self, args...);
}
struct World;
struct Context {
    World* world = nullptr;
    unsigned index = 0;
    std::vector<std::int32_t> returns{1};
    unsigned calls = 0;
    unsigned action = 0;
    unsigned target = 0;
};
std::int32_t __cdecl callback(void*);
std::int32_t __cdecl before_insert(void*);
std::int32_t __cdecl shutdown_callback(void*);
struct World {
    bool original;
    s::State state{};
    s::Environment env;
    std::array<s::Node, 12> nodes{};
    std::array<Context, 12> contexts;
    std::vector<unsigned> trace;
    explicit World(bool use_original) : original(use_original) {
        env.dispatch_locking = false;
        if (original) original_call<void>(0x18a80, &state); else s::initialize_state(state);
        for (unsigned i = 0; i < nodes.size(); ++i) {
            if (original) original_call<void>(0x11a70, &nodes[i]); else s::initialize_node(nodes[i]);
            contexts[i].world = this; contexts[i].index = i;
            nodes[i].callback = callback; nodes[i].userdata = &contexts[i]; nodes[i].flags = 2;
            nodes[i].on_shutdown = shutdown_callback;
        }
    }
    std::int32_t insert(unsigned i, std::int32_t priority, bool draw) {
        if (original) return original_call<std::int32_t>(draw ? 0x12100 : 0x11f80, &state, &nodes[i], priority);
        return s::insert(state, env, nodes[i], priority, draw);
    }
    void remove(unsigned i) {
        if (original) original_call<void>(0x12400, &state, &nodes[i]);
        else s::remove_unlocked(state, env, &nodes[i]);
    }
    std::int32_t dispatch(bool draw) {
        return original ? original_call<std::int32_t>(draw ? 0x12aa0 : 0x12810, &state)
            : draw ? s::dispatch_draw(state, env) : s::dispatch_update(state, env);
    }
};
std::int32_t __cdecl callback(void* input) {
    auto& c = *static_cast<Context*>(input);
    c.world->trace.push_back(c.index);
    const auto call_index = c.calls++;
    if (c.calls > 1000) throw std::runtime_error("Test callback entered an unintended infinite loop");
    if (call_index == 0) {
        if (c.action == 1) c.world->remove(c.target);
        if (c.action == 2) c.world->insert(c.target, c.world->nodes[c.index].priority + 1, false);
        if (c.action == 3) c.world->nodes[c.index].flags &= ~2u;
    }
    return c.returns[std::min<std::size_t>(call_index, c.returns.size() - 1)];
}
std::int32_t __cdecl before_insert(void* input) {
    auto& c = *static_cast<Context*>(input); c.world->trace.push_back(1000 + c.index); return c.returns.front();
}
std::int32_t __cdecl shutdown_callback(void* input) {
    auto& c = *static_cast<Context*>(input); c.world->trace.push_back(2000 + c.index); return 0;
}
std::uint32_t normalize(std::uint32_t word, const World& w) {
    const auto translate = [&](const void* base, std::size_t size, std::uint32_t tag) {
        const auto address = reinterpret_cast<std::uintptr_t>(base);
        return word >= address && word < address + size ? tag + word - address : word;
    };
    auto result = translate(&w.state, sizeof(w.state), 0xf0000000);
    if (result != word) return result;
    result = translate(w.nodes.data(), sizeof(w.nodes), 0xe0000000);
    if (result != word) return result;
    return translate(w.contexts.data(), sizeof(w.contexts), 0xd0000000);
}
std::vector<std::uint32_t> snapshot(const World& w) {
    std::vector<std::uint32_t> result;
    auto add = [&](const void* object, std::size_t size) {
        const auto* p = static_cast<const unsigned char*>(object);
        for (std::size_t i = 0; i < size; i += 4) {
            std::uint32_t word; std::memcpy(&word, p + i, 4); result.push_back(normalize(word, w));
        }
    };
    add(&w.state, sizeof(w.state)); add(w.nodes.data(), sizeof(w.nodes));
    result.push_back(w.env.dispatch_depth);
    return result;
}
}

int wmain(int argc, wchar_t** argv) {
    try {
        if (argc != 3) throw std::runtime_error("Usage: th20_scheduler_cpu_compare VERIFIED_TH20.exe OUTPUT.json");
        const std::filesystem::path source(argv[1]), report_path(argv[2]);
        if (std::filesystem::weakly_canonical(source) == std::filesystem::weakly_canonical(report_path))
            throw std::runtime_error("Report must not replace the source EXE");
        const auto bytes = th20::read_file(source);
        const auto digest = sha256(bytes);
        if (digest != expected_sha) throw std::runtime_error("Source SHA-256 does not match the analyzed EXE");
        const auto pe = th20::parse_pe(bytes);
        Mapping mapped(bytes, pe); mapped_image_base = mapped.address();
        // The unmodified original insertion routines use a recursive CRT mutex.
        // Start with one lock held by this test thread; their nested operations
        // then need only the real GetCurrentThreadId import, no game startup.
        auto* mutex = reinterpret_cast<std::uint32_t*>(mapped_image_base + 0x1c0240);
        std::memset(mutex, 0, 0x440); mutex[0] = 0x101; mutex[10] = GetCurrentThreadId(); mutex[11] = 1;
        unsigned imports_resolved = 0;
        for (const auto& item : pe.imports) if (item.name == "GetCurrentThreadId") {
            *reinterpret_cast<std::uintptr_t*>(mapped_image_base + item.iat_rva)
                = reinterpret_cast<std::uintptr_t>(&GetCurrentThreadId);
            ++imports_resolved;
        }
        if (imports_resolved != 1) throw std::runtime_error("Cannot identify original thread-id import");
        std::map<std::string, std::size_t> counts;
        std::vector<std::string> failures;
        std::size_t failed = 0;
        auto check = [&](const char* label, const World& a, const World& b, std::int32_t ar = 0, std::int32_t br = 0) {
            ++counts[label];
            const auto sa = snapshot(a), sb = snapshot(b);
            if (sa != sb || a.trace != b.trace || ar != br) {
                ++failed;
                if (failures.size() < 30) {
                    std::ostringstream detail;
                    detail << label << " case " << counts[label] << " returns " << ar << '/' << br;
                    if (a.trace != b.trace) { detail << " trace "; for (auto x : a.trace) detail << x << ','; detail << '/'; for (auto x : b.trace) detail << x << ','; }
                    for (unsigned i = 0; i < sa.size(); ++i) if (sa[i] != sb[i]) { detail << " word[" << i << "]=" << std::hex << sa[i] << '/' << sb[i]; break; }
                    failures.push_back(detail.str());
                }
            }
        };
        std::mt19937 random(0x4127b0);
        for (unsigned test = 0; test < 400; ++test) {
            World a(true), b(false);
            check("constructors", a, b);
            std::array<unsigned, 12> order{};
            for (unsigned i = 0; i < order.size(); ++i) order[i] = i;
            std::shuffle(order.begin(), order.end(), random);
            for (auto i : order) {
                const std::int32_t priority = test % 3 == 0 ? std::int32_t(random() % 3) - 1
                    : test % 3 == 1 ? r::signed_bits(random()) : std::numeric_limits<std::int32_t>::min();
                const bool draw = (random() & 1) != 0;
                a.nodes[i].before_insert = b.nodes[i].before_insert = before_insert;
                a.contexts[i].returns=b.contexts[i].returns={r::signed_bits(random())};
                const auto ar=a.insert(i, priority, draw),br=b.insert(i, priority, draw);
                check("ordered_insert_and_before_callback_return", a, b,ar,br);
                a.contexts[i].returns=b.contexts[i].returns={1};
            }
            check("dispatch_update_default", a, b, a.dispatch(false), b.dispatch(false));
            check("dispatch_draw_default", a, b, a.dispatch(true), b.dispatch(true));
            std::shuffle(order.begin(), order.end(), random);
            for (auto i : order) {
                // Exercise the otherwise separate state.current adjustment.
                a.state.current = &a.nodes[i].link; b.state.current = &b.nodes[i].link;
                a.remove(i); b.remove(i); check("unlink", a, b);
            }
        }
        for (bool draw : {false, true}) for (std::int32_t result : {-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0x7fffffff}) {
            for (unsigned position = 0; position < 5; ++position) for (unsigned flag_pattern = 0; flag_pattern < 4; ++flag_pattern) {
                World a(true), b(false);
                for (unsigned i = 0; i < 5; ++i) {
                    a.insert(i, std::int32_t(i), draw); b.insert(i, std::int32_t(i), draw);
                    a.nodes[i].flags = b.nodes[i].flags = (flag_pattern & 1) && i % 2 == 0 ? 0u : 2u;
                    if ((flag_pattern & 2) && i % 2 == 1) a.nodes[i].callback = b.nodes[i].callback = nullptr;
                }
                a.contexts[position].returns = b.contexts[position].returns = {result, 1};
                check(draw ? "draw_return_codes" : "update_return_codes", a, b, a.dispatch(draw), b.dispatch(draw));
            }
        }
        for (bool draw : {false, true}) for (unsigned current = 0; current < 5; ++current) for (unsigned target = 0; target < 5; ++target) {
            World a(true), b(false);
            for (unsigned i = 0; i < 5; ++i) { a.insert(i, std::int32_t(i), draw); b.insert(i, std::int32_t(i), draw); }
            a.contexts[current].action = b.contexts[current].action = 1;
            a.contexts[current].target = b.contexts[current].target = target;
            check("callback_removes_current_next_or_other", a, b, a.dispatch(draw), b.dispatch(draw));
        }
        for (unsigned current = 0; current < 5; ++current) {
            World a(true), b(false);
            for (unsigned i = 0; i < 5; ++i) { a.insert(i, std::int32_t(i * 10), false); b.insert(i, std::int32_t(i * 10), false); }
            a.contexts[current].action = b.contexts[current].action = 2;
            a.contexts[current].target = b.contexts[current].target = 5;
            check("callback_inserts_new_node", a, b, a.dispatch(false), b.dispatch(false));
        }
        for (unsigned mask = 0; mask < 32; ++mask) {
            World a(true), b(false);
            for (unsigned i = 0; i < 5; ++i) {
                a.insert(i, std::int32_t(i), false); b.insert(i, std::int32_t(i), false);
                if (mask & (1u << i)) a.nodes[i].flags = b.nodes[i].flags = 0;
            }
            a.state.shutdown = b.state.shutdown = 1;
            check("shutdown_callback_path", a, b, a.dispatch(false), b.dispatch(false));
        }
        for (unsigned test = 0; test < 300; ++test) {
            World a(true), b(false);
            const auto flags = random();
            a.nodes[0].flags = b.nodes[0].flags = flags;
            original_call<void>(0x12d30, &a.nodes[0]); s::set_owned(b.nodes[0]); check("set_owned", a, b);
            original_call<void>(0x12d80, &a.nodes[0]); s::enable(b.nodes[0]); check("enable", a, b);
            original_call<void>(0x127f0, &a.nodes[0]); s::disable(b.nodes[0]); check("disable", a, b);
            original_call<void>(0x12d50, &a.nodes[0], callback); s::set_callback(b.nodes[0], callback); check("set_callback", a, b);
            original_call<void>(0x12d10, &a.nodes[0], &a.contexts[1]); s::set_userdata(b.nodes[0], &b.contexts[1]); check("set_userdata", a, b);
            original_call<void>(0x12dc0, &a.nodes[0], before_insert); s::set_before_insert(b.nodes[0], before_insert); check("set_before_insert", a, b);
            original_call<void>(0x12da0, &a.nodes[0], shutdown_callback); s::set_shutdown_callback(b.nodes[0], shutdown_callback); check("set_shutdown_callback", a, b);
            original_call<void>(0x11b80, &a.nodes[0]); s::clear_callbacks(b.nodes[0]); check("clear_callbacks", a, b);
        }
        // Independent execution of ownership/new/delete and real lock release
        // around callbacks, with no original PE participation in this check.
        {
            s::State state; s::initialize_state(state); s::Environment env;
            unsigned calls = 0;
            auto erase = +[](void* p) -> std::int32_t { ++*static_cast<unsigned*>(p); return 0; };
            s::register_callback(state, env, 3, erase, &calls, false, true);
            const auto result = s::dispatch_update(state, env);
            ++counts["source_owned_allocation_removal"];
            if (calls != 1 || result != 1 || state.update.sentinel.next || state.update.tail != &state.update.sentinel || env.dispatch_depth) {
                ++failed; failures.push_back("source owned allocation/removal failed");
            }
        }
        std::size_t total = 0; for (const auto& row : counts) total += row.second;
        std::ofstream out(report_path, std::ios::binary | std::ios::trunc);
        if (!out) throw std::runtime_error("Cannot create report");
        out << "{\n  \"status\":\"" << (failed ? "failed" : "passed") << "\",\n  \"source_sha256\":\"" << digest
            << "\",\n  \"scheduler_cpp_sha256\":\"" << TH20_SCHEDULER_CPP_SHA
            << "\",\n  \"scheduler_hpp_sha256\":\"" << TH20_SCHEDULER_HPP_SHA
            << "\",\n  \"total\":" << total << ",\n  \"failed\":" << failed
            << ",\n  \"oracle\":\"Unmodified original x86 routines mapped in isolated test process; original entry point never called\",\n"
            << "  \"original_imports_resolved\":[\"GetCurrentThreadId\"],\n"
            << "  \"scope\":\"Constructors, priority insertion, unlink and observer repair, callback trace and dispatcher result codes; normalized complete State and Node object words\",\n"
            << "  \"limitations\":[\"Original allocator and renderer flush are not executed\",\"Original recursive mutex is exercised only nested on one owning thread; cross-thread scheduling equivalence not established\",\"Callback exception unwinding and invalid object graphs are outside the recovered contract\",\"This module is not a full game\"],\n"
            << "  \"comparisons\":{";
        bool first = true;
        for (const auto& row : counts) { if (!first) out << ','; first = false; out << '\n' << th20::json_string(row.first) << ':' << row.second; }
        out << "\n  },\n  \"failure_examples\":[";
        first = true;
        for (const auto& row : failures) { if (!first) out << ','; first = false; out << th20::json_string(row); }
        out << "]\n}\n";
        if (!out) throw std::runtime_error("Cannot finish report");
        std::cout << "Scheduler comparisons: " << total << "; failed: " << failed << '\n';
        for (const auto& row : failures) std::cout << row << '\n';
        return failed ? 1 : 0;
    } catch (const std::exception& error) { std::cerr << "error: " << error.what() << '\n'; return 2; }
}
