# TH08 purple THPrac migration

The implementation is promoted from the isolated experiment into the canonical
TH08 Eagler branch after source, unit and build checks. No deployment or GitHub
push is part of this migration. The commit introducing this file records the
experiment head; the base is recorded below.

## Authority and extraction

- Purple source: `thprac_purple`, commit
  `17780056107a014e88d35fceb3ad86c121d94287`, version 2.2.2.7.
- Normalized TH08 source SHA-256:
  `2f0f262046f8c6bf2ddbded1fed0bd781effdb91a673a4b11f3782cda8d3dc0a`.
- Existing blue 2.3.0.3 replay source SHA-256:
  `230c3aacf25e1fdb350dd3be3f5974923c28a5f82325a7382c97516c6910cc3e`.
- Baseline TH08 commit: `4839485`.
- Implementation boundaries follow eagler-touhou's THPrac playbook,
  `ADAPTING_A_GAME.md`, runtime contract and adapter behavior invariants.
- `generate-thprac.mjs` extracts native stage/section patches, version,
  glossary and section IDs. `--legacy` retains the previous blue patches
  only for recordings explicitly marked 2.3.0.3.
- `generate-thprac-tools.mjs` uses the TH10/11/15 shared-tool boundary for
  input filtering, original key HUD, APS recording and reaction test.

## Connected owners

- Practice parameter bridge: 26 words, retaining schema at index 22;
  the previous 23-word bridge is still accepted.
- Last Word: 17 native sections, original Spell Practice stage/resource
  owner and spell IDs 205–221. The custom parameter gate accepts only the
  matching selected Last Word, not arbitrary vanilla Spell Practice.
- Mystia point setup: native 520-byte ECL payload appended to the owned
  program without invalidating existing VM/script pointers. Writes and
  relative jumps remain bounds checked; retail ECL validation is retained.
- New purple recordings use the extracted purple version. Existing blue
  recordings select their original stage/section timings.
- Disable X/Z/Shift and force Shift filter merged keyboard/controller/touch
  gameplay input; replay input bypasses these filters.
- Fast retry: native dissolve/decrement hooks, 15 logical-frame Escape/R
  sequence, with transient reset on input clearing and new stage ownership.
- Enemy invincibility: U/bridge bit 10 suppresses the native enemy and
  familiar-parent life subtraction while retaining hit/score processing.
- Native F8 information shortcut retains the requested Tab alias.
- One-key game over writes the actual player life owner, not its global
  mirror. Force Last Spell follows the native ECL variable read.
- Lock timer is a display-only sidecar reset at native GUI initialization
  (00437AF3) and ECL cases 127/132. Only boss 0's GUI countdown setter
  (0042F34D, called at 0042B984) increments it; ordinary enemies and drawing
  do not advance this clock.
- MASTER overflow preserves native hooks 00417C6B/00417DF6: full decimal
  digits occupy a 25-unit lane, with distinct capture/attempt start and end
  offsets. Original executable constants (32/7/13) were verified read-only;
  counts at or below 999 and THPrac OFF retain the retail rendering path.
- Bullet/laser hitbox renderer is extracted from native RenderBtHitbox,
  retaining clip, expansion, lifetime phases and geometry arithmetic. Typed
  projectile field mappings have compile-time native-offset assertions,
  including the native low-word timer test at bullet offset 0xD88 (not an
  invented active-state substitute). Player bounds come from movement's
  collision owner. Visual validation remains pending.
- Mobile UI: queued down/up edges, retained pointer-ID ownership through
  out-of-window drags, cancellation cleanup and no persistent-fire capture
  after closing Practice. Native finger and launcher pointers share this path.
- UI font remains optional when THPrac is off. When on, shared Unifont
  covers all three locales plus the native arrow/Delta/Sigma glyphs.
- F12 game speed uses the native widget and separate logical cadence. Normal
  60 Hz keeps the existing skip-late-deadlines behavior; custom FPS disables
  presentation interpolation and retains authoritative draw/audio ticks.
  BGM pitch changes without changing sound-effect pitch.
- All-clear bonus follows native stage and Practice gates; its display retains
  the independent Practice-replay gate. DOSWNC bypasses the two native checksum
  reset owners without changing normal checksum behavior when disabled.
- RSQRT Intel/AMD tables, CPU selector and Custom import/export are connected.
  Wasm cannot capture the host's RSQRTSS instruction: Custom initially captures
  the existing browser reference seed, not an alleged native hardware result.
  Importing a native 8 MiB capture replaces that table atomically.
- Browser-session shutdown clears menu/key/pointer state, cached draw generations,
  reaction-test state and FPS widget state. Translated parameter labels reserve
  their measured width without changing native panel or font dimensions.
- Launcher declares the U touch key using its existing extra-key contract.

## Checks

- Purple and legacy generators: `--check`.
- Shared tools: `--check`.
- `check-th08-practice.mjs`: section/bridge/ownership source contracts.
- `check-th08-thprac.cpp`: PRAC/USER round trips, legacy bridge, legacy
  version provenance, point parameters and all 17 Last Word stage/spell maps.
- `check-th08-purple-ecl.cpp`: malformed injection rejection, pointer
  stability and practice-only instruction ownership.
- `check-th08-purple-input.cpp`: disabled/forced keys and exact retry sequence.
- `check-th08-purple-history.cpp`: 999 boundary, full signed-positive counts,
  native capture/attempt digit positions and final cursor offsets.
- `check-th08-purple-hitbox.cpp`: extracted renderer's ordinary bullet
  expansion, laser geometry and appearance/disappearance draw gates, clipping
  balance, disabled option and non-game mode.
- `check-th08-purple-rsqrt.cpp`: extracted Intel/AMD table reconstruction,
  IEEE specials, mantissa buckets/exponent scaling and atomic custom-table
  import. This unit needs Emscripten memory growth for the native 8 MiB tables.
- `audit-th08-purple-rsqrt.py`: read-only retail executable verification of
  all 12 purple RSQRT instruction addresses/register operands. Retail SHA-256:
  `330fbdbf58a710829d65277b4f312cfbb38d5448b3df523e79350b879213d924`.
  All 12 hooks and installer dispatch slots were audited. Two callable game
  hooks map to GraphicsMath SIMD Vec3 normalization. The other ten D3DX paths
  have no game call sites in the audited retail executable; no substitute
  gameplay APIs were invented for them.
- Canonical SDL3 build: 3,449,212 bytes;
  SHA-256 `0dfba63705d9d15533863d7e4d77974a1a48e0c25c1b1821c1a9c7d24007c661`.
- Shared touch lifecycle: 11 checks; frame cadence checks compiled with
  the local Emscripten toolchain because the script's relative WASI SDK
  path is unavailable in this worktree.
- `check-th08-purple.mjs`: freshly compiles/runs nine C++ groups, including
  production GraphicsMath objects and SpellHistory/EclProgram sources: PASS.
- `check-high-refresh-contract.mjs`: PASS with pinned common testkit
  `59b83d85a1e4a404c6ce51c4f044c5806a7d14ad`.

These are unit/source/build checks, not a claim of physical-phone or real
gameplay verification.

Canonical promotion: experiment head `17a5460bccaaa82b81e1151fa09c2cd547790d6c`
was integrated as `a26c4d6` / `1428ac2` on the local `eagler` branch. The
canonical build, nine C++ groups, both section generators, shared-tool generator
and TH08 practice/high-refresh source gates passed again after integration.

Workspace checks are NOT green: the existing TH15 catalog/integration membership
disagreement stops `check --workspace`. Separate runtime/replay contract checks
also require absent `th11-eagler` / `th06-eagler` checkout paths. The existing
TH08/TH10 portable TouchController byte-identity check fails on their baseline
differences; neither TouchController was changed by this migration. Launcher
touch-runtime and shell-protocol checks passed. These workspace failures were
retained rather than changing unrelated titles or weakening their tests.

## Manual validation boundary

- Browser startup and ordinary game rendering were observed with the final
  build; prior checks exercised F12 dropdown focus and parameter entry.
- All 17 Last Word stage/spell mappings are unit tested, but the complete
  real-gameplay matrix and physical-phone interaction are not run. The user
  requested to take over manual testing; do not report these as PASS.
- Browser Custom export is not a native host CPU capture. Use a native imported
  table when reproducing that hardware behavior.
- Local validation staging contains private resources outside Git. It is not
  a production deployment or a distributable source artifact.
