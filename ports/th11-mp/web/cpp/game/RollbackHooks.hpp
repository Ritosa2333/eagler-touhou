#pragma once
#include <cstdlib>

// Allocation scope follows ownership. Game objects and their containers live
// in the journaled heap; graphics, fonts and process caches retain system
// ownership. Changing allocation scope does not suppress tracked game writes.
#if defined(TH11_MULTIPLAYER) && defined(EAGLER_WRITE_JOURNAL)
#include <eagler/netplay/WriteJournal.hpp>
#define TH11_GAME_SCOPE ::Netplay::WriteJournal::GameScope th11_game_scope_
#define TH11_SYSTEM_SCOPE ::Netplay::WriteJournal::SystemScope th11_system_scope_
#define TH11_GAME_CALLOC(count,size) ::Netplay::WriteJournal::AllocateZeroed(count,size)
#define TH11_GAME_FREE(pointer) ::Netplay::WriteJournal::Release(pointer)
#define TH11_ROLLBACK_FRAME_OPEN() ::Netplay::WriteJournal::FrameOpen()
#else
#define TH11_GAME_SCOPE (void)0
#define TH11_SYSTEM_SCOPE (void)0
#define TH11_GAME_CALLOC(count,size) std::calloc(count,size)
#define TH11_GAME_FREE(pointer) std::free(pointer)
#define TH11_ROLLBACK_FRAME_OPEN() false
#endif
