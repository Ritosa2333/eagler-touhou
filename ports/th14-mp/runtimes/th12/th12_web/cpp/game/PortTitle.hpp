// Which title this build runs. One line, and the only place a per-title build differs from another at compile time.
//
// It exists because the engine sources are shared while the tables are not: the opcode numbers a title dispatches to
// a real handler and the numbers it skips are both read from that title's own executable, so a build that answers
// with another title's table acts where the original does nothing. Every per-title build overrides this constant -
// with a -D on the command line or by replacing this file - and the shared sources ask runtime_noop_opcode rather
// than naming a title.
#pragma once
#include <cstdint>

#ifndef PORT_TITLE
#define PORT_TITLE 12
#endif

inline constexpr uint32_t kPortTitle = PORT_TITLE;
