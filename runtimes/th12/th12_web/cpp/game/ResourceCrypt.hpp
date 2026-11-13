#pragma once
#include "Types.hpp"
#include <vector>
namespace th11 {
struct CryptParams {u8 key,step;u32 block,limit;};
// Source and destination must not overlap. Limits are in original whole blocks.
bool resource_crypt(const u8* source,u8* destination,u32 size,CryptParams params,bool encrypt) noexcept;

// ZUN PBG3 per-entry cipher tables. The index is `sum(name bytes) & 7`, and the
// table itself is chosen by archive version. Values are the ones the original
// archives were produced with; cross-checked against thtk's `thdat95.c`
// (`th95_crypt_params`, `th12_crypt_params`, `th13_crypt_params`, `th14_crypt_params`).
extern const CryptParams archive_crypt_th95[8];
extern const CryptParams archive_crypt_th12[8];
extern const CryptParams archive_crypt_th13[8];
extern const CryptParams archive_crypt_th14[8];

// ZUN reuses one table per engine generation:
//   9.5/10/10.3/11            -> th95
//   12/12.5/12.8              -> th12
//   13                        -> th13
//   14/14.3/15/16/16.5/17/18/18.5/19/20 -> th14
const CryptParams* archive_crypt_table(u32 version) noexcept;
}
