#pragma once
#include "PortTitle.hpp"
// Only actual threaded TH14 builds have these process critical sections.
// No fields are added to retail-layout RNGs, managers, or VMs.
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_WORKER) && defined(_REENTRANT)
#define TH14_NATIVE_CRITICAL_SECTIONS 1
#include <mutex>
namespace th11::th14_critical_sections {
// Retail 4f57a8 and 4f57c0 are different recursive CRITICAL_SECTION objects.
inline std::recursive_mutex anm;
inline std::recursive_mutex rng;
// C++ hash/vector bookkeeping has no retail pointer-array counterpart. Its
// short guard never wraps a VM script or the resource-template interpreter.
inline std::recursive_mutex containers;
using AnmGuard=std::lock_guard<std::recursive_mutex>;
using ContainerGuard=std::lock_guard<std::recursive_mutex>;
class RngGuard {
    bool held;
public:
    explicit RngGuard(bool enabled):held(enabled){if(held)rng.lock();}
    ~RngGuard(){if(held)rng.unlock();}
    RngGuard(const RngGuard&)=delete;
    RngGuard& operator=(const RngGuard&)=delete;
};
}
#endif
