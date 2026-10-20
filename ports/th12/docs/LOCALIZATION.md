# TH12 local Chinese resource adaptation

This topic lives on `experiment/localization`, based on `b40dfb5`, in
`D:/Programming/TouhouWeb/worktrees/th12-localization`. It follows the platform's
Localization/THCRAP and Adaptation Worktree Isolation playbooks. The canonical
Eagler integration checkout remains the promotion owner. This implements a
specific local TH12c resource source; it is not general THCRAP patch execution.

## Source identity and static findings

`localization/th12c-1.00b.json` versions all four source SHA-256 identities,
the resource difference set, the source encoding, and exceptional ending text
mapping. No archive, executable, extracted retail payload, or converted image
is tracked in Git.

The supplied translation is **Traditional Chinese encoded in CP936/GBK**;
the language ID is `lang_zh-hant`. Japanese is decoded as CP932. Both archives
have the same unique resource names; the Japanese archive contains a duplicate
`se_cardget.wav` listing. Static byte comparison found 84 changed resources:

| Source | Changed resources | Compiled use |
| --- | ---: | --- |
| Stage dialogue MSG | 42 | Text commands for all seven stages and six loadouts |
| Ending MSG | 12 | Text for each original ending command |
| ECL | 8 | Spell names only; all 113 spell IDs |
| `musiccmt.txt` | 1 | All 17 Music Room titles/comments |
| ANM | 21 | 47 changed THTX texture entries |

All 21 changed ANM files have identical non-pixel bytes in this source pair.
The compiler checks headers, sprites, programs, and bytes outside the THTX
pixel ranges before accepting a texture. These are complete replacement THTX
images, including their transparent pixels; the Runtime does not alpha-compose
the translated image over a Japanese image. Texture changes cover title/menu,
stage labels, boss names, selected ending lettering and staff images.

The Chinese PE is read only to extract the eight Music Room warning pointers
at VA `0x4b33c0`. Its program, imports, native font selection and other native
UI strings are not applied. Both EXEs remain static input files.

### Text layout and timing

TH12c changes dialogue line counts. Mapping one Japanese line to one Chinese
line would lose text or leave Japanese lines visible. The compiler instead
pairs the two files' **non-text command sequence**. Each anchor carries only
the translated text commands (15, 16, 17), their UTF-8 plaintext, and CP936 byte
widths for the original presentation layout. The Runtime verifies the complete
Japanese MSG SHA-256, reconstructs its entry offsets, replaces only text
commands, and copies every non-text command's time/opcode/payload from that
verified Japanese file. Extra translated lines use their original text group's
presentation time. Neither MSG nor ECL programs are delivered in the ZIP.

The existing core remains responsible for text slot alternation, clearing,
animation handles, wait controls and dialogue progression. UTF-8 bytes written
by the text-only reconstruction are recognized by the installed overlay's
lookup table; missing resources continue to use the Japanese CP932 path.
Formatted text uses translated per-character CP936 byte widths rather than
splitting Unicode text into arbitrary two-byte chunks.

Two ending differences are explicit source rules:

- `e07.msg`: the final translated text is at time42 rather than original41.
  Its text is bound to the original41 command.
- `e10.msg`: the Chinese time3 line merges original times3/4, and its wait4 is
  removed. The overlay blanks only original text4 and retains original wait4.

The adaptation therefore preserves Japanese game control timing. It does not
claim identical presentation timing to the native Chinese executable.

## Build

Requires Python 3.10+, Pillow and a local legally obtained thtk `thdat`.
There are no private workspace Python imports, emulator dependencies, or
runtime translation server dependencies. Install Pillow with the selected
Python environment's package manager.

```text
python scripts/prepare-localization.py --original <installation-directory> --thdat <path-to-thdat> --output <content-root>
```

The input directory contains `th12.dat`, `th12.exe`, `th12c.dat`, `th12c.exe`.
`--original` defaults to `TH12_ORIGINAL`; otherwise it is required. `--thdat`
defaults to `TH12_THDAT`, then `THDAT`, then `thdat` on PATH. `--output` defaults
to this repository's ignored `local/content`. The optional `--source-manifest`
defaults to `localization/th12c-1.00b.json`. Source identities must match before
the tool extracts either archive. Output must be separate from the installation.

The compiler calls only `thdat -l12` and `thdat -x12`, into a temporary directory
that is removed afterward. It does not call the original game, a game VM, an
Oracle, or an original-content preparation pipeline. Missing input, only one
Chinese file being supplied, or a different source revision is a build error.
A Host wrapper may skip the optional language preparation when neither Chinese
file exists and that language has not been requested. It must fail clearly
when a selected language cannot be built.

Publication layout under `<content-root>`:

```text
language/catalog.json
language/lang_zh-hant.zip
language/lang_zh-hant.manifest.json
language/lang_zh-hant.coverage.json
manifest.json
files/thcrap/th12/overlay.json
files/thcrap/th12/textures/<bank>-<entry>.png
```

The ZIP contains `manifest.json` and entries whose names omit the leading `/`:
`thcrap/th12/overlay.json` and `thcrap/th12/textures/*.png`. It uses the platform
`eagler-touhou/thcrap-static-pack/1` schema, `game: th12`,
`language: lang_zh-hant`, `runtimeVersion: independent`, no dependencies,
`sourceRevision` equal to the Chinese DAT SHA-256, and sorted file declarations
with path, sourcePath, format, bytes and SHA-256. ZIP timestamps are fixed to
1980-01-01. Only translated text/layout JSON and changed textures are packaged.

The catalog has schema `th12-localization-catalog/1`, `game: th12`,
`defaultLanguage: ja`, and `languages: [{id,title,pack:{url,bytes,sha256,
runtimeVersion,files}}]`. Its pack URL is `language/lang_zh-hant.zip`, relative
to the content root, and its hash describes the complete ZIP. Host owns copying
this catalog entry into its Site Manifest/Package Descriptor and including the
compiler plus source manifest in a self-hosting build recipe.

## Runtime API

```js
import {prepareLocalization} from './localization/loader.mjs';

// Launcher has installed/validated the selected static language pack, and
// the Runtime font owner has already loaded the licensed Unicode font.
const language = await prepareLocalization({
  language: 'lang_zh-hant',
  manifest: installedPack.manifest,
  readFile: path => installedBytes.get(path),
  fontFamily: 'Eagler Unicode',
  fontReady: installedFontPromise,
});
const menu = language.applyMenuResources(originalMenu);
language.activate();
// Now initialize/launch Runtime and Renderer using the prepared assets.
```

`manifest` is the **static ZIP manifest**, not `overlay.json`. The loader reads
`/thcrap/th12/overlay.json` itself, checks its source identity, checks every
installed file's size/SHA-256, checks PNG dimensions, and enforces the exact
47-name texture whitelist. It accepts only this title's JSON data and PNG
resources. It does not fetch network URLs or import scripts, Wasm, native code,
raw MSG/ECL programs or fonts. Font licensing, installation, and readiness stay
with the Launcher/Runtime font owner; `fontReady`, if supplied, is awaited.

Provider methods:

- `fetchOverride('/assets/<texture>.png')`: `Response` or `null`; call before
  the ordinary asset fetch. Replaces only a declared PNG.
- `await patchMessage('/assets/st01_00a.msg', originalBytes)`:
  `Uint8Array` or `null`. Call after reading the base asset and before loading
  a stage MSG into core. An unexpected source SHA or control anchor is an error.
  Path and resource allowlists restrict this to the 42 supported dialogues.
- `applyMenuResources(originalMenu)`: a copy with translated ending command
  text, spell-name cache, music text and warning; original programs stay intact.
  Apply once to the Japanese base resource, before title/ending initialization.
- `setContext({stage,character,shot})`: stage1..7, character0..2, shot0..1;
  call before loading each stage or replay. It selects the exact dialogue route.
  An invalid/empty context clears the route. Without a route, only an
  unambiguous translated UTF-8 string is returned.
- `activate()` / `deactivate()`: publish/remove the prepared presentation
  provider. Always activate the Japanese provider as well when switching back.
- `textForHex(kind, hex, {entry,cardId,resource})`: a translation/layout record
  or `null`. `kind` is `dialogue` or `spell`. Presentation modules use the
  exported `localizedHex` / `localizedFontFamily` hooks.

Japanese (`ja`/`lang_ja`) returns an inert provider without requiring a pack.
Missing translated text and unspecified textures use Japanese resources.
A selected pack that is corrupt or incomplete fails preparation before launch.
For spell-history menus, integration must prefer a cache entry marked
`localized` over a Japanese title restored from an original score file.

## Evidence and remaining work

The local build to `artifacts/localization` completed and produced 48 resources:
47 textures, 2,276 dialogue lookup records, 354 ending text replacements, 113
spell names, 17 Music Room records and eight warning lines. The compiler writes
its actual coverage and ZIP identity beside the package; use those generated
values when publishing rather than copying a historical hash from this document.

No syntax check, test suite, browser, game, Oracle, replay playback, installation
gate or font/rendering comparison was run. Source adaptation and local resource
construction do not prove Launcher integration, complete translation of native
EXE strings, glyph/pixel parity, dialogue animation timing, or full THCRAP
support. Those gates remain unverified under the user's no-test instruction.
