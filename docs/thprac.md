# Purple THPrac shared tools

`eagler::thprac` is an optional C++17 source-level component, following the
netplay library model. Include `<eagler/thprac/PracticeInput.hpp>`,
`PracticeKeyMonitor.hpp` and `PracticeSpeed.hpp` directly. Types are in
`eagler::thprac`; no title forwarding headers are needed.

The authority is the purple tools already adopted in TH08/10/11/15, extracted
without changes to key masks, 60-frame APS accounting, CSV, auto-shoot,
disable-key ordering, fast-retry or logical speed arithmetic. Retain the
attached MIT notice (Ack, 2022) when redistributing.

## UI integration

The `.inc` files are textual source components included once in the title's
private UI namespace after ImGui and standard-library headers. They intentionally
do not create global singleton owners or introduce an SDL dependency:

- `PracticeKeyHud.inc`: bind `key_monitor` to the session's shared monitor.
  Original purple drawing order, layout and colors are preserved.
- `PracticeReaction.inc`: provide `PracticeCounter::QuadPart`,
  `practice_counter_frequency`, `practice_counter_now` and
  `practice_random_generator` using the platform clock/RNG.
- `PracticeSpeed.inc`: uses the shared speed type. TH08 passes its existing
  reset flag as the optional third argument; other titles retain existing
  static UI lifetime. This is simulation speed, not high-refresh presentation.
- All localized UI strings continue through the title's `label(practice_...)`
  binding, so Launcher language and THPrac language stay synchronized.

Game-specific F12 membership, cheats, ECL patches, warp data, completion,
Replay ownership, graphics hooks, resources, and catch-up caps are **not** shared.
TH08/10 and TH11/15 deliberately retain their different `PracticeCadence` caps.
TH06/07 remain separate consumers until their purple migration is accepted;
do not replace their blue implementation merely to force common ownership.

## Build and release

CMake consumers link `eagler::thprac`. Script-based consumers add `include` to
their compiler search path and hash all shared headers/`.inc` inputs into their
object cache and build report. Ordinary builds use only header types, not ImGui
or reaction/HUD sources.

Use `EAGLER_COMMON_ROOT` for an unpublished local authority. Release consumers
must pin a published common commit in `third_party/eagler-common` before shipping.
Never push a consumer gitlink referring to an unpublished commit.

Run the common regression, title source checks, and each consumer's ON/OFF
builds. A successful compile does not establish mobile rendering equivalence.

Regenerate/verify the shared extraction with:

```sh
node tools/generate-purple-thprac.mjs <thprac-purple-source> --check
```

`--write` updates only the shared extraction. Consumer generators check this
authority and regenerate only their own title hooks/resources. Thus title
regeneration cannot silently restore deleted private copies.

## Initial extraction bases

TH08 `5852561`, TH10 `84dd04f`, TH11 `3f74853`, TH15 `09affbe`.
The local migration is isolated in each `experiment/common-purple-thprac`
worktree. The common library is based on `cedb710`. Consumer promotion and
published dependency pins remain separate from this extraction validation.

Validation at extraction: all 34 common tests pass (Release assertions enabled),
all four full THPrac builds pass; TH10/11/15 ordinary OFF builds pass. TH08
retains its existing always-built practice adapter rather than introducing a
new OFF policy. All four title generators and the central purple generator
pass source checks. TH08 input/speed and TH15 APS/CSV/input/cadence executable
regressions pass. Phone visual/device checks and release gitlink promotion are
not claimed by these gates. TH11 needs its common submodule registered during
that publication step; current validation uses the explicit workspace root.
