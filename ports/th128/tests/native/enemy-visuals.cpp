#include "game/EnemyVisuals.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>

using th128::enemy::EnemyVisuals;
using th128::enemy::VisibilityEffect;
using Bytes = std::vector<uint8_t>;

void check(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
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

Bytes synthetic(std::initializer_list<Instruction> commands, float size = 32) {
    Bytes bytes(128);
    put32(bytes, 0, 7);
    put16(bytes, 4, 2);
    put16(bytes, 6, 1);
    put16(bytes, 10, 64);
    put16(bytes, 12, 64);
    put32(bytes, 16, 80);
    put32(bytes, 64, 88);
    put32(bytes, 68, 108);
    put32(bytes, 72, 0);
    put32(bytes, 76, 128);
    bytes[80] = 't';
    for (size_t sprite = 0; sprite < 2; ++sprite) {
        put32(bytes, 88 + sprite * 20, uint32_t(sprite));
        put32(bytes, 100 + sprite * 20, th128::anm::asWord(size));
        put32(bytes, 104 + sprite * 20, th128::anm::asWord(size));
    }
    for (const auto& command : commands) {
        const size_t offset = bytes.size(), length = 8 + 4 * command.words.size();
        bytes.resize(offset + length);
        put16(bytes, offset, uint16_t(command.opcode));
        put16(bytes, offset + 2, uint16_t(length));
        put16(bytes, offset + 4, uint16_t(command.time));
        put16(bytes, offset + 6, command.mask);
        for (size_t index = 0; index < command.words.size(); ++index)
            put32(bytes, offset + 8 + index * 4, command.words[index]);
    }
    const size_t sentinel = bytes.size();
    bytes.resize(sentinel + 8);
    put16(bytes, sentinel, 65535);
    return bytes;
}

void loaded(EnemyVisuals& visuals, int bank, const Bytes& bytes) {
    std::string error = "stale error";
    check(visuals.load(bank, bytes.data(), bytes.size(), error), error);
    check(error.empty(), "successful load must clear error");
}

EnemyVisuals::Snapshot snapshot(const EnemyVisuals& visuals, uint32_t entity, int slot = 0) {
    for (const auto& value : visuals.snapshots())
        if (value.entity == entity && value.slot == slot)
            return value;
    check(false, "missing snapshot entity=" + std::to_string(entity) + " slot=" + std::to_string(slot));
    return {};
}

void syntheticCases() {
    const auto loop = synthetic({{3, 0, {0}}, {3, 5, {1}}, {4, 10, {0, 0}}});
    std::string error;
    EnemyVisuals overflow;
    loaded(overflow,1,loop);
    loaded(overflow,2,synthetic({{102,0,{0x7fffffff,2}},{63,0,{}}}));
    overflow.setRandomSources([]{return 1U;},{});
    check(overflow.create(7,0,1,0,7,error),error);
    check(!overflow.create(7,0,2,0,7,error),"wrapped negative random sprite must be rejected");
    check(snapshot(overflow,7).state.bank==1,"bad random sprite must preserve existing slot and root order");
    EnemyVisuals headOrder;
    const auto orderedRandom=synthetic({{40,0,{10000,100},1},{3,0,{0}},{4,1,{0,0}}});
    loaded(headOrder,1,orderedRandom);loaded(headOrder,2,orderedRandom);
    headOrder.setRandomSources([value=0U]()mutable{return value++;},{});
    check(headOrder.create(1,0,1,0,7,error)&&headOrder.create(2,0,2,0,7,error)&&headOrder.create(1,1,1,0,7,error),error);
    check(headOrder.tickAll(error),error);
    check(snapshot(headOrder,1,1).state.integers[0]==3&&snapshot(headOrder,2).state.integers[0]==4
        &&snapshot(headOrder,1).state.integers[0]==5,"interleaved owners/slots/banks require one global native head order");
    check(headOrder.create(1,0,1,0,7,error)&&headOrder.tickAll(error),error);
    check(snapshot(headOrder,1).state.integers[0]==7&&snapshot(headOrder,1,1).state.integers[0]==8
        &&snapshot(headOrder,2).state.integers[0]==9,"successful replacement becomes newest after its synchronous startup");
    loaded(headOrder,3,synthetic({{102,0,{0x7fffffff,2}},{63,0,{}}}));
    check(!headOrder.create(2,0,3,0,7,error)&&headOrder.tickAll(error),"bad replacement must retain live root and ordering");
    check(snapshot(headOrder,1).state.integers[0]==11&&snapshot(headOrder,1,1).state.integers[0]==12
        &&snapshot(headOrder,2).state.integers[0]==13,"failed startup consumes its RNG but never changes root order");
    headOrder.removeOwner(1);
    check(headOrder.tickAll(error)&&snapshot(headOrder,2).state.integers[0]==14,"head retirement cannot skip the surviving successor");
    EnemyVisuals visuals;
    check(!visuals.create(1, 0, 1, 0, 7, error) && !error.empty(), "unloaded bank must fail");
    check(visuals.load(0, loop.data(), loop.size(), error), "actual ECL can use bullet bank0 for secondary enemy animations");
    check(!visuals.load(-1, loop.data(), loop.size(), error), "negative bank is unsupported");
    check(!visuals.load(4, loop.data(), loop.size(), error), "unmapped include bank must fail");
    check(!visuals.load(1, nullptr, 0, error), "null resource must fail");
    loaded(visuals, 1, loop);
    for (const int slot : {-1, 16})
        check(!visuals.create(1, slot, 1, 0, 7, error), "out-of-range slot must fail");
    for (const int script : {-1, 1})
        check(!visuals.create(1, 0, 1, script, 7, error), "out-of-range script must fail");
    check(!visuals.create(1, 0, 4, 0, 7, error), "unmapped bank cannot create enemy roots");
    check(visuals.create(1, 0, 1, 0, 7, VisibilityEffect::Hide, error), error);
    const auto birth = snapshot(visuals, 1);
    check(birth.state.sprite == 0 && birth.state.clock == 1 && birth.state.ticks == 1,
          "bind executes one synchronous tick, not two");
    check(birth.state.layer == 7 && birth.state.engineX == 0 && birth.state.engineY == 0 &&
          birth.state.engineZ == 0 && !(birth.state.flags & 2), "zero bind pose and post-bind Hide");
    check(birth.sprite && birth.sprite->width == 32 && birth.sprite->height == 32,
          "snapshot carries copied real sprite metadata");
    check(visuals.visibility(1, VisibilityEffect::Show), "show existing roots");
    check(snapshot(visuals, 1).state.flags & 2, "show controls flag2, not script visible");
    check(!visuals.position(99, 1, 2, 3), "position does not create phantom owners");
    check(visuals.position(1, 11, 22, 33), "caller sets XYZ");
    check(!visuals.position(1, std::numeric_limits<float>::quiet_NaN(), 2, 3), "reject non-finite pose");
    check(snapshot(visuals, 1).state.engineX == 11 && snapshot(visuals, 1).state.engineZ == 33,
          "invalid position does not partially mutate pose");
    for (int index = 0; index < 4; ++index)
        check(visuals.tickAll(error), error);
    check(snapshot(visuals, 1).state.sprite == 0, "sprite0 persists before time5");
    check(visuals.tickAll(error), error);
    check(snapshot(visuals, 1).state.sprite == 1, "time5 command selects sprite1");
    check(snapshot(visuals, 1).state.engineY == 22, "ticks do not invent world motion");
    check(birth.state.sprite == 0 && birth.state.engineX == 0, "snapshot reads are independent");

    const auto copyEngine = synthetic({{83, 0, {}}, {3, 0, {0}}, {63, 0, {}}});
    loaded(visuals, 2, copyEngine);
    check(visuals.create(1, 15, 2, 0, 9, error), error);
    const auto second = snapshot(visuals, 1, 15);
    check(second.state.x == 0 && second.state.y == 0 && second.state.z == 0 &&
          second.state.engineX == 11 && second.state.engineZ == 33,
          "existing owner pose is applied only after zero-engine startup");
    check(!visuals.create(1, 15, 2, 99, 9, error), "invalid replacement fails");
    check(snapshot(visuals, 1, 15).state.sprite == 0, "invalid replacement keeps old slot");
    check(!visuals.load(1, loop.data(), 8, error), "malformed reload fails");
    check(visuals.snapshots().size() == 2 && snapshot(visuals, 1).state.sprite == 1,
          "malformed reload preserves bank and live roots");
    const auto oldSprite = visuals.spriteInfo(1, 0);
    const auto larger = synthetic({{3, 0, {0}}, {63, 0, {}}}, 64);
    loaded(visuals, 1, larger);
    check(visuals.snapshots().size() == 1 && snapshot(visuals, 1, 15).state.bank == 2,
          "successful reload removes only the replaced bank's roots");
    check(oldSprite && oldSprite->width == 32 && visuals.spriteInfo(1, 0)->width == 64,
          "copied sprite metadata survives reload");
    check(!visuals.spriteInfo(4, 0) && !visuals.spriteInfo(1, -1) && !visuals.spriteInfo(1, 2),
          "sprite lookup rejects unloaded banks and invalid indices");
    visuals.removeOwner(1);
    check(visuals.snapshots().empty() && visuals.tickAll(error), "owner removal is idempotent and tick safe");
    visuals.removeOwner(1);
    check(!visuals.visibility(1, VisibilityEffect::Hide), "visibility does not resurrect removed roots");

    auto failedCreate = [&](const Bytes& bytes, const std::string& reason) {
        EnemyVisuals failed;
        loaded(failed, 1, bytes);
        check(!failed.create(7, 0, 1, 0, 7, error) && error.find(reason) != std::string::npos,
              "failure must be explicit: " + reason + "; got " + error);
        check(failed.snapshots().empty(), "failed startup cannot leak a renderable root");
    };
    failedCreate(synthetic({{999, 0, {}}}), "opcode");
    failedCreate(synthetic({{88, 0, {0}}}), "child");
    EnemyVisuals missingControl;
    loaded(missingControl,1,synthetic({{111,0,{1}},{3,0,{0}},{63,0,{}}}));
    check(missingControl.create(7,0,1,0,7,error),error);
    check(!missingControl.tickAll(error)&&error.find("control snapshot")!=std::string::npos,
          "111 mode1 without declared external control stops at its real next-tick consumer");
    const auto gateFailure=error;
    check(missingControl.snapshots().empty()&&!missingControl.tickAll(error)&&error==gateFailure,
          "missing native control is a sticky non-renderable owner failure");
    check(missingControl.removeOwner(7)&&missingControl.tickAll(error),"owner retirement releases its gate failure");
    failedCreate(synthetic({{4, 0, {0, 0}}}), "budget");
    failedCreate(synthetic({{3, 0, {2}}, {63, 0, {}}}), "sprite");
    failedCreate(synthetic({{3, 0, {uint32_t(-1)}}, {63, 0, {}}}), "sprite");
    failedCreate(synthetic({{102, 0, {0, 2}}, {63, 0, {}}}), "RNG");
    failedCreate(synthetic({{48, 0, {th128::anm::asWord(10016), 0, 0}, 1}, {63, 0, {}}}), "camera");
    EnemyVisuals integerCamera;
    loaded(integerCamera,1,synthetic({{6,0,{10000,0x461c8000},3},{3,0,{0}},{63,0,{}}}));
    check(integerCamera.create(10,0,1,0,7,error),error);
    check(snapshot(integerCamera,10).state.integers[0]==0x461c8000,"integer bit pattern cannot be mistaken for a float camera reference");

    EnemyVisuals fault;
    loaded(fault, 1, synthetic({{3, 0, {0}}, {999, 1, {}}}));
    check(fault.create(44, 0, 1, 0, 7, error), error);
    check(!fault.tickAll(error) && !error.empty(), "deferred unknown instruction must fail tick");
    const auto failure = error;
    check(fault.snapshots().empty() && !fault.tickAll(error) && error == failure,
          "fault is hidden and latched instead of silently collecting a broken VM");
    fault.removeOwner(44);
    check(fault.tickAll(error) && error.empty(), "removing failed root clears its stop");

    EnemyVisuals random;
    const auto randomSprite = synthetic({{87, 0, {1}}, {102, 0, {0, 2}}, {63, 0, {}}});
    loaded(random, 1, randomSprite);
    int gameCalls = 0, displayCalls = 0;
    random.setRandomSources([&] { ++gameCalls; return 1u; }, [&] { ++displayCalls; return 0u; });
    check(random.create(0, 0, 1, 0, 7, error), error);
    check(snapshot(random, 0).state.sprite == 1 && gameCalls == 1 && displayCalls == 0,
          "op102 explicitly consumes game RNG even in display domain");
    for (int index = 0; index < 5; ++index) {
        random.snapshots();
        random.spriteInfo(1, 0);
    }
    check(gameCalls == 1 && displayCalls == 0, "reads must not execute VM or RNG");
    const auto displayRandom = synthetic({{87, 0, {1}}, {40, 0, {10000, 2}, 1},
                                          {3, 0, {10000}, 1}, {63, 0, {}}});
    loaded(random, 2, displayRandom);
    random.setRandomSources([&] { ++gameCalls; return 0u; }, {});
    check(!random.create(0, 1, 2, 0, 7, error) && error.find("RNG") != std::string::npos,
          "display RNG must not silently fall back to game RNG");
    random.setRandomSources([&] { ++gameCalls; return 0u; }, [&] { ++displayCalls; return 1u; });
    check(random.create(0, 1, 2, 0, 7, error), error);
    check(snapshot(random, 0, 1).state.sprite == 1 && gameCalls == 1 && displayCalls == 1,
          "masked variables use explicit display RNG, without sprite remapping");

    EnemyVisuals ownedRandom;
    loaded(ownedRandom, 1, synthetic({{40, 0, {10000, 100}, 1}, {3, 0, {0}}, {4, 1, {0, 0}}}));
    loaded(ownedRandom, 2, synthetic({{87, 0, {1}}, {40, 0, {10000, 100}, 1},
                                     {3, 0, {0}}, {4, 1, {0, 0}}}));
    ownedRandom.setRandomSources([value = 0u]() mutable { return value++; },
                                 [value = 10u]() mutable { return value++; });
    check(ownedRandom.create(50, 0, 1, 0, 7, error), error);
    check(ownedRandom.create(51, 0, 1, 0, 7, error), error);
    check(snapshot(ownedRandom, 50).state.integers[0] == 0 && snapshot(ownedRandom, 51).state.integers[0] == 1,
          "game RNG mutable closure state must survive distinct birth runtimes");
    check(ownedRandom.create(52, 0, 2, 0, 7, error), error);
    check(ownedRandom.create(53, 0, 2, 0, 7, error), error);
    check(snapshot(ownedRandom, 52).state.integers[0] == 10 && snapshot(ownedRandom, 53).state.integers[0] == 11,
          "display RNG mutable closure state must survive distinct birth runtimes");
    check(ownedRandom.tickAll(error), error);
    check(snapshot(ownedRandom, 50).state.integers[0] == 3 && snapshot(ownedRandom, 51).state.integers[0] == 2 &&
          snapshot(ownedRandom, 52).state.integers[0] == 13 && snapshot(ownedRandom, 53).state.integers[0] == 12,
          "scheduled roots consume both RNG domains in original newest-first global order");

    EnemyVisuals deletion;
    loaded(deletion, 1, synthetic({{3, 0, {0}}, {1, 1, {}}}));
    check(deletion.create(1, 0, 1, 0, 7, error), error);
    check(deletion.tickAll(error) && deletion.snapshots().empty(), "delete opcode retires a registered root");
    loaded(deletion, 1, synthetic({{3, 0, {0}}, {2, 0, {}}}));
    check(deletion.create(1, 0, 1, 0, 7, error) && deletion.snapshots().empty(),
          "ended startup is not registered as a live drawable");

    EnemyVisuals bounded;
    loaded(bounded, 1, loop);
    for (uint32_t index = 0; index < 4096; ++index)
        check(bounded.create(index / 16, int(index % 16), 1, 0, 7, error), error);
    check(!bounded.create(999, 0, 1, 0, 7, error) && error.find("budget") != std::string::npos,
          "4097th live root is refused");
    check(bounded.create(0, 0, 1, 0, 7, error) && bounded.snapshots().size() == 4096,
          "replacing a slot at the node cap is allowed");
    bounded.removeOwner(0);
    check(bounded.snapshots().size() == 4080 && bounded.create(999, 0, 1, 0, 7, error),
          "owner removal releases every slot and its budget");
    std::cout << "enemy visuals synthetic regressions passed\n";
}

Bytes read(const char* path) {
    std::ifstream file(path, std::ios::binary);
    check(bool(file), "cannot open actual ANM resource");
    return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
}

void exceptionCases() {
#if defined(__cpp_exceptions)
    EnemyVisuals visuals;
    loaded(visuals, 1, synthetic({{40, 0, {10000, 100}, 1}, {40, 0, {10001, 100}, 1},
                                 {3, 0, {0}}, {4, 1, {0, 0}}}));
    std::string error;
    int calls = 0;
    visuals.setRandomSources([&]() -> uint32_t { ++calls; throw std::runtime_error("source failed"); }, {});
    try {
        check(!visuals.create(1, 0, 1, 0, 7, error) && error.find("RNG callback threw") != std::string::npos,
              "throwing birth callback must return an explicit failure");
    } catch (...) {
        check(false, "RNG exception escaped enemy ANM create");
    }
    check(visuals.snapshots().empty() && calls == 1, "throw cannot publish a candidate or recall the broken source");
    visuals.setRandomSources([value = 0u]() mutable { return value++; }, {});
    check(visuals.create(1, 0, 1, 0, 7, error), "birth throw must release the reentry guard");
    visuals.setRandomSources([]() -> uint32_t { throw 7; }, {});
    check(!visuals.tickAll(error) && error.find("RNG callback threw") != std::string::npos,
          "non-standard tick callback exception is a failure too");
    const auto failure = error;
    check(visuals.snapshots().empty() && !visuals.tickAll(error) && error == failure,
          "callback throw is hidden and latched until owner removal");
    check(visuals.removeOwner(1) && visuals.tickAll(error), "failed tick must release the reentry guard");
    loaded(visuals, 2, synthetic({{87, 0, {1}}, {40, 0, {10000, 100}, 1}, {63, 0, {}}}));
    visuals.setRandomSources({}, []() -> uint32_t { throw std::runtime_error("display source failed"); });
    check(!visuals.create(2, 0, 2, 0, 7, error) && error.find("display RNG callback threw") != std::string::npos,
          "display callback exceptions fail closed independently of game RNG");
    std::cout << "enemy visuals callback-throw regressions passed\n";
#endif
}

void actualCases(char** paths) {
    EnemyVisuals visuals;
    const auto first = read(paths[0]);
    const auto second = read(paths[1]);
    const auto boss = read(paths[2]);
    const auto bullet = read(paths[3]);
    loaded(visuals, 1, first);
    loaded(visuals, 2, second);
    loaded(visuals, 3, boss);
    loaded(visuals, 0, bullet);
    std::string error;
    check(!visuals.create(42, 0, 1, 78, 7, error), "first bank contains 78 scripts, not second include's six");
    check(visuals.create(42, 0, 1, 0, 7, error), error);
    const auto birth = snapshot(visuals, 42);
    check(birth.state.sprite == 0 && birth.state.ticks == 1 && birth.state.clock == 1 &&
          birth.state.layer == 8 && birth.sprite && birth.sprite->width == 32 && birth.sprite->height == 32,
          "actual first GirlA1b bank/script startup and layer command");
    check(visuals.position(42, -123, 84, 5), "explicit enemy world pose");
    for (int tick = 1; tick <= 1024; ++tick) {
        check(visuals.tickAll(error), error);
        const auto current = snapshot(visuals, 42);
        check(current.state.sprite == (tick / 5) % 4,
              "actual script0 scheduled loop at tick=" + std::to_string(tick));
        check(current.state.engineX == -123 && current.state.engineY == 84 && current.state.engineZ == 5,
              "host world pose is stable across actual VM animation");
        check(current.sprite && current.sprite->width == 32 && current.state.ticks == tick + 1,
              "snapshot state and metadata follow real commands");
    }
    for (const int script : {3, 4, 68, 77}) {
        check(visuals.create(43, 0, 1, script, 7, error), error);
        const int expected = script == 3 || script == 4 ? 7 : script == 68 ? 156 : 175;
        check(snapshot(visuals, 43).state.sprite == expected, "actual first bank script index selection");
    }
    check(visuals.create(44, 0, 2, 0, 7, error), error);
    check(snapshot(visuals, 44).sprite->width == 64, "bank2 explicitly holds enemy_ll");
    check(visuals.create(45, 0, 3, 0, 7, error), error);
    check(snapshot(visuals, 45).sprite->width == 96, "bank3 explicitly holds boss01");
    check(birth.state.sprite == 0 && birth.state.engineX == 0 && birth.sprite->width == 32,
          "retained birth snapshot never aliases live or replaced VMs");
    EnemyVisuals aura;loaded(aura,0,bullet);
    check(aura.setUpdateControl(th128::anm::UpdateControl{true,0}),"explicit native ANM control snapshot");
    check(aura.create(100,1,0,99,7,error),error);
    check(snapshot(aura,100,1).state.flags2&0x40u,"actual boss aura retains authored111 mode1");
    for(int tick=0;tick<90;++tick)check(aura.tickAll(error),error);
    const auto running=snapshot(aura,100,1).state;
    check(running.ticks==91&&running.sprite==454,"actual99 aura executes its own authored timeline");
    check(aura.setUpdateControl(th128::anm::UpdateControl{true,2}),"native bit1 holds111 mode1");
    for(int tick=0;tick<20;++tick)check(aura.tickAll(error),error);
    const auto paused=snapshot(aura,100,1).state;
    check(paused.ticks==running.ticks&&paused.clock==running.clock&&paused.sx==running.sx&&paused.sy==running.sy&&paused.rotation==running.rotation,
        "real aura control gate freezes clocks and interpolation, not merely its draw");
    check(aura.setUpdateControl(th128::anm::UpdateControl{true,0})&&aura.tickAll(error),error);
    check(snapshot(aura,100,1).state.ticks==running.ticks+1,"cleared native control bit resumes actual authored aura");
    std::cout << "enemy visuals actual banks passed: 78 scripts, 1024 scheduled ticks\n";
}

int main(int argc, char** argv) {
    if (argc == 2 && std::string(argv[1]) == "--synthetic")
        syntheticCases();
    else if (argc == 2 && std::string(argv[1]) == "--rng-throws")
        exceptionCases();
    else {
        check(argc == 5, "pass --synthetic or enemy.anm enemy_ll.anm boss01.anm bullet.anm");
        actualCases(argv + 1);
    }
}
