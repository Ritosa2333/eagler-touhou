import { createHash } from "node:crypto";
import { mkdir, writeFile } from "node:fs/promises";
import { resolve } from "node:path";
import { PRODUCT_CONTENT } from "../../lib/content-definition.mjs";
import { PRODUCT_GAMES } from "../../lib/contracts/product-catalog.mjs";
import { RUNTIME_RELEASE_SCHEMA, runtimeStem, validateRuntimeReleaseManifest } from "../../lib/runtime-release.mjs";

const identity = bytes => ({
  bytes: bytes.length,
  sha256: createHash("sha256").update(bytes).digest("hex"),
});

function preloadFixture(game) {
  const files = [[`/${game}-fixture.dat`, 0, 4]];
  const layout = `sha256-${createHash("sha256").update(JSON.stringify(files)).digest("hex")}`;
  const script = `loadPackage({files:[{filename:"/${game}-fixture.dat",start:0,end:4}],remote_package_size:4});`;
  return { layout, script };
}

export async function writeSyntheticRuntimeRelease(root) {
  const games = {};
  for (const [game, product] of Object.entries(PRODUCT_GAMES)) {
    const stem = runtimeStem(game);
    const preload = product.dataProvider === "emscripten-preload" ? preloadFixture(game) : null;
    const dataLayout = preload?.layout || PRODUCT_CONTENT[game].dataLayout;
    const variants = [["runtime", `runtime/${game}`, "normal"]];
    if (product.multiplayerRuntime) variants.push(["multiplayerRuntime", `runtime/${game}/multiplayer`, "multiplayer"]);
    const entry = {
      dataProvider: product.dataProvider,
      dataLayout,
      features: { thprac: false, languages: false, focusHitbox: false },
    };
    for (const [key, runtimeRoot, variant] of variants) {
      const target = resolve(root, runtimeRoot);
      await mkdir(target, { recursive: true });
      const html = Buffer.from(product.runtimeFileLayout === "directory"
        ? `<meta name="eagler-data-provider" content="${product.dataProvider}">${variant}`
        : product.dataProvider === "emscripten-preload"
          ? `<script>window.parent.__eaglerPrepareManagedRuntimeDataV1;Module.getPreloadedPackage;</script>${variant}`
          : `<script>window.parent.__eaglerPrepareManagedRuntimeDataV1;</script>${variant}`);
      const js = Buffer.from(preload?.script || `globalThis.__fixture=${JSON.stringify(game)};`);
      const wasm = Buffer.from(`${game}:${variant}:wasm`);
      const payloads = product.runtimeFileLayout === "directory"
        ? Object.fromEntries(product.runtimeAssets.map(name => [name,
          name === `${stem}.html` ? html : Buffer.from(`${game}:${variant}:${name}`)]))
        : {
            [`${stem}.html`]: html,
            [`${stem}.js`]: js,
            [`${stem}.wasm`]: wasm,
          };
      for (const [name, bytes] of Object.entries(payloads)) {
        await mkdir(resolve(target, name, ".."), { recursive: true });
        await writeFile(resolve(target, name), bytes);
      }
      entry[key] = {
        root: runtimeRoot,
        files: Object.fromEntries(Object.entries(payloads).map(([name, bytes]) => [name, identity(bytes)])),
      };
    }
    games[game] = entry;
  }
  const manifest = validateRuntimeReleaseManifest({
    schema: RUNTIME_RELEASE_SCHEMA,
    protocol: "eagler-touhou/1",
    games,
  });
  await writeFile(resolve(root, "runtime-release.json"), `${JSON.stringify(manifest, null, 2)}\n`);
  return manifest;
}
