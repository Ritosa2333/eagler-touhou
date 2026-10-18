# TH11 oracle acceptance — 2026-10-06

All six required cases passed the common fixed-tick comparator, both during
original/candidate capture and with independently rerun current-core captures
against the accepted immutable golden set.

| Case | Compared ticks | Result |
| --- | ---: | --- |
| Demo 3 | 3,878 | PASS |
| Demo 2 | 4,963 | PASS |
| Demo 1 | 4,444 | PASS |
| Demo 0 | 4,764 | PASS |
| Reimu/Yukari Lunatic, six stages | 100,672 | PASS |
| Reimu/Yukari Extra clear | 43,256 | PASS |
| Total | 161,977 | PASS |

Candidate WASI core SHA-256:
`92c1b5d2037358fc55111bc70d73858c61995182c1c64d12ffa2adaa3db0462b`.
Original executable/resource identities, exact Replay hashes, compressed trace
hashes and completion receipts are in `golden/manifest.json`.

The native Lunatic playback score was 668,482,650, while the saved Replay header
contains 668,483,130. That difference is retained. The comparison uses actual
original playback, including the known stage-header point-value quantization,
and does not rewrite expected data to match the saved header.

Evidence: `artifacts/replay-verifier/standard-oracle-result.json`,
`published-quick-result.json`, `published-daily-result.json`, and the original
append-only captures. These are local generated run evidence, not golden inputs.

Scope: declared authoritative fields in the current WASI diagnostic game core,
complete stored Demo Replay fixtures and complete natural long-Replay lifecycle.
Browser presentation/audio/storage, title idle/return timing, full entity
fingerprints, high refresh and Presentation Lab are separate acceptance work.
