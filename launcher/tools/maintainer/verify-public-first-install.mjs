#!/usr/bin/env node
import puppeteer from "puppeteer-core";
import { findChromiumExecutable } from "../../lib/chromium-executable.mjs";
import { DEFAULT_PRODUCT_ID, isGameId } from "../../lib/contracts/product-catalog.mjs";

const target = process.argv[2];
if (!target) {
  throw new Error(`usage: node tools/maintainer/verify-public-first-install.mjs <launcher-url> [--game=${DEFAULT_PRODUCT_ID}] [--timeout-ms=180000] [--offline-reopen=true]`);
}
const targetUrl = new URL(target);
if (!new Set(["http:", "https:"]).has(targetUrl.protocol)) throw new Error("launcher URL must use http or https");
const values = Object.fromEntries(process.argv.slice(3).map(value => {
  const split = value.indexOf("=");
  if (!value.startsWith("--") || split < 3) throw new Error(`invalid argument: ${value}`);
  return [value.slice(2, split), value.slice(split + 1)];
}));
for (const name of Object.keys(values)) {
  if (!new Set(["game", "timeout-ms", "offline-reopen"]).has(name)) throw new Error(`unknown argument: --${name}`);
}
if (values["offline-reopen"] !== undefined && values["offline-reopen"] !== "true") {
  throw new Error("--offline-reopen must be true when supplied");
}
const verifyOffline = values["offline-reopen"] === "true";
const game = values.game || DEFAULT_PRODUCT_ID;
if (!isGameId(game)) throw new Error(`unsupported game: ${game}`);
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

const browserProxy = process.env.EAGLER_BROWSER_PROXY;
const browser = await puppeteer.launch({
  executablePath: await findChromiumExecutable(),
  headless: true,
  args: ["--disable-extensions", "--no-first-run", "--no-default-browser-check",
    ...(browserProxy ? ["--proxy-server=" + browserProxy] : [])],
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
    globalThis.__eaglerPublicFirstInstallLaunchClicks = 0;
    addEventListener("click", event => {
      if (event.target instanceof Element && event.target.closest("#launch")) globalThis.__eaglerPublicFirstInstallLaunchClicks++;
    }, true);
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
  const readDiagnostic = () => page.evaluate(selectedGame => ({
    events: (globalThis.__eaglerPublicFirstInstallEvents || []).filter(message => message.game === selectedGame),
    launchClicks: globalThis.__eaglerPublicFirstInstallLaunchClicks,
    playerOpen: document.querySelector("#player")?.classList.contains("open") ?? false,
    status: document.querySelector("#status")?.textContent?.trim() || "",
    playerStatus: document.querySelector("#playerStatus")?.textContent?.trim() || "",
    fallbackVisible: Boolean(document.querySelector("#gameDataImportWindow:not([hidden])")?.getClientRects().length),
    fallbackReason: document.querySelector("#gameDataImportReason")?.textContent?.trim() || "",
    startupErrorVisible: Boolean(document.querySelector("#startupError:not([hidden])")?.getClientRects().length),
    startupError: document.querySelector("#startupErrorText")?.textContent?.trim() || "",
    decisionOpen: document.querySelector("#decisionDialog")?.open === true,
    decisionMessage: document.querySelector("#decisionMessage")?.textContent?.trim() || "",
    transfer: document.querySelector("#transferLabel")?.textContent?.trim() || "",
  }), game);
  async function launchAndVerify(offline = false) {
    const response = await page.goto(targetUrl.href, { waitUntil: "domcontentloaded", timeout: 30_000 });
    if (offline && (!response?.ok() || !response.fromServiceWorker())) {
      throw new Error("offline Launcher did not load from its Service Worker");
    }
    await page.waitForFunction(() => globalThis.__eaglerBoot?.done === true, { timeout: 30_000 });
    const firstUseNotice = "#firstUseNoticeDialog[open]";
    async function dismissFirstUseNotice() {
      if (!await page.$(firstUseNotice)) return;
      await page.locator(firstUseNotice + " #firstUseNoticeClose").click();
      await page.waitForSelector(firstUseNotice, { hidden: true });
    }
    await dismissFirstUseNotice();
    const statusNotice = await page.$("#siteNotice:not([hidden]) #siteNoticeClose");
    if (statusNotice && await statusNotice.isVisible()) await statusNotice.click();
    const cardSelector = `.book-game-link[data-game="${game}"], .game[data-game="${game}"]:not(.game-multiplayer)`;
    async function openGameCard() {
      const card = await page.waitForSelector(cardSelector, { visible: true, timeout: 30_000 });
      await card.evaluate(element => element.scrollIntoView({block:"center", inline:"nearest", behavior:"instant"}));
      await card.click();
      const result = await page.waitForFunction((selectedGame, noticeSelector) => {
        if (new URL(location.href).searchParams.get("game") === selectedGame) return "game";
        const legacyCard = document.querySelector(`.game[data-game="${selectedGame}"]:not(.game-multiplayer)`);
        if (legacyCard?.getAttribute("aria-current") === "page" || legacyCard?.classList.contains("selected")) return "game";
        return document.querySelector(noticeSelector) ? "notice" : null;
      }, { timeout: 30_000 }, game, firstUseNotice);
      return result.jsonValue();
    }
    // Loading the first-use content can finish after the boot watchdog or
    // while the game card is being clicked.
    if (await openGameCard() === "notice") {
      await dismissFirstUseNotice();
      if (await openGameCard() !== "game") throw new Error("First-use notice blocked the game card again");
    }
    await dismissFirstUseNotice();
    const launch = await page.waitForSelector("#launch:not([disabled])", { visible: true, timeout: 30_000 });
    await launch.evaluate(element => element.scrollIntoView({block:"center", inline:"nearest", behavior:"instant"}));
    await page.waitForFunction(element => {
      const bounds = element.getBoundingClientRect();
      return element.contains(document.elementFromPoint(bounds.left + bounds.width / 2, bounds.top + bounds.height / 2));
    }, {timeout:15_000}, launch).catch(async error => {
      const obstruction = await launch.evaluate(element => {
        const bounds = element.getBoundingClientRect();
        const top = document.elementFromPoint(bounds.left + bounds.width / 2, bounds.top + bounds.height / 2);
        return {
          top: top && {tag:top.tagName, id:top.id, className:top.getAttribute("class")},
          dialogs: [...document.querySelectorAll("dialog[open]")].map(dialog => dialog.id),
          inert: !!element.closest("[inert]"),
          bounds: bounds.toJSON(),
        };
      });
      throw new Error(`launch button cannot be clicked: ${JSON.stringify(obstruction)}`, {cause:error});
    });
    await launch.click();
    const deadline = Date.now() + timeoutMs;
    let diagnostic = await readDiagnostic();
    while (Date.now() < deadline) {
      const terminal = diagnostic.events.some(message => ["first-frame", "error", "fatal"].includes(message.event));
      if (terminal || diagnostic.fallbackVisible || diagnostic.startupErrorVisible || diagnostic.decisionOpen) break;
      await new Promise(resolve => setTimeout(resolve, 250));
      diagnostic = await readDiagnostic();
    }
    if (!diagnostic.events.some(message => message.event === "first-frame")) {
      throw new Error(`${offline ? "offline reopen" : "public fresh install"} did not reach first-frame: ${JSON.stringify({ diagnostic, pageErrors, failedRequests })}`);
    }
    const installed = await packageState(page, game);
    if (!installed.currentGeneration || !installed.revision || installed.files < 1) {
      throw new Error(`first-frame was reached without a committed Package generation: ${JSON.stringify(installed)}`);
    }
    return installed;
  }

  const installed = await launchAndVerify();
  const releaseId = await page.evaluate(async () => {
    try {
      const response = await fetch("release-manifest.json", { cache: "no-store" });
      if (!response.ok) return null;
      return (await response.json())?.releaseId || null;
    } catch { return null; }
  });
  if (verifyOffline) {
    await page.waitForFunction(() => navigator.serviceWorker?.getRegistration().then(registration => !!registration?.active), {timeout:timeoutMs});
    await page.goto(targetUrl.href, {waitUntil:"domcontentloaded", timeout:30_000});
    await page.waitForFunction(() => !!navigator.serviceWorker?.controller, {timeout:30_000});
    await page.setCacheEnabled(false);
    await page.setOfflineMode(true);
    const reopened = await launchAndVerify(true);
    if (reopened.currentGeneration !== installed.currentGeneration) throw new Error("offline reopen changed the installed Package generation");
  }
  console.log(JSON.stringify({
    pass: true,
    target: targetUrl.href,
    game,
    releaseId,
    elapsedMs: Math.round(performance.now() - startedAt),
    package: installed,
    ...(verifyOffline && {offlineReopen: true}),
    pageErrors,
    failedRequests,
  }));
} finally {
  await page.close().catch(() => {});
  await browser.close().catch(() => {});
}
