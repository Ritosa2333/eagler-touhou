# Main-thread self-review: damage fields and respawn

The user prohibits subagents. This is a separate author self-review, not an
independent review or full-game acceptance. No Git commit/branch is created.

## Checked contracts

- Fixed executable identities, explicit code/data boundaries, bounded emulation,
  no fabricated allocator/API/callback returns, and no original-directory writes.
- Damage-pool allocation, wrap/reuse/full-pool behavior, unit-clock expiry,
  stationary position stores, circle contact/interval/tag/cumulative limits,
  query caps and native internal score tail.
- Actual shot impact before source reads; sticky errors after partially applied
  contact, respawn and cancellation callbacks.
- Native state2 wait, state0-before-field ordering, conditional control writes,
  preserved death position, entry integer movement, protection and return to1.
- Native bullet IRQ/effect execution rather than deleting cancelled bullets.
- Explicit HUD callback snapshots; nonzero live callbacks and populated laser
  behavior are not supplied invented success values.
- Drop configuration406/407/408 and preservation of the mandatory nonempty drop
  lifecycle; `-9989` resolves only with explicit player coordinates and reuses
  the already sampled original aim helper, without new RNG consumption.
- Browser pause/reset/variant resource ownership remains separate from WASM
  state. Newly opened diagnostic text must fit the390px viewport.

## Important findings fixed with regressions

1. Entry resets `+0xf70/+0xf74` movement deltas, not the independent rapid timer
   at `+0x438fc`. Resetting the latter restarted shooting after C was released.
   The original entry probe now starts with dirty movement deltas and a nonzero
   shot timer; only the deltas change. The world/browser regression requires
   the released timer to remain-1 and no unsolicited return volley.
2. Stationary damage-field integration still adds positive-zero velocity before
   quantization. Omitting those stores retained negative zero on X/Y/Z, unlike
   the original bytes. A negative-zero vector first failed the image comparison;
   the portable update now performs the original zero additions.
3. The new shot-result panel lacked the pre-wrap rules applied to older panels.
   A390px full respawn browser check reproduced horizontal overflow; the existing
   rules now apply to that panel too.
4. Optional explicit SSE/x87 probe configuration changed shared tool provenance.
   Six dependent reports were genuinely regenerated; every original instruction
   and data output compared unchanged after removing only engine metadata.
   Pinned report hashes were refreshed, not bypassed or relaxed.
5. A nonempty configured enemy drop list must not reach the older empty-list
   ordinary death host. A synthetic ECL regression failed before an explicit
   additional-drop lifecycle boundary was installed.

## Minor follow-up

- The live HUD callback diagnostic can later include the exact slot/target as
  well as its original call-site family. It currently stops visibly and safely.

## Requirements still not complete

## Continued Type/Drop Review

- Added exact typed18/20 profile identities, actual72/74 natural/IRQ8 execution,
  native post-ANM tails/common updates and distinct375/374 /381/380 effect trees.
  Type20's hitbox field7 and color6 cancellation381 are independently sampled;
  a type18 alias or color3 sample is not substituted.
- Additional-drop scatter/angle/RNG matches original separate stores, including
  item-callback interleaving, empty-list RNG and sticky failure. Legacy incapable
  death hosts retain the nonempty-list diagnostic rather than silently dropping
  authored items. Direct505/506 synthetic ECL stores preserve exact floats/words.
- ANM82's inherited flag was wrong: original dispatch45c501 writes0x40000000,
  not0x20000000. A real script74 red test caught it. Shared shot sync now reads
  the correct flag, and real type20 draw uses stored original emission angle plus
  native half-pi normalization instead of an unrotated sprite or atan2 recovery.
- Full Python testing exposed profile preparation overwriting76 sound IDs with
  an old seven-ID subset. A failing resource regression proved the downgrade;
  profile preparation now invokes the full original sound-table preparer.
- Current actual stage is stopped at537, after type20 creation. No nonzero
  extension is silently ignored. Full-stage/full-port work remains uncompleted.

## Remaining Requirements

## Extension/HUD Integration Review

- Verified original whole pre-ANM paths, not just scalar helpers: type16 init,
  update, half/full motion, single lifetime step and double cooldown match exact
  original stores. The missing cooldown decrement was reproduced and fixed.
- Malformed extension continuation previousTime=INT_MAX first triggered UBSan;
  numeric/provenance checks now reject before addition. Sound callbacks cannot
  replay a rejected operation or reenter the same bullet update, even if a
  nested callback clears its output error string. Explicit regressions pass.
- Actual front0/47 and charge child bits resolve the mandatory fourth-death UI
  work. This is an authored visual subset, not complete controller initialization.
  The tested front programs have no RNG requests; a request outside that subset
  stops visibly. Native ANM85 bit31 replaces its wrong inherited bit30 mapping.
- Narrow typed inherited variable writers are confined to proven word offsets,
  retain data through real spawns, and refuse other negative lvalues. Normal261
  static/emulated request and mirrored-angle integration tests now replace its
  outdated unsupported-opcode assertion; the original transform limitation for
 256 remains explicit.
- Difficulty predicate disassembly was initially misread as float inversion;
  twenty real integer returns and20float pre-return snapshots disproved it.
  Production retains original rank equality and does not generalize arbitrary
  float/int getter conversion or arbitrary-address writes.
- At that checkpoint the next type4 record still stopped; the continuation below
  supersedes that boundary, not the remaining full-game obligations.

## Midboss/control continuation self-review

This remains author self-review in the main thread, not a separate reviewer.

- Rechecked original manager lvalue dispatch against integer reads and FILD
  readers; manager words are shared per world lifetime, not copied per enemy.
  Reset/typed rejection, live child writes and zero RNG consumption are tested.
- Verified original zeroing uses actual CRT bytes on explicitly preallocated
  state, with a declared scalar-fill capability flag. No complete initialization
  or native allocator return is inferred from that constructor-tail sample.
- Compared all54 original411/412 paths with ordinary and UBSan execution.
  Negative registration keeps its retained index; positive replacement does not
  invent cleanup of an old slot; sevenfold life uses defined uint32 wrapping.
  Invalid bounded slots fail before any portable store. Boss phase/death still
  cannot fall through to ordinary enemy explosions or drops.
- Four259 prefixes stop at real creation entry, not an invented result. Only
  positive nonprimary requests are supported; primary/negative cases stay explicit.
  The live world retains the actual bank0/99 sprite454 before189/op88 fails.
- ANM111's18 literal handlers and60 native consumer prefixes distinguish pointer
  absence from missing host state, preserve unrelated bits, and verify IRQ bypass.
  Actual99 pauses its clocks/interpolation for twenty ticks and resumes after the
  original control bit clears. Source snapshots come from the same explicit
  player control state, never a fixed assumed pause/continue value.
- Existing synthetic tests asserting bank0 was forbidden or111 always failed
  immediately were updated to the recovered semantics, retaining out-of-range,
  missing-snapshot, sticky-failure and invalid-replacement coverage. An authored
  aura is not proof of native child scheduling, complete pause/freeze or rendering.
- The86/87/89 native flag-parity observation, registered-boss retirement outside
  the sampled path, full initialization, remaining callbacks and enemy child/global
  ordering remain explicit follow-up work. No unrelated behavior is guessed.
- Fresh desktop/390px browser checks reach the same3170/189/op88 boundary with
  zero browser page errors; full-suite totals are written only after completion
  by the checkpoint recorder. A screenshot or passing component is not acceptance
  of a whole boss/stage/game.

## Enemy Children/Gauge Continuation Self-Review

- Verified actual88 requests separately from the post-bind suffix. The suffix
  executes real tail registration/parent linking on declared prebound storage;
  no allocator, VM bind or Windows API return is fabricated. Saved-next prefix
  proves why per-root updating would produce incorrect child/RNG order.
- Ordinary/UBSan actual189/99 plus an unrelated root match established Scene
  for100ticks, shared RNG seed/calls, control hold/resume, colors, birth/expiry
  and composed geometry. This is portable integration evidence, not native CPU
  whole-frame equivalence. Only one-level bank0/189 attached88 is accepted.
- Reviewed node IDs/parent pointers, replacement rollback, owner removal, tail
  insertion, live4096-node boundary and failed-parent visibility. A derived
  overflow regression keeps explicit presentation errors instead of silently
  hiding bad geometry. Error/throw/reentry cannot leak staged children.
- A callback that ignored a nested rejection exposed nonsticky reentry and extra
  RNG consumption. RED/GREEN now requires one callback only, sticky operation
  failure into the owning node, no publication and clean subsequent attempts.
- Native427 records12 complete original handler/setter paths. Portable data
  preserves authored ratios/colors and bounds indices before mutation. Original
  gauge drawing/animation is not implemented and is not implied by these stores.
- Actual WASM and browser load all three real ANIM include banks independently;
  metadata/textures remain keyed by their native1/2/3 identities. Unsupported
  appearance programs still stop rather than being inferred from atlas loading.
- A dedicated desktop/390px3170 check found score-font glyph IDs overlapping
  charge-HUD IDs, despite the first broad suite passing. Added font loading and
  periodic draw-ID checks to the real world test, observed RED, then separated
  score glyphs into9000000. No game timing, resource or scoring math changed.
- The current real stop is MBoss offset516/op414, after actual primary/aura/child
  and two427 stores. Read-only414 spike proves real6300/8200 health thresholds,
 1800limit and two pointers toMBoss2. Its health/time trigger, countdown/HUD and
  mandatory VM reset/continuation remain unimplemented, so414 is still rejected.
- Fresh final full-suite results and ten browser-script evidence hashes are
  recorded only after completion; earlier142/332 results preceded the draw-ID
  regression and are not substituted for final-source verification.

## Phase/Position Continuation Self-Review

- Six original414 registrations and50complete41dea0 returns cover disabled
  records, priority, signed health boundaries, zero/positive/negative limits,
  elapsed before/at/after expiry, timer resets and timeout-specific globals.
  Four-word record isolation now has an explicit regression after a research
  fixture's five-word write accidentally made its neighbor active. Original
  instruction traces were rerun on corrected inputs, not simply rehashed.
- Owning strings preserve real callback names; negative threshold modifies only
  its own first word. Invalid slot/name is rejected before registration mutation.
  Pure ordinary/UBSan query vectors compare records, life, timer bit words,
  countdown, timeout flags, spell state and manager18, not just selected targets.
- Integration calls the query at the post-damage and common after-ECL sites,
  retaining zero-query/bypass execution. A genuine regression verifies a phase
  clamp/consumption precedes ordinary life-copy/death work. Missing timeout
  globals and selected mandatory VM continuation remain explicit sticky stops.
-301 has six original complete initializers and eight complete mode4 evaluator
  returns including real curve/timer bytes. Float stores and sentinel comparisons
  match exactly. Position is interpolated as the absolute component, not as a
  temporary combined coordinate overwritten by native combine; the synthetic
  first-step RED caught that integration mistake before acceptance.
- Real Normal branch keeps6300/1800 and does not execute harder8200 branch.
  Current real stop is MBoss offset708/op526 after entry interpolation setup,
  with boss still above the playfield. Tests/browser prove data and ownership,
  not visible completed entry, live phase switching or a finished boss battle.
- Whole-source91hash checkpoint,144Node/C++/WASM,334Python and final visual/
  browser evidence are accepted only from the fresh completed logs and recorder.
  Old142/332 numbers cannot substitute for this source state.

## Full-Port Obligations

The complete original initializer/HUD, Z charge/presentation/freeze, ice
propagation, Perfect Freeze, populated laser host, all ordinary bullet profiles
and extensions, backgrounds, bosses, complete route/difficulty progression,
endings, saved scores, native replay simulation and full rendering parity are
still full-port obligations. None is downgraded to a cosmetic issue or silently
treated as outside the user's request. This checkpoint is not a final game.

Fresh stable verification totals and build identity belong in
`../main-thread-verification.json`, not copied from an older run.
