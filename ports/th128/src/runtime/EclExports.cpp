#include "ecl/EclProgram.hpp"
#include "ecl/EclVm.hpp"
#include "Memory.hpp"
#include <cstdint>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>

namespace {
std::unordered_map<int, th128::ecl::Program> programs;
std::unique_ptr<th128::ecl::Vm> interpreter;
std::string diagnostic;
std::string vmDiagnostic;
int activeSlot = -1;
std::size_t totalBytes = 0;
constexpr std::size_t AggregateBytes = 8u * 1024u * 1024u;

void recordFault() {
    if (!interpreter || interpreter->status() != th128::ecl::VmStatus::Faulted) return;
    const auto& fault = interpreter->error();
    std::ostringstream message;
    message << "ECL " << fault.subroutine << " offset=" << fault.offset << " opcode=" << fault.opcode
            << ": " << fault.message;
    vmDiagnostic = message.str();
}
}

extern "C" {
const char* th128_ecl_error() { return vmDiagnostic.empty() ? diagnostic.c_str() : vmDiagnostic.c_str(); }
int th128_ecl_status() { return interpreter ? int(interpreter->status()) : int(th128::ecl::VmStatus::Ready); }

int th128_load_ecl(int slot, const std::uint8_t* bytes, std::uint32_t length) {
    diagnostic.clear();
    if (slot < 0 || slot >= 64 || !th128::runtime::inputRange(bytes,length) || length > AggregateBytes) {
        diagnostic = "Invalid ECL slot/input bounds";
        return 0;
    }
    if (interpreter && slot == activeSlot) {
        diagnostic = "Reset ECL VM before replacing its retained resource slot";
        return 0;
    }
    const auto previous = programs.find(slot);
    const auto oldBytes = previous == programs.end() ? 0 : previous->second.resource().bytes.size();
    if (length > AggregateBytes - (totalBytes - oldBytes)) {
        diagnostic = "ECL aggregate resource budget exceeded";
        return 0;
    }
    th128::ecl::Program candidate;
    if (!candidate.load(bytes, length, diagnostic)) return 0;
    programs[slot] = std::move(candidate);
    totalBytes = totalBytes - oldBytes + length;
    return 1;
}

int th128_ecl_start(int slot, int difficulty) {
    diagnostic.clear();
    vmDiagnostic.clear();
    interpreter.reset();
    activeSlot = -1;
    const auto resource = programs.find(slot);
    if (resource == programs.end() || difficulty < 0 || difficulty > 4) {
        diagnostic = "Invalid ECL resource/difficulty";
        return 0;
    }
    interpreter = std::make_unique<th128::ecl::Vm>(resource->second);
    activeSlot = slot;
    const bool started = interpreter->start("main", std::uint8_t(1u << difficulty));
    recordFault();
    return started ? 1 : 0;
}

int th128_ecl_tick() {
    if (!interpreter) {
        diagnostic = "ECL diagnostic VM has not been started";
        return 0;
    }
    const auto result = interpreter->runTick();
    recordFault();
    return result.status == th128::ecl::VmStatus::Faulted ? 0 : 1;
}

void th128_ecl_reset() {
    interpreter.reset();
    activeSlot = -1;
    programs.clear();
    diagnostic.clear();
    vmDiagnostic.clear();
    totalBytes = 0;
}
}
