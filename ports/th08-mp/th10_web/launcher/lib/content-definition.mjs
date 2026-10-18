import { PRODUCT_GAMES } from "./contracts/product-catalog.mjs";

const numbered = (prefix, values, extension) => Object.freeze(values.map(value => `${prefix}_${value}.${extension}`));

// Product content shape only. This file declares names and Runtime mounts, but
// never points at a workstation build, asset directory, or generated identity.
export const PRODUCT_CONTENT = Object.freeze({
  th06: Object.freeze({
    music: Object.freeze({
      wav: Object.freeze({ mount: PRODUCT_GAMES.th06.package.musicMounts.wav, files: numbered("th06", Array.from({ length: 17 }, (_, index) => String(index + 1).padStart(2, "0")), "wav") }),
      ogg: Object.freeze({ mount: PRODUCT_GAMES.th06.package.musicMounts.ogg, files: numbered("th06", Array.from({ length: 17 }, (_, index) => String(index + 1).padStart(2, "0")), "ogg") }),
    }),
  }),
  th07: Object.freeze({
    music: Object.freeze({
      wav: Object.freeze({ mount: PRODUCT_GAMES.th07.package.musicMounts.wav, files: Object.freeze(["thbgm.dat"]) }),
      ogg: Object.freeze({ mount: PRODUCT_GAMES.th07.package.musicMounts.ogg, files: numbered("th07", ["01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "13b", "14", "15", "16", "17", "18", "19"], "ogg") }),
    }),
  }),
  th08: Object.freeze({
    dataLayout: "sha256-8df4f18fe2e70e5b505906276dfb7b0f8aee5854eb931fbca5c1b9dbf065d566",
    music: Object.freeze({
      ogg: Object.freeze({ mount: PRODUCT_GAMES.th08.package.musicMounts.ogg, files: numbered("th08", ["01", "00", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "13b", "15", "16", "17", "18", "19", "20"], "ogg") }),
    }),
  }),
});
