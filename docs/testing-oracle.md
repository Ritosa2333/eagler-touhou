# Native oracle verification

The th09, th11, and th15 normal and multiplayer suites compare the current WASI core with the selected original executable and its accepted resource inputs. Each discovered `tests/cpp/*.test.mjs` file belongs to exactly one candidate group or one explicit maintenance task in that title's `oracle-suite.json`.

The frozen runner verifies the current core build receipt and native build inventory before running tests. Its scoped native snapshot recursively covers compiled game sources in `cpp/game/`, candidate tests and their helpers in `tests/cpp/`, `scripts/cpp/`, `scripts/native/`, common headers and replay-verifier testkit, the title replay-verifier tools, the native runner controls, installed Unicorn and Koffi package contents, target/package metadata, and the built core plus receipt. The C++ test include closure also records `cpp/sdl/ThcrapLayout.hpp`, `cpp/sdl/PresentationCamera.hpp`, and `cpp/sdl/PresentationUv.hpp`. th15 normal and multiplayer runs also bind the installed `fflate` package because their dialogue oracle imports it. Installed package junctions are recorded by resolved target and full contents. Changes outside this execution closure, such as Launcher, browser-only, or documentation files, do not interrupt a native comparison. The current Git `HEAD` is recorded as run context and does not decide the result.

Both th15 web package manifests pin `fflate` 0.8.3, matching the version locked by Launcher. These title packages do not maintain separate lockfiles; exact manifest versions define their direct test dependencies. Local native test installs can expose `fflate` through an owned `node_modules/fflate` junction without replacing the other native dependencies.

Each suite declares the `full-native/v1` environment profile. The runner removes inherited test selectors before applying its fixed defaults, the group's explicit full-set selection, and receipt-bound paths; every group result records its effective scope. th09 replay iteration has a 32-bit frame ceiling, th11 world comparisons have no frame limit or resume checkpoint and select all declared demos, and campaign soak selects all declared pairs. The report therefore distinguishes a complete input range from a checkpoint tail. th15 records its explicit stage-flow range and its separate 3,600-frame gameplay sample.

It checks each group's declared original executable, accepted asset tree, replay, trace, or toolchain by SHA-256 before and after execution, including inputs captured for groups blocked by another missing input. It also hashes the complete accepted `reference/` tree before and after a run, including Git-ignored files; declared reference junctions record both their resolved target and full contents. Additions, removals, target changes, and byte changes set the independent `baselineChanged` result and fail the run. Scoped native input additions, removals, target changes, and byte changes set `nativeInputsChanged` and fail the run. A candidate result passes when every selected group ran at least one test and completed without failures, skips, cancellations, or todo cases. Missing private inputs appear as a blocked group; ready groups still run and report their own evidence.

Run a read-only input check from a title's `web` directory:

```powershell
node ../../../tools/testing/oracle-cli.mjs --root=. --mode=preflight
```

The local ignored receipt named by `EAGLER_ORACLE_INPUT_RECEIPT` supplies private path and SHA-256 pairs. Its document uses schema `eagler-touhou/oracle-private-inputs/1`, names the owning title, and stores input entries as `{ "kind": "file" | "directory", "path": "...", "sha256": "..." }`. An optional `envPath` must resolve to the same location as `path`, so the test process receives the exact file or directory whose bytes were verified. The receipt is local configuration and does not enter the source tree.

Directory hashes cover the complete regular-file inventory. Files are sorted by `/`-separated relative path, then encoded as `path`, NUL, decimal byte length, NUL, lowercase file SHA-256, newline; SHA-256 is computed over the concatenated records. Symlinks and non-file entries are rejected. This makes both added files and byte changes invalidate the receipt.

th11 normal and MP verify DAT scene traversal separately from the original callback order. The receipt input `accepted-scene-schedule` binds an independently accepted JSON file, with schema `eagler-touhou/th11-scene-schedule-reference/1`, `game: "th11"`, the original `executableSha256`, and `registrations` containing `[priority, kind, layer]` rows. Both variants compare against the same original executable identity. The `candidate-glyph-cache-fonts` directory receipt binds `font0.bin`, `font1.bin`, `font2.bin`, `font3.bin`, `cp932.bin`, and `blend4444.bin` as candidate inputs for the separate 160-sample native font comparison. Expected pixels come from the original executable and live Windows GDI. The `gdi-host-library` and `gdi-source-font` receipts bind the GDI host library and the available source font. Receipts supply explicit private paths and hashes, so the two variants can share these inputs outside their source directories. Each private-input group reports missing inputs during preflight.

th15 normal and MP score tests generate their saved-state and default-record inputs in memory from the SHA-256-verified original executable. They capture the original save writes, reopen them in a fresh native manager, verify the complete state round trip, then pass copied buffers to the candidate tests. No host save files are read or written. Font definitions and assets remain explicit receipt-bound inputs through `TH15_ORACLE_FONT_DEFINITIONS` and `TH15_ORACLE_FONT_ASSETS`.

Run candidate comparisons with a unique evidence name and a bounded number of Node workers:

```powershell
$env:EAGLER_ORACLE_JOBS = '2'
node ../../../tools/testing/oracle-cli.mjs --root=. --name=manual-20261019
```

Each candidate group runs serially and records its input identity, duration, counts, and log beneath `artifacts/cpp/verification/<name>/`. A same-name result is replaced with a `running` record before input validation; errors write a `failed` record, and completion atomically publishes the final result. The title-level result is nonzero unless every candidate group is complete.

Byte comparisons retain an immutable copy of both buffers at the first mismatch. The report records the first differing offset, byte values, lengths, and adjacent bytes before native fixture cleanup can reuse WASM memory.

Replay comparisons assert their declared end boundary. th09 covers the full recorded input range or an original-confirmed stage transition. th11 demos cover every recorded gameplay frame before the separate terminal marker; campaign comparisons require the final stage and terminal lifecycle. A shortened loop cannot produce a passing full-range comparison.

Maintenance tasks are separate Node test entries. They require an explicit task and evidence name, a positive test count with no skipped or cancelled cases, and output beneath the ignored `artifacts/validation/oracle-maintenance/<task>/<name>/` staging directory. Symlinked staging paths are rejected. For example, `--mode=maintenance --task=archive-collect --name=review-20261019` stages a th11 archive extraction for review without changing accepted resource files. The th15 native manual-image capture uses the same maintenance path. Maintenance output does not become a candidate baseline by itself.
