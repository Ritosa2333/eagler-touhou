import { PRODUCT_CONTENT } from "./content-definition.mjs";
import { projectRelativeWorkspaceDirectory, projectRelativeWorkspacePath } from "./workspace-layout.mjs";
import { relative, resolve } from "node:path";

const ws = projectRelativeWorkspacePath;
const wsDir = projectRelativeWorkspaceDirectory;
const project = resolve(import.meta.dirname, "..");
const projectRelativePath = path => {
  const value = relative(project, resolve(path)).replaceAll("\\", "/");
  return value.startsWith(".") ? value : `./${value}`;
};
const th08ContentRoot = process.env.EAGLER_TH08_CONTENT_DIR?.trim()
  ? resolve(process.env.EAGLER_TH08_CONTENT_DIR.trim())
  : null;
const th08DataFile = process.env.EAGLER_TH08_DATA_FILE?.trim()
  ? resolve(process.env.EAGLER_TH08_DATA_FILE.trim())
  : null;
const th10ContentRoot = process.env.EAGLER_TH10_CONTENT_DIR?.trim()
  ? resolve(process.env.EAGLER_TH10_CONTENT_DIR.trim())
  : null;
const th09ContentRoot = process.env.EAGLER_TH09_CONTENT_DIR?.trim()
  ? resolve(process.env.EAGLER_TH09_CONTENT_DIR.trim())
  : null;

export const DEVELOPMENT_CONTENT = Object.freeze({
  shared: Object.freeze({
    vanillaFont: process.env.EAGLER_DEVELOPMENT_VANILLA_FONT
      ? projectRelativePath(process.env.EAGLER_DEVELOPMENT_VANILLA_FONT)
      : ws("th06", "assets", "msgothic.ttc"),
    unicodeFont: process.env.EAGLER_DEVELOPMENT_UNICODE_FONT
      ? projectRelativePath(process.env.EAGLER_DEVELOPMENT_UNICODE_FONT)
      : ws("dependencies", "unifont-15.1.05", "unifont-15.1.05.otf"),
  }),
  games: Object.freeze({
    th06: Object.freeze({
      runtime: `${ws("th06", "build-web-eagler-thprac-test", "th06.html")}?hosted=1&v=audio-music-modes-1`,
      multiplayerRuntime: `${ws("th06", "build-web-netplay-th06", "th06.html")}?hosted=1&v=th06-netplay-local-2`,
      data: Object.freeze({
        source: ws("th06", "build-web-eagler-thprac-test", "th06.data"),
        runtimeScript: ws("th06", "build-web-eagler-thprac-test", "th06.js"),
      }),
      music: Object.freeze({
        wav: Object.freeze({
          base: wsDir("th06", "assets", "bgm"),
          ...PRODUCT_CONTENT.th06.music.wav,
        }),
        ogg: Object.freeze({
          base: wsDir("th06", "assets-ogg", "bgm"),
          ...PRODUCT_CONTENT.th06.music.ogg,
        }),
      }),
    }),
    th07: Object.freeze({
      runtime: `${ws("th07", "build-web-eagler-thprac", "th07.html")}?hosted=1&v=audio-music-modes-1`,
      multiplayerRuntime: `${ws("th07", "build-web-th07-netplay", "th07.html")}?hosted=1&v=th07-netplay-local-2`,
      data: Object.freeze({
        source: ws("th07", "build-web-eagler-thprac", "th07.data"),
        runtimeScript: ws("th07", "build-web-eagler-thprac", "th07.js"),
      }),
      music: Object.freeze({
        wav: Object.freeze({ base: wsDir("th07", "assets"), ...PRODUCT_CONTENT.th07.music.wav }),
        ogg: Object.freeze({
          base: wsDir("th07", "assets-ogg", "bgm-ogg"),
          ...PRODUCT_CONTENT.th07.music.ogg,
        }),
      }),
    }),
    th08: Object.freeze({
      runtime: `${ws("th08", "build-eagler", "th08.html")}?hosted=1`,
      data: Object.freeze(th08DataFile ? {
        source: projectRelativePath(th08DataFile),
        layout: PRODUCT_CONTENT.th08.dataLayout,
      } : {
        identity: Object.freeze({
          bytes: 46838025,
          sha256: "9d7edf43b8ddd347cbb641836f6b5050745dd936f688daebbf9382ca557043bb",
          layout: PRODUCT_CONTENT.th08.dataLayout,
        }),
      }),
      music: Object.freeze(th08ContentRoot ? {
        ogg: Object.freeze({
          base: `${projectRelativePath(resolve(th08ContentRoot, "bgm-ogg"))}/`,
          ...PRODUCT_CONTENT.th08.music.ogg,
        }),
      } : {}),
    }),
    th09: Object.freeze({
      runtime: `${ws("th09", "th09_web", "build-eagler", "th09.html")}?hosted=1`,
      multiplayerRuntime: `${ws("th09", "th09_web", "build-eagler-multiplayer", "th09.html")}?hosted=1`,
      data: Object.freeze({
        source: th09ContentRoot
          ? projectRelativePath(resolve(th09ContentRoot, "th09.data"))
          : ws("th09", "[th09] 东方花映塚 (日文版)", "th09.dat"),
        layout: PRODUCT_CONTENT.th09.dataLayout,
      }),
      music: Object.freeze(th09ContentRoot ? {
        ogg: Object.freeze({
          base: `${projectRelativePath(resolve(th09ContentRoot, "music"))}/`,
          ...PRODUCT_CONTENT.th09.music.ogg,
        }),
      } : {}),
    }),
    th10: Object.freeze({
      runtime: `${ws("th10", "build-eagler", "th10.html")}?hosted=1`,
      data: Object.freeze(th10ContentRoot ? {
        source: resolve(th10ContentRoot, "th10.data"),
        layout: PRODUCT_CONTENT.th10.dataLayout,
      } : {
        identity: Object.freeze({
          bytes: 27696219,
          sha256: "1fb1d0ffe34115f563f5feb43755c0feee2315b0ac2b32e2f9e84c81e9433bea",
          layout: PRODUCT_CONTENT.th10.dataLayout,
        }),
      }),
      music: Object.freeze(th10ContentRoot ? {
        ogg: Object.freeze({
          base: `${resolve(th10ContentRoot, "bgm-ogg").replaceAll("\\", "/")}/`,
          ...PRODUCT_CONTENT.th10.music.ogg,
        }),
      } : {}),
    }),
  }),
});
