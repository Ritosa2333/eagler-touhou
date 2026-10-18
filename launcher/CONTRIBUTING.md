# Contributing to Eagler Touhou

Eagler Touhou spans a browser Launcher, multiple game Runtime repositories,
deployment tooling, and optional network services. Contributions are welcome,
but changes should preserve the ownership boundaries documented in
[`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md) and the supported public surface
in [`docs/PRODUCT_SURFACE.md`](docs/PRODUCT_SURFACE.md).

## Before you start

- Node.js 22 or newer is required for the Launcher repository.
- Python 3 is required by selected build, deployment, and browser-validation
  paths.
- The public repository does **not** contain original Touhou game data, music,
  extracted artwork, user saves, or other game-derived private content. Do not
  add such material to a pull request.
- A standalone clone is expected to pass the repository core gate. The full
  sibling-repository workspace is only required for cross-repository Runtime
  integration work.

Install the locked Node dependencies with:

```bash
npm ci --ignore-scripts
```

Browser lanes are optional and have a separate Python dependency owner:

```bash
python -m pip install -r tests/requirements-browser.txt
python -m playwright install chromium webkit
```

Install Firefox separately when running a lane that explicitly requests it.
The default `npm run check` does not require Playwright or downloaded browsers.

## Normal validation

For changes contained in this repository, the default gate is:

```bash
npm run check
```

For changes that intentionally depend on sibling Runtime repositories, also
run:

```bash
npm run check:workspace
```

Browser, device, live-network, full media conversion, and deployment lanes are
explicit commands rather than hidden parts of the edit-loop gate. Run the lane
that owns the behavior you changed; do not add source-text assertions as a
substitute for browser or Runtime behavior that can be tested at its real
boundary.

See [`tests/README.md`](tests/README.md) for the test-lane policy.

## Source ownership

- Browser Launcher behavior belongs in `src/launcher/`.
- Browser-visible schemas/protocols belong in `src/contracts/`.
- Package installation/storage behavior belongs in `package/`.
- Published-format read compatibility belongs in `legacy/`; do not add new
  legacy writers.
- Reusable Node logic belongs in `lib/`; `scripts/` should remain CLI-oriented.
- Standalone relay/server behavior belongs in `server/`.
- Static Web source belongs in `public/`.

Root contract files are compatibility/public import facades. Keep them thin;
do not grow a second implementation behind a root-level filename.

Generated browser modules under `.cache/build/browser/`, Host output under
`dist/`, and validation evidence under `artifacts/` are not authoritative
source and should not be hand-edited.

## Tests follow ownership

Tests should protect stable behavior, schemas, protocols, safety boundaries,
and artifact ownership. They should not fail merely because an equivalent
implementation was rearranged.

Source-shape assertions are appropriate only when the structure itself is the
contract, for example a protocol boundary, publication rule, ABI, migration
allowlist, or explicit security constraint.

If a legitimate refactor breaks a test that only detects implementation shape,
fix or retire that test instead of restoring accidental structure in product
code.

## Cross-repository Runtime changes

Workspace repository names and build policy are resolved from the canonical
configuration under `config/`; do not duplicate sibling paths in new scripts.
Runtime release production and Host assembly are separate responsibilities:
Host deployment consumes a verified, original-resource-free Runtime Release
and should not silently rebuild game Runtimes from source.

## Pull-request hygiene

- Keep commits focused enough to review by ownership boundary.
- Do not commit `node_modules`, `.cache`, `.npm-cache`, `.deploy-python`,
  `dist`, `artifacts`, screenshots, private originals, or locally extracted
  game assets.
- Run `git diff --check` before submitting.
- Update the owning documentation when a public contract, supported behavior,
  deployment procedure, or compatibility boundary changes.
