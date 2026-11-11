# DOSBox-X / js-dos runtime for TH03

Pinned npm package: `emulators@8.5.2`, obtained 2026-10-12.

- Package: https://www.npmjs.com/package/emulators/v/8.5.2
- Emulator source: https://github.com/js-dos/emulators/tree/6bbf0925d221a7304ea8e0dda1e89b8dc01569b2
- DOSBox-X source: https://github.com/js-dos/dosbox-x
- Original project: https://github.com/joncampbell123/dosbox-x
- GPL-2.0 license: included in `LICENSE`.

The original npm archive is retained in `vendor-source/emulators-8.5.2.tgz`.
Pinned glue, binaries, package metadata and extracted upstream worklet are in
`vendor-source/emulators-8.5.2/`. Distribution hashes are in `runtime.json`.

`emulators.js`, `wdosbox-x-jspi.wasm` and `audio-worklet.js` are unchanged upstream
assets (the worklet was extracted verbatim from the upstream source map).
`dosbox-jspi.mjs` adjusts the worker dispatcher and exports the JSPI factory.
`wdosbox-x-fast.wasm` excludes held modifier keys from artificial timer repeat;
the exact audited binary transformation is `tools/fix_modifier_repeat.cjs`.

The TH03 adapter is `web/dosbox-adapter.js`, `web/dosbox-disk.js` and
`web/dosbox-snapshots.js`. It runs the existing patched TH03 executables and
GAME.BAT with the PMD driver under DOSBox-X internal DOS. It does not execute
the NP21 core or VEM486 pager.

Local execution uses JSPI with Simple CPU and automatic cycles. Network
execution uses deterministic virtual time and a source-built full Asyncify
core, including CPU and nested BIOS call paths. `core-build.json` records
the fixed source revisions, build tool fingerprints and matching runtime hashes.
The native rollback interface includes continuation access and four mutable
globals. Complete RAM, execution state and filesystem
undo are captured by the adapter. Network correctness requires restore/replay
and multi-client verification; merely shipping this build is not validation.

The shipped network core was built by `tools/build_th03_core_candidate.py` from
emulators revision 6bbf0925d221a7304ea8e0dda1e89b8dc01569b2 and DOSBox-X revision
788fde99d3d451eab3621009186e5abe516f2b9c, using Emscripten 4.0.15 and Binaryen 132.
The local JSPI runtime retains its existing build and binaries. Build tools and
source copies are local development material, excluded from this distribution.

The x86 CPU is interpreted. This build does not include a dynamic x86 compiler.
Unsupported local browsers receive a startup error. There is no NP21 fallback.

2026-10-14: `wdosbox-x-rollback-tracked.wasm` adds a separate 4 KiB page-write
bitmap to the complete Asyncify core, using the same instrumentation as TH04.
`dosbox-rollback-tracked.mjs` is generated from TH03's own glue, retaining its
zero-wait/coalesced scheduling and filesystem interfaces. Dynamic stores and
bulk writes mark pages; fixed-address stores are conservatively scanned. Host
heap writes and TH03 pause/touch mailbox writes mark pages as well. The paired
`snapshot-diff-tracked.wasm` compares marked pages byte-for-byte; snapshots use
batched before-images and reusable buffers. Only rollback-enabled network games
on multi-memory engines select this core. Local JSPI and confirmed-only network
games use the untracked full Asyncify core. Historical journal build entry:
`tools/build_write_journal.cjs`;
Binaryen 132 and Acorn are development dependencies under `build/`.

2026-10-15: Both network cores and their matching glue now come from TH03's
source-built candidate. Native virtual scheduling preserves zero-time
cooperative yields and the existing 16000-cycle budget. A guarded synchronous
Normal interpreter retains the complete interpreter fallback. Exact memcmp uses
eight-byte equality checks. PC-98 planar lookup code is present but disabled;
the RGB presenter remains active. No TH04 guest-address idle fusion is enabled.
The tracked snapshot helper caches eight bitmap bytes, skips clean groups and
retains the original byte scan for sampled dense inputs. The original SIMD page
comparator, packed before-images and restore layout are preserved. Standalone
differential/storage checks passed; TH03 gameplay and mobile performance remain
unverified. The distribution manifests select native scheduling and CPU, with
planar lookup disabled (`cpu` profile plus `adaptive-word8` snapshots).

2026-10-15 battle update: `tools/build_battle_idle_candidate.mjs` applies a
pinned Binaryen transform to both TH03 network cores. It recognizes the exact
TH03 RAM wait sequence MOV AL,[6599]; MOV AH,0; CMP AX,[11ea]; JA back. It charges
complete four-instruction iterations against the existing budget and executes
the final iterations normally. The guarded synchronous lane and direct RAM
checks remain mandatory. No new mutable state or guest executable edits are
introduced. Synthetic instruction parity (862 cases per core), restored replay,
tracked-store coverage and helper initialization passed without booting DOS.
Actual battle performance and multi-client gameplay still need manual testing.
