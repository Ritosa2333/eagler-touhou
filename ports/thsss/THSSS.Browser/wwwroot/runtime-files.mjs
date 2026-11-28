const files = new Map();

function keyOf(path) {
  const text = String(path || "").replace(/\\/g, "/");
  return text.startsWith("/") ? text : "/" + text;
}

export function putRuntimeFile(path, data) {
  const bytes = data instanceof Uint8Array ? data : new Uint8Array(data || []);
  const copy = new Uint8Array(bytes.byteLength);
  copy.set(bytes);
  files.set(keyOf(path), copy);
  if (typeof globalThis !== "undefined") globalThis.__thsssFiles = files;
  return copy;
}

export function readRuntimeFile(path) {
  return files.get(keyOf(path)) || null;
}

export async function installRemoteResources(lists, fetchImpl) {
  const fetchFn = fetchImpl || fetch;
  let count = 0;
  const groups = Array.isArray(lists) ? lists : [];
  for (const list of groups) {
    if (!Array.isArray(list)) continue;
    for (const item of list) {
      if (!item || typeof item.url !== "string" || typeof item.path !== "string" || !item.path.startsWith("/")) continue;
      const path = keyOf(item.path);
      if (path.includes("..")) throw new Error("Resource path is not allowed");
      const response = await fetchFn(item.url);
      if (!response.ok) throw new Error("Missing resource " + path);
      putRuntimeFile(path, new Uint8Array(await response.arrayBuffer()));
      count++;
    }
  }
  return count;
}

export function installRuntimeFileSystem(target) {
  const fs = {
    mkdirTree() {},
    writeFile(path, data) {
      putRuntimeFile(path, data);
    },
  };
  target.FS = fs;
  target.Module = target.Module || {};
  target.Module.FS = fs;
  if (!target.Module.touhouMusicMode) target.Module.touhouMusicMode = "none";
  target.__thsssFiles = files;
}
