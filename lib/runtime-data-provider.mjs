import { PRODUCT_GAMES } from "./contracts/product-catalog.mjs";

const PROVIDERS = Object.freeze({
  "emscripten-preload": Object.freeze({
    layout: "emscripten-load-package",
    runtimeLayouts: Object.freeze(["flat"]),
    shellMarkers: Object.freeze([
      "window.parent.__eaglerPrepareManagedRuntimeDataV1",
      "Module.getPreloadedPackage",
    ]),
  }),
  "retail-memory": Object.freeze({
    layout: "declared-content",
    runtimeLayouts: Object.freeze(["directory"]),
    shellMarkers: Object.freeze([
      "window.parent.__eaglerPrepareManagedRuntimeDataV1",
    ]),
  }),
});

export function runtimeAdapterProfile(game) {
  const product = PRODUCT_GAMES[game];
  if (!product) throw new Error(`${game}: unknown product`);
  const provider = PROVIDERS[product.dataProvider];
  if (!provider) throw new Error(`${game}: unknown Runtime DATA provider`);
  const runtimeLayout = product.runtimeFileLayout === "directory" ? "directory" : "flat";
  if (!provider.runtimeLayouts.includes(runtimeLayout)) {
    throw new Error(`${game}: unsupported adapter profile ${runtimeLayout}+${product.dataProvider}`);
  }
  return Object.freeze({ dataProvider: product.dataProvider, runtimeLayout });
}

export function runtimeDataProvider(game) {
  const name = PRODUCT_GAMES[game]?.dataProvider;
  const contract = PROVIDERS[name];
  if (!contract) throw new Error(`${game}: unknown Runtime DATA provider`);
  runtimeAdapterProfile(game);
  return { name, ...contract };
}

export function assertRuntimeDataShell(source, game, variant) {
  if (typeof source !== "string") throw new Error(`${game} ${variant}: Runtime shell is not text`);
  const provider = runtimeDataProvider(game);
  if (PRODUCT_GAMES[game]?.runtimeFileLayout === "directory") {
    const declaration = `<meta name="eagler-data-provider" content="${provider.name}">`;
    if (!source.includes(declaration)) {
      throw new Error(`${game} ${variant} Runtime shell is stale: ${provider.name} provider declaration is missing`);
    }
    return provider;
  }
  const missing = provider.shellMarkers.filter(marker => !source.includes(marker));
  if (missing.length) {
    throw new Error(`${game} ${variant} Runtime shell is stale: ${provider.name} provider markers are missing`);
  }
  return provider;
}
