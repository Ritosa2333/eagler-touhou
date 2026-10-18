#pragma once
#include "gameplay.hpp"

// These declarations intentionally have no fallback bodies. Their owners must
// be reconstructed and linked before GameController lifecycle can run in-game.
// They identify the precise remaining boundary instead of inventing state.
namespace th20::source::gameplay::unrecovered {
runtime::CallbackOwner*& owner(Owner);                  // documented globals/accessors in gameplay.hpp
void commit_progress_if_present();                      // nullable5c6108->50f660
void screen_transition(float,float);                    // 4a0aa0(this=5c0698)
void cleanup(Cleanup);                                 // per-address list in Cleanup enum
}
