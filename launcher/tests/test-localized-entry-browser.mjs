import assert from "node:assert/strict";
import puppeteer from "puppeteer-core";

const baseUrl = process.argv.find(value => value.startsWith("--url="))?.slice("--url=".length) || "http://127.0.0.1:8130/";
const executablePath = process.env.CHROME_PATH || "C:/Program Files/Google/Chrome/Application/chrome.exe";
let browser;
try {
  browser = await puppeteer.launch({ executablePath, headless: true, args: ["--no-first-run"] });
  const page = await browser.newPage();
  const failures = [];
  const fontRequests = new Set();
  page.on("console", message => { if (message.type() === "error") failures.push(message.text()); });
  page.on("pageerror", error => failures.push(error.message));
  page.on("request", request => {
    if (/\.woff2(?:$|\?)/.test(request.url())) fontRequests.add(new URL(request.url()).pathname);
  });
  for (const [path, locale, title] of [
    ["", "zh-CN", "东方Project 原作 STG"],
    ["en.html", "en", "Touhou Project Original STGs"],
  ]) {
    await page.goto(new URL(path, baseUrl).href, { waitUntil: "networkidle0" });
    const state = await page.evaluate(() => ({
      lang: document.documentElement.lang,
      locale: document.documentElement.dataset.uiLocale,
      title: document.title,
      description: document.querySelector('meta[name="description"]')?.content,
      cards: document.querySelectorAll(".game").length,
      canonical: document.querySelector('link[rel="canonical"]')?.href || "",
    }));
    assert.equal(state.lang, locale);
    assert.equal(state.locale, locale);
    assert.match(state.title, new RegExp(title));
    assert.ok(state.description);
    assert.ok(state.cards >= 4);
  }
  assert.equal([...fontRequests].some(path => path.includes("-deferred.woff2")), false,
    `initial localized entries fetched deferred font subsets: ${[...fontRequests].join(", ")}`);
  // Follow the actual Router link while retaining the English session/document.
  await page.evaluate(() => {
    window.__localeRouteDocument = 'same-document';
    document.querySelector('.shelf-lobby-link').click();
  });
  await page.waitForFunction(() => location.pathname.endsWith('/lobby.html'));
  await page.waitForSelector('.lobby-main');
  assert.deepEqual(await page.evaluate(() => ({lang:document.documentElement.lang,
    locale:document.documentElement.dataset.uiLocale,marker:window.__localeRouteDocument})),
    {lang:'en',locale:'en',marker:'same-document'});
  assert.match(await page.title(),/Multiplayer lobby/);
  await page.evaluate(() => history.back());
  await page.waitForFunction(() => location.pathname.endsWith('/en.html') && document.documentElement.lang === 'en');
  await page.select("#uiLanguageSelect", "zh-CN");
  await page.waitForFunction(() => document.documentElement.lang === "zh-CN" && location.pathname.endsWith("/"));
  await page.evaluate(() => window.dispatchEvent(new PointerEvent("pointerdown")));
  await page.waitForFunction(() => {
    const link = document.querySelector('link[data-deferred-ui-fonts]');
    return link instanceof HTMLLinkElement && !!link.sheet;
  });
  assert.deepEqual(failures, []);
  console.log(JSON.stringify({ localizedEntries: "PASS", browser: "Chrome", languages: ["zh-CN", "en"], fonts: [...fontRequests] }));
} finally {
  await browser?.close();
}
