import { putRuntimeFile } from "../../runtime-files.mjs";

function keyOf(path) {
  const text = String(path || "").replace(/\\/g, "/");
  return text.startsWith("/") ? text : "/" + text;
}

export function isMusicPath(path) {
  return /^\/BGM\//i.test(keyOf(path));
}

function isOggBytes(bytes) {
  return !!bytes && bytes.byteLength >= 4 &&
    bytes[0] === 0x4f && bytes[1] === 0x67 && bytes[2] === 0x67 && bytes[3] === 0x53;
}

function resourceItems(lists) {
  const items = [];
  const groups = Array.isArray(lists) ? lists : [];
  for (const list of groups) {
    if (!Array.isArray(list)) continue;
    for (const item of list) {
      if (!item || typeof item.url !== "string" || typeof item.path !== "string" || !item.path.startsWith("/")) continue;
      const path = keyOf(item.path);
      if (path.includes("..")) throw new Error("Resource path is not allowed");
      items.push({
        url: item.url,
        path,
        size: Number(item.size) || Number(item.bytes) || 0,
      });
    }
  }
  return items;
}

/** Soft-fail /BGM installs so missing or damaged tracks surface as music-error
 *  with retry-music, instead of failing the whole configure. */
export function createMusicTransfer({ emit, putFile = putRuntimeFile, fetchImpl } = {}) {
  const pending = new Map();
  const fetchFn = () => fetchImpl || fetch;

  async function pull(item) {
    const response = await fetchFn()(item.url);
    if (!response.ok) throw new Error("Missing resource " + item.path);
    const bytes = new Uint8Array(await response.arrayBuffer());
    if (!isOggBytes(bytes)) throw new Error("Damaged music resource " + item.path);
    putFile(item.path, bytes);
    if (typeof emit === "function") {
      emit("transfer", {
        mode: "ogg",
        loaded: bytes.byteLength,
        total: item.size > 0 ? item.size : bytes.byteLength,
        path: item.path,
      });
    }
    pending.delete(item.path);
    return bytes.byteLength;
  }

  function partition(lists) {
    const music = [];
    const other = [];
    for (const item of resourceItems(lists)) {
      if (isMusicPath(item.path)) music.push(item);
      else other.push(item);
    }
    return { music, other };
  }

  async function installOther(items) {
    let count = 0;
    for (const item of items) {
      const response = await fetchFn()(item.url);
      if (!response.ok) throw new Error("Missing resource " + item.path);
      putFile(item.path, new Uint8Array(await response.arrayBuffer()));
      count++;
    }
    return count;
  }

  async function installMusic(items) {
    let installed = 0;
    let failed = 0;
    for (const item of items) {
      try {
        await pull(item);
        installed++;
      } catch (err) {
        pending.set(item.path, { url: item.url, path: item.path, size: item.size });
        failed++;
      }
    }
    if (failed > 0 && typeof emit === "function") {
      emit(installed > 0 ? "music-incomplete" : "music-error", { failed });
    } else if (installed > 0 && pending.size === 0 && typeof emit === "function") {
      emit("music-complete", {
        mode: "ogg",
        loaded: installed,
        total: installed,
      });
    }
    return { installed, failed, pending: pending.size };
  }

  async function install(lists) {
    const { music, other } = partition(lists);
    const otherCount = await installOther(other);
    const musicResult = await installMusic(music);
    return { other: otherCount, ...musicResult };
  }

  async function retry() {
    const queue = [...pending.values()];
    if (!queue.length) return { installed: 0, failed: 0, pending: 0 };
    pending.clear();
    return installMusic(queue);
  }

  return {
    isMusicPath,
    pendingCount: () => pending.size,
    pendingPaths: () => [...pending.keys()],
    clear() {
      pending.clear();
    },
    install,
    retry,
  };
}
