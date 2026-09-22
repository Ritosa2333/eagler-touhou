/** One program generation, independent of Launcher and Package Store identities. */
export const RUNTIME_MANIFEST_SCHEMA = "eagler-touhou/runtime-manifest/1";
export const RUNTIME_GENERATION_SCHEMA = "eagler-touhou/runtime-generation/1";
export const RUNTIME_MANIFEST_FILE = "runtime-manifest.json";
export const RUNTIME_GENERATION_FILE = "runtime-generation.json";
export const RUNTIME_PROTOCOL = "eagler-touhou/1";
export const RUNTIME_PREPARE = "PREPARE_RUNTIME_GENERATION";
export const RUNTIME_CAPABILITIES = "GET_RUNTIME_GENERATION_CAPABILITIES";
export const RUNTIME_CACHE_PROTOCOL = "eagler-touhou/runtime-cache/2";
export const RUNTIME_CACHE_MAX_PREVIOUS = 2;
export interface RuntimeFile { path: string; bytes: number; sha256: string; }
export interface RuntimeGeneration { generation: string; entry: string; files: RuntimeFile[]; }
export interface RuntimeGroup { root: string; current: RuntimeGeneration; previous: RuntimeGeneration[]; }
export interface RuntimeManifest {
  schema: typeof RUNTIME_MANIFEST_SCHEMA;
  protocol: typeof RUNTIME_PROTOCOL;
  groups: RuntimeGroup[];
}
const SHA256 = /^[a-f0-9]{64}$/;
const SEGMENT = /^[A-Za-z0-9][A-Za-z0-9._-]*$/;
export function isRuntimePath(value: unknown): value is string {
  return typeof value === "string" && value.length <= 1024 && value.split("/")
    .every(part => SEGMENT.test(part) && part !== "." && part !== "..");
}
export function isRuntimeRoot(value: unknown): value is string {
  return typeof value === "string" && value.startsWith("runtime/") && value.endsWith("/") &&
    isRuntimePath(value.slice(0, -1)) && value.split("/").length >= 3 &&
    !value.split("/").some(part => SHA256.test(part));
}
function record(value: unknown): value is Record<string, unknown> {
  return !!value && typeof value === "object" && !Array.isArray(value);
}
export function canonicalRuntimePayload(entry: string, files: readonly RuntimeFile[]): string {
  if (!isRuntimePath(entry) || !entry.endsWith(".html") || !Array.isArray(files) ||
      !files.length || files.length > 2048) throw new Error("Invalid Runtime generation file set");
  const seen = new Set<string>();
  for (const file of files) {
    if (!record(file) || !isRuntimePath(file.path) || file.path === RUNTIME_GENERATION_FILE ||
        seen.has(file.path) || typeof file.bytes !== "number" || !Number.isSafeInteger(file.bytes) || file.bytes <= 0 ||
        typeof file.sha256 !== "string" || !SHA256.test(file.sha256)) {
      throw new Error("Invalid Runtime file identity");
    }
    seen.add(file.path);
  }
  if (!seen.has(entry)) throw new Error("Runtime entry is not in its file set");
  // Code-point order, not locale-dependent sort. Hash final file bytes, never a
  // timestamp, source commit, parent site release, game data, or the hash itself.
  const ordered = [...files].sort((a, b) => a.path < b.path ? -1 : a.path > b.path ? 1 : 0);
  return JSON.stringify([RUNTIME_GENERATION_SCHEMA, entry,
    ordered.map(file => [file.path, file.bytes, file.sha256])]);
}
export function validateRuntimeGeneration(value: unknown): RuntimeGeneration {
  if (!record(value) || typeof value.generation !== "string" || !SHA256.test(value.generation) ||
      typeof value.entry !== "string" || !Array.isArray(value.files)) throw new Error("Invalid Runtime generation");
  canonicalRuntimePayload(value.entry, value.files as RuntimeFile[]);
  return { generation: value.generation, entry: value.entry,
    files: (value.files as RuntimeFile[]).map(({ path, bytes, sha256 }) => ({ path, bytes, sha256 })) };
}
export function validateRuntimeManifest(value: unknown): RuntimeManifest {
  if (!record(value) || value.schema !== RUNTIME_MANIFEST_SCHEMA || value.protocol !== RUNTIME_PROTOCOL ||
      !Array.isArray(value.groups) || value.groups.length > 64) throw new Error("Unsupported Runtime Manifest");
  const roots = new Set<string>();
  const groups = value.groups.map(raw => {
    if (!record(raw) || !isRuntimeRoot(raw.root) || roots.has(raw.root) || !Array.isArray(raw.previous) ||
        raw.previous.length > RUNTIME_CACHE_MAX_PREVIOUS) throw new Error("Invalid Runtime group");
    roots.add(raw.root);
    const current = validateRuntimeGeneration(raw.current);
    const previous = raw.previous.map(validateRuntimeGeneration);
    const ids = new Set([current.generation]);
    for (const item of previous) {
      if (ids.has(item.generation) || item.entry !== current.entry) throw new Error("Invalid Runtime rollback set");
      ids.add(item.generation);
    }
    return { root: raw.root, current, previous };
  });
  return { schema: RUNTIME_MANIFEST_SCHEMA, protocol: RUNTIME_PROTOCOL, groups };
}
export function runtimeGenerationBase(root: string, generation: string): string {
  if (!isRuntimeRoot(root) || !SHA256.test(generation)) throw new Error("Invalid immutable Runtime base");
  return `${root}${generation}/`;
}
export function runtimeGenerationEntry(root: string, generation: RuntimeGeneration): string {
  return runtimeGenerationBase(root, generation.generation) + generation.entry;
}
export function parseRuntimeGenerationPath(path: string): { root: string; generation: string; file: string } | null {
  if (!isRuntimePath(path) || !path.startsWith("runtime/")) return null;
  const parts = path.split("/");
  const index = parts.findIndex(part => SHA256.test(part));
  if (index < 2 || index === parts.length - 1) return null;
  const root = parts.slice(0, index).join("/") + "/";
  const generation = parts[index]!;
  const file = parts.slice(index + 1).join("/");
  return isRuntimeRoot(root) ? { root, generation, file } : null;
}
export function findRuntimeGroup(manifest: RuntimeManifest, path: string): RuntimeGroup | undefined {
  const immutable = parseRuntimeGenerationPath(path);
  return immutable ? manifest.groups.find(group => group.root === immutable.root)
    : manifest.groups.find(group => path === group.root + group.current.entry);
}

export interface RuntimePrepareRequest {
  type: typeof RUNTIME_PREPARE;
  entry: string;
  catalog?: RuntimeManifest;
  exclude: string[];
}
export function validateRuntimePrepareRequest(value: unknown): RuntimePrepareRequest {
  if (!record(value) || value.type !== RUNTIME_PREPARE || !isRuntimePath(value.entry) ||
      !value.entry.startsWith("runtime/") || !value.entry.endsWith(".html")) throw new Error("Invalid Runtime preparation entry");
  const exclude = value.exclude ?? [];
  if (!Array.isArray(exclude) || exclude.length > RUNTIME_CACHE_MAX_PREVIOUS + 1 ||
      exclude.some(id => typeof id !== "string" || !SHA256.test(id)) || new Set(exclude).size !== exclude.length) {
    throw new Error("Invalid Runtime preparation exclusions");
  }
  return { type: RUNTIME_PREPARE, entry: value.entry, exclude: [...exclude] as string[],
    ...(value.catalog === undefined ? {} : { catalog: validateRuntimeManifest(value.catalog) }) };
}
