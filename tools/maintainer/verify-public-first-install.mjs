#!/usr/bin/env node
import puppeteer from "puppeteer-core";
import { findChromiumExecutable } from "../../lib/chromium-executable.mjs";

const target = process.argv[2];
if (!target) {
  throw new Error("usage: node tools/maintainer/verify-public-first-install.mjs <launcher-url> [--game=th07] [--timeout-ms=180000]");
}
const targetUrl = new URL(target);
if (!new Set(["http:", "https:"]).has(targetUrl.protocol)) throw new Error("launcher URL must use http or https");
const values = Object.fromEntries(process.argv.slice(3).map(value => {
  const split = value.indexOf("=");
  if (!value.startsWith("--") || split < 3) throw new Error(`invalid argument: ${value}`);
  return [value.slice(2, split), value.slice(split + 1)];
}));
for (const name of Object.keys(values)) {
  if (!new Set(["game", "timeout-ms"]).has(name)) throw new Error(`unknown argument: --${name}`);
}
const game = values.game || "th07";
if (!/^th(?:06|07|08|10)$/.test(game)) throw new Error(`unsupported game: ${game}`);
const timeoutMs = Number.parseInt(values["timeout-ms"] || "180000", 10);
if (!Number.isInteger(timeoutMs) || timeoutMs < 30_000 || timeoutMs > 600_000) {
  throw new Error("--timeout-ms must be between 30000 and 600000");
}

async function packageState(page, gameId) {
  return page.evaluate(async selectedGame => {
    const requestResult = request => new Promise((resolve, reject) => {
      request.onsuccess = () => resolve(request.result);
      request.onerror = () => reject(request.error || new Error("IndexedDB request failed"));
    });
    const db = await new Promise((resolve, reject) => {
      const request = indexedDB.open("eagler-touhou-package-store-v1");
      request.onsuccess = () => resolve(request.result);
      request.onerror = () => reject(request.error || new Error("Package Store open failed"));
    });
    try {
      const transaction = db.transaction(["installations", "generations"], "readonly");
      const installation = await requestResult(transaction.objectStore("installations").get(selectedGame));
      const generation = installation?.currentGeneration
        ? await requestResult(transaction.objectStore("generations").get([selectedGame, installation.currentGeneration]))
        : null;
      return {
        currentGeneration: installation?.currentGeneration || null,
        source: installation?.source || null,
        revision: generation?.descriptor?.revision || null,
        files: Object.keys(generation?.files || {}).length,
      };
    } finally {
      db.close();
    }
  }, gameId);
}

const browser = await puppeteer.launch({
  executablePath: await findChromiumExecutable(),
  headless: true,
  args: ["--disable-extensions", "--no-first-run", "--no-default-browser-check"],
});
const page = await browser.newPage();
const pageErrors = [];
const failedRequests = [];
const startedAt = performance.now();
try {
  page.on("pageerror", error => pageErrors.push(error.message));
  page.on("requestfailed", request => failedRequests.push({
    url: request.url(),
    error: request.failure()?.errorText || "request failed",
  }));
  await page.evaluateOnNewDocument(() => {
    globalThis.__eaglerPublicFirstInstallEvents = [];
    addEventListener("message", event => {
      const message = event.data;
      if (!message || typeof message !== "object" || message.protocol !== "eagler-touhou/1") return;
      if (["ready", "first-frame", "error", "fatal"].includes(message.event)) {
        globalThis.__eaglerPublicFirstInstallEvents.push({
          game: message.game || null,
          event: message.event,
          error: message.error || null,
        });
      }
    });
  });
  await page.goto(targetUrl.href, { waitUntil: "domcontentloaded", timeout: 30_000 });
  await page.waitForFunction(() => globalThis.__eaglerBoot?.done === true, { timeout: 30_000 });
  await page.evaluate(() => document.querySelector("#firstUseNoticeDialog")?.close());
  await page.waitForSelector(`.game[data-game="${game}"]:not(.game-multiplayer)`, { visible: true, timeout: 30_000 });
  await page.evaluate(selectedGame => {
    const card = document.querySelector(`.game[data-game="${selectedGame}"]:not(.game-multiplayer)`);
    if (!(card instanceof HTMLElement)) throw new Error(`game card is missing: ${selectedGame}`);
    card.click();
  }, game);
  await page.waitForFunction(selectedGame => {
    const card = document.querySelector(`.game[data-game="${selectedGame}"]:not(.game-multiplayer)`);
    return card?.getAttribute("aria-current") === "page" || card?.classList.contains("selected");
  }, { timeout: 30_000 }, game);
  await page.evaluate(() => {
    const launch = document.querySelector("#launch");
    if (!(launch instanceof HTMLButtonElement) || launch.disabled) throw new Error("launch action is unavailable");
    launch.click();
  });
  const readDiagnostic = () => page.evaluate(selectedGame => ({
    events: (globalThis.__eaglerPublicFirstInstallEvents || []).filter(message => message.game === selectedGame),
    status: document.querySelector("#status")?.textContent?.trim() || "",
    playerStatus: document.querySelector("#playerStatus")?.textContent?.trim() || "",
    fallbackVisible: !document.querySelector("#gameDataImportWindow")?.hidden,
    fallbackReason: document.querySelector("#gameDataImportReason")?.textContent?.trim() || "",
    startupErrorVisible: !document.querySelector("#startupError")?.hidden,
    startupError: document.querySelector("#startupErrorText")?.textContent?.trim() || "",
    decisionOpen: document.querySelector("#decisionDialog")?.open === true,
    decisionMessage: document.querySelector("#decisionMessage")?.textContent?.trim() || "",
    transfer: document.querySelector("#transferLabel")?.textContent?.trim() || "",
  }), game);
  const deadline = Date.now() + timeoutMs;
  let diagnostic = await readDiagnostic();
  while (Date.now() < deadline) {
    const terminalEvent = diagnostic.events.some(message => ["first-frame", "error", "fatal"].includes(message.event));
    if (terminalEvent || diagnostic.fallbackVisible || diagnostic.startupErrorVisible || diagnostic.decisionOpen) break;
    await new Promise(resolve => setTimeout(resolve, 250));
    diagnostic = await readDiagnostic();
  }
  const firstFrame = diagnostic.events.some(message => message.event === "first-frame");
  if (!firstFrame) {
    throw new Error(`public fresh install did not reach first-frame: ${JSON.stringify({ diagnostic, pageErrors, failedRequests })}`);
  }
  const installed = await packageState(page, game);
  if (!installed.currentGeneration || !installed.revision || installed.files < 1) {
    throw new Error(`first-frame was reached without a committed Package generation: ${JSON.stringify(installed)}`);
  }
  const releaseId = await page.evaluate(async () => {
    try {
      const response = await fetch("release-manifest.json", { cache: "no-store" });
      if (!response.ok) return null;
      return (await response.json())?.releaseId || null;
    } catch { return null; }
  });
  console.log(JSON.stringify({
    pass: true,
    target: targetUrl.href,
    game,
    releaseId,
    elapsedMs: Math.round(performance.now() - startedAt),
    package: installed,
    pageErrors,
    failedRequests,
  }));
} finally {
  await page.close().catch(() => {});
  await browser.close().catch(() => {});
}
