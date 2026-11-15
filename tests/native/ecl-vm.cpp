#include "ecl/EclVm.hpp"
#include "../ecl-fixture.hpp"

#include <cmath>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <limits>

using namespace ecl_fixture;
using namespace th128::ecl;

Program load(const std::vector<std::pair<std::string, Bytes>>& subs) {
    const auto bytes = program(subs);
    Program result;
    std::string error;
    require(result.load(bytes.data(), bytes.size(), error), error);
    return result;
}

Program single(const Bytes& bytes) { return load({{"main", bytes}}); }

ThreadSnapshot first(const Vm& vm) {
    require(!vm.threads().empty(), "missing main thread snapshot");
    return vm.threads()[0];
}

void fault(const Bytes& bytes, ErrorCode expected, Limits limits = {}) {
    Vm vm(single(bytes), limits);
    require(vm.start("main"), "fault fixture start");
    require(vm.runTick().status == VmStatus::Faulted, "fault fixture did not halt");
    require(vm.error().code == expected, "wrong fault: " + vm.error().message);
    const auto diagnostic = vm.error().message;
    require(vm.runTick().instructions == 0 && vm.error().message == diagnostic, "fault must be sticky");
}

void numericTests() {
    const auto binary = [&](std::uint16_t opcode, bool floating, std::uint32_t left,
                            std::uint32_t right, std::uint32_t expected) {
        Vm vm(single(join({instruction(floating ? 44 : 42, {left}),
                           instruction(floating ? 44 : 42, {right}), instruction(opcode), instruction(10)})));
        require(vm.start("main") && vm.runTick().status == VmStatus::Finished, "binary numeric run");
        const auto thread = first(vm);
        require(thread.stack.size() == 1 && thread.stack[0].bits == expected,
                "binary numeric opcode " + std::to_string(opcode));
    };
    binary(50, false, 0x7fffffff, 1, 0x80000000);
    binary(52, false, 0, 1, 0xffffffff);
    binary(54, false, 0xffffffff, 2, 0xfffffffe);
    binary(56, false, static_cast<std::uint32_t>(-7), 3, static_cast<std::uint32_t>(-2));
    binary(58, false, static_cast<std::uint32_t>(-7), 3, 0xffffffff);
    binary(51, true, floatBits(3.5f), floatBits(2.f), floatBits(5.5f));
    binary(53, true, floatBits(3.5f), floatBits(2.f), floatBits(1.5f));
    binary(55, true, floatBits(3.5f), floatBits(2.f), floatBits(7.f));
    binary(57, true, floatBits(7.f), floatBits(2.f), floatBits(3.5f));
    for (std::uint16_t opcode = 59; opcode <= 70; ++opcode) {
        const bool floating = (opcode & 1) == 0;
        const auto expected = opcode >= 63 && opcode <= 66 ? 1u :
                              opcode == 61 || opcode == 62 ? 1u : 0u;
        binary(opcode, floating, floating ? floatBits(2.f) : 2u,
               floating ? floatBits(3.f) : 3u, expected);
    }
    binary(73, false, 0, 5, 1);
    binary(74, false, 0, 5, 0);
    binary(75, false, 6, 3, 5);
    binary(76, false, 6, 3, 7);
    binary(77, false, 6, 3, 2);
    Vm unary(single(join({instruction(42, {0}), instruction(71), instruction(42, {7}), instruction(71),
                          instruction(44, {floatBits(0.f)}), instruction(72),
                          instruction(42, {0x80000000}), instruction(84),
                          instruction(44, {floatBits(3.5f)}), instruction(85), instruction(10)})));
    require(unary.start("main") && unary.runTick().status == VmStatus::Finished, "unary run");
    const auto stack = first(unary).stack;
    require(stack.size() == 5 && stack[0].asInteger() == 1 && stack[1].asInteger() == 0 &&
            stack[2].asInteger() == 1 && stack[3].bits == 0x80000000 && stack[4].asFloat() == -3.5f,
            "TH128 71 is integer logical-not; TH128 85 is fchs, not TH12 raw-word negation");
    require(Value::floating(2147483648.f).asInteger() == std::numeric_limits<std::int32_t>::min() &&
            Value::floating(-2.75f).asInteger() == -2, "defined native integer conversion");
    fault(join({instruction(42, {1}), instruction(42, {0}), instruction(56)}), ErrorCode::NumericError);
    fault(join({instruction(42, {0x80000000}), instruction(42, {0xffffffff}), instruction(56)}), ErrorCode::NumericError);
    fault(join({instruction(44, {floatBits(1.f)}), instruction(44, {0}), instruction(57)}), ErrorCode::NumericError);
    fault(instruction(44, {0x7fc00000}), ErrorCode::NumericError);
    fault(instruction(50), ErrorCode::StackUnderflow);
    std::puts("PASS ECL numeric wrap, conversions, comparisons, TH128 differences and numeric faults");
}

void controlTests() {
    Vm locals(single(join({instruction(40, {8}), instruction(42, {123}), instruction(43, {0}, 0, 1),
                           instruction(42, {0}, 0, 1), instruction(44, {floatBits(2.5f)}),
                           instruction(45, {floatBits(4.f)}, 0, 1), instruction(44, {floatBits(4.f)}, 0, 1),
                           instruction(44, {floatBits(-1.f)}, 0, 1), instruction(10)})));
    require(locals.start("main") && locals.runTick().status == VmStatus::Finished, "masked locals run");
    const auto values = first(locals);
    require(values.locals.size() == 2 && values.locals[0] == 123 && values.locals[1] == floatBits(2.5f) &&
            values.stack.size() == 2 && values.stack[0].asInteger() == 123 && values.stack[1].asFloat() == 2.5f,
            "local byte addresses, float addresses and masked -1 typed pop");

    Vm timing(single(join({instruction(65000, {}, 0, 0, 2), instruction(42, {7}, 2), instruction(10, {}, 3)})));
    require(timing.start("main", 1), "timing start");
    require(timing.runTick().skipped == 1 && first(timing).time == 1.f, "rank skip and tick zero");
    require(timing.runTick().instructions == 0 && first(timing).time == 2.f, "future instruction wait");
    require(timing.runTick().status == VmStatus::Running && first(timing).stack[0].asInteger() == 7, "due time");
    require(timing.runTick().status == VmStatus::Finished, "delayed return");
    Vm wait(single(join({instruction(83, {2}), instruction(42, {9}), instruction(10)})));
    require(wait.start("main") && wait.runTick().status == VmStatus::Running && first(wait).time == -1.f,
            "wait subtracts integer duration before tick advance");
    require(wait.runTick().instructions == 0 && wait.runTick().status == VmStatus::Finished, "wait resumes");

    Vm branch(single(join({instruction(42, {0}), instruction(13, {56, 5}), instruction(65001),
                           instruction(0), instruction(42, {17}, 5), instruction(10, {}, 5)})));
    require(branch.start("main") && branch.runTick().status == VmStatus::Finished, "relative jump/time reset");
    require(first(branch).stack.size() == 1 && first(branch).stack[0].asInteger() == 17, "jump target boundary");
    Vm notTaken(single(join({instruction(42, {1}), instruction(13, {24, 0}), instruction(42, {5}), instruction(10)})));
    require(notTaken.start("main") && notTaken.runTick().status == VmStatus::Finished &&
            first(notTaken).stack[0].asInteger() == 5, "conditional pop and fallthrough");
    fault(instruction(12, {4, 0}), ErrorCode::InvalidBranch);
    fault(instruction(12, {24, 0}), ErrorCode::InvalidBranch);
    fault(instruction(12, {0xffffffff, 0}), ErrorCode::InvalidBranch);
    Limits budget;
    budget.maxInstructionsPerTick = 7;
    fault(instruction(12, {0, 0}), ErrorCode::InstructionBudget, budget);
    Limits stack;
    stack.maxStackValues = 1;
    fault(join({instruction(42, {1}), instruction(42, {2})}), ErrorCode::StackLimit, stack);
    fault(join({instruction(40, {4}), instruction(42, {4}, 0, 1)}), ErrorCode::LocalBounds);
    fault(join({instruction(40, {4}), instruction(42, {2}, 0, 1)}), ErrorCode::LocalBounds);
    fault(instruction(42, {0xffffd8f0}, 0, 1), ErrorCode::UnsupportedReference);
    fault(instruction(42, {0xffffffff}, 0, 1), ErrorCode::StackUnderflow);
    fault(instruction(43, {0}), ErrorCode::InvalidInstruction);
    fault(instruction(40, {4097}), ErrorCode::LocalBounds);
    fault(instruction(0, {}, 0, 0, 255, 0, 4), ErrorCode::InvalidInstruction);
    fault(instruction(42, {1}, 0, 2), ErrorCode::InvalidInstruction);
    fault(instruction(0), ErrorCode::InvalidBranch);
    fault(instruction(65535), ErrorCode::UnsupportedOpcode);
    std::puts("PASS ECL local/mask/rank/time/branch and bounded fail-stop fixtures");
}

void callTests() {
    auto arguments = stringWords("callee");
    arguments.insert(arguments.end(), {std::uint32_t('i') | (std::uint32_t('f') << 8), 7,
                                        std::uint32_t('f') | (std::uint32_t('i') << 8), floatBits(2.75f)});
    const auto main = join({instruction(40, {4}), instruction(42, {99}), instruction(43, {0}, 0, 1),
                            instruction(11, arguments, 0, 0, 255, 3), instruction(42, {0}, 0, 1), instruction(10)});
    const auto callee = join({instruction(40, {8}), instruction(44, {floatBits(0.f)}, 0, 1),
                              instruction(42, {4}, 0, 1), instruction(64000), instruction(10)});
    Vm vm(load({{"main", main}, {"callee", callee}}));
    int checks = 0;
    std::string error;
    require(vm.registerHostOpcode(64000, [&](HostCall& call) {
        require(call.instruction().opcode == 64000 && call.threadId() == 0, "host call identity");
        const auto current = first(vm);
        require(current.callDepth == 1 && current.locals[0] == floatBits(7.f) && current.locals[1] == 2 &&
                current.stack[0].asFloat() == 7.f && current.stack[1].asInteger() == 2, "typed call descriptors");
        ++checks;
        return true;
    }, error), error);
    require(vm.start("main") && vm.runTick().status == VmStatus::Finished && checks == 1, "call and return");
    require(first(vm).locals[0] == 99 && first(vm).stack.size() == 1 && first(vm).stack[0].asInteger() == 99,
            "caller local/operand stack restored");
    fault(instruction(11, stringWords("absent"), 0, 0, 255, 1), ErrorCode::MissingSubroutine);
    auto recursive = stringWords("main");
    Limits recursion;
    recursion.maxCallDepth = 2;
    fault(instruction(11, recursive, 0, 0, 255, 1), ErrorCode::CallLimit, recursion);
    auto malformed = stringWords("main");
    malformed.insert(malformed.end(), {std::uint32_t('i') | (std::uint32_t('f') << 8), 7, 0x7a7a, 0});
    fault(instruction(11, malformed, 0, 0, 255, 3), ErrorCode::InvalidInstruction);
    fault(instruction(11, {128, 0}, 0, 0, 255, 1), ErrorCode::InvalidInstruction);
    std::puts("PASS ECL checked calls, argument conversions, stack isolation and call-depth bounds");
}

void asyncTests() {
    auto spawn = stringWords("worker");
    spawn.push_back(7);
    const auto worker = join({instruction(42, {5}), instruction(10)});
    const auto main = join({instruction(16, spawn, 0, 0, 255, 2), instruction(83, {2}), instruction(10)});
    Vm vm(load({{"main", main}, {"worker", worker}}));
    require(vm.start("main") && vm.runTick().status == VmStatus::Running, "spawn run");
    auto threads = vm.threads();
    require(threads.size() == 2 && threads[1].asyncId == 7 && threads[1].time == 0 && threads[1].stack.empty(),
            "bounded scheduler starts newly created threads next tick");
    require(vm.runTick().status == VmStatus::Running && vm.threads()[1].stack[0].asInteger() == 5,
            "async executes independently");
    require(vm.runTick().status == VmStatus::Finished, "all threads complete");
    Vm cancel(load({{"main", join({instruction(16, spawn, 0, 0, 255, 2), instruction(17, {7}), instruction(10)})},
                    {"worker", instruction(65535)}}));
    require(cancel.start("main") && cancel.runTick().status == VmStatus::Finished, "kill tagged async");
    Vm cancelAll(load({{"main", join({instruction(15, stringWords("worker"), 0, 0, 255, 1),
                                      instruction(21), instruction(10)})}, {"worker", instruction(65535)}}));
    require(cancelAll.start("main") && cancelAll.runTick().status == VmStatus::Finished, "kill all async");
    Limits threadsLimit;
    threadsLimit.maxThreads = 1;
    Vm limited(load({{"main", main}, {"worker", worker}}), threadsLimit);
    require(limited.start("main") && limited.runTick().status == VmStatus::Faulted &&
            limited.error().code == ErrorCode::ThreadLimit, "async limit");
    std::puts("PASS ECL asynchronous contexts, next-tick policy, cancellation and thread bound");
}

void hostTests() {
    auto registration = std::vector<std::uint32_t>{3};
    const auto name = stringWords("OnDebug");
    registration.insert(registration.end(), name.begin(), name.end());
    Vm vm(single(join({instruction(22, registration, 0, 0, 255, 2), instruction(10)})));
    std::string error;
    bool registered = false;
    require(vm.registerHostOpcode(22, [&](HostCall& call) {
        require(call.callbackRegistration() && call.callbackRegistration()->slot == 3 &&
                call.callbackRegistration()->subroutine == "OnDebug", "TH128 opcode 22 Sm registration");
        registered = true;
        return true;
    }, error), error);
    require(!vm.registerHostOpcode(42, [](HostCall&) { return true; }, error), "cannot override core opcodes");
    require(vm.start("main") && vm.runTick().status == VmStatus::Finished && registered, "accepted registration");
    registration[0] = 0xffffffff;
    Vm maskedRegistration(single(join({instruction(42, {3}), instruction(22, registration, 0, 1, 255, 2), instruction(10)})));
    require(maskedRegistration.registerHostOpcode(22, [](HostCall& call) {
        std::int32_t slot = 0;
        require(call.callbackRegistration() && call.callbackRegistration()->slot == 3 &&
                call.readInteger(0, slot) && slot == 3, "decoded registration slot is consumed exactly once");
        return true;
    }, error), error);
    require(maskedRegistration.start("main") && maskedRegistration.runTick().status == VmStatus::Finished &&
            first(maskedRegistration).stack.empty(), "masked callback registration slot cache");
    registration[0] = 3;
    fault(instruction(22, registration, 0, 0, 255, 2), ErrorCode::UnsupportedOpcode);
    fault(instruction(22, stringWords("WrongCallForm"), 0, 0, 255, 1), ErrorCode::InvalidInstruction);
    Vm rejecting(single(join({instruction(402, {32}), instruction(65535)})));
    int invocations = 0;
    require(rejecting.registerHostOpcode(402, [&](HostCall& call) {
        std::int32_t value = 0;
        require(call.readInteger(0, value) && value == 32, "host raw operands");
        ++invocations;
        return false;
    }, error), error);
    require(rejecting.start("main") && rejecting.runTick().status == VmStatus::Faulted && invocations == 1 &&
            rejecting.error().opcode == 402 && rejecting.error().offset == 0 && rejecting.error().subroutine == "main",
            "rejected host opcode failstop and structured error");
    Vm badHost(single(instruction(65000)));
    require(badHost.registerHostOpcode(65000, [](HostCall& call) {
        std::int32_t value = 0;
        call.readInteger(0, value);
        return true;
    }, error), error);
    require(badHost.start("main") && badHost.runTick().status == VmStatus::Faulted &&
            badHost.error().code == ErrorCode::InvalidInstruction, "host read failure cannot be ignored");
    Program source = single(instruction(10));
    Vm snapshot(source);
    const auto different = program({{"other", instruction(65535)}});
    require(source.load(different.data(), different.size(), error), error);
    require(snapshot.start("main") && snapshot.runTick().status == VmStatus::Finished, "VM immutable program snapshot");
    Vm invalid(Program{});
    require(!invalid.start("main") && invalid.error().code == ErrorCode::InvalidProgram, "unloaded program");
    Vm missing(single(instruction(10)));
    require(!missing.start("other") && missing.error().code == ErrorCode::MissingSubroutine, "entry missing");
    Limits invalidLimits;
    invalidLimits.maxInstructionsPerTick = 0;
    Vm zero(single(instruction(10)), invalidLimits);
    require(!zero.start("main") && zero.error().code == ErrorCode::InvalidLimits, "invalid bounds");
    std::puts("PASS ECL explicit host acceptance, Sm registration, sticky diagnostics and snapshot ownership");
}

void edgeTests() {
    Vm notStarted(single(instruction(10)));
    require(notStarted.runTick().status == VmStatus::Faulted && notStarted.error().code == ErrorCode::InvalidProgram,
            "tick before start must be an explicit misuse diagnostic");
    Vm precision(single(join({instruction(12, {24, 16777216}),
                              instruction(42, {99}, 16777217), instruction(10, {}, 16777217)})));
    require(precision.start("main") && precision.runTick().status == VmStatus::Faulted &&
            precision.error().code == ErrorCode::NumericError && first(precision).stack.empty(),
            "integer command times must not round down into the current float clock");
    Vm negativePrecision(single(join({instruction(12, {24, static_cast<std::uint32_t>(-16777218)}),
                                      instruction(42, {99}), instruction(10)})));
    require(negativePrecision.start("main") && negativePrecision.runTick().status == VmStatus::Faulted &&
            negativePrecision.error().code == ErrorCode::NumericError,
            "negative float clocks must not silently advance two frames for a one-frame tick");
    Vm decrement(single(join({instruction(40, {4}), instruction(78, {0}, 0, 1),
                              instruction(42, {0}, 0, 1), instruction(10)})));
    require(decrement.start("main") && decrement.runTick().status == VmStatus::Finished &&
            first(decrement).stack[0].bits == 0 && first(decrement).stack[1].bits == 0xffffffff,
            "post-decrement pushes old value and wraps stored integer");
    Vm free(single(join({instruction(40, {4}), instruction(41), instruction(40, {0}), instruction(10)})));
    require(free.start("main") && free.runTick().status == VmStatus::Finished && first(free).locals.empty(),
            "checked local free and zero allocation");
    fault(instruction(41), ErrorCode::LocalBounds);
    fault(join({instruction(40, {4}), instruction(40, {4})}), ErrorCode::LocalBounds);
    fault(join({instruction(42, {1}), instruction(13, {4, 0})}), ErrorCode::InvalidBranch);
    fault(instruction(45, {floatBits(0.5f)}, 0, 1), ErrorCode::LocalBounds);
    Limits skippedBudget;
    skippedBudget.maxInstructionsPerTick = 2;
    fault(join({instruction(65000, {}, 0, 0, 2), instruction(65000, {}, 0, 0, 2),
                instruction(65000, {}, 0, 0, 2), instruction(10)}), ErrorCode::InstructionBudget, skippedBudget);

    std::string error;
    Vm recursiveTick(single(instruction(65000)));
    require(recursiveTick.registerHostOpcode(65000, [&](HostCall&) {
        recursiveTick.runTick();
        return true;
    }, error), error);
    require(recursiveTick.start("main") && recursiveTick.runTick().status == VmStatus::Faulted &&
            recursiveTick.error().code == ErrorCode::ReentrantCall, "recursive host ticks failstop");
    Vm recursiveStart(single(instruction(65000)));
    require(recursiveStart.registerHostOpcode(65000, [&](HostCall&) {
        recursiveStart.start("main");
        return true;
    }, error), error);
    require(recursiveStart.start("main") && recursiveStart.runTick().status == VmStatus::Faulted &&
            recursiveStart.error().code == ErrorCode::ReentrantCall, "recursive host start cannot clear errors");
    Vm throwing(single(instruction(65000)));
    require(throwing.registerHostOpcode(65000, [](HostCall&) -> bool {
        throw std::runtime_error("host fixture exception");
    }, error), error);
    require(throwing.start("main") && throwing.runTick().status == VmStatus::Faulted &&
            throwing.error().code == ErrorCode::HostFailure, "host exception converted to failstop");

    auto masked = stringWords("callee");
    masked.insert(masked.end(), {std::uint32_t('i') | (std::uint32_t('i') << 8), 0});
    Vm descriptors(load({{"main", join({instruction(40, {4}), instruction(42, {777}), instruction(43, {0}, 0, 1),
                                         instruction(11, masked, 0, 2, 255, 2), instruction(10)})},
                          {"callee", join({instruction(40, {4}), instruction(65000), instruction(10)})}}));
    int checked = 0;
    require(descriptors.registerHostOpcode(65000, [&](HostCall&) {
        require(first(descriptors).locals[0] == 777, "mask uses logical parameter, not raw word index");
        ++checked;
        return true;
    }, error), error);
    require(descriptors.start("main") && descriptors.runTick().status == VmStatus::Finished && checked == 1,
            "typed masked argument fixture");

    Limits aggregate;
    aggregate.maxInstructionsPerTick = 4;
    const auto spawnWorker = instruction(15, stringWords("worker"), 0, 0, 255, 1);
    Vm aggregateBudget(load({{"main", join({spawnWorker, instruction(83, {1}), instruction(0), instruction(10)})},
                             {"worker", join({instruction(0), instruction(0), instruction(10)})}}), aggregate);
    require(aggregateBudget.start("main") && aggregateBudget.runTick().status == VmStatus::Running &&
            aggregateBudget.runTick().status == VmStatus::Faulted &&
            aggregateBudget.error().code == ErrorCode::InstructionBudget,
            "instruction allowance is shared across main and asynchronous contexts");
    Vm registryMutation(single(instruction(65000)));
    require(registryMutation.registerHostOpcode(65000, [&](HostCall&) {
        registryMutation.registerHostOpcode(65001, [](HostCall&) { return true; }, error);
        return true;
    }, error), error);
    require(registryMutation.start("main") && registryMutation.runTick().status == VmStatus::Faulted &&
            registryMutation.error().code == ErrorCode::ReentrantCall, "host cannot invalidate its active registry callback");
    std::puts("PASS ECL precision, decrement, local lifetime, rank budget, reentry and typed-mask edge cases");
}

void realVm(int count, char** paths) {
    int mains = 0;
    for (int index = 0; index < count; ++index) {
        std::ifstream input(paths[index], std::ios::binary);
        require(input.good(), "real ECL fixture unreadable");
        const Bytes bytes((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
        Program program;
        std::string error;
        require(program.load(bytes.data(), bytes.size(), error), error);
        if (!program.find("main"))
            continue;
        ++mains;
        Vm vm(program);
        require(vm.start("main"), "actual main start");
        const auto result = vm.runTick();
        require(result.status == VmStatus::Faulted && vm.error().code == ErrorCode::UnsupportedOpcode &&
                vm.error().opcode == 402 && vm.error().offset == 20 && result.instructions == 2,
                std::string(paths[index]) + " expected first explicit unsupported host opcode 402, before 22: " + vm.error().message);
    }
    require(count == 33 && mains == 16, "real VM corpus count");
    std::puts("PASS ECL 33 resources loaded; 16 actual mains stop explicitly at opcode 402 offset 20, before 22");
}

int main(int argc, char** argv) {
    try {
        if (argc > 1)
            realVm(argc - 1, argv + 1);
        else {
            numericTests();
            controlTests();
            callTests();
            asyncTests();
            hostTests();
            edgeTests();
        }
        return 0;
    } catch (const std::exception& error) {
        std::fprintf(stderr, "FAIL ECL VM: %s\n", error.what());
        return 1;
    }
}
