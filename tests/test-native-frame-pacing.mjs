import { readFileSync } from "node:fs";
import { PRODUCT_GAMES } from "../lib/contracts/product-catalog.mjs";
import { workspacePath } from "../lib/workspace-layout.mjs";

const preloadGames = Object.entries(PRODUCT_GAMES)
  .filter(([, product]) => product.dataProvider === "emscripten-preload")
  .map(([game]) => game)
  .sort();

for (const game of preloadGames) {
  const windowSource = readFileSync(workspacePath(game, "src", "GameWindow.cpp"), "utf8");
  const glesSource = readFileSync(workspacePath(game, "src", "graphics", "Gles.cpp"), "utf8");

  for (const [needle, label] of [
    ["SDL_GetDisplayForWindow", "window display detection"],
    ["SDL_GetCurrentDisplayMode", "current display mode detection"],
    ["refresh_rate", "display refresh-rate use"],
    ["SDL_DelayPrecise", "precise presentation pacing fallback"],
  ]) {
    if (!windowSource.includes(needle)) throw new Error(`${game}: missing ${label}`);
  }
  const webMarker = glesSource.lastIndexOf("#ifdef __EMSCRIPTEN__", glesSource.indexOf("SDL_GL_SetSwapInterval(-1)"));
  const adaptiveIdx = glesSource.indexOf("SDL_GL_SetSwapInterval(-1)", webMarker);
  const nativeElse = glesSource.indexOf("#else", adaptiveIdx);
  const branchEnd = glesSource.indexOf("#endif", nativeElse);
  if (webMarker < 0 || adaptiveIdx < webMarker || nativeElse < adaptiveIdx || branchEnd < nativeElse)
    throw new Error(`${game}: browser/native swap-interval branches are not isolated`);

  const webSwapBranch = glesSource.slice(webMarker, nativeElse);
  const nativeSwapBranch = glesSource.slice(nativeElse, branchEnd);
  if (!webSwapBranch.includes("SDL_GL_SetSwapInterval(-1)") || !webSwapBranch.includes("SDL_GL_SetSwapInterval(1)"))
    throw new Error(`${game}: browser swap behavior must retain adaptive-to-1 fallback`);
  if (!nativeSwapBranch.includes("SDL_GL_SetSwapInterval(1)"))
    throw new Error(`${game}: native strict VSync interval 1 is not requested`);
  if (nativeSwapBranch.includes("SDL_GL_SetSwapInterval(-1)"))
    throw new Error(`${game}: adaptive VSync must not be preferred in the native strict-VSync branch`);
}

function simulate(displayHz, seconds) {
  const presentationDt = 1 / displayHz;
  const simulationDt = 1 / 60;
  const callbacks = Math.round(displayHz * seconds);
  let accumulator = 0;
  let updates = 0;
  let draws = 0;
  for (let i = 0; i < callbacks; i++) {
    accumulator += presentationDt;
    while (accumulator + 1e-12 >= simulationDt) {
      accumulator -= simulationDt;
      updates++;
    }
    draws++;
  }
  return { displayHz, callbacks, updates, draws };
}

const cases = [60, 75, 120, 144, 165, 180, 240].map(hz => simulate(hz, 10));
for (const result of cases) {
  if (Math.abs(result.updates - 600) > 1)
    throw new Error(`${result.displayHz} Hz: simulation must remain 60 Hz`);
  if (result.draws !== Math.round(result.displayHz * 10))
    throw new Error(`${result.displayHz} Hz: presentation must follow display refresh`);
}

console.log(JSON.stringify({ games: preloadGames, simulationHz: 60, nativePresentation: "display-refresh", strictVsync: true, cases }));
