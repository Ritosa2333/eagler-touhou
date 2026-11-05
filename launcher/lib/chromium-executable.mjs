import { access, readdir } from "node:fs/promises";
import { resolve } from "node:path";

async function exists(path) {
  try { await access(path); return true; } catch { return false; }
}

export async function findChromiumExecutable() {
  const explicit = process.env.EAGLER_CHROME_PATH;
  if (explicit) {
    if (!await exists(explicit)) throw new Error(`EAGLER_CHROME_PATH does not exist: ${explicit}`);
    return explicit;
  }

  const candidates = [];
  const localAppData = process.env.LOCALAPPDATA;
  if (localAppData) {
    const playwrightCache = resolve(localAppData, "ms-playwright");
    if (await exists(playwrightCache)) {
      const entries = await readdir(playwrightCache, { withFileTypes: true });
      for (const entry of entries.filter(item => item.isDirectory()).reverse()) {
        candidates.push(resolve(playwrightCache, entry.name, "chrome-win64", "chrome.exe"));
        candidates.push(resolve(playwrightCache, entry.name, "chrome-linux", "chrome"));
      }
    }
  }
  candidates.push(
    "C:/Program Files/Google/Chrome/Application/chrome.exe",
    "C:/Program Files (x86)/Google/Chrome/Application/chrome.exe",
    "C:/Program Files/Microsoft/Edge/Application/msedge.exe",
    "C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe",
    "/usr/bin/google-chrome",
    "/usr/bin/chromium",
    "/Applications/Google Chrome.app/Contents/MacOS/Google Chrome",
  );
  for (const candidate of candidates) if (await exists(candidate)) return candidate;
  throw new Error("Chrome/Chromium was not found; set EAGLER_CHROME_PATH");
}
