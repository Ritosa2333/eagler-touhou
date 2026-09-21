import { createHash, randomUUID } from "node:crypto";
import { copyFile, lstat, mkdir, readFile, readdir, rename, rm, writeFile } from "node:fs/promises";
import { dirname, resolve, posix } from "node:path";
import { parse } from "acorn";
import {
  RUNTIME_GENERATION_FILE, RUNTIME_GENERATION_SCHEMA, RUNTIME_MANIFEST_FILE,
  RUNTIME_MANIFEST_SCHEMA, RUNTIME_PROTOCOL, RUNTIME_CACHE_MAX_PREVIOUS,
  canonicalRuntimePayload, isRuntimePath, isRuntimeRoot, runtimeGenerationBase,
  runtimeGenerationEntry, parseRuntimeGenerationPath, validateRuntimeGeneration,
  validateRuntimeManifest,
} from "./contracts/runtime-generations.mjs";
import { PRODUCT_GAMES } from "./contracts/product-catalog.mjs";

const hash = bytes => createHash("sha256").update(bytes).digest("hex");
const json = value => `${JSON.stringify(value, null, 2)}\n`;
export const runtimeGenerationId = (entry, files) => hash(canonicalRuntimePayload(entry, files));

async function ordinaryFile(path) {
  const info = await lstat(path);
  if (!info.isFile()) throw new Error(`Runtime source must be an ordinary file: ${path}`);
  return readFile(path);
}

/** Reject statically resolvable executable dependencies outside the frozen set.
 * Dynamic loaders still require browser conformance tests; this is not an
 * attempt to statically prove arbitrary JavaScript correct. */
export function assertPortableRuntimeSources(files) {
  const names = new Set(files.keys());
  const base = new URL("https://runtime.invalid/frozen/");
  function reference(from, specifier, required = false) {
    if (typeof specifier !== "string" || (!required && !/\.(?:[cm]?js|wasm|bin|json)(?:[?#]|$)/i.test(specifier))) return;
    if (/^(?:data|blob):/i.test(specifier)) return;
    const url = new URL(specifier, new URL(from, base));
    if (url.origin !== base.origin || !url.pathname.startsWith(base.pathname)) {
      throw new Error(`Runtime executable reference escapes its generation: ${from} -> ${specifier}`);
    }
    const path = decodeURIComponent(url.pathname.slice(base.pathname.length));
    if (!names.has(path)) throw new Error(`Runtime dependency is not declared: ${from} -> ${specifier}`);
  }
  function script(from, source) {
    const ast = parse(source, { ecmaVersion: "latest", sourceType: "module", allowHashBang: true });
    function visit(node) {
      if (!node || typeof node !== "object") return;
      if (["ImportDeclaration", "ExportAllDeclaration", "ExportNamedDeclaration"].includes(node.type) && node.source) {
        reference(from, node.source.value, true);
      } else if (node.type === "ImportExpression" && node.source?.type === "Literal") {
        reference(from, node.source.value, true);
      } else if (node.type === "NewExpression" && node.callee?.type === "Identifier" &&
          ["URL", "Worker", "SharedWorker"].includes(node.callee.name) && node.arguments?.[0]?.type === "Literal") {
        reference(from, node.arguments[0].value, node.callee.name !== "URL");
      } else if (node.type === "CallExpression" && node.callee?.type === "Identifier" &&
          ["fetch", "importScripts"].includes(node.callee.name)) {
        for (const arg of node.arguments) if (arg.type === "Literal") reference(from, arg.value, node.callee.name === "importScripts");
      }
      for (const [key, child] of Object.entries(node)) {
        if (["start", "end", "loc"].includes(key)) continue;
        if (Array.isArray(child)) child.forEach(visit);
        else if (child && typeof child === "object") visit(child);
      }
    }
    visit(ast);
  }
  for (const [name, bytes] of files) {
    if (/\.[cm]?js$/i.test(name)) script(name, bytes.toString("utf8"));
    if (!/\.html$/i.test(name)) continue;
    const html = bytes.toString("utf8");
    if (/<base\b/i.test(html)) throw new Error(`Runtime HTML must not replace its generation base: ${name}`);
    for (const match of html.matchAll(/<script\b([^>]*)>([\s\S]*?)<\/script\s*>/gi)) {
      const src = match[1].match(/\bsrc\s*=\s*(?:"([^"]+)"|'([^']+)'|([^\s>]+))/i);
      if (src) reference(name, src[1] || src[2] || src[3], true);
      else if (!/\btype\s*=\s*["']?(?:application\/json|importmap)/i.test(match[1])) script(name, match[2]);
    }
  }
}

export async function verifyRuntimeGenerationDirectory(site, root, raw) {
  if (!isRuntimeRoot(root)) throw new Error("Invalid Runtime root");
  const generation = validateRuntimeGeneration(raw);
  if (runtimeGenerationId(generation.entry, generation.files) !== generation.generation) {
    throw new Error(`Runtime generation identity mismatch: ${root}`);
  }
  const directory = resolve(site, runtimeGenerationBase(root, generation.generation));
  const sources = new Map();
  for (const file of generation.files) {
    const bytes = await ordinaryFile(resolve(directory, file.path));
    if (bytes.length !== file.bytes || hash(bytes) !== file.sha256) throw new Error(`Runtime generation file mismatch: ${root}${file.path}`);
    sources.set(file.path, bytes);
  }
  const sidecar = JSON.parse(await ordinaryFile(resolve(directory, RUNTIME_GENERATION_FILE)));
  if (sidecar.schema !== RUNTIME_GENERATION_SCHEMA || sidecar.protocol !== RUNTIME_PROTOCOL || sidecar.root !== root ||
      JSON.stringify(validateRuntimeGeneration(sidecar)) !== JSON.stringify(generation)) {
    throw new Error(`Runtime generation descriptor mismatch: ${root}`);
  }
  const expected = new Set([...sources.keys(), RUNTIME_GENERATION_FILE]);
  async function walk(path = "") {
    for (const item of await readdir(resolve(directory, path), { withFileTypes: true })) {
      const name = path + item.name;
      if (item.isDirectory()) await walk(name + "/");
      else if (!item.isFile() || !expected.delete(name)) throw new Error(`Unexpected Runtime generation file: ${name}`);
    }
  }
  await walk();
  if (expected.size) throw new Error(`Missing Runtime generation files: ${[...expected]}`);
  assertPortableRuntimeSources(sources);
  return generation;
}

/** Materialize final bytes once. A same-ID directory is verified, never overwritten. */
export async function writeRuntimeGeneration({ site, root, entry, source, names, expected }) {
  if (!isRuntimeRoot(root) || !isRuntimePath(entry) || !Array.isArray(names)) throw new Error("Invalid Runtime generation input");
  const sources = new Map();
  let total = 0;
  for (const path of names) {
    if (!isRuntimePath(path) || sources.has(path) || path === RUNTIME_GENERATION_FILE) throw new Error(`Invalid Runtime input: ${path}`);
    const bytes = await ordinaryFile(resolve(source, path));
    total += bytes.length;
    if (total > 512 * 1024 * 1024) throw new Error("Runtime code set exceeds 512 MiB; game data must stay separate");
    if (expected && (expected[path]?.bytes !== bytes.length || expected[path]?.sha256 !== hash(bytes))) {
      throw new Error(`Runtime source changed after its manifest was written: ${path}`);
    }
    sources.set(path, bytes);
  }
  assertPortableRuntimeSources(sources);
  const files = [...sources].map(([path, bytes]) => ({ path, bytes: bytes.length, sha256: hash(bytes) }))
    .sort((a, b) => a.path < b.path ? -1 : a.path > b.path ? 1 : 0);
  const generation = { generation: runtimeGenerationId(entry, files), entry, files };
  const target = resolve(site, runtimeGenerationBase(root, generation.generation));
  let exists = false;
  try { await lstat(target); exists = true; }
  catch (error) { if (error.code !== "ENOENT") throw error; }
  if (exists) {
    await verifyRuntimeGenerationDirectory(site, root, generation);
    return generation;
  }
  const temporary = resolve(site, ".tmp", `runtime-${randomUUID()}`);
  await mkdir(temporary, { recursive: true });
  try {
    for (const [path, bytes] of sources) {
      await mkdir(dirname(resolve(temporary, path)), { recursive: true });
      await writeFile(resolve(temporary, path), bytes, { flag: "wx" });
    }
    await writeFile(resolve(temporary, RUNTIME_GENERATION_FILE), json({
      schema: RUNTIME_GENERATION_SCHEMA, protocol: RUNTIME_PROTOCOL, root, ...generation,
    }));
    await mkdir(dirname(target), { recursive: true });
    await rename(temporary, target);
  } finally { await rm(temporary, { recursive: true, force: true }); }
  return generation;
}

export async function readRuntimeManifest(site, { optional = false } = {}) {
  try { return validateRuntimeManifest(JSON.parse(await readFile(resolve(site, RUNTIME_MANIFEST_FILE), "utf8"))); }
  catch (error) { if (optional && error.code === "ENOENT") return null; throw error; }
}

async function preserveArchivedGenerations(site, previousSite, root) {
  if (!previousSite || resolve(previousSite) === resolve(site)) return;
  let entries;
  try { entries = await readdir(resolve(previousSite, root), { withFileTypes: true }); }
  catch (error) { if (error.code === "ENOENT") return; throw error; }
  if (entries.length > 10000) throw new Error(`Too many retained Runtime generations: ${root}`);
  for (const item of entries) {
    if (!/^[a-f0-9]{64}$/.test(item.name)) continue;
    if (!item.isDirectory()) throw new Error(`Runtime archive is not a directory: ${root}${item.name}`);
    const source = resolve(previousSite, root, item.name);
    const raw = JSON.parse(await readFile(resolve(source, RUNTIME_GENERATION_FILE), "utf8"));
    const generation = await verifyRuntimeGenerationDirectory(previousSite, root, raw);
    if (generation.generation !== item.name) throw new Error(`Runtime archive directory identity mismatch: ${root}`);
    await writeRuntimeGeneration({ site, root, source, entry: generation.entry,
      names: generation.files.map(file => file.path), expected: Object.fromEntries(generation.files.map(file => [file.path, file])) });
  }
}

export async function publishRuntimeManifest(site, inputs, { previousSite = site } = {}) {
  const old = await readRuntimeManifest(previousSite, { optional: true });
  const groups = [];
  const currentRoots = new Set(inputs.map(group => group.root));
  // Removing a game from the current catalog must not strand an older page
  // already running it. Retain its immutable server URLs without advertising it.
  for (const former of old?.groups || []) if (!currentRoots.has(former.root)) {
    await preserveArchivedGenerations(site, previousSite, former.root);
  }
  for (const { root, current, previous: suppliedPrevious = [] } of inputs) {
    await verifyRuntimeGenerationDirectory(site, root, current);
    await preserveArchivedGenerations(site, previousSite, root);
    const former = old?.groups.find(group => group.root === root);
    const seen = new Set([current.generation]);
    const previous = [];
    for (const candidate of [...suppliedPrevious, ...(former ? [former.current, ...former.previous] : [])]) {
      if (seen.has(candidate.generation)) continue;
      seen.add(candidate.generation);
      await verifyRuntimeGenerationDirectory(site, root, candidate);
      if (previous.length < RUNTIME_CACHE_MAX_PREVIOUS) previous.push(candidate);
    }
    groups.push({ root, current, previous });
  }
  const manifest = validateRuntimeManifest({ schema: RUNTIME_MANIFEST_SCHEMA, protocol: RUNTIME_PROTOCOL, groups });
  // Compatibility navigation only, never a second mutable JS/Wasm publication.
  for (const group of groups) {
    const target = resolve(site, group.root, group.current.entry);
    const relative = posix.relative(posix.dirname(group.current.entry), `${group.current.generation}/${group.current.entry}`);
    await mkdir(dirname(target), { recursive: true });
    await writeFile(target, `<!doctype html><meta charset="utf-8"><title>Opening Runtime</title>\n<script>location.replace(${JSON.stringify(relative)} + location.search + location.hash);</script>\n`);
  }
  const temporary = resolve(site, ".tmp", `runtime-manifest-${randomUUID()}.json`);
  await mkdir(dirname(temporary), { recursive: true });
  await writeFile(temporary, json(manifest));
  // The stable pointer changes only after every referenced generation exists.
  await rename(temporary, resolve(site, RUNTIME_MANIFEST_FILE));
  return manifest;
}

export async function freezeHostRuntimes(site, manifest, { previousSite = site } = {}) {
  const { runtimeFileNames } = await import("./runtime-release.mjs");
  const groups = [];
  for (const [game, host] of Object.entries(manifest.games)) {
    for (const field of ["runtime", "multiplayerRuntime"]) {
      if (!host[field]) continue;
      const url = new URL(host[field], "https://runtime.invalid/");
      if (url.origin !== "https://runtime.invalid") throw new Error("Published Runtime must be same-origin");
      const path = url.pathname.slice(1), parsed = parseRuntimeGenerationPath(path);
      let root, current;
      if (parsed) {
        root = parsed.root;
        current = await verifyRuntimeGenerationDirectory(site, root,
          JSON.parse(await readFile(resolve(site, root, parsed.generation, RUNTIME_GENERATION_FILE), "utf8")));
      } else {
        root = path.slice(0, path.lastIndexOf("/") + 1);
        const source = resolve(site, root);
        const directory = PRODUCT_GAMES[game]?.runtimeFileLayout === "directory";
        const expected = directory ? JSON.parse(await readFile(resolve(source, "runtime-files.json"), "utf8")).files : null;
        const names = directory ? runtimeFileNames(game, expected) : ["html", "js", "wasm"].map(extension => `${game}.${extension}`);
        current = await writeRuntimeGeneration({ site, root, source, entry: `${game}.html`, names, expected });
        // These are exact build-owned loose files, not other generations or data.
        for (const name of names) await rm(resolve(source, name));
        if (directory) await rm(resolve(source, "runtime-files.json"));
      }
      url.pathname = "/" + runtimeGenerationEntry(root, current);
      url.searchParams.delete("v");
      host[field] = url.pathname.slice(1) + url.search;
      groups.push({ root, current });
    }
  }
  manifest.shared.runtimeManifest = RUNTIME_MANIFEST_FILE;
  return publishRuntimeManifest(site, groups, { previousSite });
}

export async function verifyRuntimePublication(site, host) {
  const manifest = await readRuntimeManifest(site);
  for (const group of manifest.groups) {
    for (const generation of [group.current, ...group.previous]) await verifyRuntimeGenerationDirectory(site, group.root, generation);
  }
  if (host) {
    if (host.shared.runtimeManifest !== RUNTIME_MANIFEST_FILE) throw new Error("Host Manifest is missing its Runtime Manifest pointer");
    const actual = new Set();
    for (const entry of Object.values(host.games)) for (const field of ["runtime", "multiplayerRuntime"]) {
      if (entry[field]) actual.add(new URL(entry[field], "https://runtime.invalid/").pathname.slice(1));
    }
    const expected = manifest.groups.map(group => runtimeGenerationEntry(group.root, group.current));
    if (actual.size !== expected.length || expected.some(path => !actual.has(path))) throw new Error("Host / Runtime Manifest entries disagree");
  }
  return manifest;
}
