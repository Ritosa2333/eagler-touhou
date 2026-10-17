# External resource mode

`external` is the supported paired deployment mode for a small user-facing
site backed by a separate, complete Hosted resource origin.

```text
one verified Hosted generation
  |-- Hosted site   -> complete resource origin
  `-- External site -> Launcher, Runtime and Package metadata
```

The Hosted site owns `games/**` and `shared/**` together with the Launcher and
Runtime. The derived External site keeps the Launcher, App Shell, Package
Descriptors and every selected Runtime HTML/JavaScript/WebAssembly file, but
omits the Package payloads. Its Web server redirects `/games/**` and
`/shared/**` to the Hosted resource origin.

This mode is separate from `externalImportSource` / `gameDataFallback`. Those
settings expose an optional manual download link; they do not select the
automatic External resource mode.

## Generate a paired deployment

A formal release already contains the matching directories:

```text
candidate/hosted-site/    # resource origin
candidate/external-site/  # user-facing site
```

Both come from the same Hosted build. Do not combine directories from
different release candidates.

A self-host operator can derive the same topology from the normal Host output:

```powershell
npm run host:build

npm run package:external-site -- `
  --source=dist/site `
  --runtime-release=runtime-release `
  --output=dist/external-site `
  --profile=web-release-external
```

The default game selection follows `dist/site/deployment.json`. Supplying
`--games=...` deliberately creates a subset instead of a complete pair for
that Hosted generation.
`--test-build=1` propagates test-card visibility only when the Hosted source
already contains a test-only product; it does not add a missing product or
Runtime. Omit it from a normal public deployment. The output path must differ
from the Hosted source. It is disposable generated state: after staging
succeeds, the packager replaces an existing output directory.

The External packager first verifies the Hosted source. It copies Package
Descriptors and their revisions from that source rather than reconstructing
resource identities from another directory.

Current generators publish language archives at stable per-game paths. The
manifest SHA-256 is appended by the Launcher as `v`, separating cache identity
from the operator-facing file name.

### Migrating historical language URLs

When an existing resource origin still serves content-addressed historical
language filenames, migrate them during its next normal asset update rather
than creating a Launcher-side path adapter:

1. upload the same verified archive bytes at the new stable
   `games/thXX/language/lang_*.zip` path;
2. verify the stable URL's bytes and CORS response before publishing Package
   Descriptors that reference it; and
3. retain the old immutable URL only as a bounded server-side fallback for
   already-published descriptors.

New generators, catalogs, and descriptors must never emit the historical
layout. Remove the old resource-origin objects after the minimum supported
Launcher baseline no longer references their descriptor generation and at
least one normal asset-update cycle has elapsed. The fallback therefore has a
retirement condition without becoming a second repository contract.

Do not rebuild a reduced Hosted source merely to give the External site a
different Relay URL or origin-migration setting. Relay and migration are
deployment-owned metadata; Package DATA, music, languages, and descriptors are
content-generation metadata. Mixing an External manifest from one generation
with `/games/**` or `/shared/**` redirects to a different Hosted generation is
unsupported even when the base DATA bytes happen to match.

Project maintainers who must re-derive an External deployment from an existing
published Hosted Package generation should use
`tools/maintainer/recover-external-site.mjs`; see
`tools/maintainer/EXTERNAL_RECOVERY_LANE.md`. That maintainer-only recovery lane
emits `web-validation-*` artifacts and is **not** a formal Release entrypoint;
formal candidates remain owned by [`RELEASE.md`](RELEASE.md) and `npm run
release`. The recovery lane permits explicit
deployment-owned overrides while requiring the derived site to retain the
selected Hosted generation's Package pointers, descriptors, music/language
capabilities, and content identity.

Verify both artifacts before upload:

```powershell
npm run verify:server -- dist/site
npm run verify:server -- dist/external-site
```

## Publish the two sites

The following topology uses placeholder names:

| Role | Public URL | Document root |
| --- | --- | --- |
| Hosted resource origin | `https://asset.example.com/` | `/var/www/eagler-resource/current` |
| External user site | `https://play.example.com/` | `/var/www/eagler-external/current` |

Publish the contents of `hosted-site/` or `dist/site/` directly at the resource
document root. Publish the matching `external-site/` directly at the user-site
document root. Generated sites are flat roots; do not add another application
path around their contents.

The External server redirects only Package payload routes:

```nginx
location ^~ /games/ {
    return 307 https://asset.example.com$request_uri;
}

location ^~ /shared/ {
    return 307 https://asset.example.com$request_uri;
}
```

These are external redirects. The user-site server sends the small redirect
response and the browser downloads the resource directly from
`asset.example.com`. Using `proxy_pass` instead would make the user-site server
carry the resource bandwidth.

The resource origin must preserve every path declared by the Package
Descriptors and allow the External site's HTTPS Origin through CORS:

```nginx
add_header Access-Control-Allow-Origin "https://play.example.com" always;
add_header Access-Control-Expose-Headers "Accept-Ranges, Content-Length, Content-Range, ETag" always;
add_header Vary "Origin" always;
```

nginx serves static byte ranges by default. A CDN or reverse proxy in front of
the resource origin must not discard the request's `Range` header or the
response's `Accept-Ranges`, `Content-Length` and `Content-Range` headers. A
satisfiable single-range request must return `206 Partial Content` with an
exact `Content-Range`.

The CORS headers must appear on the final resource response, including `206`
and relevant error responses. nginx stops inheriting parent `add_header`
directives when a child location declares any `add_header`; include the CORS
policy in every location that owns cache or content headers.

[`examples/deployment/nginx-external-pair.conf`](https://github.com/YomotsuHisami/eagler-touhou/blob/main/examples/deployment/nginx-external-pair.conf)
shows both server roles together. It is an adaptable example, not an nginx,
certificate or firewall installer.

## Cache and Service Worker boundary

HTML, JSON and unversioned entrypoints must revalidate. Content-versioned
resources may use long-lived immutable caching. A CDN cache key must retain
resource query parameters, including `v`.

The External App Shell Service Worker owns the Launcher and local Runtime
shell. It does not own `games/**` or `shared/**` Package payloads. A Service
Worker registered on the Hosted resource Origin cannot control the External
site's Origin.

## Cutover and rollback

Upload each site to a new versioned directory. Verify it before atomically
switching the document-root symlink or equivalent hosting revision. Do not copy
a new generation over the active directory file by file.

Make the matching Hosted resource generation reachable before switching the
External site. Roll back both sites to their matching generations when Package
metadata or payload identity changed. Switching only one side can leave valid
metadata pointing at different bytes.

## Public verification

Verify both public origins after publication:

```powershell
npm run verify:deployed -- https://asset.example.com/
npm run verify:deployed -- https://play.example.com/
```

Hosted verification reads the complete declared deployment inventory.
External verification checks every Package file's redirect, final CORS
response, declared `Content-Length` when present, and one-byte Range response.

For manual diagnosis, choose a real payload path from a published Package
Descriptor and inspect the same boundaries:

```text
curl -I https://play.example.com/games/th08/th08.data
curl -I -H "Origin: https://play.example.com" https://asset.example.com/games/th08/th08.data
curl -D - -o /dev/null -H "Origin: https://play.example.com" -H "Range: bytes=0-0" https://asset.example.com/games/th08/th08.data
```

The first response must be a 307 redirect to the same path and query on the
resource origin. The second must expose the configured CORS policy. The third
must return `206`, one byte of content and a matching `Content-Range`.

Finally, use a supported browser at the External URL to install or update one
Package and launch its Runtime. Command-line checks do not enforce browser CORS
or prove IndexedDB installation and Runtime startup.

## Resource-mode ownership

| Mode | Launcher and Runtime | Game/shared payloads | Acquisition |
| --- | --- | --- | --- |
| `hosted` | published | published locally | automatic Package install |
| `external` | published | redirected to matching Hosted origin | automatic Package install |
| `import` | published | omitted | user-selected local Package |

A transient network failure does not change the configured resource mode,
switch the current Package generation or persist a preference to use Import.
Only a completed install or import transaction can make its generation
current. An interrupted pending generation and its partial objects are
internal recovery state; normal failure handling cancels them, while cleanup
after an abrupt browser or process termination is a separate storage-lifecycle
boundary.
