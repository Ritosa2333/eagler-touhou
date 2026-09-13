# Self-hosting

Eagler Touhou can generate a complete, verified static site from a resource-free
Runtime Release and legally-owned TH06/TH07/TH08/TH10 installations. The normal
self-host workflow is intentionally infrastructure-neutral: it does not install
or configure nginx, Caddy, Apache, IIS, systemd, TLS, a CDN, firewall rules, or
any other operator infrastructure.

## Requirements

- Node.js 22 or newer;
- Python 3;
- a verified all-product `runtime-release/` supplied by the project;
- legally-owned TH06, TH07, TH08 and TH10 installations;
- FFmpeg on PATH for the bundled TH10 OGG preparer, or a supplied
  `games/th10/assets-ogg/` containing `th10.data` and `bgm-ogg/`.

The self-host bundle includes the TH10 content preparer and its Wasm/module
dependencies. It extracts DATA/OGG from `games/th10`; the existing thtk artwork
step also reconstructs `th10-card.webp` from the original title background.

PowerShell, CMake, Ninja, Emscripten and the Runtime source repositories are
not required for ordinary self-hosting.

## Input layout

Use this layout in either a source checkout or a distributable self-host bundle:

```text
EaglerTouhou-Host/
├─ package.json
├─ package-lock.json
├─ eagler-touhou.config.json    # optional persistent operator config
├─ runtime-release/             # resource-free Runtime HTML/JS/WASM + metadata
├─ shared/                      # bundled fonts when present
│  ├─ unifont.otf
│  └─ japanese-font.otf
├─ games/
│  ├─ th06/                     # copy the TH06 installation here
│  ├─ th07/                     # copy the TH07 installation here
│  ├─ th08/                     # copy the TH08 installation here
│  └─ th10/                     # copy the TH10 installation here
├─ .cache/                      # reusable local build state
└─ dist/                        # generated output; do not edit
```

The persistent operator-owned inputs are `games/` and
`eagler-touhou.config.json`. `runtime-release/` is a project release artifact.
`dist/` is disposable output and must be regenerated rather than edited.

## Generate and run the site

To build and verify the static hosted site:

```text
npm run host:build
```

The result is:

```text
dist/site/
```

To build the same site and serve it locally on `127.0.0.1:8130`:

```text
npm run host
```

The local server is for development, LAN use and checking the generated site.
For a public deployment, copy `dist/site/` to the Web server, object store, CDN,
container image or hosting platform of your choice.

To keep the large Package resources on a separate HTTPS origin, derive an
External user site from the verified Hosted output:

```powershell
npm run package:external-site -- `
  --source=dist/site `
  --runtime-release=runtime-release `
  --output=dist/external-site `
  --profile=web-release-external
```

Publish `dist/site/` as the complete resource origin and
`dist/external-site/` as the matching user-facing site. The user-facing server
redirects `/games/` and `/shared/` to the resource origin; the resource origin
provides CORS and byte-range responses. Follow
[External resource mode](EXTERNAL_RESOURCE_MODE.md) for the complete paired
deployment and public verification procedure.

To inspect inputs without generating the site:

```text
npm run host:doctor
```

## Dependencies and caches

The npm commands are the public entrypoints on Windows, Linux and macOS.
If their locked Node dependencies are absent, the Host tooling runs `npm ci`
automatically. It never downloads original Touhou game content.

Python build dependencies are installed into:

```text
.cache/python/
```

Expensive reproducible outputs such as language packs, converted OGG and
prepared artwork live under `.cache/generated/` and may be reused across site
rebuilds. Deleting `.cache/` is always allowed when a cold rebuild is wanted.

On Windows, the Host tooling downloads the pinned official thtk 12 archive,
verifies its SHA-256, and stores it under `.cache/tools/thtk/12/`. On other
platforms, `thdat` and `thmsg` must be installed by the operator and available
on `PATH`; `thanm` is optional.

## Default content preparation

The normal hosted build:

- validates the Runtime Release and the three original game directories;
- prepares Japanese, Simplified Chinese and English for TH06/TH07;
- prepares the Launcher-facing artwork needed by the selected products;
- converts OGG using the project-owned production baselines when OGG is
  enabled;
- assembles Package DATA and optional package components;
- builds the deployment-specific App Shell;
- verifies the finished static site before reporting success.

The default music selection is MIDI plus OGG. A different set can be supplied
to the Node entrypoint, for example:

```text
node host/build.mjs --build-only --music=midi
```

## Generate Import artifacts

To generate an import site and verified game-content ZIPs:

```text
npm run import
```

This uses the same original inputs and Runtime Release as the hosted build and
produces:

```text
dist/import-site/
dist/import/th06.zip
dist/import/th07.zip
dist/import/th08.zip
```

The Import site contains the Launcher and App-managed Runtime but does not
publish the original game payload. The ZIP set is staged and then published as
one directory so a failed build does not intentionally expose a partial set.

## Deploy the generated site

Eagler Touhou owns the generated site's behavior, not the operator's operating
system. A public host must provide the required MIME types, cache behavior,
HTTPS/security context, and any configured WebSocket endpoint. It does not have
to use a particular Web server or Linux distribution.

After publishing, validate the real public URL:

```text
npm run verify:deployed -- https://example.com/
```

See [Self-hosting reference](SELF_HOSTING_REFERENCE.md) for Host configuration,
Import mode, Relay/TURN integration, migration/HSTS behavior and Web-server
requirements. Non-authoritative server examples live under
`examples/deployment/`.

## Self-host bundle

Project maintainers may publish a **self-host bundle** containing the portable
Host tooling, compiled Launcher files, Runtime Release and redistributable
shared resources while leaving all three `games/` directories empty. It is a
distribution format for the workflow above, not a second hosting system.

The bundle itself contains no maintainer PowerShell entrypoints and ordinary
recipients still use only the npm commands documented here. Creating the bundle
is a project-maintainer release task described in [Release engineering](RELEASE.md).
