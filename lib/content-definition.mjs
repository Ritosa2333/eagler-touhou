import { PRODUCT_GAMES } from "./contracts/product-catalog.mjs";

const numbered = (prefix, values, extension) => Object.freeze(values.map(value => `${prefix}_${value}.${extension}`));
const TH06_ARCHIVES = Object.freeze([
  "紅魔郷CM.DAT", "紅魔郷ED.DAT", "紅魔郷IN.DAT",
  "紅魔郷MD.DAT", "紅魔郷ST.DAT", "紅魔郷TL.DAT",
]);

// Product content shape only. This file declares names and Runtime mounts, but
// never points at a workstation build, asset directory, or generated identity.
export const PRODUCT_CONTENT = Object.freeze({
  th06: Object.freeze({
    original: Object.freeze({
      files: TH06_ARCHIVES,
      oggSourceFiles: Object.freeze(Array.from({ length: 17 }, (_, index) => `bgm/th06_${String(index + 1).padStart(2, "0")}.wav`)),
    }),
    music: Object.freeze({
      wav: Object.freeze({ mount: PRODUCT_GAMES.th06.package.musicMounts.wav, files: numbered("th06", Array.from({ length: 17 }, (_, index) => String(index + 1).padStart(2, "0")), "wav") }),
      ogg: Object.freeze({ mount: PRODUCT_GAMES.th06.package.musicMounts.ogg, files: numbered("th06", Array.from({ length: 17 }, (_, index) => String(index + 1).padStart(2, "0")), "ogg") }),
    }),
    hostPreparation: Object.freeze({
      artwork: Object.freeze({
        kind: "pbg3-title-plus-pe-icon",
        card: Object.freeze({ archive: "紅魔郷TL.DAT", entry: "title00.jpg" }),
        icon: Object.freeze({ executableCandidates: Object.freeze(["th06.exe", "東方紅魔郷.exe", "紅魔郷.exe"]) }),
      }),
      languagePack: Object.freeze({
        kind: "thcrap-runtime-compiler",
        inputMode: "archives",
        developmentEnv: "EAGLER_TH06_ARCHIVES",
        developmentFiles: Object.freeze(["紅魔郷ST.DAT", "紅魔郷ED.DAT"]),
      }),
      ogg: Object.freeze({ kind: "verified-converter", outputDirectory: "bgm" }),
      dataAssets: Object.freeze({
        kind: "legacy-preload-with-focus-hitbox",
        focusHitbox: Object.freeze({
          sourceGame: "th07",
          extractor: "extract-th07-texture",
          archive: "th07.dat",
          anm: "etama.anm",
          texture: "data/etama/etama2.png",
          output: "eagler-hitbox.png",
        }),
      }),
    }),
  }),
  th07: Object.freeze({
    original: Object.freeze({
      files: Object.freeze(["th07.dat"]),
      oggSourceFiles: Object.freeze(["thbgm.dat"]),
    }),
    music: Object.freeze({
      wav: Object.freeze({ mount: PRODUCT_GAMES.th07.package.musicMounts.wav, files: Object.freeze(["thbgm.dat"]) }),
      ogg: Object.freeze({ mount: PRODUCT_GAMES.th07.package.musicMounts.ogg, files: numbered("th07", ["01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "13b", "14", "15", "16", "17", "18", "19"], "ogg") }),
    }),
    hostPreparation: Object.freeze({
      artwork: Object.freeze({
        kind: "pbg4-title",
        card: Object.freeze({ archive: "th07.dat", entry: "title00.jpg" }),
      }),
      languagePack: Object.freeze({
        kind: "thcrap-runtime-compiler",
        inputMode: "archive",
        archive: "th07.dat",
        developmentEnv: "EAGLER_TH07_ARCHIVES",
        developmentFiles: Object.freeze(["th07.dat"]),
      }),
      ogg: Object.freeze({ kind: "verified-converter", outputDirectory: "bgm-ogg" }),
    }),
  }),
  th08: Object.freeze({
    original: Object.freeze({
      files: Object.freeze(["th08.dat"]),
      oggSourceFiles: Object.freeze(["thbgm.dat"]),
    }),
    dataLayout: "sha256-8df4f18fe2e70e5b505906276dfb7b0f8aee5854eb931fbca5c1b9dbf065d566",
    music: Object.freeze({
      ogg: Object.freeze({ mount: PRODUCT_GAMES.th08.package.musicMounts.ogg, files: numbered("th08", ["01", "00", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "13b", "15", "16", "17", "18", "19", "20"], "ogg") }),
    }),
    hostPreparation: Object.freeze({
      artwork: Object.freeze({
        kind: "pbgz-title",
        card: Object.freeze({ archive: "th08.dat", entry: "title/title00.png" }),
      }),
      languagePack: Object.freeze({
        kind: "thcrap-runtime-compiler",
        inputMode: "archive",
        archive: "th08.dat",
        developmentEnv: "EAGLER_TH08_ARCHIVES",
        developmentFiles: Object.freeze(["th08.dat"]),
      }),
      ogg: Object.freeze({ kind: "verified-converter", outputDirectory: "bgm-ogg" }),
      dataAssets: Object.freeze({ kind: "original-file", source: "th08.dat" }),
    }),
  }),
  th10: Object.freeze({
    original: Object.freeze({
      files: Object.freeze(["th10.dat"]),
      oggSourceFiles: Object.freeze(["thbgm.dat"]),
      preparedAlternative: Object.freeze({
        directory: "assets-ogg",
        markerFiles: Object.freeze(["th10.data", "bgm-ogg/th10_00.ogg"]),
      }),
    }),
    dataLayout: "sha256-6b25b2ce0bf32d56880d32b4a0d26541eae670ee31e4de7126fcfd4d95f446a7",
    music: Object.freeze({
      ogg: Object.freeze({
        mount: PRODUCT_GAMES.th10.package.musicMounts.ogg,
        // thbgm.dat's retail order starts with title (02), then stage 1 (00).
        // Keep that order in the package so the Runtime's first available
        // tracks are audible while retaining the original th10_XX names.
        files: numbered("th10", ["02", "00", "01", ...Array.from({ length: 10 }, (_, index) => String(index + 3).padStart(2, "0")), "15", "16", "13", "14", "17"], "ogg"),
      }),
    }),
    hostPreparation: Object.freeze({
      artwork: Object.freeze({
        kind: "thtk-anm-title",
        card: Object.freeze({ archive: "th10.dat", anm: "title.anm", textures: Object.freeze(["title/title00a.png", "title/title00b.png"]) }),
      }),
      preparedContent: Object.freeze({
        directory: "assets-ogg",
        markerFiles: Object.freeze(["th10.data", "bgm-ogg/th10_00.ogg"]),
        script: "scripts/prepare-th10-content.mjs",
      }),
      ogg: Object.freeze({ kind: "prepared-content" }),
      dataAssets: Object.freeze({ kind: "prepared-content" }),
    }),
  }),
});
