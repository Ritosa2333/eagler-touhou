# TH15 oracle acceptance — 2026-10-06

All five required cases passed the common comparator against independently
captured original-executable state. Independently generated current-core traces
also passed against the accepted immutable golden set.

| Case | Compared ticks | Result |
| --- | ---: | --- |
| Demo 0 | 7,339 | PASS |
| Demo 1 | 6,620 | PASS |
| Demo 2 | 6,269 | PASS |
| Marisa Lunatic, all six recorded stages | 146,871 | PASS |
| Marisa Extra clear | 58,483 | PASS |
| Total | 225,582 | PASS |

Candidate WASI core SHA-256:
`89c7ccc1e37b2e4c32f022a340891a2bd2469c40b292c21b25baf4cb7e9af739`.
The diagnostic driver binds production RunCompletion. Native and candidate
terminal scores were 2,001,749,730 for Lunatic and 665,861,540 for Extra.

Intermediate-stage completion is proven by the original next-stage request,
not an ending flag. The final Lunatic and Extra receipts observe scene flags
`0x4010`. Earlier intermediate receipts retain a diagnostic player-mode flag
value; that value is not their completion authority. Player mode and scene flags
have separate owners, protected by focused regression tests.

Backend support evidence independently compared all 146,870 available gameplay
ticks in the pinned real original-process capture with the x86 provider's 13
declared fields. Every comparison matched. The process reference stops before
the final clear tick, so it is cross-validation evidence, not a replacement for
the complete newly captured oracle. Observation-hook optimization additionally
matched the full 6,269-tick Demo 2 trace.

Run evidence is under `artifacts/replay-verifier/`: `standard-oracle-result.json`,
`published-all-result.json`, `retail-backend-final.json`, original append-only
rows and completion receipts. Golden provenance and compressed content hashes
are bound in `golden/manifest.json`.

The schema covers 13 declared gameplay words. It does not certify all entity
memory, raw shot/damage allocation slots, visual RNG or repeated Draw purity.
The older full-memory Reimu/Sanae world comparisons still have pool/ANM
differences and remain unresolved outside this selected-field gate.
Daily providers restore each recorded stage independently; frontend cross-stage
loading and ending scenes are separate. Demo coverage consumes stored input
fixtures, not the title idle/3900-frame timeout/return lifecycle.

No Browser Runtime, audio/storage, real-device, high-refresh or Presentation Lab
acceptance is implied by these WASI-core results.

After high-refresh PR #3, a source-hash-verified diagnostic core
`cc354b0d6bdab968e405ad1df6fe718f0984982dbe978dcbc444becac6f4bfba`
again matched all 225,582 ticks against this golden set. This is a selected-state
core regression; full browser Replay and Presentation Lab coverage remain separate.
