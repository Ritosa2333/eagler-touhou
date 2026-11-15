#pragma once
#include "Types.hpp"

namespace th11 {

// The interpreter's dispatch ring, for a report that has to say how a failing position was reached
// rather than only where it is. The opcode recorded is the one that ran, which the loader has
// already translated into this port's numbering; the offset is the command's byte offset inside its
// own script, the same quantity the failure report prints.
//
// The ring is a member of the VM that fills it (AnmVm::trace, AnmVm::push_trace). It began as a
// file-scope global, and sixteen animations each pushed into the one ring, so the "path" a report
// copied held positions from whichever scripts ran last rather than the history of the one that
// failed: section 381 measured that by reading a trajectory as file data and finding that only four
// of its sixteen positions were instruction starts of a single script, the run every script family
// shares. Per VM, the trajectory reads as one script's dispatch.
// The ring carries the command's length as well, because that is the field the walk advances by:
// `instruction = current + current->length`. A position whose length is zero is a position the walk
// cannot leave, which is how a creation failure can be a spin the update guard turns into one error
// (section 382), and a position whose length disagrees with the title's own file is where a loaded
// copy stops being the file's own command stream. An entry stays eight bytes, so the port-only VM
// extension it is counted in keeps its size.
struct AnmTraceEntry { u32 offset; u16 opcode; u16 length; };
constexpr u32 anm_trace_size = 16;

}
