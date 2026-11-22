// Build a player-importable eagler Package ZIP for Touhou BossRush (BRO).
//
// BRO is importOnly: the runtime/WASM ships with the port, but the 1004 game
// resource files are supplied by the player. This script turns the BRO source
// tree's web/dist/package-manifest.json + game/ resources into the ZIP format
// the Launcher's Import window accepts (descriptor "eagler-touhou/package/1").
//
// Usage:
//   node scripts/package-bro-import.mjs <broRepoRoot> <output.zip> [--deflate]
//
// Example:
//   node scripts/package-bro-import.mjs "E:/Web/TouHou BossRush" "E:/Web/TouHou BossRush/bro-import-package.zip" --deflate
//
// Runtime files (bro.wasm / bro.mjs / src/*.mjs / dist/*.json) are NOT bundled:
// they are deployed with ports/bossrush/web/. Only game resources are packed.

import { createHash } from "node:crypto";
import { readFile, stat, writeFile, mkdir } from "node:fs/promises";
import { dirname, join, resolve } from "node:path";
import { strToU8, zipSync } from "fflate";
import { canonicalPackagePayload, validatePackageDescriptor } from "../package/package-descriptor.mjs";

const sha256 = (bytes) => createHash("sha256").update(bytes).digest("hex");

const broRoot = resolve(process.argv[2] || "");
const output = process.argv[3] ? resolve(process.argv[3]) : resolve(broRoot, "bro-import-package.zip");
const deflate = process.argv.includes("--deflate");
if (!process.argv[2] || !process.argv[3]) {
  throw new Error("usage: node scripts/package-bro-import.mjs <broRepoRoot> <output.zip> [--deflate]");
}

const manifestPath = join(broRoot, "web", "dist", "package-manifest.json");
const manifest = JSON.parse(await readFile(manifestPath, "utf8"));
if (manifest.schema !== "bro/package-manifest/1") throw new Error("unexpected BRO manifest schema");
if (!Array.isArray(manifest.files) || manifest.files.length === 0) throw new Error("BRO manifest has no files");

// 1-byte placeholder DATA file. BRO never reads /bro.data (managed-shell just
// checks identity), but the managed-data protocol requires a dataFile target.
const dataBytes = new Uint8Array([0]);
const dataHash = sha256(dataBytes);

const descriptor = {
  schema: "eagler-touhou/package/1",
  game: "bro",
  revision: "pending",
  runtimeRequirement: {
    protocol: "eagler-touhou/1",
    target: "bro",
    dataFile: "game-data",
    dataLayout: manifest.version || "sha256-unknown",
  },
  files: {
    "game-data": {
      source: "bro.data",
      target: "/bro.data",
      revision: dataHash.slice(0, 16),
      bytes: dataBytes.length,
      sha256: dataHash,
    },
  },
  base: { files: ["game-data"] },
  components: {},
};

const entries = { "bro.data": dataBytes };
let payloadBytes = dataBytes.length;
const seenSources = new Set(["bro.data"]);

for (const file of manifest.files) {
  const { p, n, h } = file;
  if (!p || typeof n !== "number" || typeof h !== "string") throw new Error(`invalid manifest entry: ${JSON.stringify(file)}`);
  if (seenSources.has(`game/${p}`)) throw new Error(`duplicate resource source: ${p}`);
  seenSources.add(`game/${p}`);
  const source = join(broRoot, "game", ...p.split("/"));
  const info = await stat(source).catch(() => null);
  if (!info?.isFile()) throw new Error(`missing BRO resource: ${source}`);
  const bytes = await readFile(source);
  if (bytes.length !== n) throw new Error(`size mismatch for ${p}: ${bytes.length} != ${n}`);
  const actualHash = sha256(bytes);
  if (actualHash !== h) throw new Error(`sha256 mismatch for ${p}: ${actualHash} != ${h}`);

  // FILE_ID allows [a-z0-9._:-] only and max 128 chars; resource paths may
  // contain spaces, slashes, CJK and exceed that. Encode the path as hex so the
  // id is always safe/collision-free. source/target keep the real path.
  const encodedPath = Buffer.from(p, "utf8").toString("hex");
  const fileId = `asset:${encodedPath}`;
  descriptor.files[fileId] = {
    source: `game/${p}`,
    // BRO's resource_cache normalizes away a leading "game/" and "/" so this
    // matches the manifest's relative path `p` at runtime.
    target: `/game/${p}`,
    revision: h.slice(0, 16),
    bytes: n,
    sha256: h,
  };
  descriptor.base.files.push(fileId);
  const payload = new Uint8Array(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  // PNG/JPG/ogg/fonts are already compressed; STORE keeps import fast and
  // byte-identical. --deflate only compresses the tiny descriptor.
  entries[`game/${p}`] = deflate && !/\.(ogg|png|jpg|jpeg|webp|ttf|ttc|otf)$/i.test(p)
    ? [payload, { level: 6 }]
    : payload;
  payloadBytes += bytes.length;
}

descriptor.revision = sha256(strToU8(canonicalPackagePayload(descriptor))).slice(0, 16);
validatePackageDescriptor(descriptor);
if (descriptor.game !== "bro") throw new Error("descriptor game mismatch");

entries["package.json"] = strToU8(`${JSON.stringify(descriptor, null, 2)}\n`);
const archive = zipSync(entries, { level: deflate ? 6 : 0 });
const identity = sha256(archive).slice(0, 16);
await mkdir(dirname(output), { recursive: true });
await writeFile(output, archive);

console.log(JSON.stringify({
  game: "bro",
  revision: descriptor.revision,
  identity,
  output,
  files: Object.keys(descriptor.files).length,
  resources: manifest.files.length,
  payloadBytes,
  archiveBytes: archive.length,
  method: deflate ? "MIXED" : "STORE",
  descriptor: "package.json",
  runtimeRequirement: descriptor.runtimeRequirement,
}, null, 2));
