# Browser package subsystem

This directory owns Eagler Touhou's **browser-local game package** model. It is
unrelated to npm's root `package.json` despite the short directory name.

The subsystem owns:

- the `eagler-touhou/package/1` descriptor contract;
- generation planning and current/pending installation state;
- IndexedDB Package Store persistence;
- remote/local installation orchestration;
- offline ZIP parsing/import.

New package writes and imports use the canonical Package Store model. Historical
formats that still need read/migration compatibility belong in `legacy/` and
must not grow new producers.

The adjacent `.d.mts` files are narrow TypeScript declaration surfaces for the
retained JavaScript implementation modules. Eliminating declaration bridges is
not an architectural goal by itself.

Launcher-specific presentation and orchestration belong in `src/launcher/`;
package storage/install policy should remain here so it can be tested without
the Launcher UI.
