# Legacy import compatibility

This directory is the bounded read-only compatibility owner for two previously
published inputs:

- `eagler-touhou/game-data-pack/1` and
  `eagler-touhou/offline-game-pack/1` ZIPs;
- the `eagler-touhou-game-data-v1` Cache Storage and associated localStorage
  metadata written by older Launchers.

New imports and installations must use the Package Store. Legacy data is read
only to perform a one-way migration; a successful migration deletes the old
records, and no production path may create new records in these formats.

The compatibility owner may be retired only in a release that simultaneously:

1. raises the documented minimum supported upgrade baseline beyond every
   Launcher release that wrote these records;
2. provides a standalone export/recovery path for users who skipped that
   baseline; and
3. removes the startup migration call, the readers/adapters, and their tests in
   one reviewed change.

Until those conditions are met, these modules are compatibility code rather
than examples or a second package implementation.

## Short-revision Package Store upgrade window

Package ZIPs published before full per-file SHA-256 declarations may contain
only a 16-character file revision. When such an installed generation is
updated to a descriptor with the same revision and byte count, the installer
hashes the existing object once, reuses it only on an exact SHA-256 match, and
records that verified identity so another game can reuse identical content.
New descriptors and stored objects always carry full SHA-256 identities; this
path must not become an alternate producer format.

Retire this bridge after the minimum supported upgrade baseline is newer than
the last short-revision offline packages published before 2026-09-12 and one
normal release cycle has elapsed after that baseline. Retirement removes the
missing-hash branch and its regression fixture together; content-addressed
reuse of fully identified objects remains part of the Package Store.

## Current compatibility ownership

| Boundary | Callers and evidence | Removal condition |
| --- | --- | --- |
| Site Manifest schema/file and operator flag | Metadata/directory readers, package writer and verifier. `test-site-manifest-compat.mjs` checks 404-only fallback, strict current errors and alias tampering. | Raise the supported site/client baseline beyond schema-1 Host Manifest consumers and verify an upgrade/export path. Remove the generated URL alias and its tests together. |
| Old `EAGLER_FRONTEND=react` setting | Operator preflight only. It is a no-op, never a UI selection. `main` is rejected. | Operators have migrated their launch scripts. |
| First-use acknowledgement keys | React FirstUseNotice calls the pure adapter in `src/contracts/compat/first-use-notice.mts`. Mounted notice tests cover old nonempty values, empty values and storage failure. | Raise the supported client baseline and remove keys plus regression cases together. |
| Fixed-URL Runtime snapshot caches | App Shell/runtime cache workers use `runtime-generation-reader.js`. Runtime cache cases verify per-byte identity and incomplete snapshots. | The documented old-cache upgrade window ends and existing data has a recovery/export route. |
| Published `legacy/*.mjs` URLs | `path-aliases.mjs` maps them to this directory. The React artifact seals the bundled responses but does not precache these historical entrypoints. Legacy ZIP/migration tests execute those same source owners. | Follow the package/storage baseline conditions above. |

Source code is consolidated here. Historical URL spellings and protocol markers
may remain at their wire boundaries; they are not duplicate implementations or
current UI renderers. New Site Manifest output is schema-checked before any
legacy response is derived from it. Neither adapter modifies saves or Replay.
