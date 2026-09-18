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
| Runtime URL | default/development product bootstrap; concrete formal URLs come from Host Manifest |
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
3. **Host Manifest / installed Package** - what this deployment/browser can use.

A lower level may reduce an optional/profile capability but must not exceed the
level above it. Required adapter capabilities are deliberately outside this
Boolean ceiling: a formal adapter/Runtime/Host combination that cannot satisfy
one is incomplete, not a supported product with that feature disabled.

Examples:

- a Host cannot enable thprac for a product whose catalog says `false`;
- a Runtime Release built without language support cannot become language-capable
  because a Host publishes a language pack;
- a product with `musicCapabilities.midi: false` cannot expose MIDI merely
  because the Host Manifest has the required `midi` object shape.

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

Host Manifest owns concrete deployment state: Runtime URLs, DATA identity,
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
- Host Manifest says what is published;
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
