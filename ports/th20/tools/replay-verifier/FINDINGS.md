# Replay parity investigation — 2026-10-12

## Confirmed correction

`Weapon::end_phase`, original VA `00532f20`, clears the phase-active byte at
offset `0x34` and returns zero. It does not retire existing shots. Evidence:
`analysis/ghidra/pseudocode/00532f20.c` and
`source_reconstruction/overlay_system/evidence/00532f20.asm`.

The previous SDL-only red-stone patch retired live rage-pattern shots at phase
end. In the supplied replay's Stage 6, that removed a later hit and changed
gameplay RNG consumption. The first measured difference was input frame 1045:

| State | Original | Before correction |
| --- | ---: | ---: |
| RNG | 1660535977 | 275043261 |
| Score | 42221113 | 42221112 |

The correction removes this retirement loop. New shots select normal patterns
when the phase flag is cleared; already fired shots retain their original
damage and lifetime. No extra RNG calls or replay inputs are introduced.

## Measured coverage

Two bounded original captures reached 3000 and 6000 input frames. Their
captured 32 state words match the corrected candidate at every shared frame.
This includes exact RNG state, player position, input, score and life counters.
The original also loses lives in this fixture.

Identities:

- Original EXE: `a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897`
- Replay: `e85fb9481d948a0db43043915a90bdce8f5a0593df42cc4d8c59e9476c46eb68`
- DAT: `9db8d7c43fbacec95614163d4c3e1d254e82169f8550177ee849831130590494`
- Loaded candidate WASM: `a316ae1e39f5667ee3a5012c548aeb8f8efd57e7f5dee2c1ca0f0909935616fc`

Local capture files are under ignored `build-perf-probe/`: `original-recheck`
and `candidate-recheck`, with JSON receipts and `.rows.jsonl` state streams.
Use `compare.mjs` to regenerate the comparison receipt.

## Remaining limits

This is **INCOMPLETE**, not a whole-replay PASS. An earlier original long
capture timed out at frame 4893; a fresh process subsequently reached 6000.
An earlier candidate long capture stopped consuming inputs after frame 36331,
with game flags `0x50` and lives `-1`. That run failed and is not evidence of
natural completion. Its old collector omitted the thrown error from the
receipt; the current collector records errors and rejects such captures.

The 32-word schema does not compare every bullet, laser, enemy, animation or
ECL object. Later six-stage dialogue/Boss behavior, full completion, touch
interaction and other stones still require further comparison. The adapter's
tests protect exact tick ordering, fixture identity, resolution identity,
runtime-error rejection and incomplete-versus-complete reporting.

## Replacement fixture and oracle correction

The user replaced the fixture with `th20_12.rpy`, SHA-256
`6a45b217a80e01dae461e94e978d660cad53f088a8ddc51521c2379cc0e53e00`.
The state schema is now v2 with 36 words, adding point value, special-item
count, point ratio and overlay meter.

A debugger-launched original first diverged in special-item count at Stage 1
input frame 4086 (+4 in the candidate), then score at 4121. Item inventories
located the cause at 4034: identical item creation and RNG, but several original
positions were `0xfe6efeee` in both X/Y. This is Windows debug-heap freed-block
poisoning. Damage hit callbacks can free their Region before the retail code
reads it again for the reward position. The source already snapshots the
contact before dispatch to avoid reading freed storage.

The collector now sets `_NO_DEBUG_HEAP=1` for the private original process.
With the ordinary heap, all 36 state words match for the first 4200 frames;
all sampled item states/types/positions/velocities match for frames 3600-4090.
See `original-normalheap-r12`, `candidate-stats-r12`, and
`normalheap-comparison-r12` in ignored `build-perf-probe/`. This remains
INCOMPLETE; a full replay capture is in progress. Do not modify gameplay to
reproduce debugger heap poisoning. Earlier debug-heap comparisons are not
ordinary-launch whole-replay evidence.

## Current fixture: th20_01.rpy

The user selected `AppData/Roaming/ShanghaiAlice/th20/replay/th20_01.rpy`
after stopping the previous replay because retail playback also desynced.
SHA-256: `6ff1260c3e9762cb43e45f07bc50684c9725203551033946fec4ab62092f9a4a`.
A private immutable test copy is retained under ignored
`build-perf-probe/fixtures/th20_01.rpy`. Recorded stage lengths are
12847, 13886, 19081, 19290, 17636 and 24903 frames (Stages 1-6).

The first 6000 completed frames match all 36 state words exactly, using
`original-r01-prefix`, `candidate-r01-prefix` and `r01-prefix-comparison`.
Both collectors exited normally; the result is INCOMPLETE because this is
a bounded prefix. The complete captures run sequentially at below-normal
process priority, and browser verification is capped at 60 ticks per second
to reduce desktop contention after the user's computer became unresponsive.

## Oracle reliability warning after manual playback feedback

The user reports that direct retail playback of th20_01.rpy is normal, while
our instrumented playback desyncs. The interrupted full original capture
reached Stage 2 with lives -1; this is not accepted as ordinary retail behavior
and must not drive gameplay fixes. No game was restarted after this feedback.

Read-only checks on 2026-10-12 found:
- Installed and downloaded EXEs differ in 199 bytes, all in DOS stub/Rich-header
  file offsets 0x40-0x10f. The existing opcode comparison reports no instruction
  differences; there is no evidence here of a different gameplay version.
- The private synthesized configuration differs from the user's configuration
  in flags (0 versus 0x81) and saved window coordinates. It is not a faithful
  copy of the manual-launch environment.
- The collector launches under a debugger, modifies the private mutex, forces
  focus state, overwrites four physical-input slots each update, and observes
  software breakpoints with single stepping. Debug-heap poisoning was one
  confirmed artifact; disabling it did not establish full-run reliability.

Before further original/source parity claims, validate the harness against a
normal launch of the same executable, fixture and configuration, then introduce
observation and automation separately. Earlier matching prefixes remain
measurements of this harness, not proof of ordinary full-playback equivalence.

## thprac mechanisms checked on 2026-10-12

See `reports/TH20_THPRAC_REPLAY_FIXES.md` for the main-story transition,
Extra resolution and expired summon explanations and source identities.
The SDL animation reset now clears the whole VM while preserving its pool
index, following thprac's reset hook. Twelve poisoned-lifetime tests pass.
The summon attack death callback previously captured an Entry reference;
gauge timeout frees that Entry while its attack enemy may remain alive.
It now captures the selected color by value. Both changes compile in the
475-translation-unit WASM build. No full replay PASS is claimed.
Transition timer offsets and old expired-summon outcomes are not inferred
from matching prefixes or manufactured from the current machine's timing.

## Immutable recorded score audit and timing exclusions

The user requested recorded scores as independent replay evidence. For this
fixture, Stage 1 ends at displayed score 5,843,300, exactly the stored Stage 2
start score. The continuous candidate ends Stage 2 at 13,021,900 instead of
the stored Stage 3 start score 32,069,330, with lives -1 instead of 4. Direct
Stage 2 playback also fails that checkpoint (24,502,260). The header's final
displayed score is 598,394,770; it has not been validated against completed
result-screen scoring.

Candidate capture receipts now contain the immutable score audit; a score
mismatch prevents parity acceptance. Restored stage starting scores cannot
prove preceding simulation correctness. Audit code retains all 64 score bits.

Controlled continuous-play diagnostic runs through Stage 2 frame 4028 found
no state-word differences for charge-age offsets 0, 1 and 2, or for shot-clock
offset -12 (12972 modulo 120 becomes zero). Neither phase explains the early
Stage 2 divergence in this fixture. These experimental runs are explicitly
excluded from ordinary replay acceptance. Writable clock experiments now
require the compile-time TH20_REPLAY_CLOCK_EXPERIMENTS macro; default builds
only expose read-only activation snapshots.

## Replay option cache repair

Read-only firing diagnostics found different option offsets from Stage 2 frame
1 between continuous and direct entry, before the scalar RNG/score difference
at frame 84. Loading applies the saved profile and refreshes the player's power
before the recorded player table and weapon selection are restored. Equal
power levels then suppress option rebuilding, leaving the previous weapon's
offsets and initializer attached to the replay's shots.

SDL replay activation now restores weapon selection before refreshing power.
It compares all active normal/focused fixed-coordinate option offsets to the
selected weapon's offsets and invalidates the power cache when they differ.
The same power refresh used during ordinary play then rebuilds the options.
There are no timing offsets, substituted inputs, RNG changes, or score writes.

`r01-option-fixed-continuous` captures 27,000 ticks. Stage 1 remains identical
to the earlier candidate at every captured state word. Stage 2 ends at
32,069,330 with four lives, exactly the stored Stage 3 checkpoint, and naturally
enters Stage 3. Its first 130 Stage 2 ticks now match direct entry's 36 words.
Previously continuous Stage 2 ended at 13,021,900 with lives -1. This is a
confirmed improvement for the fixture; it does not establish full-run parity.
The candidate WASM identity is
`7ef9ea7b0ccd09c0faa9b5e370f4bf7fa47d988886c90feee35485e7016e8917`.
