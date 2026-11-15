#pragma once

#include "EclProgram.hpp"

#include <functional>
#include <memory>
#include <list>
#include <optional>
#include <unordered_map>

namespace th128::ecl {

enum class ValueType { Integer, Float };

struct Value {
    std::uint32_t bits = 0;
    ValueType type = ValueType::Integer;

    static Value integer(std::int32_t value);
    static Value floating(float value);
    std::int32_t asInteger() const;
    float asFloat() const;
};

enum class VmStatus { Ready, Running, Finished, Faulted };

enum class ErrorCode {
    None, InvalidProgram, InvalidLimits, MissingSubroutine, InvalidInstruction,
    UnsupportedOpcode, UnsupportedReference, StackUnderflow, StackLimit,
    CallLimit, ThreadLimit, InstructionBudget, InvalidBranch, LocalBounds,
    NumericError, HostFailure, ReentrantCall
};

struct Error {
    ErrorCode code = ErrorCode::None;
    std::string message;
    std::string subroutine;
    std::uint32_t offset = 0;
    std::uint16_t opcode = 0;
    std::uint64_t threadId = 0;
};

struct Limits {
    std::size_t maxStackValues = 512;
    std::size_t maxLocalBytes = 4096;
    std::size_t maxCallDepth = 64;
    std::size_t maxThreads = 16;
    std::size_t maxInstructionsPerTick = 10000;
    std::size_t maxHostOpcodes = 256;
};

struct TickResult {
    VmStatus status = VmStatus::Ready;
    std::size_t instructions = 0;
    std::size_t skipped = 0;
};

struct ThreadSnapshot {
    std::uint64_t id = 0;
    std::int32_t asyncId = -1;
    bool active = false;
    std::string subroutine;
    std::uint32_t offset = 0;
    float time = 0;
    std::size_t callDepth = 0;
    std::vector<std::uint32_t> locals;
    std::vector<Value> stack;
};

struct CallbackRegistration {
    std::int32_t slot = 0;
    std::string subroutine;
};

class Vm;

class HostCall {
public:
    static constexpr std::size_t automaticParameter = static_cast<std::size_t>(-1);
    HostCall(const HostCall&) = delete;
    HostCall& operator=(const HostCall&) = delete;

    const Instruction& instruction() const;
    std::uint64_t threadId() const;
    float time() const;
    bool subtractTime(std::int32_t ticks);
    const CallbackRegistration* callbackRegistration() const;
    bool readInteger(std::size_t word, std::int32_t& value, std::size_t parameter = automaticParameter);
    bool readFloat(std::size_t word, float& value, std::size_t parameter = automaticParameter);
    bool readString(std::size_t word, std::string& value, std::size_t parameter = automaticParameter);
    bool storeInteger(std::int32_t byteOffset, std::int32_t value);
    bool storeFloat(std::int32_t byteOffset, float value);
    bool push(Value value);
    bool holdCurrentCommand();

private:
    friend class Vm;
    HostCall(Vm& vm, const Instruction& instruction, std::optional<CallbackRegistration> registration);
    Vm& vm_;
    const Instruction& instruction_;
    std::optional<CallbackRegistration> registration_;
    bool held_=false;
};

using HostOpcode = std::function<bool(HostCall&)>;
using ReferenceRead = std::function<std::optional<Value>(std::int32_t,bool)>;
using ReferenceWrite = std::function<bool(std::int32_t,Value)>;

class Vm {
public:
    explicit Vm(const Program& program, Limits limits = {});
    ~Vm();
    Vm(const Vm&) = delete;
    Vm& operator=(const Vm&) = delete;

    bool registerHostOpcode(std::uint16_t opcode, HostOpcode callback, std::string& error);
    bool enableOriginalHostMissDomain(std::string& error);
    bool enableOriginalAsyncRegistry(std::string& error);
    bool setReferenceResolver(ReferenceRead read,ReferenceWrite write,std::string& error);
    bool start(std::string_view entry, std::uint8_t rankMask = 1);
    TickResult runTick();
    VmStatus status() const;
    const Error& error() const;
    std::vector<ThreadSnapshot> threads() const;

private:
    friend class HostCall;
    struct Frame;
    struct Thread;

    bool fail(ErrorCode code, std::string message);
    bool execute(Thread& thread, const Instruction& instruction);
    bool executeHost(Thread& thread, const Instruction& instruction);
    bool numeric(Thread& thread, std::uint16_t opcode);
    bool invoke(Thread& thread, const Instruction& instruction);
    bool push(Thread& thread, Value value);
    bool pop(Thread& thread, Value& value);
    bool store(Thread& thread, std::int32_t offset, std::uint32_t bits,bool floating=false);
    bool readArgument(Thread& thread, const Instruction& instruction, std::size_t word,
                      std::size_t parameter, bool floating, Value& value);
    bool readString(const Instruction& instruction, std::size_t word, std::size_t parameter,
                    std::string& value, std::size_t& next);
    bool shape(const Instruction& instruction, std::size_t words, std::size_t parameters,
               std::uint16_t allowedMask = 0);
    bool limitsValid() const;
    static bool coreOpcode(std::uint16_t opcode);
    Thread* findThread(std::uint64_t id);
    void runThread(Thread& thread, TickResult& result);

    Program program_;
    Limits limits_;
    VmStatus status_ = VmStatus::Ready;
    Error error_;
    std::uint8_t rankMask_ = 1;
    bool executing_ = false;
    bool originalHostMissDomain_ = false;
    bool originalAsyncRegistry_ = false;
    std::uint64_t nextThreadId_ = 1;
    std::list<std::unique_ptr<Thread>> threads_;
    std::unordered_map<std::uint16_t, HostOpcode> hostOpcodes_;
    ReferenceRead referenceRead_;
    ReferenceWrite referenceWrite_;
    Thread* currentThread_ = nullptr;
    const Subroutine* currentSubroutine_ = nullptr;
    const Instruction* currentInstruction_ = nullptr;
};

}
