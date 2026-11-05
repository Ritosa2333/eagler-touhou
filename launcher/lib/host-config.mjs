import { readFile } from "node:fs/promises";

export const HOST_CONFIG_SCHEMA = "eagler-touhou/host-config/1";

export const DEFAULT_HOST_CONFIG = Object.freeze({
  schema: HOST_CONFIG_SCHEMA,
  netplay: Object.freeze({ relay: "" }),
  externalImportSource: Object.freeze({ url: "", hint: "" }),
});

function text(value) {
  return typeof value === "string" ? value.trim() : "";
}

export function validateHostConfig(value, source = "host config") {
  if (!value || typeof value !== "object" || value.schema !== HOST_CONFIG_SCHEMA) {
    throw new Error(`invalid ${source}: expected schema ${HOST_CONFIG_SCHEMA}`);
  }
  const relay = text(value.netplay?.relay);
  if (relay) {
    let url;
    try { url = new URL(relay); } catch { throw new Error(`invalid ${source}: netplay.relay must be a ws:// or wss:// URL`); }
    if (!/^wss?:$/.test(url.protocol)) throw new Error(`invalid ${source}: netplay.relay must be a ws:// or wss:// URL`);
  }
  const externalUrl = text(value.externalImportSource?.url);
  const externalHint = text(value.externalImportSource?.hint);
  if (externalUrl) {
    let url;
    try { url = new URL(externalUrl); } catch { throw new Error(`invalid ${source}: externalImportSource.url must be an https:// URL`); }
    if (url.protocol !== "https:") throw new Error(`invalid ${source}: externalImportSource.url must be an https:// URL`);
  }
  return Object.freeze({
    schema: HOST_CONFIG_SCHEMA,
    netplay: Object.freeze({ relay }),
    externalImportSource: Object.freeze({ url: externalUrl, hint: externalHint }),
  });
}

export async function readHostConfig(path, { allowMissing = true } = {}) {
  let raw;
  try {
    raw = await readFile(path, "utf8");
  } catch (error) {
    if (allowMissing && error?.code === "ENOENT") {
      return Object.freeze({ ...DEFAULT_HOST_CONFIG, present: false });
    }
    throw error;
  }
  return Object.freeze({ ...validateHostConfig(JSON.parse(raw), path), present: true });
}

export function hostConfigWarnings(config) {
  const warnings = [];
  if (!config.netplay.relay) {
    warnings.push("WebSocket relay is not configured; multiplayer will be disabled.");
  } else {
    warnings.push("TURN relay availability is server-managed and was not verified by this local check.");
  }
  if (!config.externalImportSource.url) {
    warnings.push("External import source is not configured; no administrator-provided package download link will be shown.");
  }
  return Object.freeze(warnings);
}
