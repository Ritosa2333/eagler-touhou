import { execFile } from "node:child_process";
import { createHash } from "node:crypto";
import { mkdtemp, mkdir, readFile, rm, stat, writeFile } from "node:fs/promises";
import { basename, extname, join, resolve } from "node:path";
import { tmpdir } from "node:os";
import { promisify } from "node:util";
import { fileURLToPath } from "node:url";
import { createThcrapClient, downloadThcrapPack } from "../integrations/thcrap.mjs";
import { ThcrapRuntimeCompiler } from "../server/thcrap-compiler.mjs";
import { ThtkRunner } from "../server/thtk-runner.mjs";
import { createStaticThcrapPack } from "../server/thcrap-static-pack.mjs";
import { PRODUCT_CONTENT } from "../lib/content-definition.mjs";

const args = new Map();
for (let index = 2; index < process.argv.length; index++) {
  const key = process.argv[index];
  if (!key.startsWith("--")) throw new Error(`unexpected argument: ${key}`);
  const value = process.argv[index + 1];
  if (!value || value.startsWith("--")) throw new Error(`missing value for ${key}`);
  args.set(key.slice(2), value); index++;
}

const required = name => {
  const value = args.get(name);
  if (!value) throw new Error(`missing --${name}=PATH`);
  return resolve(value);
};
const project = resolve(fileURLToPath(new URL("..", import.meta.url)));
const workspace = resolve(project, "..");
const game = String(args.get("game") || "").toLowerCase();
if (!game) throw new Error("missing --game=GAME");
if (PRODUCT_CONTENT[game]?.hostPreparation?.languagePack?.kind !== "thcrap-runtime-compiler") {
  throw new Error(`unsupported thcrap Runtime language adapter: ${game}`);
}
const language = args.get("language") || "lang_zh-hans";
if (args.has("runtime-version") && String(args.get("runtime-version")).toLowerCase() !== "auto") {
  process.stderr.write("note: --runtime-version is retained for CLI compatibility but language packs are Runtime-independent\n");
}
const repository = args.get("repository");
const output = resolve(args.get("output") || "prepared/thcrap-static");
const archives = (args.get("archives") || required("archive")).split(";").filter(Boolean).map(value => resolve(value));
const fontFile = args.get("font-file")
  ? resolve(args.get("font-file"))
  : resolve(workspace, "dependencies", "unifont-15.1.05", "unifont-15.1.05.otf");
const fontName = args.get("font-name") || "Unifont";
const fontPython = args.get("font-python") || "python";
const refresh = ["1", "true", "yes"].includes(String(args.get("refresh") || "").toLowerCase());

async function localInputFingerprint() {
  const files = [];
  for (const path of archives) {
    const info = await stat(path);
    files.push({ name: basename(path), bytes: info.size, mtimeMs: Math.trunc(info.mtimeMs) });
  }
  const fontInfo = await stat(fontFile);
  const builderFiles = [
    resolve(project, "scripts/prepare-th06-language-pack.mjs"),
    resolve(project, "scripts/subset-font.py"),
    resolve(project, "integrations/thcrap.mjs"),
    resolve(project, "server/thcrap-compiler.mjs"),
    resolve(project, "server/thcrap-static-pack.mjs"),
    resolve(project, "server/thtk-runner.mjs"),
  ];
  const builders = [];
  for (const path of builderFiles) {
    const info = await stat(path);
    builders.push({ name: basename(path), bytes: info.size, mtimeMs: Math.trunc(info.mtimeMs) });
  }
  const payload = JSON.stringify({
    schema: "eagler-touhou/language-pack-cache-input/1",
    game,
    language,
    repository: repository || null,
    fontName,
    archives: files,
    font: { name: basename(fontFile), bytes: fontInfo.size, mtimeMs: Math.trunc(fontInfo.mtimeMs) },
    builders,
  });
  return createHash("sha256").update(payload).digest("hex");
}

async function tryReuseCache(fingerprint) {
  if (refresh) return null;
  let metadata;
  let catalog;
  try {
    metadata = JSON.parse(await readFile(resolve(output, `${language}.cache.json`), "utf8"));
    catalog = JSON.parse(await readFile(resolve(output, "catalog.json"), "utf8"));
  } catch {
    return null;
  }
  if (metadata?.schema !== "eagler-touhou/language-pack-cache/1" || metadata.fingerprint !== fingerprint ||
      catalog?.schema !== "eagler-touhou/thcrap-static-catalog/1" || catalog.game !== game || !Array.isArray(catalog.languages)) return null;
  const entry = catalog.languages.find(item => String(item?.id || "").toLowerCase() === language.toLowerCase());
  if (!entry?.pack?.url || !Number.isSafeInteger(entry.pack.bytes) || !/^[a-f0-9]{64}$/i.test(entry.pack.sha256 || "")) return null;
  const archivePath = resolve(output, entry.pack.url);
  let archiveBytes;
  try { archiveBytes = await readFile(archivePath); } catch { return null; }
  const digest = createHash("sha256").update(archiveBytes).digest("hex");
  if (archiveBytes.length !== entry.pack.bytes || digest !== String(entry.pack.sha256).toLowerCase()) return null;
  return { entry, archiveBytes };
}

const inputFingerprint = await localInputFingerprint();
const cached = await tryReuseCache(inputFingerprint);
if (cached) {
  console.log(JSON.stringify({
    output,
    language,
    cached: true,
    files: cached.entry.pack.files ?? null,
    bytes: cached.archiveBytes.length,
    sha256: cached.entry.pack.sha256,
    pack: cached.entry.pack.url,
  }));
  process.exit(0);
}

const thdat = required("thdat");
const thmsg = required("thmsg");
const client = createThcrapClient({ repository });
const runner = new ThtkRunner({ thdat, thmsg });
const compiler = new ThcrapRuntimeCompiler({ runner, archives: { [game]: archives } });

const pack = await client.resolveLanguage(language, game);
const downloaded = await downloadThcrapPack(pack, {
  onProgress: ({ completed, total, path }) => process.stderr.write(`\r${completed}/${total} ${path}                    `)
});
const resources = await compiler.processPack(downloaded.resources);

function collectStrings(value, output) {
  if (typeof value === "string") {
    for (const character of value.normalize("NFC")) {
      const codepoint = character.codePointAt(0);
      if (codepoint >= 0x20 && !(codepoint >= 0x7f && codepoint <= 0x9f)) output.add(character);
    }
  } else if (Array.isArray(value)) {
    for (const item of value) collectStrings(item, output);
  } else if (value && typeof value === "object") {
    for (const item of Object.values(value)) collectStrings(item, output);
  }
}

async function applyFontOverride(input) {
  if (!fontFile) return input;
  const characters = new Set(Array.from({ length: 95 }, (_, index) => String.fromCodePoint(0x20 + index)));
  for (const resource of downloaded.resources) {
    if (resource.kind !== "jdiff" && resource.kind !== "table") continue;
    try { collectStrings(JSON.parse(new TextDecoder("utf-8", { fatal: true }).decode(resource.bytes)), characters); } catch {}
  }
  const extension = extname(fontFile).toLowerCase();
  if (!new Set([".otf", ".ttf"]).has(extension)) throw new Error(`unsupported font format: ${fontFile}`);
  const outputName = `${basename(fontFile, extension).replace(/[^a-zA-Z0-9._-]+/g, "-")}-subset${extension}`;
  const targetPath = `/thcrap/${game}/fonts/${outputName}`;
  const temporary = await mkdtemp(join(tmpdir(), "eagler-thcrap-font-"));
  try {
    const characterFile = join(temporary, "characters.txt");
    const outputFile = join(temporary, outputName);
    await writeFile(characterFile, [...characters].sort().join(""), "utf8");
    const subset = await promisify(execFile)(fontPython, [
      resolve(project, "scripts/subset-font.py"), fontFile,
      `--text-file=${characterFile}`,
      `--output-file=${outputFile}`
    ], { maxBuffer: 8 * 1024 * 1024 });
    const filtered = input.filter(resource => !/^\/thcrap\/[^/]+\/fonts\//i.test(resource.targetPath || ""));
    let optionsFound = false;
    for (const resource of filtered) {
      if (resource.targetPath !== `/thcrap/${game}/localization/options.json`) continue;
      const options = JSON.parse(Buffer.from(resource.bytes).toString("utf8"));
      options.font = fontName;
      options.fontFile = outputName;
      resource.bytes = Buffer.from(`${JSON.stringify(options)}\n`);
      optionsFound = true;
    }
    if (!optionsFound) throw new Error(`${game}: localization options were not generated`);
    filtered.push({
      path: outputName,
      mountPath: targetPath,
      targetPath,
      bytes: await readFile(outputFile),
      extension,
      format: extension === ".otf" ? "font/otf" : "font/ttf"
    });
    const audit = JSON.parse(subset.stdout);
    process.stderr.write(`\nfont subset: ${audit.included}/${audit.requested} characters -> ${outputName}; fallback: ${audit.fallback.join(",") || "none"}\n`);
    return filtered;
  } finally {
    await rm(temporary, { recursive: true, force: true });
  }
}

const preparedResources = await applyFontOverride(resources);
const result = createStaticThcrapPack({ pack, resources: preparedResources });
const preparedRuntimeVersion = result.manifest.runtimeVersion;
const packDirectory = resolve(output, "language");
await mkdir(packDirectory, { recursive: true });
await writeFile(resolve(packDirectory, result.fileName), result.archive);
await writeFile(resolve(packDirectory, `${language}.manifest.json`), `${JSON.stringify(result.manifest, null, 2)}\n`);
let catalog = { schema: "eagler-touhou/thcrap-static-catalog/1", game, runtimeVersion: preparedRuntimeVersion, languages: [] };
try { catalog = JSON.parse(await readFile(resolve(output, "catalog.json"), "utf8")); } catch {}
if (catalog.schema !== "eagler-touhou/thcrap-static-catalog/1" || catalog.game !== game ||
    (catalog.runtimeVersion && !["auto", "pending", "independent"].includes(String(catalog.runtimeVersion).toLowerCase()) &&
      !/^[a-f0-9]{16,64}$/i.test(catalog.runtimeVersion)) ||
    !Array.isArray(catalog.languages)) {
  throw new Error(`invalid existing ${game.toUpperCase()} language catalog: ${output}`);
}
catalog.runtimeVersion = preparedRuntimeVersion;
catalog.languages = catalog.languages.filter(item => item?.id !== result.catalog.id);
catalog.languages.push({ ...result.catalog, pack: { ...result.catalog.pack } });
catalog.languages.sort((a, b) => a.id.localeCompare(b.id));
await writeFile(resolve(output, "catalog.json"), `${JSON.stringify({
  schema: "eagler-touhou/thcrap-static-catalog/1",
  game,
  runtimeVersion: preparedRuntimeVersion,
  languages: catalog.languages
}, null, 2)}\n`);
await writeFile(resolve(output, `${language}.cache.json`), `${JSON.stringify({
  schema: "eagler-touhou/language-pack-cache/1",
  fingerprint: inputFingerprint,
  pack: result.catalog.pack.url,
  sha256: result.sha256,
  bytes: result.archive.length,
}, null, 2)}\n`);
process.stderr.write("\n");
console.log(JSON.stringify({ output, language, cached: false, files: result.manifest.files.length, bytes: result.archive.length, sha256: result.sha256, pack: result.catalog.pack.url }));
