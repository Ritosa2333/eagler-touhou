# Game adapter contract

This document defines **where adapter facts live**. For the step-by-step porting
procedure, lifecycle, tests and Definition of Done, see
[`ADAPTING_A_GAME.md`](ADAPTING_A_GAME.md).

The core governance rule is:

> A product difference belongs in a declaration or a specialized format owner;
> shared orchestration must not accumulate title-number conditionals.

Capability classification is owned by `src/contracts/adapter-capabilities.mts`.
Required capabilities are not product flags; inherited Launcher capabilities
need no per-game UI reimplementation; profile-required capabilities become
mandatory when their optional parent profile is enabled; optional capabilities
are product declarations; adapter-internal mechanisms, original-format adapters
and legacy compatibility are separate implementation classes. See
`ADAPTER_CAPABILITIES.md` before adding a new feature field.

The static product registry is the existing `PRODUCT_GAMES` in
`src/contracts/product-catalog.mts`. Do not introduce a parallel registry.

## Static product declarations

| Declaration | Direct consumer / meaning |
| --- | --- |
| title / number / subtitle / card artwork | Launcher catalog and Host artwork publication |
| Runtime URL | default/development product bootstrap; concrete formal URLs come from Site Manifest |
| `storage.saveRoot` / `scoreFile` / `configFiles` | user-file bridge and score/config tools |
| `features.thprac` / `focusHitbox` | static capability ceiling; Runtime/Host may reduce but not enable |
| `features.languages` | publisher/language-pipeline capability ceiling |
| `replay.prefix` | required Replay filename namespace; Replay management is part of the all-game adapter contract |
| `dataProvider` | selects Runtime DATA ingestion contract (`emscripten-preload` or `retail-memory`) |
| `runtimeFileLayout` / `runtimeAssets` / `requiredShared` | closed Runtime Release shape for directory Runtimes |
| `package.dataFileId` / `dataTarget` | Package Store DATA identity and Runtime mount target |
| `package.musicSourceDirectories` / `musicMounts` | content preparation source shape and Runtime music target |
| `package.rawDataImport` | optional original-file acquisition names; validation still uses Host DATA identity |
| `musicCapabilities.midi` | optional MIDI capability; normal OGG/no-music are required adapter behavior |
| `support.sourceRepository` / adaptation notice | support/navigation metadata, not Launcher title checks |
| `multiplayerRuntime` / `multiplayer` | multiplayer availability plus declared player-count subset, difficulty table, loadout table and transport identity; Launcher/Relay never infer these from game number or an older title's shape |

Installation generations, hashes, byte sizes, Runtime cache versions and OGG
collection versions are **generated/deployment facts**. They must not enter the
static product registry.

Site-level branding assets are also not product capabilities. For example, the
current shell favicon is reconstructed from TH06 retail artwork, but
`HOST_SITE_ARTWORK_FILES` owns it globally; adding or selecting a game must not
attach that asset to the TH06 capability surface.

## Concrete capability attestation

There are three distinct levels for **optional/profile capability attestation**:

1. **Product Catalog** - maximum capability the product supports.
2. **Runtime Release** - what the concrete compiled Runtime build attests.
3. **Site Manifest / installed Package** - what this deployment/browser can use.

A lower level may reduce an optional/profile capability but must not exceed the
level above it. Required adapter capabilities are deliberately outside this
Boolean ceiling: a formal adapter/Runtime/Host combination that cannot satisfy
one is incomplete, not a supported product with that feature disabled.

Examples:

- a Host cannot enable thprac for a product whose catalog says `false`;
- a Runtime Release built without language support cannot become language-capable
  because a Host publishes a language pack;
- a product with `musicCapabilities.midi: false` cannot expose MIDI merely
  because the Site Manifest has the required `midi` object shape.

`PRODUCT_FEATURE_POLICY` owns the mapping between these layers. Runtime Release
attests every optional product feature, while Host `features` remains a
deliberately small dynamic Boolean surface. It may currently contain only
`thprac` and `focusHitbox`. Language support instead publishes through the
richer `languages` / `languageOptions` Host surface, so its absence from Host
`features` is intentional rather than an adapter omission. Replay management is
required all-game behavior and is not a generic Host toggle at all.

## Runtime protocol ownership

`src/contracts/runtime-protocol.mts` owns the browser-visible command/event
vocabulary and typed payload surface.

The core lifecycle distinction is contractual:

- `ready` = shell/storage/DATA command bridge is ready;
- `first-frame` = launched game has actually presented a frame.

Do not collapse these signals or use a timer as a substitute for either.

Core commands are exported in `RUNTIME_PROTOCOL_COMMANDS`.
`RUNTIME_PROTOCOL_COMMAND_BEHAVIOR` is authoritative for whether a command is
required, conditional on an implementation path, or profile-required, and
separately whether its reply envelope is required. In particular, an optional
ACK for live input does not make touch/keyboard optional. A Runtime must reject
an inapplicable conditional/profile command rather than silently acknowledge it.

Runtime messages must be same-origin, from the expected parent/iframe, use the
current protocol, identify the selected game, and carry the navigation epoch
assigned to the current Runtime session. The Launcher places that epoch in the
Runtime URL as `runtimeEpoch`; every command/event/response must echo the same
positive integer. A Runtime must reject a command from another epoch, and the
Launcher must reject an inbound event/response whose epoch is not current.
`WindowProxy` identity alone is not a navigation identity because the same
iframe object survives document replacement. Same-origin direct bridges used by
the Runtime (for example managed DATA or an immediate-input fast path) must bind
to the same epoch rather than bypassing the protocol's session boundary.

User-file commands expose only the product's allowlisted save/Replay surface,
never arbitrary Runtime FS access.

### Synchronous keyboard mapping

New adapters expose `window.__eaglerKeyboardInputBridge` with schema
`eagler-touhou/keyboard-input/1`, the host protocol, game, epoch and origin.
Launcher binds a matching `__eaglerKeyboardMapping` to the current Runtime
document. The mapping table and physical/virtual key ownership live in
`src/launcher/keyboard-mapping.mts`; adapters do not copy the alias table.

The native Runtime listener calls the mapper before publishing any raw key.
The callback returns canonical key transitions synchronously, or `null` to
leave an unrelated key on its original path. An empty array consumes an event
without producing another edge. A consumed native event must stop propagation
so SDL or a second listener cannot restore the raw key. `keyboard` and
`keyboard-clear` commands use the same synchronous receiver. Physical commands
carry `inputSource: physical`; existing touch key controls remain canonical
virtual input. A true return means consumed once; a throw must never cause a
retry through another carrier.

Direction keys/WASD, Z/K, X/J, Shift/L/Space and C/H coexist by default. New
aliases take priority over the original use of the physical key, including in
practice menus. HTML text editing and operating-system/browser modifier chords
retain their existing ownership. Multiple physical keys and touch controls
are merged before an UP is emitted. Runtime resets must clear the frontend
mapping and pending touch-key pulses as well as native state; repeats and late
releases cannot re-arm the cancelled owners.

TH09 advertises `th09-local` only during local human-versus-human difficulty,
character/stage selection, gameplay and its match UI. Its two-player profiles
are opt-in and stored independently in `eagler-touhou-th09-local-input-v1`.
The dialog offers per-player keyboard layouts or controller assignment. Native
LEFT/RIGHT movement (including diagonals) and action presets, WASD, arrow keys,
custom capture and named layouts are available. A physical key cannot control
different actions on both players; Pause/Confirm/Skip/Retry can be shared.
The same controller cannot own both seats. WASD uses F/G/left Shift for actions;
arrow keys use Numpad 1/2/3, allowing these two presets to coexist. Retry defaults
to Backspace in these explicit local layouts so R remains a left-zone diagonal.

The synchronous keyboard port optionally implements `sampleLocal(seat)`.
The Runtime bridge returns `-1` outside this context or when local mapping is
off, otherwise one independent 12-bit semantic word per player. TH09 converts
those words at its original input sample boundary, alongside separately latched
controller words; it does not merge physical keyboard halves back into both
players. Unseen short Fire/movement strokes survive one capture; observed holds
release on the next capture without a tail. Profile, epoch and lifecycle changes
clear ownership. Single-player/LAN use the general layout; disabling local
profiles restores original in-game device settings. Touch remains independent,
with the local touch lane targeting the first player. Shared Runtime reset,
Replay and network sampling retain their existing ownership.

TH06/TH07 physical-only practice helpers query the bridge's canonical SDL
scancode state without consuming gameplay pulses.

An older Runtime without this schema keeps its original keys. Deploying only
Launcher does not enable aliases in those older Runtime packages. The bridge
is a backward-compatible input extension, not a replacement of the core
`eagler-touhou/1` protocol. Source integration covers TH06/07/08/09/10/11/15/20.
`tests/test-keyboard-mapping-browser.py --workspace <workspace>` checks the
production Launcher and all eight JS input boundaries in Chromium. Its game
sinks are observed fixtures; complete-game, Replay and real-device acceptance
remain separate evidence.

### Launcher input settings and gamepad mapping

The default-expanded **Input settings** group precedes Advanced settings. Its
two compact entries open a bounded dialog. Its Keyboard and Gamepad tabs share
action names, but use distinct capture flows. Details scroll inside the dialog
rather than expanding the options rail.
The shared editor uses a compact action/binding table for both devices. Keyboard
presets are **Arrow keys** (`arrows`, UI: 上下左右) and **WASD** (`wasd`). The
Arrow keys preset clears aliases and restores the original physical key roles.
WASD enables the shared aliases while retaining the original controls. Editing
creates a custom layout; switching presets preserves the custom draft. Named
layouts are immutable snapshots until explicitly saved with the same name.
Preset selection, the draft and named layouts survive reload in
`eagler-touhou-keyboard-layouts-v2`. Existing v1 preferences migrate without
changing their bindings. The active aliases are also written to the v1 key for
downgrade compatibility; named layout metadata never enters Runtime messages.
This is one Launcher feature shared by all titles, with no title-local presets.
A user alias
wins over the original function of its physical key; other original controls
remain available. Editing aliases clears held ownership before they change.

Adapters expose `__eaglerGamepadInputBridge` with schema
`eagler-touhou/gamepad-input/1` and the same protocol/game/epoch/origin identity.
Launcher binds `__eaglerGamepadMapping`, whose synchronous `sample(seat)` returns
`-1` for native ownership or a 12-bit semantic action snapshot. Bit order is
Shoot, Bomb, Focus, Pause, Up, Down, Left, Right, Skip, Confirm, C action, Retry.
`0` is a valid neutral Launcher snapshot, including disconnect and suspension.
The matching port also exposes `enabled()`; Runtime queries `owned()` to
suppress native KeyConfig capture while Launcher mapping owns the device.
Do not mistake it for absence and fall back to SDL. A Runtime must disable its
native controller contribution while this owner is active, merge the semantic
snapshot with independent keyboard/touch state, and retain its existing logical
input / recording / network sample boundary. TH09 has an independent local
two-player editor; general settings select one controller, and other titles
sample seat 0 without fixed 1P/2P labels. Native local-multiplayer configurations
remain available with the local profile off. TH20 resolves its first eight configured keyboard
actions through current native bindings instead of assuming unchanged Z/X keys.

`eagler-common/browser/browser-gamepad-input.hpp` is the shared adapter helper.
`tools/sync-gamepad-input.mjs [--check]` maintains self-contained copies in the
eight Runtime repositories. JS input modules use the existing keyboard sync
tools. Configuration and UI stay in Launcher, with no copied device policy in
Runtime. Gameplay does not use the editor's 50 ms discovery/capture timer: it
reads a fresh Gamepad snapshot synchronously when Runtime samples input. This
adds no message queue or deliberate frame delay; it is not an end-to-end
physical latency measurement.

Gamepad mapping is opt-in so existing native configurations keep working.
Profiles and the two seat assignments are stored separately from game saves in
`eagler-touhou-gamepad-mapping-v1`. Standard layouts have semantic defaults.
Unrecognized layouts start unbound and learn physical buttons or signed axis
travel, including triggers resting at -1. Raw indices never become cross-device
bindings: profiles are scoped by browser device ID, layout mapping, button/axis
counts and occurrence. A different browser, driver/layout or indistinguishable
identical device may need reassignment or relearning. Arbitrary encoded hat axes
are not guaranteed by signed-axis learning. Devices absent from the browser's
Gamepad API cannot be configured here. Keyboard-like Gamepad IDs remain on the
existing D-pad fallback and are excluded from this physical controller owner.

While a browser session is active, a still-connected device keeps its occurrence
ID when an identical peer disconnects; that event cannot transfer the survivor's
profile or seat. Analog button values obey the selected threshold even when the
browser's own `pressed` flag has already become true. The virtual-controller
matrix and full TH09 Runtime lane are documented in the
[Input and Touch playbook](playbooks/input-touch.md#verification).

Blur, hidden documents, pagehide, device connection changes, mapping edits and
Runtime resets suspend the mapped controller. Neutral input is required before
held controls can re-arm. Learning never sends game input. Disabling Launcher
mapping returns control to the original native settings without rewriting the
game's configuration file. C-key features and Retry semantics remain title-owned.
Older Runtime packages require an update before either mapping editor applies.

The two browser scripts verify real production React/mapping/JS boundaries with
synthetic device snapshots. Eight input translation units also pass Emscripten
syntax compilation, including practice-enabled variants where available. These
checks do not replace full game builds, Replay roundtrips, real-controller
compatibility or device latency acceptance.

## DATA ownership

`lib/runtime-data-provider.mjs` owns provider classes.

- `emscripten-preload` uses the generated Emscripten package layout and verifies
  normal/multiplayer layout equality.
- `retail-memory` uses the product content declaration as the canonical layout
  and supplies DATA through the managed parent bridge.

Adding another title with an existing provider should require declarations and
Runtime implementation, not another provider implementation.

`lib/content-definition.mjs` separately owns original-content shape and
`hostPreparation`. `hostPreparation` is a format/build recipe, not a capability
toggle. A product that declares optional `languages`, for example, may point at
the current `thcrap-runtime-compiler`; a later title may instead declare a
different language preparation kind without changing the meaning of the
`languages` capability. Likewise all titles owe required OGG behavior, while
the Host recipe may be a verified converter or a prepared-content bundle.

## Runtime Release ownership

Runtime Release contains distributable Runtime code/resources only. Original
game DATA, music and artwork do not belong there.

Directory Runtimes remain a closed hash-identified set. The product's
`runtimeAssets` is the minimum required set; `runtime-files.json` identifies the
actual closed set. File names outside the permitted Runtime resource classes are
rejected.

`runtimeFileLayout: "directory"` is a layout contract, not shorthand for TH08 or
TH10. Shared Host/packaging code must branch on layout/provider declarations,
not those game IDs.

## Host and Package ownership

Site Manifest owns concrete deployment state: Runtime URLs, DATA identity,
published music/language resources and server capabilities.

Package Descriptor owns installable files, their Runtime targets, optional
components and Runtime/DATA compatibility requirement.

Release Catalog is only the current Package revision + descriptor pointer. It
must not become another content manifest.

Hosted, External and Import sites must derive from the same product/package
contracts. Import is not permission to invent a second storage format or
disable a Runtime feature that does not require hosted original content.

## Language and music

Language availability is a composition of product capability, Runtime support,
Host publication and installed Package state. Browser-visible choices come from
Host `languageOptions` plus installed language components. Japanese baseline is
represented explicitly; unknown language IDs are not inferred from a game.

Music availability has the same layered model:

- `musicCapabilities.midi` says whether this product adds optional MIDI;
- required OGG/no-music behavior comes from the adapter contract, not a product
  opt-out;
- Site Manifest says what is published;
- Package Store says what is installed locally;
- Launcher resolves the effective mode without title-name checks.

The Host MIDI manifest may set `supported: false` to explicitly attest that the
publication/Runtime does not expose MIDI.

## Raw retail DATA import

Raw retail import is an **acquisition convenience**, not a package type.

If a product declares `package.rawDataImport.fileNames`, the Launcher:

1. matches the selected original filename;
2. validates bytes + SHA-256 against Host DATA identity;
3. creates a normal Package Descriptor using the product's `dataFileId` and
   `dataTarget`;
4. installs it through the canonical Package Store.

Do not add title-specific IndexedDB stores or direct Runtime writes for raw
imports.

## Compatibility boundary

Historical `game-data-pack/1` and `offline-game-pack/1` formats are read-only
compatibility surfaces. ZIP parsing belongs to `legacy/legacy-game-pack.mjs`;
migration/cleanup belongs to `legacy/legacy-import-storage.mjs`. New imports
must cross into Package Store and must not produce additional legacy state.

Legacy compatibility scope is historical. Do not add a new title to every old
cache/localStorage path merely because the current product registry grew.

## Legitimate specialized implementations

Not every game name is a bug. Product-specific code is appropriate when the
underlying original format is genuinely different, for example:

- DAT/archive decoding;
- original Replay binary format handling;
- original score/config format parsing;
- historical migration from a previously shipped adapter;
- game simulation and rendering inside the Runtime itself.

The code/comment must identify the format/compatibility reason. A comment like
"THXX needs this" is not sufficient contract documentation.

## Adapter audit rule

Before reading older adapters as precedent, generate the selected product's
machine contract first:

```text
npm run adapter:inspect -- --game=thXX
```

Use its required definitions, optional/profile activation, protocol field
classification, format-adapter metadata, compatibility list and verification
owners as the starting checklist. An inactive optional profile is not missing
work; an active profile must satisfy its complete behavior list. A legacy or
implementation-detail entry is evidence about existing adapters, not permission
to copy that mechanism into the new Runtime.

Then search at least these owners for the new game ID and for existing game-ID
conditionals:

- `src/launcher/`;
- `src/contracts/`;
- `package/` and `legacy/`;
- `lib/`;
- `scripts/package-runtime-release.mjs`;
- `scripts/package-server.mjs`;
- `scripts/verify-server-build.mjs`;
- App Shell policy/frontend manifest;
- Runtime source repository;
- repository and browser tests.

Classify every hit against the machine contract as one of:

1. **declaration** - desired;
2. **format-specific implementation** - allowed with reason;
3. **compatibility history** - allowed but bounded;
4. **shared-flow game-name branch** - refactor before finishing the adapter.

If a hit does not fit an existing machine class, update the capability/content
model first rather than documenting an ad-hoc exception in the implementation.

Temporary audit findings do not belong in long-lived documentation; stable
contract conclusions do.
