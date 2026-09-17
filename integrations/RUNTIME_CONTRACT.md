# thcrap / thprac Runtime contract

The Eagler Host and the game Runtimes share runtime data only; neither links the other's source.

## thcrap

1. The Host requests `GET /api/thcrap/{game}/{language}/manifest.json`.
2. The server reads `repo.js`, `patch.js`, `files.js`, and resources from the patch repository and validates each thcrap CRC32.
3. The server compiles `.jdiff` into the original game's `msg*.dat` / `end*.end`, and compiles `spells.js`, `stages.js`, `musiccmt.js`, and `themes.js` into `ETL1` localization tables. Images retain their original formats.
4. Before calling the game's `Module.callMain()`, the Host must download every resource in the manifest and write it to its `targetPath`. Only a manifest with `runtimeReady: true` may launch.

The Runtime knows only the `/thcrap/th06/` or `/thcrap/th07/` file tree. It does not know language IDs, repository URLs, cache locations, or server implementations. When no translation pack is installed, file lookup falls back to the original archive automatically.

Some language packs provide font preferences in files such as `th06/th06.js`. The server normalizes these into `localization/options.json` for the Host, but thcrap supplies only font names, not font files. The Host must select fonts it can legally provide that cover the required Unicode glyphs. Successful resource compilation does not prove complete font coverage.

## thprac

Before calling `Module.callMain()`, the Host sets:

```js
Module.eaglerOptions.thprac = {
  enabled: true,
  game: "th06", // or th07
  ...
};
```

TH06 and TH07 use this legacy module option. TH08 receives the canonical
`thpracEnabled` and `thpracLocale` fields through the Runtime `configure`
command and owns its live practice state internally.

All three games embed practice parameters directly in the Replay's PRAC trailer
and read them only from the Replay itself during playback. An external
`*.rpy.thprac.json` sidecar was never a published format and is not part of the
supported Replay data model. TH06 additionally exports
`_EaglerThpracSaveReplaySlot(slot)` so the Host can request an in-game save to
Replay slot 1–99; the Host remains responsible for synchronizing IDBFS
afterward.

TH06 and TH07 provide coarse stage navigation, direct frame navigation,
initial resources/score/Rank (including cherry points in TH07), and practice
Replay metadata. Their exact-offset upstream features remain deferred. TH08
also provides exact section and multi-phase spell starts, dialogue control,
game-specific gauge/time/night/familiar parameters, source-level ECL/STD/ANM
patches, and practice assists. A Runtime must expose only the features listed
for that game in `integrations/thprac.mjs`.
