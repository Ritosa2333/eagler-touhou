import { writeRuntimeGeneration } from "../../lib/runtime-generations.mjs";
import { rm } from "node:fs/promises";
import { createHash } from "node:crypto";
import { mkdir, writeFile } from "node:fs/promises";
import { resolve } from "node:path";
import { PRODUCT_CONTENT } from "../../lib/content-definition.mjs";
import { PRODUCT_GAMES } from "../../lib/contracts/product-catalog.mjs";
import { RUNTIME_RELEASE_SCHEMA, runtimeStem, validateRuntimeReleaseManifest } from "../../lib/runtime-release.mjs";
import { readFile } from "node:fs/promises";
import { extractGameDataLayout } from "../../lib/runtime-data-layout.mjs";

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

function flatRuntimeShellFixture(game, variant, provider) {
  return `<script>
const protocol="eagler-touhou/1",game="${game}";
const epoch=Number(new URLSearchParams(location.search).get("runtimeEpoch"));
window.addEventListener("message",event=>{const message=event.data||{};if(message.epoch!==epoch)return;});
window.parent.postMessage({protocol,game,epoch,event:"ready"},location.origin);
const prepare=window.parent.__eaglerPrepareManagedRuntimeDataV1;
prepare({game,generation:"fixture",epoch});
${provider === "emscripten-preload" ? "Module.getPreloadedPackage;" : ""}
</script>${variant}`;
}

function directoryProtocolFixture(game) {
  return `const protocol="eagler-touhou/1",game="${game}";
const epoch=Number(new URLSearchParams(location.search).get("runtimeEpoch"));
window.addEventListener("message",event=>{const m=event.data||{};if(m.epoch!==epoch)return;});
parent.__eaglerPrepareManagedRuntimeDataV1({game,generation:"fixture",epoch});
parent.postMessage({protocol,game,epoch,event:"ready"},location.origin);`;
}

function directoryManagedDataFixture() {
  return `export async function mountManagedData(parentWindow,query,game){
const epoch=Number(query.get("runtimeEpoch"));
return parentWindow.__eaglerPrepareManagedRuntimeDataV1({game,generation:"fixture",epoch});
}`;
}

// Workspace Host integration may substitute real preload programs. Other
// registered products remain synthetic and are never presented as game proof.
export async function writeSyntheticRuntimeRelease(root, {preloadBuilds = {}} = {}) {
  const games = {};
  for (const [game, product] of Object.entries(PRODUCT_GAMES)) {
    const stem = runtimeStem(game);
    const preload = product.dataProvider === "emscripten-preload" ? preloadFixture(game) : null;
    const dataLayout = preloadBuilds[game]
      ? extractGameDataLayout(await readFile(resolve(preloadBuilds[game].runtime, `${stem}.js`), "utf8"), game).layout
      : preload?.layout || PRODUCT_CONTENT[game].dataLayout;
    const variants = [["runtime", `runtime/${game}`, "normal"]];
    if (product.multiplayerRuntime) variants.push(["multiplayerRuntime", `runtime/${game}/multiplayer`, "multiplayer"]);
    const entry = {
      dataProvider: product.dataProvider,
      dataLayout,
      features: { thprac: false, languages: false, focusHitbox: false },
    };
    if (preloadBuilds[game] && product.features.focusHitbox) {
      const layout = extractGameDataLayout(await readFile(resolve(preloadBuilds[game].runtime, `${stem}.js`), "utf8"), game);
      entry.features.focusHitbox = layout.files.some(([path]) => path === '/eagler-hitbox.png');
    }
    for (const [key, runtimeRoot, variant] of variants) {
      const target = resolve(root, runtimeRoot);
      await mkdir(target, { recursive: true });
      const html = Buffer.from(product.runtimeFileLayout === "directory"
        ? `<meta name="eagler-data-provider" content="${product.dataProvider}">${variant}`
        : flatRuntimeShellFixture(game, variant, product.dataProvider));
      const js = Buffer.from(preload?.script || `globalThis.__fixture=${JSON.stringify(game)};`);
      const wasm = Buffer.from(`${game}:${variant}:wasm`);
      const payloads = product.runtimeFileLayout === "directory"
        ? Object.fromEntries(product.runtimeAssets.map(name => [name,
          name === `${stem}.html` ? html :
          ["shell.mjs", "managed-shell.mjs"].includes(name) ? Buffer.from(directoryProtocolFixture(game)) :
          ["eagler-host.mjs", "managed-data.mjs"].includes(name) ? Buffer.from(directoryManagedDataFixture()) :
          Buffer.from(name.endsWith(".mjs") || name.endsWith(".js") ? `// ${game}:${variant}:${name}\n` : `${game}:${variant}:${name}`)]))
        : {
            [`${stem}.html`]: html,
            [`${stem}.js`]: js,
            [`${stem}.wasm`]: wasm,
          };
      if (preloadBuilds[game]) {
        assertPreload(product, game);
        const build = preloadBuilds[game][key];
        if (!build) throw new Error(`${game}: real fixture build missing for ${key}`);
        for (const name of Object.keys(payloads)) payloads[name] = await readFile(resolve(build, name));
        if (extractGameDataLayout(payloads[`${stem}.js`].toString("utf8"), game).layout !== dataLayout)
          throw new Error(`${game}: real fixture variants have different DATA layouts`);
      }
      for (const [name, bytes] of Object.entries(payloads)) {
        await mkdir(resolve(target, name, ".."), { recursive: true });
        await writeFile(resolve(target, name), bytes);
      }
      const generation = await writeRuntimeGeneration({ site: root, root: runtimeRoot + "/", source: target,
        entry: `${stem}.html`, names: Object.keys(payloads) });
      for (const name of Object.keys(payloads)) await rm(resolve(target, name));
      entry[key] = {
        root: runtimeRoot + "/" + generation.generation,
        generation: generation.generation,
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

function assertPreload(product, game) {
  if (product.runtimeFileLayout !== "preload" && product.dataProvider !== "emscripten-preload")
    throw new Error(`${game}: real fixture substitution only accepts preload programs`);
}
