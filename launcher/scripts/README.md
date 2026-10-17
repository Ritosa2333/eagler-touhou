# Maintainer commands

`scripts/` contains portable product build, packaging, verification, and CI
entrypoints. Reusable policy and
assembly logic should live in `lib/`; automated tests live in `tests/`.
Machine-, account-, or maintainer-infrastructure-specific utilities belong in
`tools/maintainer/`, not in this portable command surface.

The directory is intentionally flat by default. A subdirectory is introduced
only when it represents a real product/build subsystem rather than merely a
file category.

## Command families

- `build-*`, `prepare-*`, `subset-*`, `convert_*` - build/input preparation;
- `package-*`, `release.mjs` - artifact producers;
- `verify-*`, `audit-*`, `check.mjs` - verification and publication gates;
- `serve*.mjs` - local development/static serving;
- `resolve-*`, `inspect-*`, `list-*`, `write-*` - thin CLI adapters;

Live/public maintainer probes are deliberately documented separately in
[`tools/maintainer/`](../tools/maintainer/README.md).

Prefer the stable npm entrypoints documented in the root README and
`docs/README.md` for normal workflows. Tests already scheduled by
`tests/test-plan.mjs` do not need one npm alias per file; focused maintainers can
run the owning test file directly.

If a script starts exporting reusable APIs or accumulating business policy,
move that reusable owner to `lib/` and keep the script as a thin CLI wrapper.
