# thcrap / thprac Runtime contract

This document owns the bounded Host/Launcher-to-Runtime integration surface for
the optional language and thprac profiles. It does not make the development
compiler service or a particular portable implementation part of the browser
protocol.

The products that currently declare these profiles are TH06/TH07. A future
title may use a different format adapter while preserving the same product-level
capability rules.

## thcrap / language packages

### Formal publication path

The formal product path is **static package preparation**, not a Runtime making
arbitrary thcrap network requests:

1. `lib/content-definition.mjs` declares the title's language preparation kind
   under `hostPreparation.languagePack`.
2. Host tooling resolves the requested thcrap patch sources and compiles the
   supported resources into the Runtime file tree. `.jdiff`/message resources
   become the original game's translated data files or validated localization
   tables; image resources retain their supported image representation.
3. `scripts/prepare-th06-language-pack.mjs` produces a static language catalog
   and archives with byte/hash identity.
4. Site/package assembly publishes only the language packs that were actually
   prepared and selected. `host-manifest.json` exposes the selectable
   `languageOptions`; Package Descriptors may also carry installed language
   components.
5. The Launcher obtains the selected archive from the Package Store or the
   declared static URL, validates the declared identity, installs its files plus
   required fonts into the Runtime filesystem, and only then launches the game.

The Runtime therefore knows only the mounted localization/resource tree (for
the existing adapters, `/thcrap/th06/` or `/thcrap/th07/`) and the selected
language. It does not own repository URLs, CDN/cache topology, thcrap language
IDs, or Package Store policy. When a translated resource is absent, lookup
falls back to the title's original resource.

Japanese/original text and translated text have separate font ownership. The
existing Web adapters keep the original Japanese path on the original-style
Japanese font and use the Unicode/Unifont path for translated text. A successful
patch compile does not prove complete glyph coverage.

### Development live-compiler path

The development server may expose `/api/thcrap/...` through
`server/thcrap-service.mjs`. That endpoint is a development/diagnostic service
for compiling and inspecting live patch sources. It is **not** the formal
publication contract and a production Runtime must not depend on it.

## thprac

thprac is enabled only when both the Product Catalog and the concrete Runtime
Release attest the optional profile.

Before authoritative game launch, the Launcher sends the standard Runtime
`configure` payload with:

- `options.thpracEnabled`;
- `options.thpracLocale` (`zh-CN`, `ja-JP`, or `en-US` for the current
  TH06/TH07 adapters).

The Runtime materializes those prelaunch values into its own practice owner.
Current TH06/TH07 shells expose them to the game through `Module.eaglerOptions`,
but that JavaScript object is an implementation detail rather than another Host
protocol.

Live Practice state belongs to the Runtime. Stage/section/frame/resource/score/
Rank parameters, restart/retry ownership and Replay mirroring must be resolved
inside the title's real Practice lifecycle rather than by a Launcher-side mock.

Both current adapters embed the practice parameters required for playback in the
supported Replay model. An external `*.rpy.thprac.json` sidecar was never a
published format and is not part of the supported Replay data model.

The shared product contract intentionally covers the practice subset that has
actually been ported. Upstream thprac features that are not implemented by the
adapter must not be exposed merely because the thprac profile is enabled.
