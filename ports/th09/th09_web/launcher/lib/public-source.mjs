import { existsSync } from "node:fs";
import { resolve, sep } from "node:path";
import { fileURLToPath } from "node:url";

const project = resolve(fileURLToPath(new URL("..", import.meta.url)));

export const PUBLIC_SOURCE_ROOT = resolve(project, "public");

function normalizePublicPath(value) {
  const path = String(value || "").replaceAll("\\", "/").replace(/^\.\//, "");
  if (!path || path.startsWith("/") || path.split("/").some(part => !part || part === "." || part === "..")) {
    throw new Error(`invalid public source path: ${value}`);
  }
  return path;
}

export function resolvePublicSource(value) {
  const path = normalizePublicPath(value);
  const source = resolve(PUBLIC_SOURCE_ROOT, path);
  if (source === PUBLIC_SOURCE_ROOT || !source.startsWith(PUBLIC_SOURCE_ROOT + sep)) {
    throw new Error(`public source path escapes its root: ${value}`);
  }
  return source;
}

export function resolvePublicOrRepositorySource(value) {
  const path = normalizePublicPath(value);
  const publicSource = resolvePublicSource(path);
  return existsSync(publicSource) ? publicSource : resolve(project, path);
}
