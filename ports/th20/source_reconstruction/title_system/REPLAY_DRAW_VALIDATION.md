# Replay page rendering and specimen format defect

`replay_draw.cpp` reconstructs full original `5240d0` (returns 1), and `50a220` (copies filename bytes 7..10 into shared five-byte storage originally `5c6100`). The list contains 25 rows per page; phase 4 interpolates the selected row using the Timer's floating value, then displays seven stage records. A stage's result comes from the next playback stage's PlayerTable when available, including the original mutating continue-count clamp. Otherwise it uses the final UserHeader score. Recording-stage pointers are not substituted for playback records.

`extract_replay_draw_data.py` reproduces all original label and format tables. The original SHA-256 is `a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897`.

The format at `5751c4`, referenced through `5b0a7c`, contains one more `%s` than calls `5244d7` and `52493a` supply. This was investigated by running the complete, unpatched original `5240d0`, not merely inferred from decompiler output. `replay_format_probe.cpp` loads and decodes the actual supplied demo resource using recovered decryption/LZSS source, constructs its real UserHeader and playback record views, and runs the original drawing function. The fixture supplies original CRT initialization data, the actual host timezone, a valid Renderer, and a valid custom replay filename. No source game or original executable file is changed.

All four unmodified original demo headers have slowdown 0 and both page 0 and custom page 1 return 1. Custom rows really include `(null) 0.0%`. For demo1, resource SHA-256 is `07a00087014921f2f53a7dede1d5bd0b39b1b49b9cc71af6ac776d1c40badb48`; decoded SHA-256 is `968079dab965b9f99c39597d8adb5f3520496085485768dd18465f666bcb54d0`.

The same valid decoded metadata, with only the test-owned slowdown field changed, demonstrates the invalid pointer path:

| Float slowdown | Original page 0 | Original custom page 1 | Double low word |
|---|---|---|---|
| 0 | returns 1 | returns 1, `(null) 0.0%` | `00000000` |
| 1 | returns 1 | returns 1, `(null) 0.0%` | `00000000` |
| 12.5 | returns 1 | returns 1, `(null) 0.0%` | `00000000` |
| 0.1 (`3dcccccd`) | returns 1 | access violation `c0000005`, original VA `5482f0`, address `a0000000` | `a0000000` |
| 0.001 | returns 1 | access violation `c0000005`, original VA `5482f0`, address `e0000000` | `e0000000` |

Every input/decoded SHA, timestamp, selected labels, format, result, relocated exception EIP and fault address is retained in adjacent `replay_probe_*.txt` files. Hexadecimal `lines=19` in those logs means 25 lines.

The C++ source explicitly supplies the observed null/zero formatting arguments when the original pointer word is zero. A nonzero word raises `std::domain_error` instead of dereferencing an arbitrary address. The original also reads beyond its argument list for the final floating value, so these malformed-format branches are **not claimed to be universally 1:1**. This bounded handling preserves the observed usable demo rows without silently fixing their displayed `(null)` text.

Reproduce the native resource probes with target `th20_replay_format_probe` in `sprite_renderer/pool_test/build`: arguments are `ORIGINAL.exe ACTUAL.rpy PAGE OUTPUT.txt [SLOWDOWN_FLOAT]`. The optional value affects only isolated decoded test storage. The normal gameplay build never links this probe or loads original machine code.

The shared original-CPU suite adds **28,672 checks, zero differences**, from4,096 full5240d0 calls. It compares the complete TitleInf, Renderer,100 ReplayInf owner views,100 UserHeaders,eight StageRecords, shared five-byte filename buffer and return. Test cases cover list/transition/stage phases, null entries, normal/spell metadata, all valid label tables, negative/extreme integer ages, independent fractional Timer values,64-bit scores, current/next/final stage choices, mutating continue clamp, five scales and near-full text queues. Null-double-word custom-page values include0,±1,±12.5,100,.5. These queue/state checks are not GPU rendering or Replay loading/playback equality. Valid positive timestamps use the actual host non-DST timezone.

`draw_validation.json` records hashes and counts alongside Music/Stage/Spell-practice evidence. `replay_format_probe.json` is separate fault evidence and its cases are not included in successful equality counts.
