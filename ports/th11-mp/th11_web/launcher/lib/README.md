# `lib/` ownership

`lib/` contains reusable **Node-side** engineering logic. It is not a generic
browser utility directory and it is not a home for CLI entrypoints.

Typical owners here include:

- Launcher/App Shell build and manifest assembly;
- self-host bundle, Runtime Release, release-plan, provenance, and artifact policy;
- workspace/build-profile resolution;
- publication and public-tree policy;
- reusable verifiers used by more than one command/test;
- Node adapters for the typed browser-visible contracts under `lib/contracts/`.

Command-line argument parsing, console presentation, and one-shot maintainer
entrypoints belong in `scripts/`. Browser product behavior belongs in `src/`
or `package/`. Tests belong in `tests/`.

`lib/contracts/` deliberately exposes Node-consumable contract modules backed
by the typed owners in `src/contracts/`. The root-level contract files are thin
stable re-export facades over these modules.

Do not move code here merely because it is shared by two nearby functions. A
module should represent reusable policy, assembly, validation, or another
cohesive owner.
