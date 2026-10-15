# TH075 thcrap host preparation

TH075 follows the TH06–08 static language chain: resolve the selected patch
stack, validate downloaded CRCs, compile supported resources, publish the
catalog/ZIP identity through Host and Package Descriptor, then install before
Runtime launch. The development HTTP thcrap service is not the publication path.

The format adapter lives in `server/th075-thcrap-compiler.mjs`. It accepts
Japanese `th075.dat`/`th075b.dat` or the verified prepared `th075.data`. It
follows native Base/Patched ownership for every sprite, portrait and menu bank.
The output contains replacement pixels/text and an audited GG75 font; it does
not embed unmodified retail banks or scripts. Source executable patches and
DLL hooks are not executed. thpatch `scenario.txt.jdiff` is an ordered ending
transcript and must not be applied to ordinary Story dialogue positions.

```powershell
node scripts/prepare-th06-language-pack.mjs --game th075 `
  --archive D:/private/th075.data --language lang_zh-hans `
  --font-file D:/fonts/unifont.otf --output D:/prepared/th075-languages
```

Repeat for `lang_en` to reproduce the default language selection. An alternative
font requires its own `--font-license`; the default license is the bundled
Unifont OFL text. Python needs the host's existing Pillow and fonttools
dependencies. Download caching is keyed by expected CRC and source URL, and
cache hits are revalidated. Compiled ZIPs retain SHA-256 publication identity.
`--th075-language-packs=...` uses the existing server-package publisher.
For source development, `EAGLER_TH075_LANGUAGE_PACKS` names the prepared catalog.

Verified Simplified Chinese coverage: 101 image banks, ten ending scripts,
Music Room comments/titles and all required glyphs. English reflects its
smaller source set. Missing translations retain the Japanese resource. No
claim is made that thpatch currently translates every TH075 Story line.
The generated options file records unsupported resources and effective scope.

Gates: compiler/static-pack/patch-precedence tests, UI/tool type checks,
self-host bundle closure, actual Launcher runs in ja/zh-hans/en, a non-published
native harness checking every replacement and eleven fighters, and translated
menu-entry latency/heap checks. The existing TH06–08 upstream-contract L3 test
requires its separately configured pinned source checkout; a newly cloned
latest thcrap-tsa has different JSONC/string-ID inputs and is not substituted
for that missing fixture or reported as a PASS.

Isolation: Launcher `eagler` base `7cf9822`, experiment
`experiment/th075-thcrap` in `worktrees/launcher-th075-thcrap`; title base
`d792a62`, experiment `experiment/thcrap` in `worktrees/th075-thcrap`.
The canonical Eagler checkout remains the accepted Runtime consumer after
review/promotion. The delivery receipt records exact experiment/integration
heads and rerun canonical gates. Japanese DATA and other titles remain intact.

English correction (2026-10-15): thpatch/lang_en alone supplies Music Room
and one inherited sprite image, not English Story or menu artwork.
Use scripts/prepare-th075-english-static.mjs with the SHA-256 pinned
IaMP English Translation v1.1 th075e.dat to add 574 image replacements and
all eleven translated character scripts. The tool extracts only data, maps
aliases, fits seven 512px spell names to original 256px slots, and reflows six
ending speaker groups across original pages. It never applies DLL/EXE/PAT
patches or changes script control flow. Full legacy layout parity and full
ending playthrough are not established by resource/native/browser checks.
