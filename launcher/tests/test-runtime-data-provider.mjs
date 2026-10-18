/** L0 provider contract. Preconditions: registered product adapters only.
 * Mutations: synthetic Runtime shell markers. Proves provider-driven shell
 * and layout gates. Does not prove DATA transfer or Runtime execution. */
import assert from "node:assert/strict";
import { PRODUCT_GAMES } from "../lib/contracts/product-catalog.mjs";
import { assertRuntimeDataShell, runtimeAdapterProfile, runtimeDataProvider } from "../lib/runtime-data-provider.mjs";

for (const [game, product] of Object.entries(PRODUCT_GAMES)) {
  const provider = runtimeDataProvider(game);
  assert.equal(provider.name, product.dataProvider);
  const profile = runtimeAdapterProfile(game);
  assert.equal(profile.dataProvider, product.dataProvider);
  assert.equal(profile.runtimeLayout, product.runtimeFileLayout === "directory" ? "directory" : "flat");
  const declaration = `<meta name="eagler-data-provider" content="${provider.name}">`;
  const complete = product.runtimeFileLayout === "directory"
    ? [declaration, ...provider.shellMarkers].join("\n")
    : provider.shellMarkers.join("\n");
  assert.equal(assertRuntimeDataShell(complete, game, "fixture").name, provider.name);
  if (product.runtimeFileLayout === "directory") {
    assert.throws(() => assertRuntimeDataShell(provider.shellMarkers.join("\n"), game, "fixture"), /provider declaration is missing/);
    continue;
  }
  for (const marker of provider.shellMarkers) {
    const incomplete = provider.shellMarkers.filter(value => value !== marker).join("\n");
    assert.throws(() => assertRuntimeDataShell(incomplete, game, "fixture"), /provider markers are missing/);
  }
}

assert.throws(() => runtimeDataProvider("unknown"), /unknown Runtime DATA provider/);
console.log(`Runtime DATA provider L0: PASS (${Object.keys(PRODUCT_GAMES).length} adapters)`);
