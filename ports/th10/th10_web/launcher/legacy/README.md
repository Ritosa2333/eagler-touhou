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
