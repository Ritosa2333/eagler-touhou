function normalizeUserPath(path) {
  const text = String(path || "").replace(/\\/g, "/").replace(/^\/+/, "");
  if (text === "Score.dat" || text === "Setting.INI" || text === "Log.txt") return text;
  const replay = text.match(/^(?:replay|Replay)\/([^/]+)$/);
  if (!replay) return null;
  if (
    /^thSSS_(?:\d{2}|ud[0-9a-f]{4})\.(?:rpy|trpy)$/i.test(replay[1])
    || /^AutoSave\.(?:rpy|trpy)$/i.test(replay[1])
  ) {
    return "Replay/" + replay[1];
  }
  return null;
}

function listedPath(stored) {
  const replay = String(stored).match(/^Replay\/([^/]+)$/);
  return replay ? "replay/" + replay[1] : stored;
}

function decodedSize(value) {
  if (typeof value !== "string" || !value) return 0;
  const pad = value.endsWith("==") ? 2 : value.endsWith("=") ? 1 : 0;
  return Math.max(0, Math.floor((value.length * 3) / 4) - pad);
}

function base64ToBytes(value) {
  const binary = atob(value || "");
  const out = new Uint8Array(binary.length);
  for (let i = 0; i < binary.length; i++) out[i] = binary.charCodeAt(i);
  return out;
}

function bytesOf(value) {
  if (value instanceof Uint8Array) return value;
  if (Array.isArray(value)) return Uint8Array.from(value);
  return null;
}

function toByteArray(bytes) {
  const out = new Array(bytes.length);
  for (let i = 0; i < bytes.length; i++) out[i] = bytes[i];
  return out;
}

export function createFileBridge({ getRuntime, reply }) {
  return {
    async handle(message) {
      const runtime = getRuntime();
      const saves = runtime && runtime.saves;
      if (!saves) {
        reply(message, false, { error: "Storage is not ready" });
        return;
      }
      if (message.command === "list") {
        const files = [];
        for (const stored of Object.keys(saves)) {
          const canonical = normalizeUserPath(stored);
          if (!canonical) continue;
          files.push({ path: listedPath(canonical), size: decodedSize(saves[stored]) });
        }
        reply(message, true, { files });
        return;
      }
      if (message.command === "sync") {
        if (runtime.flushSaves) await runtime.flushSaves();
        reply(message, true, {});
        return;
      }
      const path = normalizeUserPath(message.path);
      if (!path) {
        reply(message, false, { error: "Path is not a user file", errno: 1 });
        return;
      }
      if (message.command === "read") {
        const value = saves[path];
        if (typeof value !== "string") {
          reply(message, false, { error: "Missing file", errno: 44 });
          return;
        }
        reply(message, true, { bytes: toByteArray(base64ToBytes(value)) });
        return;
      }
      if (message.command === "write") {
        const bytes = bytesOf(message.bytes);
        if (!bytes || !runtime.writeUserFile) {
          reply(message, false, { error: "Missing bytes" });
          return;
        }
        await runtime.writeUserFile(path, bytes);
        reply(message, true, {});
        return;
      }
      if (message.command === "remove") {
        if (runtime.removeUserFile) await runtime.removeUserFile(path);
        else delete saves[path];
        reply(message, true, {});
        return;
      }
      reply(message, false, { error: "Unsupported file command" });
    },
  };
}
