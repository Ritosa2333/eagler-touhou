#pragma once
// Native practice continuation owns the existing opaque handoff; no normal-record qualification.
extern "C" {
int th128_world_prepare_thprac_carry(unsigned token);
int th128_world_prepare_thprac_replay_initial(const void* data,unsigned bytes);
int th128_world_restore_thprac_replay_initial();
int th128_world_finish_thprac_ending(unsigned generation);
}
