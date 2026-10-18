# Confirmed all-seat input archive

`eagler::input_replay` is a file codec, not a title simulator, native Replay
format adapter, transport or state snapshot. A title supplies its versioned
description, appends reconciled and confirmed input frames, and reconstructs
its native lifecycle during playback. This extended multiplayer recording must
never be exported as a supposedly retail-compatible single-player Replay.

## Format version 1

All words and binary32 coordinates are little-endian. No C++ padding, pointer,
host handle, address, or live network session identifier is serialized.

| Offset | Size | Value |
| --- | --- | --- |
| 0 | 8 | ASCII `EAGLRPY1` |
| 8 | 4 | Format version, exactly 1 |
| 12 | 4 | Nonzero title ID |
| 16 | 4 | Nonzero deterministic gameplay ABI |
| 20 | 1 | Active seat count, 2 or 3 |
| 21 | 1 | Recorded viewpoint, less than the seat count |
| 22 | 2 | Reserved, must be zero |
| 24 | 4 | Description length, at most 16 KiB (16,384 bytes) |
| 28 | 4 | Chapter count, 1 through 64 |
| 32 | 4 | Input frame count, 1 through 250,000 |
| 36 | 4 | FNV-1a-32 of the whole file except this four-byte word |

The header is followed by the opaque title description, chapter index, and
interleaved seat samples. Each eight-byte chapter is a nonzero unique label and
its first frame. The first chapter starts at zero; subsequent frame offsets are
strictly increasing and less than the total frame count. Labels are title-owned
and independently validated (for example, stage plus native retry generation).

Each frame contains exactly `seatCount` samples. Each twelve-byte sample has
16-bit held buttons, 8-bit `AnalogMode` (0 through 4), 8-bit flags
(`unlimited=1`, `touchUsed=2`, `touchBomb=4`, no other bits), and two finite
binary32 coordinates. Signed zero and all finite coordinate bit patterns are
preserved. There are no implicit/default seats or trailing payloads. Exact
calculated length must match and total size may not exceed 16 MiB. The checksum
detects accidental corruption; it is not authentication or a substitute for
structural and title-specific validation.

## Ownership

`Begin` reserves the bounded recording owner before frame zero, so allocator
order cannot depend on remote packet/confirmation arrival. `Append` accepts
only the next global frame and a complete valid row. A new chapter cannot reuse
an old label. Titles may preserve a recording across native retries while the
network-local frame counter starts again at zero.

`RollbackCore::ConfirmedInputs` rejects missing/expired simulation history,
unsimulated/abandoned futures, holes in any seat's confirmed prefix, and frames
at or after an unreconciled rollback request. It also checks the actual input
against the simulated decision. It does not restore title memory or prove
snapshot coverage. Reconcile first, then export and retire output in order.

`Inspect` validates all samples without constructing a frame owner for every
native menu preview. `Decode`, `Inspect`, `Begin` and `Encode` leave their
destination unchanged on invalid input. `Encode` can output an explicit
confirmed prefix with a consistently trimmed chapter index, or a replacement
display description, without mutating the live archive. A prefix is never
evidence that a requested longer verification completed.

Titles still owe their metadata/gameplay-ABI/resource validation, native input
and stage/generation ordering, read-only playback storage, local viewer controls,
menu integration, canonical coverage and later-stage seeking. Codec PASS does
not claim these features. The 250,000-frame/16 MiB bound is a file-owner limit,
not permission for a title to silently present a truncated run as complete.

## Tests and phase boundary

`tests/input-replay-test.cpp` covers 2P/3P bit-exact roundtrips, signed zero,
metadata replacement, prefixes, every byte truncation, corrupted/resealed
malformed files, chapter validation, invalid modes/flags/non-finite coordinates,
unchanged outputs after rejection, delayed/corrected input, abandoned futures,
and expired core history. Title/browser tests remain separate evidence.

Complete multiplayer functionality and correctness, retaining an independently
verified ordinary single-player baseline, before performance work. Do not mix
the phases or replace goldens to hide a divergence.
