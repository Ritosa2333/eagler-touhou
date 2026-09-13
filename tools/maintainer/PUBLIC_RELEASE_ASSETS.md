# Public project Release assets

The formal Eagler Touhou Release bundle is not itself a public GitHub Release
payload. It can contain Hosted output, game Packages and offline ZIPs derived
from legally-owned originals. Those artifacts remain private/operator-owned.

The public project Release should instead publish two resource-free assets
derived from the same verified formal candidate:

1. `EaglerTouhou-SelfHost.zip` - the recommended download for
   ordinary self-host operators. It contains the portable Host tooling,
   compiled Launcher, redistributable shared resources, the verified
   `runtime-release/`, and empty `games/th06`, `games/th07`, `games/th08`, and `games/th10`
   directories.
2. `EaglerTouhou-Runtime.zip` - the standalone Runtime Release for
   advanced operators and integration/update work. It contains a canonical
   top-level `runtime-release/` directory and no original game resources.

Generate both, plus `SHA256SUMS.txt`, with:

```powershell
pwsh -NoProfile -ExecutionPolicy Bypass `
  -File tools/maintainer/build-github-release-assets.ps1 `
  -Candidate D:\Releases\candidate `
  -OutputDirectory D:\ReleaseAssets\0.1.0
```

The script first verifies the complete formal candidate, then verifies and
extracts its `runtime-release/`. It delegates the Self-host asset to
`build-self-host-bundle.ps1` and emits `release-assets.json` with the source
Release identity. The Runtime ZIP is only a transport envelope around that
same verified Runtime Release; it does not rebuild Runtime files.

Never publish any of the following as public project assets:

- a formal release candidate directory as a whole;
- `hosted-site/` when it contains generated original-game resources;
- `game-package/`;
- `offline-zip/` game-content packages;
- legally-owned original game directories;
- user saves, Replays, credentials, private server configuration, or logs.

## Staging before production promotion

Static deployment verification is necessary but does not prove a real browser
can complete first installation. After an explicitly authorized staging
deployment, run both mature browser/package paths from fresh Chromium sessions:

```powershell
node tools/maintainer/verify-public-first-install.mjs https://staging.example.invalid/ --game=th06
node tools/maintainer/verify-public-first-install.mjs https://staging.example.invalid/ --game=th07
```

The probe requires the remote Package installation to commit a current
generation in IndexedDB and then requires the Runtime to emit a real
`first-frame`. A green static `verify:deployed` result does not replace this
browser-level gate.

These maintainers tools create and verify local/public-staging artifacts only.
They do not upload to GitHub and do not authorize a production cutover.
