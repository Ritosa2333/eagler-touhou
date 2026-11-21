#include "EclVm.hpp"
#include "HostResultRules.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <exception>
#include <iterator>
#include <limits>
#include <utility>

namespace th128::ecl {
namespace {

static_assert(sizeof(float) == sizeof(std::uint32_t) && std::numeric_limits<float>::is_iec559);

std::int32_t signedBits(std::uint32_t bits) {
    std::int32_t result;
    std::memcpy(&result, &bits, sizeof(result));
    return result;
}

float floatingBits(std::uint32_t bits) {
    float result;
    std::memcpy(&result, &bits, sizeof(result));
    return result;
}

}

struct Vm::Frame {
    const Subroutine* subroutine = nullptr;
    std::size_t instructionIndex = 0;
    float time = 0;
    bool allocated = false;
    std::vector<std::uint32_t> locals;
    std::vector<Value> stack;
};

struct Vm::Thread {
    std::uint64_t id = 0;
    std::int32_t asyncId = -1;
    bool active = true;
    bool held = false;
    std::vector<Frame> frames;
};

Value Value::integer(std::int32_t value) { return {static_cast<std::uint32_t>(value), ValueType::Integer}; }

Value Value::floating(float value) {
    Value result;
    result.type = ValueType::Float;
    std::memcpy(&result.bits, &value, sizeof(value));
    return result;
}

std::int32_t Value::asInteger() const {
    if (type == ValueType::Integer)
        return signedBits(bits);
    const float value = floatingBits(bits);
    if (!std::isfinite(value) || double(value) < -2147483648.0 || double(value) >= 2147483648.0)
        return std::numeric_limits<std::int32_t>::min();
    return static_cast<std::int32_t>(value);
}

float Value::asFloat() const {
    return type == ValueType::Integer ? static_cast<float>(signedBits(bits)) : floatingBits(bits);
}

HostCall::HostCall(Vm& vm, const Instruction& instruction, std::optional<CallbackRegistration> registration)
    : vm_(vm), instruction_(instruction), registration_(std::move(registration)) {}

const Instruction& HostCall::instruction() const { return instruction_; }
std::uint64_t HostCall::threadId() const { return vm_.currentThread_->id; }
float HostCall::time() const { return vm_.currentThread_->frames.back().time; }
bool HostCall::subtractTime(std::int32_t ticks){
    if(vm_.status_!=VmStatus::Running||!vm_.currentThread_||vm_.currentThread_->frames.empty())return false;
    auto& clock=vm_.currentThread_->frames.back().time;
    const float delta=static_cast<float>(ticks),next=static_cast<float>(double(clock)-delta);
    if(!std::isfinite(next))return vm_.fail(ErrorCode::NumericError,"nonfinite context clock subtraction");
    clock=next;return true;
}
const CallbackRegistration* HostCall::callbackRegistration() const {
    return registration_ ? &*registration_ : nullptr;
}

bool HostCall::readInteger(std::size_t word, std::int32_t& value, std::size_t parameter) {
    if (vm_.status_ != VmStatus::Running)
        return false;
    if (registration_ && word == 0 && (parameter == automaticParameter || parameter == 0)) {
        value = registration_->slot;
        return true;
    }
    Value resolved;
    if (!vm_.readArgument(*vm_.currentThread_, instruction_, word,
                          parameter == automaticParameter ? word : parameter, false, resolved))
        return false;
    value = resolved.asInteger();
    return true;
}

bool HostCall::readFloat(std::size_t word, float& value, std::size_t parameter) {
    Value resolved;
    if (!vm_.readArgument(*vm_.currentThread_, instruction_, word,
                          parameter == automaticParameter ? word : parameter, true, resolved))
        return false;
    value = resolved.asFloat();
    return true;
}

bool HostCall::readString(std::size_t word, std::string& value, std::size_t parameter) {
    std::size_t next = 0;
    return vm_.readString(instruction_, word, parameter == automaticParameter ? word : parameter, value, next);
}

bool HostCall::storeInteger(std::int32_t byteOffset, std::int32_t value) {
    return vm_.store(*vm_.currentThread_, byteOffset, static_cast<std::uint32_t>(value));
}

bool HostCall::storeFloat(std::int32_t byteOffset, float value) {
    if (!std::isfinite(value))
        return vm_.fail(ErrorCode::NumericError, "host supplied a nonfinite float");
    return vm_.store(*vm_.currentThread_, byteOffset, Value::floating(value).bits,true);
}

bool HostCall::push(Value value) { return vm_.push(*vm_.currentThread_, value); }
bool HostCall::holdCurrentCommand(){
    if(vm_.status_!=VmStatus::Running)return false;
    if(!vm_.executing_||!vm_.currentThread_||instruction_.parameterCount||!instruction_.rawWords.empty())
        return vm_.fail(ErrorCode::InvalidInstruction,"held host command requires an executing zero-operand instruction");
    held_=true;return true;
}

Vm::Vm(const Program& program, Limits limits) : program_(program), limits_(limits) {}
Vm::~Vm() = default;
VmStatus Vm::status() const { return status_; }
const Error& Vm::error() const { return error_; }

bool Vm::limitsValid() const {
    return limits_.maxStackValues > 0 && limits_.maxStackValues <= 4096 &&
           limits_.maxLocalBytes >= 4 && limits_.maxLocalBytes <= 4096 && (limits_.maxLocalBytes & 3) == 0 &&
           limits_.maxCallDepth > 0 && limits_.maxCallDepth <= 256 &&
           limits_.maxThreads > 0 && limits_.maxThreads <= 64 &&
           limits_.maxInstructionsPerTick > 0 && limits_.maxInstructionsPerTick <= 1000000 &&
           limits_.maxHostOpcodes > 0 && limits_.maxHostOpcodes <= 1024;
}

bool Vm::coreOpcode(std::uint16_t opcode) {
    return opcode == 0 || opcode == 1 || (opcode >= 10 && opcode <= 17) || opcode == 21 ||
           (opcode >= 40 && opcode <= 45) || (opcode >= 50 && opcode <= 78) ||
           opcode == 81 || opcode == 82 || opcode == 83 || opcode == 84 || opcode == 85 || opcode == 86 || opcode == 87 || opcode == 89;
}

bool Vm::fail(ErrorCode code, std::string message) {
    if (error_.code != ErrorCode::None)
        return false;
    error_.code = code;
    error_.message = std::move(message);
    if (currentThread_)
        error_.threadId = currentThread_->id;
    if (currentSubroutine_) {
        error_.subroutine = currentSubroutine_->name;
        error_.offset = currentSubroutine_->codeSize;
    }
    if (currentInstruction_) {
        error_.opcode = currentInstruction_->opcode;
        error_.offset = currentInstruction_->offset;
    }
    status_ = VmStatus::Faulted;
    return false;
}

bool Vm::registerHostOpcode(std::uint16_t opcode, HostOpcode callback, std::string& error) {
    if (executing_) {
        fail(ErrorCode::ReentrantCall, "host registry changed during a tick");
        error = error_.message;
        return false;
    }
    if (!callback || coreOpcode(opcode) || hostOpcodes_.count(opcode) ||
        hostOpcodes_.size() >= std::min<std::size_t>(limits_.maxHostOpcodes, 1024)) {
        error = "null, core, duplicate, or excessive host opcode registration";
        return false;
    }
    hostOpcodes_.emplace(opcode, std::move(callback));
    error.clear();
    return true;
}

bool Vm::enableOriginalHostMissDomain(std::string& error) {
    if (status_ != VmStatus::Ready || executing_ || !threads_.empty()) {
        error = "original host-miss domain requires a fresh VM";
        return false;
    }
    originalHostMissDomain_ = true;
    error.clear();
    return true;
}

bool Vm::enableOriginalAsyncRegistry(std::string& error) {
    if (status_ != VmStatus::Ready || executing_ || !threads_.empty()) {
        error = "original async heap registry requires a fresh VM";
        return false;
    }
    originalAsyncRegistry_ = true;
    error.clear();
    return true;
}

bool Vm::setReferenceResolver(ReferenceRead read,ReferenceWrite write,std::string& error) {
    if(executing_) {
        fail(ErrorCode::ReentrantCall,"reference provider changed during a tick");
        error=error_.message;
        return false;
    }
    referenceRead_=std::move(read);
    referenceWrite_=std::move(write);
    error.clear();
    return true;
}

bool Vm::start(std::string_view entry, std::uint8_t rankMask) {
    if (executing_)
        return fail(ErrorCode::ReentrantCall, "start called during a tick");
    currentThread_ = nullptr;
    currentSubroutine_ = nullptr;
    currentInstruction_ = nullptr;
    error_ = {};
    status_ = VmStatus::Ready;
    threads_.clear();
    nextThreadId_ = 1;
    if (!program_.loaded())
        return fail(ErrorCode::InvalidProgram, "VM requires a successfully loaded program");
    if (!limitsValid() || rankMask == 0)
        return fail(ErrorCode::InvalidLimits, "VM limits or rank mask are outside bounded ranges");
    const auto* sub = program_.find(entry);
    if (!sub)
        return fail(ErrorCode::MissingSubroutine, "entry subroutine not found: " + std::string(entry));
    currentSubroutine_ = sub;
    if (sub->instructions.empty())
        return fail(ErrorCode::InvalidBranch, "entry subroutine is empty");
    auto thread = std::make_unique<Thread>();
    Frame frame;
    frame.subroutine = sub;
    thread->frames.push_back(std::move(frame));
    threads_.push_back(std::move(thread));
    rankMask_ = rankMask;
    status_ = VmStatus::Running;
    currentSubroutine_ = nullptr;
    return true;
}

Vm::Thread* Vm::findThread(std::uint64_t id) {
    for (auto& thread : threads_)
        if (thread->id == id)
            return thread.get();
    return nullptr;
}

void Vm::runThread(Thread& thread, TickResult& result) {
    currentThread_ = &thread;
    thread.held=false;
    while (thread.active && status_ == VmStatus::Running) {
        auto& frame = thread.frames.back();
        currentSubroutine_ = frame.subroutine;
        currentInstruction_ = nullptr;
        if (frame.instructionIndex >= frame.subroutine->instructions.size()) {
            fail(ErrorCode::InvalidBranch, "execution fell off the subroutine boundary");
            break;
        }
        const auto& instruction = frame.subroutine->instructions[frame.instructionIndex];
        currentInstruction_ = &instruction;
        if (double(instruction.time) > double(frame.time))
            break;
        if (result.instructions >= limits_.maxInstructionsPerTick) {
            fail(ErrorCode::InstructionBudget, "per-tick aggregate instruction budget exceeded");
            break;
        }
        const auto instructionIndex=frame.instructionIndex;
        ++result.instructions;
        ++frame.instructionIndex;
        if (!(instruction.rankMask & rankMask_)) {
            ++result.skipped;
            continue;
        }
        if (!execute(thread, instruction))
            break;
        if(thread.held){
            thread.frames.back().instructionIndex=instructionIndex;
            break;
        }
    }
    if (thread.active && status_ == VmStatus::Running && !thread.held) {
        auto& time = thread.frames.back().time;
        const float next = time + 1.f;
        if (!std::isfinite(next) || double(next) - double(time) != 1.0)
            fail(ErrorCode::NumericError, "thread clock cannot advance exactly one tick");
        else
            time = next;
    }
}

TickResult Vm::runTick() {
    TickResult result{status_, 0, 0};
    if (executing_) {
        fail(ErrorCode::ReentrantCall, "recursive runTick call");
        result.status = status_;
        return result;
    }
    if (status_ == VmStatus::Ready) {
        fail(ErrorCode::InvalidProgram, "runTick requires a successful start");
        result.status = status_;
        return result;
    }
    if (status_ != VmStatus::Running)
        return result;
    executing_ = true;
    if (originalAsyncRegistry_) {
        const auto retirePrimary = [&] {
            for (auto& remaining : threads_)
                remaining->active = false;
            threads_.erase(std::next(threads_.begin()), threads_.end());
            status_ = VmStatus::Finished;
        };
        for (auto node = threads_.begin(); node != threads_.end() && status_ == VmStatus::Running;) {
            const auto successor = std::next(node);
            auto& thread = **node;
            if (thread.id == 0 && !thread.active) {
                retirePrimary();
                break;
            }
            if (thread.active)
                runThread(thread, result);
            if (status_ != VmStatus::Running)
                break;
            if (thread.id == 0 && !thread.active) {
                retirePrimary();
                break;
            }
            if (!thread.active) {
                currentThread_ = nullptr;
                currentSubroutine_ = nullptr;
                currentInstruction_ = nullptr;
                threads_.erase(node);
            }
            node = successor;
        }
    } else {
        std::vector<std::uint64_t> scheduled;
        for (const auto& thread : threads_)
            if (thread->active)
                scheduled.push_back(thread->id);
        for (const auto id : scheduled) {
            auto* thread = findThread(id);
            if (!thread || !thread->active || status_ != VmStatus::Running)
                continue;
            runThread(*thread, result);
        }
    }
    if (!originalAsyncRegistry_ && status_ == VmStatus::Running && std::none_of(threads_.begin(), threads_.end(),
            [](const auto& thread) { return thread->active; }))
        status_ = VmStatus::Finished;
    currentThread_ = nullptr;
    currentSubroutine_ = nullptr;
    currentInstruction_ = nullptr;
    executing_ = false;
    result.status = status_;
    return result;
}

std::vector<ThreadSnapshot> Vm::threads() const {
    std::vector<ThreadSnapshot> result;
    for (const auto& thread : threads_) {
        const auto& frame = thread->frames.back();
        ThreadSnapshot snapshot;
        snapshot.id = thread->id;
        snapshot.asyncId = thread->asyncId;
        snapshot.active = thread->active;
        snapshot.subroutine = frame.subroutine->name;
        snapshot.offset = frame.instructionIndex < frame.subroutine->instructions.size()
            ? frame.subroutine->instructions[frame.instructionIndex].offset : frame.subroutine->codeSize;
        snapshot.time = frame.time;
        snapshot.callDepth = thread->frames.size() - 1;
        snapshot.locals = frame.locals;
        snapshot.stack = frame.stack;
        result.push_back(std::move(snapshot));
    }
    return result;
}

bool Vm::shape(const Instruction& instruction, std::size_t words, std::size_t parameters,
               std::uint16_t allowedMask) {
    if (instruction.rawWords.size() != words || instruction.parameterCount != parameters ||
        (instruction.parameterMask & ~allowedMask))
        return fail(ErrorCode::InvalidInstruction, "opcode operands do not match the verified layout");
    return true;
}

bool Vm::push(Thread& thread, Value value) {
    if (status_ != VmStatus::Running)
        return false;
    if (value.type == ValueType::Float && !std::isfinite(value.asFloat()))
        return fail(ErrorCode::NumericError, "nonfinite expression result");
    std::size_t total = 0;
    for (const auto& frame : thread.frames)
        total += frame.stack.size();
    if (total >= limits_.maxStackValues)
        return fail(ErrorCode::StackLimit, "aggregate per-thread operand stack bound exceeded");
    thread.frames.back().stack.push_back(value);
    return true;
}

bool Vm::pop(Thread& thread, Value& value) {
    if (status_ != VmStatus::Running)
        return false;
    auto& stack = thread.frames.back().stack;
    if (stack.empty())
        return fail(ErrorCode::StackUnderflow, "expression stack underflow");
    value = stack.back();
    stack.pop_back();
    return true;
}

bool Vm::store(Thread& thread, std::int32_t offset, std::uint32_t bits,bool floating) {
    if (status_ != VmStatus::Running)
        return false;
    if (offset < 0) {
        if(offset==-1||!referenceWrite_)return fail(ErrorCode::UnsupportedReference,"global or stack lvalue is not implemented");
        bool handled=false;
#if defined(__cpp_exceptions)
        try {
#endif
            handled=referenceWrite_(offset,{bits,floating?ValueType::Float:ValueType::Integer});
#if defined(__cpp_exceptions)
        } catch(const std::exception& error) {return fail(ErrorCode::HostFailure,error.what());}
          catch(...) {return fail(ErrorCode::HostFailure,"reference writer threw");}
#endif
        if(status_!=VmStatus::Running)return false;
        return handled||fail(ErrorCode::UnsupportedReference,"host declined global lvalue: "+std::to_string(offset));
    }
    auto& locals = thread.frames.back().locals;
    if ((offset & 3) || static_cast<std::size_t>(offset) / 4 >= locals.size())
        return fail(ErrorCode::LocalBounds, "local destination is not an allocated aligned byte address");
    locals[static_cast<std::size_t>(offset) / 4] = bits;
    return true;
}

bool Vm::readArgument(Thread& thread, const Instruction& instruction, std::size_t word,
                      std::size_t parameter, bool floating, Value& value) {
    if (status_ != VmStatus::Running)
        return false;
    if (word >= instruction.rawWords.size() || parameter >= instruction.parameterCount)
        return fail(ErrorCode::InvalidInstruction, "operand index exceeds instruction payload");
    const auto raw = instruction.rawWords[word];
    value = {raw, floating ? ValueType::Float : ValueType::Integer};
    if (floating && !std::isfinite(value.asFloat()))
        return fail(ErrorCode::NumericError, "nonfinite float operand");
    if (parameter >= 16 || !(instruction.parameterMask & (std::uint16_t(1) << parameter)))
        return true;
    if (floating && (double(value.asFloat()) < -2147483648.0 || double(value.asFloat()) >= 2147483648.0 ||
                     std::trunc(value.asFloat()) != value.asFloat()))
        return fail(ErrorCode::LocalBounds, "float reference is not an exact signed integer address");
    const auto offset = value.asInteger();
    if (offset == -1) {
        Value popped;
        if (!pop(thread, popped))
            return false;
        value = floating ? Value::floating(popped.asFloat()) : Value::integer(popped.asInteger());
        return true;
    }
    if (offset < 0) {
        if(!referenceRead_)return fail(ErrorCode::UnsupportedReference,"game/global variable reference is not implemented: "+std::to_string(offset));
        std::optional<Value> supplied;
#if defined(__cpp_exceptions)
        try {
#endif
            supplied=referenceRead_(offset,floating);
#if defined(__cpp_exceptions)
        } catch(const std::exception& error) {return fail(ErrorCode::HostFailure,error.what());}
          catch(...) {return fail(ErrorCode::HostFailure,"reference reader threw");}
#endif
        if(status_!=VmStatus::Running)return false;
        if(!supplied)return fail(ErrorCode::UnsupportedReference,"host declined global reference: "+std::to_string(offset));
        if(supplied->type==ValueType::Float&&!std::isfinite(supplied->asFloat()))
            return fail(ErrorCode::NumericError,"reference provider supplied a nonfinite value");
        value=floating?Value::floating(supplied->asFloat()):Value::integer(supplied->asInteger());
        return true;
    }
    const auto& locals = thread.frames.back().locals;
    if ((offset & 3) || static_cast<std::size_t>(offset) / 4 >= locals.size())
        return fail(ErrorCode::LocalBounds, "local reference is not an allocated aligned byte address");
    value.bits = locals[static_cast<std::size_t>(offset) / 4];
    if (floating && !std::isfinite(value.asFloat()))
        return fail(ErrorCode::NumericError, "local contains a nonfinite float");
    return true;
}

bool Vm::readString(const Instruction& instruction, std::size_t word, std::size_t parameter,
                    std::string& value, std::size_t& next) {
    if (status_ != VmStatus::Running)
        return false;
    if (word >= instruction.rawWords.size() || parameter >= instruction.parameterCount ||
        (parameter < 16 && (instruction.parameterMask & (std::uint16_t(1) << parameter))))
        return fail(ErrorCode::InvalidInstruction, "missing or reference-masked inline string");
    const std::size_t length = instruction.rawWords[word];
    if (length == 0 || (length & 3) || length > 4096 || length / 4 > instruction.rawWords.size() - word - 1)
        return fail(ErrorCode::InvalidInstruction, "invalid padded string extent");
    value.clear();
    bool terminated = false;
    for (std::size_t index = 0; index < length; ++index) {
        const auto character = static_cast<std::uint8_t>(instruction.rawWords[word + 1 + index / 4] >> ((index & 3) * 8));
        if (character == 0)
            terminated = true;
        else if (terminated || character < 32 || character > 126)
            return fail(ErrorCode::InvalidInstruction, "unterminated, non-ASCII, or nonzero-padded inline string");
        else
            value.push_back(static_cast<char>(character));
    }
    if (!terminated)
        return fail(ErrorCode::InvalidInstruction, "inline string has no terminator");
    next = word + 1 + length / 4;
    return true;
}

bool Vm::numeric(Thread& thread, std::uint16_t opcode) {
    Value right;
    if (!pop(thread, right))
        return false;
    if (opcode == 71 || opcode == 72 || opcode == 84 || opcode == 85) {
        if (opcode == 71)
            return push(thread, Value::integer(right.asInteger() == 0));
        if (opcode == 72)
            return push(thread, Value::integer(right.asFloat() == 0.f));
        if (opcode == 84)
            return push(thread, Value::integer(signedBits(0u - static_cast<std::uint32_t>(right.asInteger()))));
        return push(thread, Value::floating(-right.asFloat()));
    }
    Value left;
    if (!pop(thread, left))
        return false;
    const auto leftInt = left.asInteger();
    const auto rightInt = right.asInteger();
    const auto leftBits = static_cast<std::uint32_t>(leftInt);
    const auto rightBits = static_cast<std::uint32_t>(rightInt);
    const float leftFloat = left.asFloat();
    const float rightFloat = right.asFloat();
    Value result;
    switch (opcode) {
    case 50: result = Value::integer(signedBits(leftBits + rightBits)); break;
    case 51: result = Value::floating(static_cast<float>(double(leftFloat) + rightFloat)); break;
    case 52: result = Value::integer(signedBits(leftBits - rightBits)); break;
    case 53: result = Value::floating(static_cast<float>(double(leftFloat) - rightFloat)); break;
    case 54: result = Value::integer(signedBits(leftBits * rightBits)); break;
    case 55: result = Value::floating(static_cast<float>(double(leftFloat) * rightFloat)); break;
    case 56:
    case 58:
        if (rightInt == 0 || (leftInt == std::numeric_limits<std::int32_t>::min() && rightInt == -1))
            return fail(ErrorCode::NumericError, "integer division by zero or signed division overflow");
        result = Value::integer(opcode == 56 ? leftInt / rightInt : leftInt % rightInt);
        break;
    case 57:
        if (rightFloat == 0.f)
            return fail(ErrorCode::NumericError, "floating division by zero");
        result = Value::floating(static_cast<float>(double(leftFloat) / rightFloat));
        break;
    case 59: result = Value::integer(leftInt == rightInt); break;
    case 60: result = Value::integer(leftFloat == rightFloat); break;
    case 61: result = Value::integer(leftInt != rightInt); break;
    case 62: result = Value::integer(leftFloat != rightFloat); break;
    case 63: result = Value::integer(leftInt < rightInt); break;
    case 64: result = Value::integer(leftFloat < rightFloat); break;
    case 65: result = Value::integer(leftInt <= rightInt); break;
    case 66: result = Value::integer(leftFloat <= rightFloat); break;
    case 67: result = Value::integer(leftInt > rightInt); break;
    case 68: result = Value::integer(leftFloat > rightFloat); break;
    case 69: result = Value::integer(leftInt >= rightInt); break;
    case 70: result = Value::integer(leftFloat >= rightFloat); break;
    case 73: result = Value::integer(leftInt != 0 || rightInt != 0); break;
    case 74: result = Value::integer(leftInt != 0 && rightInt != 0); break;
    case 75: result = Value::integer(signedBits(leftBits ^ rightBits)); break;
    case 76: result = Value::integer(signedBits(leftBits | rightBits)); break;
    case 77: result = Value::integer(signedBits(leftBits & rightBits)); break;
    default: return fail(ErrorCode::UnsupportedOpcode, "numeric opcode has no implementation");
    }
    return push(thread, result);
}

bool Vm::invoke(Thread& thread, const Instruction& instruction) {
    const std::size_t fixed = instruction.opcode == 16 ? 2 : 1;
    if (instruction.parameterCount < fixed)
        return fail(ErrorCode::InvalidInstruction, "call is missing its fixed parameters");
    std::string name;
    std::size_t descriptors = 0;
    if (!readString(instruction, 0, 0, name, descriptors))
        return false;
    std::int32_t asyncId = -1;
    if (instruction.opcode == 16) {
        Value tag;
        if (!readArgument(thread, instruction, descriptors, 1, false, tag))
            return false;
        asyncId = tag.asInteger();
        ++descriptors;
    }
    const std::size_t arguments = instruction.parameterCount - fixed;
    if (descriptors + arguments * 2 != instruction.rawWords.size())
        return fail(ErrorCode::InvalidInstruction, "typed call descriptor count does not match payload");
    Frame next;
    next.subroutine = program_.find(name);
    if (!next.subroutine || next.subroutine->instructions.empty())
        return fail(ErrorCode::MissingSubroutine, "call target is missing or empty (includes are not auto-loaded): " + name);
    if (instruction.opcode == 11 && thread.frames.size() >= limits_.maxCallDepth)
        return fail(ErrorCode::CallLimit, "per-thread call-frame bound exceeded");
    std::size_t localBytes = instruction.opcode == 11 ? arguments * 4 : 0;
    if (instruction.opcode == 11)
        for (const auto& frame : thread.frames)
            localBytes += frame.locals.size() * 4;
    else
        localBytes = arguments * 4;
    if (localBytes > limits_.maxLocalBytes)
        return fail(ErrorCode::LocalBounds, "call arguments exceed aggregate local storage bound");
    for (std::size_t index = 0; index < arguments; ++index) {
        const auto descriptor = instruction.rawWords[descriptors + index * 2];
        const auto source = descriptor & 255;
        const auto destination = (descriptor >> 8) & 255;
        if ((descriptor >> 16) || (source != 'i' && source != 'f') || (destination != 'i' && destination != 'f'))
            return fail(ErrorCode::InvalidInstruction, "unverified typed argument descriptor");
        Value value;
        if (!readArgument(thread, instruction, descriptors + index * 2 + 1, fixed + index, source == 'f', value))
            return false;
        next.locals.push_back(destination == 'f' ? Value::floating(value.asFloat()).bits :
                              static_cast<std::uint32_t>(value.asInteger()));
    }
    if (instruction.opcode == 11) {
        thread.frames.push_back(std::move(next));
        return true;
    }
    const auto inactive = originalAsyncRegistry_ ? threads_.end() :
        std::find_if(threads_.begin(), threads_.end(), [](const auto& candidate) { return !candidate->active; });
    if ((!originalAsyncRegistry_ && inactive == threads_.end() && threads_.size() >= limits_.maxThreads) ||
        nextThreadId_ == std::numeric_limits<std::uint64_t>::max())
        return fail(ErrorCode::ThreadLimit, "asynchronous thread or identity bound exceeded");
    auto child = std::make_unique<Thread>();
    child->id = nextThreadId_++;
    child->asyncId = asyncId;
    child->frames.push_back(std::move(next));
    if (originalAsyncRegistry_)
        threads_.insert(std::next(threads_.begin()), std::move(child));
    else if (inactive == threads_.end())
        threads_.push_back(std::move(child));
    else
        *inactive = std::move(child);
    return true;
}

bool Vm::executeHost(Thread& thread, const Instruction& instruction) {
    std::optional<CallbackRegistration> registration;
    if (instruction.opcode == 22) {
        if (instruction.parameterCount != 2 || (instruction.parameterMask & ~std::uint16_t(1)))
            return fail(ErrorCode::InvalidInstruction, "TH128 opcode 22 requires Sm, not a subroutine call");
        registration.emplace();
        Value slot;
        std::size_t next = 0;
        if (!readArgument(thread, instruction, 0, 0, false, slot) ||
            !readString(instruction, 1, 1, registration->subroutine, next))
            return false;
        if (next != instruction.rawWords.size())
            return fail(ErrorCode::InvalidInstruction, "extra data after TH128 callback registration");
        registration->slot = slot.asInteger();
    }
    const auto found = hostOpcodes_.find(instruction.opcode);
    if (found == hostOpcodes_.end()) {
        if (originalHostMissDomain_ && instruction.opcode > 615
            && !isOriginalHostTableOpcode(instruction.opcode) && hostResultAdvances(0))
            return true;
        return fail(ErrorCode::UnsupportedOpcode, "unregistered host opcode " + std::to_string(instruction.opcode));
    }
    HostCall call(*this, instruction, std::move(registration));
    bool handled = false;
#if defined(__cpp_exceptions)
    try {
        handled = found->second(call);
    } catch (const std::exception& exception) {
        return fail(ErrorCode::HostFailure, "host callback threw: " + std::string(exception.what()));
    } catch (...) {
        return fail(ErrorCode::HostFailure, "host callback threw a non-standard exception");
    }
#else
    handled = found->second(call);
#endif
    if (status_ != VmStatus::Running)
        return false;
    if(!handled)return fail(ErrorCode::UnsupportedOpcode, "host declined opcode " + std::to_string(instruction.opcode));
    currentThread_->held=call.held_;
    return true;
}

bool Vm::execute(Thread& thread, const Instruction& instruction) {
    const std::uint32_t mask = instruction.parameterCount >= 16 ? 65535u :
                               (1u << instruction.parameterCount) - 1u;
    if (instruction.stackOffset != 0 || (instruction.parameterMask & ~mask) ||
        instruction.parameterCount > instruction.rawWords.size())
        return fail(ErrorCode::InvalidInstruction, "nonzero stackOffset or inconsistent parameter metadata is unsupported");
    const auto opcode = instruction.opcode;
    auto& frame = thread.frames.back();
    if (opcode == 0 || opcode == 1 || opcode == 10 || opcode == 21 || opcode == 41) {
        if (!shape(instruction, 0, 0))
            return false;
        if (opcode == 1)
            thread.active = false;
        else if (opcode == 10) {
            if (thread.frames.size() == 1)
                thread.active = false;
            else
                thread.frames.pop_back();
        } else if (opcode == 21) {
            if (originalAsyncRegistry_ && thread.id != 0)
                return fail(ErrorCode::UnsupportedOpcode, "original async opcode21 self-invalidation is unsupported");
            for (auto& other : threads_)
                if (other->id != 0)
                    other->active = false;
        } else if (opcode == 41) {
            if (!frame.allocated)
                return fail(ErrorCode::LocalBounds, "stackFree has no matching stackAlloc");
            frame.locals.clear();
            frame.allocated = false;
        }
        if (thread.id == 0 && !thread.active)
            for (auto& other : threads_)
                other->active = false;
        return true;
    }
    if (opcode == 11 || opcode == 15 || opcode == 16)
        return invoke(thread, instruction);
    if (opcode >= 12 && opcode <= 14) {
        if (!shape(instruction, 2, 2))
            return false;
        const auto target = std::int64_t(instruction.offset) + signedBits(instruction.rawWords[0]);
        if (target < 0 || target >= frame.subroutine->codeSize ||
            !frame.subroutine->instructionAt(static_cast<std::uint32_t>(target)))
            return fail(ErrorCode::InvalidBranch, "relative branch does not target an instruction boundary");
        bool take = opcode == 12;
        if (!take) {
            Value condition;
            if (!pop(thread, condition))
                return false;
            take = opcode == 13 ? condition.asInteger() == 0 : condition.asInteger() != 0;
        }
        if (take) {
            const auto* destination = frame.subroutine->instructionAt(static_cast<std::uint32_t>(target));
            frame.instructionIndex = static_cast<std::size_t>(destination - frame.subroutine->instructions.data());
            frame.time = static_cast<float>(signedBits(instruction.rawWords[1]));
        }
        return true;
    }
    if (opcode == 17 || opcode == 40 || opcode == 42 || opcode == 44 || opcode == 83) {
        if (!shape(instruction, 1, 1, 1))
            return false;
        Value value;
        if (!readArgument(thread, instruction, 0, 0, opcode == 44, value))
            return false;
        if (opcode == 42 || opcode == 44)
            return push(thread, value);
        if (opcode == 17) {
            if (originalAsyncRegistry_) {
                const auto found = std::find_if(threads_.begin(), threads_.end(),
                    [&](const auto& other) { return other->asyncId == value.asInteger(); });
                if (found != threads_.end()) {
                    if (found->get() == &thread)
                        return fail(ErrorCode::UnsupportedOpcode, "original async opcode17 self-invalidation is unsupported");
                    (*found)->active = false;
                }
            } else {
                for (auto& other : threads_)
                    if (other->id != 0 && other->asyncId == value.asInteger())
                        other->active = false;
            }
        } else if (opcode == 83) {
            frame.time = static_cast<float>(double(frame.time) - value.asInteger());
            if (!std::isfinite(frame.time))
                return fail(ErrorCode::NumericError, "wait overflowed thread time");
        } else {
            const auto bytes = value.asInteger();
            if (frame.allocated || bytes < 0 || (bytes & 3) || static_cast<std::size_t>(bytes) < frame.locals.size() * 4)
                return fail(ErrorCode::LocalBounds, "invalid or repeated local stack allocation");
            std::size_t total = static_cast<std::size_t>(bytes);
            for (std::size_t index = 0; index + 1 < thread.frames.size(); ++index)
                total += thread.frames[index].locals.size() * 4;
            if (total > limits_.maxLocalBytes)
                return fail(ErrorCode::LocalBounds, "aggregate per-thread local byte bound exceeded");
            frame.locals.resize(static_cast<std::size_t>(bytes) / 4, 0);
            frame.allocated = true;
        }
        return true;
    }
    if (opcode == 43 || opcode == 45 || opcode == 78) {
        if (!shape(instruction, 1, 1, 1) || instruction.parameterMask != 1)
            return fail(ErrorCode::InvalidInstruction, "store/decrement requires a reference-masked destination");
        const auto raw = instruction.rawWords[0];
        std::int32_t offset = signedBits(raw);
        if (opcode == 45) {
            const auto address = floatingBits(raw);
            if (!std::isfinite(address) || double(address) < -2147483648.0 || double(address) >= 2147483648.0 ||
                std::trunc(address) != address)
                return fail(ErrorCode::LocalBounds, "float destination is not an exact signed integer address");
            offset = static_cast<std::int32_t>(address);
        }
        Value value;
        if (opcode == 78) {
            if (!readArgument(thread, instruction, 0, 0, false, value) ||
                !store(thread, offset, static_cast<std::uint32_t>(value.asInteger()) - 1u))
                return false;
            return push(thread, Value::integer(value.asInteger()));
        }
        if (!pop(thread, value))
            return false;
        return store(thread, offset, opcode == 43 ? static_cast<std::uint32_t>(value.asInteger()) :
                     Value::floating(value.asFloat()).bits,opcode==45);
    }
    if(opcode==89){
        if(!shape(instruction,3,3,7)||(instruction.parameterMask&1u)==0)
            return fail(ErrorCode::InvalidInstruction,"angle difference requires a masked float destination");
        Value first,second;
        if(!readArgument(thread,instruction,1,1,true,first)||!readArgument(thread,instruction,2,2,true,second))return false;
        const float address=floatingBits(instruction.rawWords[0]);
        if(!std::isfinite(address)||double(address)<-2147483648.0||double(address)>=2147483648.0||std::trunc(address)!=address)
            return fail(ErrorCode::LocalBounds,"angle difference destination is not an exact signed integer address");
        const auto offset=std::int32_t(address);
        if(offset<0)return fail(ErrorCode::UnsupportedReference,"angle difference global lvalue owner is not established");
        const auto& locals=thread.frames.back().locals;
        if((offset&3)||std::size_t(offset)/4>=locals.size())
            return fail(ErrorCode::LocalBounds,"angle difference destination is not an allocated aligned local address");
        double difference=double(second.asFloat())-first.asFloat();
        if(difference>3.1415927410125732)difference-=6.2831854820251465;
        else if(difference< -3.1415927410125732)difference+=6.2831854820251465;
        const float result=float(difference);
        if(!std::isfinite(result))return fail(ErrorCode::NumericError,"nonfinite angle difference result");
        return store(thread,offset,Value::floating(result).bits,true);
    }
    if(opcode==87){
        if(!shape(instruction,5,5,31)||(instruction.parameterMask&1u)==0)
            return fail(ErrorCode::InvalidInstruction,"angle operation requires a masked float destination");
        Value originX,originY,targetX,targetY;
        if(!readArgument(thread,instruction,1,1,true,originX)||!readArgument(thread,instruction,2,2,true,originY)
            ||!readArgument(thread,instruction,3,3,true,targetX)||!readArgument(thread,instruction,4,4,true,targetY))return false;
        const float deltaY=float(double(targetY.asFloat())-originY.asFloat());
        const float deltaX=float(double(targetX.asFloat())-originX.asFloat());
        const float angle=float(std::atan2(double(deltaY),double(deltaX)));
        const float address=floatingBits(instruction.rawWords[0]);
        if(!std::isfinite(address)||double(address)<-2147483648.0||double(address)>=2147483648.0||std::trunc(address)!=address)
            return fail(ErrorCode::LocalBounds,"angle destination is not an exact signed integer address");
        if(!std::isfinite(angle))return fail(ErrorCode::NumericError,"nonfinite angle result");
        return store(thread,std::int32_t(address),Value::floating(angle).bits,true);
    }
    if(opcode==81||opcode==82||opcode==86){
        const auto count=opcode==81?4u:opcode==82?1u:3u;
        if(!shape(instruction,count,count,std::uint16_t((1u<<count)-1))||(instruction.parameterMask&(opcode==81?3u:1u))!=(opcode==81?3u:1u))
            return fail(ErrorCode::InvalidInstruction,"vector operation requires masked float destinations");
        const auto destination=[&](std::size_t index,float value){
            const float address=floatingBits(instruction.rawWords[index]);
            if(!std::isfinite(address)||double(address)<-2147483648.||double(address)>=2147483648.||std::trunc(address)!=address)
                return fail(ErrorCode::LocalBounds,"vector destination is not an exact signed integer address");
            if(!std::isfinite(value))return fail(ErrorCode::NumericError,"nonfinite vector result");
            return store(thread,std::int32_t(address),Value::floating(value).bits,true);
        };
        Value first,second;
        const std::size_t begin=opcode==81?2:opcode==82?0:1;
        if(!readArgument(thread,instruction,begin,begin,true,first))return false;
        float angle=first.asFloat();
        if(opcode==81||opcode==82){
            unsigned adjustments=0;
            while(angle>3.1415927410125732f&&adjustments<34){angle=float(double(angle)-6.2831854820251465);++adjustments;}
            while(angle< -3.1415927410125732f&&adjustments<34){angle=float(double(angle)+6.2831854820251465);++adjustments;}
        }
        if(opcode==82)return destination(0,angle);
        if(!readArgument(thread,instruction,begin+1,begin+1,true,second))return false;
        if(opcode==86)return destination(0,float(double(first.asFloat())*first.asFloat()+double(second.asFloat())*second.asFloat()));
        return destination(0,float(std::cos(double(angle))*second.asFloat()))&&destination(1,float(std::sin(double(angle))*second.asFloat()));
    }
    if ((opcode >= 50 && opcode <= 77) || opcode == 84 || opcode == 85)
        return shape(instruction, 0, 0) && numeric(thread, opcode);
    return executeHost(thread, instruction);
}

}
