#include "game/CancellationEffects.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>

using th128::effects::CancellationEffects;
using th128::anm::Membership;
using Bytes = std::vector<uint8_t>;
using Handle = CancellationEffects::Handle;

void check(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

void near(float actual, float expected) {
    check(std::isfinite(actual) && std::abs(actual - expected) < .0001f,
          "float mismatch: " + std::to_string(actual) + " != " + std::to_string(expected));
}

void put16(Bytes& bytes, size_t offset, uint16_t value) {
    bytes.at(offset) = uint8_t(value);
    bytes.at(offset + 1) = uint8_t(value >> 8);
}

void put32(Bytes& bytes, size_t offset, uint32_t value) {
    for (size_t index = 0; index < 4; ++index)
        bytes.at(offset + index) = uint8_t(value >> (index * 8));
}

struct Instruction {
    int opcode, time;
    std::vector<uint32_t> words;
    uint16_t mask = 0;
};

void append(Bytes& bytes, const std::vector<Instruction>& program) {
    for (const auto& command : program) {
        const size_t offset = bytes.size(), length = 8 + command.words.size() * 4;
        bytes.resize(offset + length);
        put16(bytes, offset, uint16_t(command.opcode));
        put16(bytes, offset + 2, uint16_t(length));
        put16(bytes, offset + 4, uint16_t(command.time));
        put16(bytes, offset + 6, command.mask);
        for (size_t index = 0; index < command.words.size(); ++index)
            put32(bytes, offset + 8 + index * 4, command.words[index]);
    }
    const size_t end = bytes.size();
    bytes.resize(end + 8);
    put16(bytes, end, 65535);
}

Bytes synthetic(std::vector<Instruction> root, std::vector<Instruction> child = {{3, 0, {0}}, {63, 0, {}}},
                float size = 16, const std::unordered_map<int, std::vector<Instruction>>& tails = {}) {
    constexpr size_t ScriptCount = 361, TableEnd = 64 + 8 + ScriptCount * 8;
    Bytes bytes(TableEnd + 4 + 40);
    put32(bytes, 0, 7);
    put16(bytes, 4, 2);
    put16(bytes, 6, ScriptCount);
    put16(bytes, 10, 64);
    put16(bytes, 12, 64);
    put32(bytes, 16, TableEnd);
    bytes[TableEnd] = 't';
    for (size_t sprite = 0; sprite < 2; ++sprite) {
        const size_t offset = TableEnd + 4 + sprite * 20;
        put32(bytes, 64 + sprite * 4, uint32_t(offset));
        put32(bytes, offset, uint32_t(sprite));
        put32(bytes, offset + 12, th128::anm::asWord(size));
        put32(bytes, offset + 16, th128::anm::asWord(size));
    }
    for (size_t script = 0; script < ScriptCount; ++script) {
        put32(bytes, 72 + script * 8, uint32_t(script));
        put32(bytes, 76 + script * 8, uint32_t(bytes.size()));
        const auto tail = tails.find(int(script));
        append(bytes, script == 357 ? root : script == 356 ? child :
                      tail != tails.end() ? tail->second : std::vector<Instruction>{{2, 0, {}}});
    }
    return bytes;
}

void loaded(CancellationEffects& effects, const Bytes& bytes) {
    std::string error = "stale";
    check(effects.load(bytes.data(), bytes.size(), error), error);
    check(error.empty(), "load must clear stale errors");
}

Handle spawn(CancellationEffects& effects, float x = 332, float y = 40, float z = 5) {
    std::string error;
    const auto handle = effects.spawn(357, x, y, z, 23, error);
    check(handle.has_value() && *handle != 0 && error.empty(), "spawn failed: " + error);
    return *handle;
}

CancellationEffects::Snapshot root(const CancellationEffects& effects, Handle handle) {
    for (const auto& value : effects.snapshots())
        if (value.handle == handle && value.parent == 0)
            return value;
    check(false, "missing root snapshot");
    return {};
}

std::vector<CancellationEffects::Snapshot> children(const CancellationEffects& effects, Handle handle) {
    std::vector<CancellationEffects::Snapshot> result;
    for (const auto& value : effects.snapshots())
        if (value.handle == handle && value.parent != 0)
            result.push_back(value);
    return result;
}

void syntheticCases() {
    const auto attached = synthetic({{88, 0, {356}}, {63, 0, {}}},
                                    {{68, 0, {6}}, {3, 0, {0}}, {63, 0, {}}});
    CancellationEffects effects;
    std::string error;
    check(!effects.spawn(357, 0, 0, 0, 23, error) && !error.empty(), "unloaded spawn must fail");
    check(!effects.load(nullptr, 0, error), "null bank must fail");
    loaded(effects, attached);
    check(!effects.spawn(357, 0, 0, 0, 23, error) && error.find("RNG") != std::string::npos,
          "shared game RNG is required, never seeded implicitly");
    effects.setGameRandom([] { return 0u; });
    for (const int script : {-1, 356, 360})
        check(!effects.spawn(script, 0, 0, 0, 23, error), "other root programs/colors are unsupported");
    check(!effects.spawn(357, 0, 0, 0, 31, error), "unknown draw layer is rejected");
    check(!effects.spawn(357, std::numeric_limits<float>::infinity(), 0, 0, 23, error), "invalid pose rejected");
    const auto first = spawn(effects);
    const auto birth = root(effects, first);
    const auto bornChild = children(effects, first).at(0);
    check(effects.rootCount() == 1 && effects.nodeCount() == 2, "one registered root plus attached child");
    check(birth.membership == Membership::Primary && bornChild.membership == Membership::Primary,
          "root357 and attached op88 children share the same primary-tail domain");
    check(birth.state.ticks == 1 && bornChild.state.ticks == 1, "synchronous bind ticks root and child once");
    check(birth.state.layer == 23 && bornChild.state.layer == 6, "authored child layer is not scheduling priority");
    near(birth.world.x, 332); near(birth.world.y, 40); near(birth.world.z, 5);
    near(bornChild.state.engineX, 0); near(bornChild.state.engineY, 0);
    near(bornChild.world.x, 332); near(bornChild.world.y, 40); near(bornChild.world.z, 5);
    check(bornChild.sprite && bornChild.sprite->width == 16, "child uses actual copied bank metadata");
    check(effects.tick(error), error);
    check(root(effects, first).state.ticks == 2 && children(effects, first)[0].state.ticks == 2,
          "same-cycle primary pass advances root and pre-existing child exactly once after birth");
    check(effects.tickPrimary(error), error);
    check(root(effects, first).state.ticks == 3 && children(effects, first)[0].state.ticks == 3,
          "tickPrimary alias is one further primary pass, never two passes in one call");
    check(!effects.requestNativeDrift(first, error) && error.find("unhandled") != std::string::npos,
          "unproved velocity/camera native drift is explicit, not approximated");
    check(!effects.setRootPositionInterpolation(first, 8, 12, 4, -1, 0, error), "negative duration rejected");
    check(!effects.setRootPositionInterpolation(first, 8, 12, 4, 4, 99, error), "unknown interpolation mode rejected");
    check(!effects.setRootPositionInterpolation(first, NAN, 12, 4, 4, 0, error), "non-finite target rejected");
    check(effects.setRootPositionInterpolation(first, 8, 12, 4, 4, 0, error), error);
    near(root(effects, first).world.x, 332);
    for (int index = 1; index <= 4; ++index) {
        check(effects.tick(error), error);
        near(root(effects, first).world.x, 332 + index * 2.f);
        near(children(effects, first)[0].world.x, 332 + index * 2.f);
        near(children(effects, first)[0].world.y, 40 + index * 3.f);
        near(children(effects, first)[0].world.z, 5 + index);
    }
    near(bornChild.world.x, 332);
    check(bornChild.state.ticks == 1 && birth.state.x == 0, "snapshots never alias live VM state");
    const auto oldSprite = effects.spriteInfo(0);
    check(!effects.load(attached.data(), 8, error) && effects.nodeCount() == 2,
          "malformed reload preserves active tree");
    const auto larger = synthetic({{88, 0, {356}}, {63, 0, {}}}, {{3, 0, {0}}, {63, 0, {}}}, 32);
    loaded(effects, larger);
    check(effects.rootCount() == 0 && effects.nodeCount() == 0 && !effects.remove(first), "reload retires all old handles");
    check(oldSprite && oldSprite->width == 16 && effects.spriteInfo(0)->width == 32,
          "metadata copies survive bank reload");
    const auto newer = spawn(effects);
    check(newer > first && !effects.setRootPositionInterpolation(first, 1, 2, 3, 30, 6, error),
          "old handles cannot mutate a new root after reload");
    check(effects.remove(newer) && effects.nodeCount() == 0 && effects.rootCount() == 0,
          "explicit removal collects descendants and owned-root metadata");

    const auto randomLoop = synthetic({{40, 0, {10000, 100}, 1}, {3, 0, {0}}, {4, 1, {0, 0}}});
    loaded(effects, randomLoop);
    effects.setGameRandom([value = 0u]() mutable { return value++; });
    const auto randomFirst = spawn(effects), randomSecond = spawn(effects);
    check(root(effects, randomFirst).state.integers[0] == 0 && root(effects, randomSecond).state.integers[0] == 1,
          "callable-owned RNG state survives Scene runtime copies between births");
    check(effects.tick(error), error);
    check(root(effects, randomFirst).state.integers[0] == 2 && root(effects, randomSecond).state.integers[0] == 3,
          "callable-owned RNG state continues across primary ticks");
    check(effects.tickPrimary(error) && root(effects, randomFirst).state.integers[0] == 4 &&
          root(effects, randomSecond).state.integers[0] == 5,
          "primary alias advances each root exactly once using its shared RNG stream");

    auto rejected = [&](const Bytes& bytes, const std::string& reason) {
        check(!effects.load(bytes.data(), bytes.size(), error) && error.find(reason) != std::string::npos,
              "unknown behavior must fail closed: " + reason + "; got " + error);
        check(effects.rootCount() == 2 && effects.nodeCount() == 2, "rejected reload must not destroy old roots");
    };
    const auto floatLoopDestination = synthetic({{5, 0, {th128::anm::asWord(10004.f), 0, 0}, 1}, {63, 0, {}}});
    const auto negativeInterpolation = synthetic({{56, 0, {4, uint32_t(-1), 0, 0, 0}}, {63, 0, {}}});
    const bool mutationBlocked = !effects.load(floatLoopDestination.data(), floatLoopDestination.size(), error);
    const bool negativeModeBlocked = !effects.load(negativeInterpolation.data(), negativeInterpolation.size(), error);
    check(mutationBlocked && negativeModeBlocked,
          "typed operand preflight must block literal mutation and negative modes: mutation=" +
          std::to_string(mutationBlocked) + " negativeMode=" + std::to_string(negativeModeBlocked));
    check(effects.rootCount() == 2 && effects.nodeCount() == 2, "typed-invalid reload preserves prior roots");
    rejected(synthetic({{999, 0, {}}}), "opcode");
    rejected(synthetic({{111, 0, {1}}}), "111");
    rejected(synthetic({{67, 0, {9}}, {63, 0, {}}}), "geometry");
    rejected(synthetic({{3, 0, {uint32_t(-1)}}, {63, 0, {}}}), "sprite");
    rejected(synthetic({{3, 0, {2}}, {63, 0, {}}}), "sprite");
    rejected(synthetic({{52, 0, {256, 0, 0}}, {63, 0, {}}}), "color");
    rejected(synthetic({{95, 0, {356}}, {63, 0, {}}}), "child");
    rejected(synthetic({{88, 0, {356}}, {4, 1, {0, 0}}}), "repeating");
    rejected(synthetic({{88, 0, {356}}, {63, 0, {}}}, {{88, 0, {356}}, {63, 0, {}}}), "child");
    rejected(synthetic({{87, 0, {1}}, {40, 0, {10000, 2}, 1}, {63, 0, {}}}), "display");
    rejected(synthetic({{48, 0, {th128::anm::asWord(10016), 0, 0}, 1}, {63, 0, {}}}), "camera");

    loaded(effects, randomLoop);
    effects.setGameRandom([&] {
        std::string inner;
        check(!effects.tick(inner) && inner.find("reentr") != std::string::npos, "tick reentry must fail");
        check(!effects.load(randomLoop.data(), randomLoop.size(), inner), "reload reentry must fail");
        check(!effects.clear() && !effects.remove(1) && !effects.setGameRandom({}), "callback mutation must fail");
        check(effects.snapshots().empty(), "callback cannot observe partially constructed draw records");
        return 0u;
    });
    spawn(effects);
    check(effects.clear() && effects.nodeCount() == 0 && effects.rootCount() == 0, "clear releases all ownership");
    effects.setGameRandom([] { return 0u; });
    loaded(effects, synthetic({{4, 0, {0, 0}}}));
    check(!effects.spawn(357, 0, 0, 0, 23, error) && error.find("budget") != std::string::npos,
          "VM instruction budget bounds startup loops");
    check(effects.nodeCount() == 0 && effects.rootCount() == 0 && effects.snapshots().empty(),
          "faulting startup cannot leak nodes or root records");
    const auto failure = error;
    check(!effects.tick(error) && error == failure, "execution failure remains latched");
    check(effects.clear() && effects.tick(error), "clear explicitly recovers the latch");

    const auto triple = synthetic({{88, 0, {356}}, {88, 0, {356}}, {63, 0, {}}});
    loaded(effects, triple);
    Handle oldest = 0;
    for (int index = 0; index < 1024; ++index) {
        const auto handle = spawn(effects);
        if (index == 0) oldest = handle;
    }
    check(effects.nodeCount() == 3072 && effects.rootCount() == 1024, "total-node plus owned-root budget accounting");
    check(!effects.spawn(357, 0, 0, 0, 23, error) && error.find("budget") != std::string::npos &&
          effects.nodeCount() == 3072 && effects.rootCount() == 1024,
          "reserve delayed child capacity plus owned-root metadata before accepting another root");
    check(effects.remove(oldest) && effects.nodeCount() == 3069 && effects.rootCount() == 1023,
          "removal frees three Scene nodes and their reservation");
    spawn(effects);
    check(effects.clear() && effects.nodeCount() == 0 && effects.rootCount() == 0, "clear cannot retain resource metadata");
    std::cout << "cancellation effects synthetic regressions passed\n";
}

void exceptionCases() {
#if defined(__cpp_exceptions)
    CancellationEffects effects;
    const auto bytes = synthetic({{88, 0, {356}}, {63, 0, {}}},
                                  {{40, 0, {10000, 100}, 1}, {40, 0, {10001, 100}, 1}, {63, 0, {}}});
    loaded(effects, bytes);
    int calls = 0;
    effects.setGameRandom([&]() -> uint32_t { ++calls; throw std::runtime_error("source failure"); });
    std::string error;
    try {
        check(!effects.spawn(357, 0, 0, 0, 23, error) && error.find("RNG callback threw") != std::string::npos,
              "child callback throw must return a bounded error");
    } catch (...) {
        check(false, "callback exception escaped Scene cancellation spawn");
    }
    check(calls == 1 && effects.nodeCount() == 0 && effects.rootCount() == 0 && effects.snapshots().empty(),
          "throwing child cannot publish partial roots, leak descendants or recall source");
    check(effects.clear(), "callback throw releases reentry guard");
    effects.setGameRandom([] { return 0u; });
    loaded(effects, synthetic({{40, 0, {10000, 100}, 1}, {4, 1, {0, 0}}}));
    spawn(effects);
    effects.setGameRandom([]() -> uint32_t { throw 9; });
    check(!effects.tick(error) && error.find("RNG callback threw") != std::string::npos,
          "non-standard callback throw during a phase also fails closed");
    check(effects.nodeCount() == 0 && effects.rootCount() == 0 && effects.clear(), "tick failure releases every resource");
    std::cout << "cancellation effects callback-throw regressions passed\n";
#endif
}

template <typename Effects>
void nativeDriftCases(const Bytes& bytes) {
    std::string error;
    if constexpr (requires(Effects& candidate) {
        candidate.initializeNativeDrift(Handle{}, std::array<float, 3>{}, 1.f, std::array<float, 3>{}, error);
    }) {
        size_t count = 0;
        check(bool(std::cin >> count) && count > 0 && count <= 16, "bounded original-prefix golden count required");
        for (size_t index = 0; index < count; ++index) {
            std::array<uint32_t, 3> velocityWords{}, targetWords{};
            for (auto& word : velocityWords) check(bool(std::cin >> word), "native velocity word required");
            for (auto& word : targetWords) check(bool(std::cin >> word), "native prefix target word required");
            std::array<float, 3> velocity{}, target{}, start{};
            for (size_t axis = 0; axis < 3; ++axis) {
                velocity[axis] = th128::anm::asFloat(velocityWords[axis]);
                target[axis] = th128::anm::asFloat(targetWords[axis]);
            }
            Effects effects;
            loaded(effects, bytes);
            int calls = 0;
            effects.setGameRandom([&] { ++calls; return 0u; });
            const auto handle = spawn(effects), baseline = spawn(effects);
            check(!effects.initializeNativeDrift(handle, velocity, .5f, start, error), "unknown native clock must fail");
            check(!effects.initializeNativeDrift(handle, velocity, 1.f, {1, 0, 0}, error), "nonzero global start must fail");
            check(!effects.initializeNativeDrift(handle, velocity, 1.f, {-0.f, 0, 0}, error), "global start must be explicit positive zero");
            check(!effects.initializeNativeDrift(handle, {INFINITY, 0, 0}, 1.f, start, error), "non-finite velocity must fail");
            check(!effects.initializeNativeDrift(handle, {std::numeric_limits<float>::max(), 0, 0}, 1.f, start, error),
                  "overflowing native target must fail before changing the track");
            check(effects.initializeNativeDrift(handle, velocity, 1.f, start, error), error);
            check(root(effects, handle).state.ticks == 1 && root(effects, handle).state.x == 0 && calls == 20,
                  "native initializer changes only the position track, never ticks or consumes RNG");
            check(!effects.initializeNativeDrift(handle, velocity, 1.f, start, error), "already-initialized root is outside bounded fresh context");
            check(effects.setRootPositionInterpolation(baseline, target[0], target[1], target[2], 30, 6, error), error);
            for (int pass = 1; pass <= 30; ++pass) {
                check(effects.tick(error), error);
                const auto actual = root(effects, handle), expected = root(effects, baseline);
                check(th128::anm::asWord(actual.state.x) == th128::anm::asWord(expected.state.x) &&
                      th128::anm::asWord(actual.state.y) == th128::anm::asWord(expected.state.y) &&
                      th128::anm::asWord(actual.state.z) == th128::anm::asWord(expected.state.z),
                      "bounded initializer must delegate to the existing VM track with native prefix targets");
            }
            const auto final = root(effects, handle);
            near(final.state.x, target[0]); near(final.state.y, target[1]); near(final.state.z, target[2]);
            check(!effects.initializeNativeDrift(handle, velocity, 1.f, start, error), "non-fresh/nonzero script position must fail");
            check(effects.remove(handle) && !effects.initializeNativeDrift(handle, velocity, 1.f, start, error),
                  "stale handle cannot initialize a different root");
            check(effects.remove(baseline) && effects.rootCount() == 0 && effects.nodeCount() == 0,
                  "drift initialization must not retain extra resources");
        }
        std::cout << "guarded native drift prefix goldens passed\n";
    } else {
        check(false, "guarded native cancellation drift API is not implemented");
    }
}

void unifiedSyntheticCases() {
    const std::vector<Instruction> randomLoop{{40, 0, {10000, 1000}, 1}, {3, 0, {0}}, {4, 1, {0, 0}}};
    const auto bytes = synthetic({{40, 0, {10000, 1000}, 1}, {88, 1, {356}}, {63, 1, {}}}, randomLoop, 16,
                                 {{83, randomLoop}, {84, randomLoop}, {175, randomLoop}});
    CancellationEffects effects;
    loaded(effects, bytes);
    effects.setGameRandom([value = 0u]() mutable { return value++; });
    std::string error;
    const auto cancel = spawn(effects);
    auto tail = [&](int script, int layer) {
        const auto handle = effects.spawn(script, 332, 40, 5, layer, error);
        check(handle.has_value(), "known primary-tail script must spawn: " + std::to_string(script) + "; " + error);
        return *handle;
    };
    const auto hit83 = tail(83, 0), hit84 = tail(84, 0), graze = tail(175, 23);
    check(root(effects, cancel).state.integers[0] == 0 && root(effects, hit83).state.integers[0] == 1 &&
          root(effects, hit84).state.integers[0] == 2 && root(effects, graze).state.integers[0] == 3,
          "all known tail births share one callable-owned RNG stream");
    for (const auto handle : {hit83, hit84, graze})
        check(!effects.initializeNativeDrift(handle, {1, 2, 3}, 1, {0, 0, 0}, error),
              "native cancellation drift is forbidden on non357 roots");
    check(effects.tick(error), error);
    const auto child = children(effects, cancel).at(0);
    check(child.state.integers[0] == 8 && child.state.ticks == 2 &&
          root(effects, hit83).state.integers[0] == 5 && root(effects, hit84).state.integers[0] == 6 &&
          root(effects, graze).state.integers[0] == 7,
          "one Scene tail pass interleaves delayed child bind4, tails5/6/7, then child tick8; grouped passes must fail");
    check(effects.tickPrimary(error), error);
    check(root(effects, hit83).state.integers[0] == 9 && root(effects, hit84).state.integers[0] == 10 &&
          root(effects, graze).state.integers[0] == 11 && children(effects, cancel)[0].state.integers[0] == 12,
          "subsequent global tail pass retains birth insertion order across all scripts");
    check(effects.rootCount() == 4 && effects.nodeCount() == 5, "unified roots own exactly one shared child");
    check(effects.clear(), "unified clear");
    const std::vector<Instruction> hold{{63, 0, {}}};
    loaded(effects, synthetic({{88, 0, {356}}, {88, 0, {356}}, {63, 0, {}}}, randomLoop, 16,
                              {{83, hold}, {84, hold}, {175, hold}}));
    for (int index = 0; index < 2048; ++index) tail(index % 2 ? 84 : 175, index % 2 ? 0 : 23);
    check(effects.rootCount() == 2048 && effects.nodeCount() == 2048,
          "childless root reservations cost one node plus one root record, not cancellation's four units");
    check(!effects.spawn(83, 0, 0, 0, 0, error) && error.find("budget") != std::string::npos,
          "unified mixed-root total budget remains4096");
    check(effects.clear(), "unified budget cleanup");
    std::cout << "unified tail synthetic RNG ordering passed\n";
}

void unifiedActualCases(const Bytes& bytes) {
    CancellationEffects effects;
    loaded(effects, bytes);
    int calls = 0;
    effects.setGameRandom([&] { ++calls; return 0u; });
    std::string error;
    auto tail = [&](int script, int layer) {
        const auto handle = effects.spawn(script, 332, 40, 5, layer, error);
        check(handle.has_value(), "actual unified root=" + std::to_string(script) + "; " + error);
        return *handle;
    };
    const auto graze = tail(175, 23);
    check(root(effects, graze).state.sprite == 428 && root(effects, graze).state.layer == 16 && calls == 1,
          "actual175 authored sprite/layer and one shared game32 callback read; calls=" + std::to_string(calls));
    const auto hit = tail(83, 0);
    near(root(effects, hit).world.x, 332); near(root(effects, hit).world.y, 40);
    check(root(effects, hit).state.sprite == 446 && root(effects, hit).state.layer == 16 && calls == 1,
          "actual83 is nonrandom and preserves physical screen pose");
    for (int index = 0; index < 32; ++index) {
        const auto particle = tail(84, 0);
        check(root(effects, particle).state.sprite == 428 && root(effects, particle).state.layer == 16 &&
              root(effects, particle).membership == Membership::Primary, "actual84 hit particle binds primary-tail");
    }
    check(effects.rootCount() == 34 && effects.nodeCount() == 34 && calls == 129,
          "actual33-root hit effect plus graze has 32 four-read particles without child nodes");
    const auto retained = effects.snapshots();
    for (int pass = 1; pass <= 45; ++pass) {
        check(effects.tick(error), error);
        if (pass == 12) check(effects.rootCount() == 33, "actual175 retires at12 while all33 hit roots survive");
        if (pass == 28) check(effects.rootCount() == 1 && root(effects, hit).state.script == 83,
                              "zero-RNG actual84 duration20 plus wait8 retires all32 particles");
        if (pass == 30) check(effects.rootCount() == 0 && effects.nodeCount() == 0, "actual83 retires at30 with no metadata leak");
    }
    check(effects.snapshots().empty() && calls == 129 && retained.size() == 34 && retained[0].sprite,
          "retired mixed snapshots retain copied metadata without later RNG reads");
    th128::anm::Scene reference;
    check(reference.registry.loadBank(0, bytes.data(), bytes.size(), error), error);
    effects.clear();
    uint32_t actualCalls = 0, referenceCalls = 0;
    auto sequence = [](uint32_t& count) { const auto value = count++; return value == 0 ? 1u : value * 0x10000001u; };
    effects.setGameRandom([&] { return sequence(actualCalls); });
    reference.runtime.gameRandom32 = [&] { return sequence(referenceCalls); };
    reference.runtime.unsupported = [&](int, int, int, const char* reason) { error = reason; };
    auto compare = [&] {
        check(actualCalls == referenceCalls && effects.nodeCount() == reference.nodes().size(),
              "actual mixed roots/children consume RNG and retire exactly as one creation-ordered Scene");
        for (const auto& snapshot : effects.snapshots()) {
            const auto found = reference.nodes().find(snapshot.node);
            check(found != reference.nodes().end(), "unified Scene ID exists in reference traversal");
            const auto& expected = found->second->vm.state();
            check(snapshot.handle == found->second->owner && snapshot.state.script == expected.script &&
                  snapshot.state.ticks == expected.ticks && snapshot.state.clock == expected.clock &&
                  snapshot.state.integers == expected.integers && snapshot.state.primary == expected.primary &&
                  snapshot.state.sprite == expected.sprite && snapshot.state.layer == expected.layer,
                  "unified actual snapshot follows one mixed global tail, not per-kind groups");
            for (const auto pair : {std::pair{snapshot.state.x, expected.x}, {snapshot.state.y, expected.y},
                                    {snapshot.state.sx, expected.sx}, {snapshot.state.rotation, expected.rotation}})
                check(th128::anm::asWord(pair.first) == th128::anm::asWord(pair.second), "mixed-tail float state is bit-identical");
        }
    };
    check(effects.load(bytes.data(), bytes.size(), error), error);
    for (const int script : {357, 84, 175, 83, 357, 84}) {
        const int layer = script == 83 || script == 84 ? 0 : 23;
        const auto handle = tail(script, layer);
        check(reference.bind(handle, handle, 0, script, Membership::Primary, 332, 40, {}, false, nullptr, 0, 0, layer, false, 5),
              "mixed actual reference root binds");
        compare();
    }
    for (int pass = 0; pass < 64; ++pass) {
        check(effects.tick(error), error);
        reference.tickPrimary(); reference.collect();
        compare();
    }
    check(error.empty() && effects.rootCount() == 0 && effects.nodeCount() == 0, "mixed delayed actual357 and hit/graze roots all retire");
    std::cout << "unified actual175/83/84/357 global-tail and33-hit cleanup passed\n";
}

void actualCases(const char* path) {
    std::string error;
    std::ifstream file(path, std::ios::binary);
    check(bool(file), "cannot open actual bullet.anm");
    const Bytes bytes{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
    CancellationEffects effects;
    loaded(effects, bytes);
    int calls = 0;
    effects.setGameRandom([&] { ++calls; return 0u; });
    const auto handle = spawn(effects);
    const auto birth = root(effects, handle);
    const auto bornChildren = children(effects, handle);
    check(effects.rootCount() == 1 && effects.nodeCount() == 3 && bornChildren.size() == 2 && calls == 10,
          "actual357 startup creates two real356 children, with ten actual game RNG reads for zero source");
    check(birth.state.script == 357 && birth.state.clock == -39 && birth.state.ticks == 1 &&
          birth.state.layer == 23 && !birth.sprite, "actual357 root waits without an invented sprite");
    for (const auto& child : bornChildren) {
        check(child.state.script == 356 && child.state.sprite == 580 && child.state.ticks == 1 &&
              child.membership == Membership::Primary && child.state.mode == 1 && child.state.layer == 6 &&
              child.state.primary == 0xff00ff00u && child.sprite && child.sprite->width == 32,
              "actual356 executes geometry/color/layer/sprite commands at startup");
        near(child.state.engineX, 0); near(child.world.x, 331.98f); near(child.world.y, 39.98f); near(child.world.z, 5);
    }
    check(!effects.requestNativeDrift(handle, error), "native drift remains explicitly unhandled");
    check(effects.setRootPositionInterpolation(handle, 12, -8, 4, 30, 6, error), error);
    for (int phase = 1; phase <= 40; ++phase) {
        check(effects.tick(error), error);
        if (phase < 40) {
            const auto current = children(effects, handle);
            check(current.size() == 2 && effects.rootCount() == 1 && effects.nodeCount() == 3,
                  "actual child counts remain owned through the effect lifetime");
            for (const auto& child : current)
                check(child.state.sprite == 580 + phase / 5 && child.state.ticks == phase + 1,
                      "actual356 time-stamped sprite progression, no age-selected replacement");
            check(root(effects, handle).membership == Membership::Primary &&
                  root(effects, handle).state.ticks == phase + 1,
                  "actual357 root advances once per shared primary pass, not a secondary8 pass");
            if (phase == 30) {
                near(root(effects, handle).world.x, 344); near(root(effects, handle).world.y, 32);
                near(root(effects, handle).world.z, 9);
            }
        }
    }
    check(effects.rootCount() == 0 && effects.nodeCount() == 0 && effects.snapshots().empty() && calls == 12,
          "actual357 retirement collects all356 descendants and root records without extra RNG");
    near(bornChildren[0].world.x, 331.98f);
    check(bornChildren[0].state.sprite == 580 && birth.state.ticks == 1, "retained snapshots survive retirement");
    std::vector<CancellationEffects::Snapshot> retained;
    {
        CancellationEffects temporary;
        loaded(temporary, bytes);
        temporary.setGameRandom([] { return 0u; });
        spawn(temporary);
        retained = temporary.snapshots();
    }
    check(retained.size() == 3 && retained[1].sprite.has_value(), "snapshot metadata survives sidecar destruction");
    th128::anm::Scene comparison;
    check(comparison.registry.loadBank(0, bytes.data(), bytes.size(), error), error);
    int comparisonCalls = 0;
    comparison.runtime.gameRandom32 = [&] { ++comparisonCalls; return 0u; };
    comparison.runtime.unsupported = [&](int, int, int, const char* reason) { error = reason; };
    check(comparison.bind(1, 1, 0, 361, Membership::Primary, 332, 40, {}, false, nullptr, 0, 0, 23),
          "actual361 comparison binds without relabeling child scripts");
    check(error.empty() && comparison.nodes().size() == 3 && comparisonCalls == 10,
          "actual361, not357, creates the separate360 pair");
    for (const auto& [id, node] : comparison.nodes())
        if (node->parent)
            check(node->vm.state().script == 360 && node->vm.state().primary == 0xffc0c000u,
                  "actual360 is a differently tinted program, not a substitute for356");
    for (int phase = 0; phase < 40; ++phase) {
        comparison.tickPrimary(); comparison.collect();
    }
    check(error.empty() && comparison.nodes().empty() && comparisonCalls == 12,
          "separate real361/360 Scene comparison also retires without leaks");
    CancellationEffects delayed;
    loaded(delayed, bytes);
    delayed.setGameRandom([first = true]() mutable { if (first) { first = false; return 1u; } return 0u; });
    const auto delayedHandle = spawn(delayed);
    check(delayed.nodeCount() == 1 && root(delayed, delayedHandle).state.ticks == 1,
          "actual357 can defer children after its synchronous birth tick");
    check(delayed.tick(error), error);
    check(delayed.nodeCount() == 3 && children(delayed, delayedHandle).size() == 2 &&
          root(delayed, delayedHandle).state.ticks == 2,
          "primary-tail root creates both deferred children in its actual VM pass");
    for (const auto& child : children(delayed, delayedHandle))
        check(child.membership == Membership::Primary && child.state.ticks == 1,
              "tail births after a saved end successor receive only their bind tick in this pass");
    check(delayed.tickPrimary(error), error);
    for (const auto& child : children(delayed, delayedHandle))
        check(child.state.ticks == 2, "next single primary pass advances deferred children once");
    nativeDriftCases<CancellationEffects>(bytes);
    unifiedActualCases(bytes);
    {
        CancellationEffects explosion;loaded(explosion,bytes);
        std::uint32_t calls=0;explosion.setGameRandom([&]{++calls;return 0u;});
        const auto handle=explosion.spawn(88,320,116,.1f,4,error);
        check(bool(handle),"original enemy-death ANM88 must construct its authored child tree: "+error);
        check(explosion.nodeCount()==10,"actual death root88 creates89/90 and seven97 children");
        th128::anm::Scene reference;
        check(reference.registry.loadBank(0,bytes.data(),bytes.size(),error),error);
        std::uint32_t referenceCalls=0;reference.runtime.gameRandom32=[&]{++referenceCalls;return 0u;};
        check(reference.bind(*handle,*handle,0,88,Membership::Primary,320,116,{},false,nullptr,0,0,4,false,.1f),"direct native-data reference binds");
        for(unsigned pass=0;pass<65;++pass){
            check(calls==referenceCalls&&explosion.nodeCount()==reference.nodes().size(),"death ANM preserves native shared-RNG/tree lifetime");
            check(explosion.tickPrimary(error),error);reference.tickPrimary();reference.collect();
        }
        check(explosion.nodeCount()==0,"authored death root and particles all retire");
    }
    std::cout << "cancellation effects actual357/356 passed: 40 primary passes; separate361/360 verified\n";
}

int main(int argc, char** argv) {
    check(argc == 2, "pass --synthetic, --throws or actual bullet.anm");
    if (std::string(argv[1]) == "--synthetic") syntheticCases();
    else if (std::string(argv[1]) == "--unified-synthetic") unifiedSyntheticCases();
    else if (std::string(argv[1]) == "--throws") exceptionCases();
    else actualCases(argv[1]);
}
