import { createHash, randomUUID } from "node:crypto";
import { existsSync } from "node:fs";
import { mkdir, readFile, rename, rm, writeFile } from "node:fs/promises";
import { dirname, resolve } from "node:path";
import { commandExists } from "./process.mjs";

const VERSION = "12";
const ARCHIVE = "thtk-bin-12.zip";
const DOWNLOAD_URL = "https://github.com/thpatch/thtk/releases/download/12/thtk-bin-12.zip";
const SHA256 = "f6acc00f377b6537e8d504794aec8445cb1e0d6490d2c89d56b3315677765154";

function formatBytes(bytes) {
  if (bytes >= 1024 * 1024) return `${(bytes / (1024 * 1024)).toFixed(1)} MiB`;
  if (bytes >= 1024) return `${(bytes / 1024).toFixed(1)} KiB`;
  return `${bytes} B`;
}

function sleep(ms) {
  return new Promise(resolve => setTimeout(resolve, ms));
}

async function archiveHash(path) {
  try {
    return createHash("sha256").update(await readFile(path)).digest("hex");
  } catch (error) {
    if (error?.code === "ENOENT") return null;
    throw error;
  }
}

async function downloadArchive() {
  const response = await fetch(DOWNLOAD_URL, {
    redirect: "follow",
    headers: { "user-agent": "eagler-touhou-host/1" },
  });
  if (!response.ok) throw new Error(`HTTP ${response.status}`);
  const total = Number(response.headers.get("content-length")) || 0;
  if (!response.body) return Buffer.from(await response.arrayBuffer());

  const chunks = [];
  const reader = response.body.getReader();
  let downloaded = 0;
  while (true) {
    const { value, done } = await reader.read();
    if (done) break;
    const chunk = Buffer.from(value);
    chunks.push(chunk);
    downloaded += chunk.length;
    if (process.stdout.isTTY) {
      const detail = total
        ? `${formatBytes(downloaded)} / ${formatBytes(total)} (${Math.min(100, Math.floor(downloaded * 100 / total))}%)`
        : `${formatBytes(downloaded)} downloaded`;
      process.stdout.write(`\r[Build] Downloading thtk ${VERSION}: ${detail}`);
    }
  }
  if (process.stdout.isTTY) process.stdout.write("\n");
  console.log(`[Build] thtk download complete: ${formatBytes(downloaded)}`);
  return Buffer.concat(chunks);
}

async function downloadVerifiedArchive() {
  let lastError;
  for (let attempt = 1; attempt <= 3; attempt++) {
    try {
      const bytes = await downloadArchive();
      const hash = createHash("sha256").update(bytes).digest("hex");
      if (hash !== SHA256) throw new Error(`SHA-256 mismatch (expected ${SHA256})`);
      return bytes;
    } catch (error) {
      lastError = error;
      if (attempt >= 3) break;
      console.warn(`[Build] thtk download attempt ${attempt}/3 failed; retrying: ${error.message || error}`);
      await sleep(2000 * attempt);
    }
  }
  throw new Error(`Unable to prepare thtk ${VERSION} from official upstream: ${lastError?.message || lastError}`);
}

function safeZipPath(value) {
  const path = String(value).replaceAll("\\", "/");
  if (!path || path.startsWith("/") || /^[A-Za-z]:/.test(path) || path.split("/").some(part => part === "..")) {
    throw new Error(`unsafe thtk archive path: ${value}`);
  }
  return path;
}

async function extractArchive(bytes, destination) {
  const { unzipSync } = await import("fflate");
  const entries = unzipSync(new Uint8Array(bytes.buffer, bytes.byteOffset, bytes.byteLength));
  for (const [rawName, content] of Object.entries(entries)) {
    const name = safeZipPath(rawName);
    if (name.endsWith("/")) continue;
    const path = resolve(destination, name);
    await mkdir(dirname(path), { recursive: true });
    await writeFile(path, content);
  }
}

async function ensureWindowsThtk(hostRoot) {
  const cache = resolve(hostRoot, ".cache", "tools", "thtk", VERSION);
  const archive = resolve(cache, ARCHIVE);
  const extracted = resolve(cache, "extracted");
  const toolRoot = resolve(extracted, `thtk-bin-${VERSION}`);
  const tools = {
    thdat: resolve(toolRoot, "thdat.exe"),
    thmsg: resolve(toolRoot, "thmsg.exe"),
    thanm: resolve(toolRoot, "thanm.exe"),
  };
  if (Object.values(tools).every(existsSync)) return Object.freeze(tools);

  await mkdir(cache, { recursive: true });
  if (await archiveHash(archive) !== SHA256) {
    await rm(archive, { force: true });
    console.log(`[Build] Downloading thtk ${VERSION} from the official upstream release`);
    const bytes = await downloadVerifiedArchive();
    await writeFile(archive, bytes);
  }
  const bytes = await readFile(archive);
  const staging = `${extracted}.incomplete-${randomUUID()}`;
  try {
    await rm(staging, { recursive: true, force: true });
    await extractArchive(bytes, staging);
    const stagedToolRoot = resolve(staging, `thtk-bin-${VERSION}`);
    for (const name of ["thdat.exe", "thmsg.exe", "thanm.exe"]) {
      if (!existsSync(resolve(stagedToolRoot, name))) {
        throw new Error(`thtk ${VERSION} archive did not contain ${name}`);
      }
    }
    await rm(extracted, { recursive: true, force: true });
    await rename(staging, extracted);
  } finally {
    await rm(staging, { recursive: true, force: true });
  }
  return Object.freeze(tools);
}

export async function ensureThtk({ hostRoot }) {
  if (process.platform === "win32") return ensureWindowsThtk(hostRoot);
  for (const command of ["thdat", "thmsg"]) {
    if (!await commandExists(command)) {
      throw new Error(`thtk ${VERSION} is required on this platform; install thdat and thmsg and put them on PATH`);
    }
  }
  return Object.freeze({
    thdat: "thdat",
    thmsg: "thmsg",
    thanm: await commandExists("thanm") ? "thanm" : null,
  });
}
