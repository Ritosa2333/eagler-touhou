import { PRODUCT_GAMES } from "./contracts/product-catalog.mjs";

const PROVIDERS = Object.freeze({
  "emscripten-preload": Object.freeze({
    layout: "emscripten-load-package",
    shellMarkers: Object.freeze([
      "window.parent.__eaglerPrepareManagedRuntimeDataV1",
      "Module.getPreloadedPackage",
    ]),
  }),
  "retail-memory": Object.freeze({
    layout: "declared-content",
    shellMarkers: Object.freeze([
      "window.parent.__eaglerPrepareManagedRuntimeDataV1",
    ]),
  }),
});

export function runtimeDataProvider(game) {
  const name = PRODUCT_GAMES[game]?.dataProvider;
  const contract = PROVIDERS[name];
  if (!contract) throw new Error(`${game}: unknown Runtime DATA provider`);
  return { name, ...contract };
}

export function assertRuntimeDataShell(source, game, variant) {
  if (typeof source !== "string") throw new Error(`${game} ${variant}: Runtime shell is not text`);
  const provider = runtimeDataProvider(game);
  const missing = provider.shellMarkers.filter(marker => !source.includes(marker));
  if (missing.length) {
    throw new Error(`${game} ${variant} Runtime shell is stale: ${provider.name} provider markers are missing`);
  }
  return provider;
}
