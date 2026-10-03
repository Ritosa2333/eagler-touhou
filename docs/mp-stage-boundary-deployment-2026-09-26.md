# TH08MP Stage Clear fix: production deployment

The user explicitly requested publishing the previously verified TH08 fix to
`https://touhou.vip/` before completing the current smoothness experiments.

- Runtime generation: `eef24cb58085ba70796f9ea4aa5801ec198d6aca59763ce3a416ecd14a006090`.
- WASM SHA-256: `30c5e0ecafc920972958348dc70c900b7b7db5dc15a6f5eefec8bf1283e2e930`.
- Includes the Stage Clear immutable-capture reuse fix and confirmed-prefix
  checkpoint elision documented in `mp-stage-boundary-frontier-2026-09-26.md`.
- Excludes the later early-input-send and live Bullet partitioned-journal
  experiments. The published artifact was not rebuilt from the current dirty
  worktree. It is the earlier production build with its retained successful
  600-frame delayed-input regression, exact build manifest and no fixture exports.
- Launcher source/UI, TH06/07/10 runtimes, ordinary TH08 and product visibility
  remain byte-identical to the live baseline. TH08MP stays publicly enabled.

The candidate was derived from the current production site's full file
inventory, verified locally, then applied with a whole-directory atomic exchange.
All 300 baseline files were checked before cutover; the 23-file patch produces
316 files. Existing immutable generations are retained. The server rechecks the
baseline immediately before switching to avoid overwriting a concurrent release.

Rollback directory: `/var/www/eagler-touhou/rollback-20260926-194226`.

Public verification downloaded and hashed all changed files plus Launcher and
TH06/07/10MP controls: 59 files PASS, including WASM/JS MIME, immutable Runtime
cache headers and a genuine unknown-route 404. This does not claim remote-phone
gameplay acceptance or completion of the separate dense-scene optimization.

Local evidence: `artifacts/main-stage-clear-release-20260926/` contains the
baseline, frozen Runtime input, build manifest, production regression, complete
candidate, scoped patch, atomic deployment result and public verification.
