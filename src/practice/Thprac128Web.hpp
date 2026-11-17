#pragma once
// Actual Web ABI for the owned TH128 practice consumers; source semantics in Thprac128Chapter/Section.
// Payload: 11 signed little-endian 32-bit words. Display score is passed as bounded scoreRaw.
#define TH128_THPRAC_WEB_ABI 1
extern "C" {
int th128_world_prepare_thprac128(const void* data,unsigned bytes);
const char* th128_world_thprac128_state();
}
