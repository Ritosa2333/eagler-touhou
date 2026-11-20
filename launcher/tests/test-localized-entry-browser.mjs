import assert from "node:assert/strict";
import puppeteer from "puppeteer-core";

const baseUrl = process.argv.find(value => value.startsWith("--url="))?.slice("--url=".length) || "http://127.0.0.1:8130/";
const executablePath = process.env.CHROME_PATH || "C:/Program Files/Google/Chrome/Application/chrome.exe";
const devServer = process.argv.includes("--dev-server");
const selectChinese = async page => {
  await page.waitForSelector("#mastheadMenuToggle:not([disabled])", {visible: true});
  await page.click("#mastheadMenuToggle");
  await page.waitForSelector(".masthead-language-trigger", {visible: true});
  await page.click(".masthead-language-trigger");
  const option = '.masthead-menu-language [role="menuitemradio"]';
  await page.waitForFunction(selector => [...document.querySelectorAll(selector)].some(node => ["简体中文", "Simplified Chinese"].includes(node.textContent.trim())), {}, option);
  await page.$$eval(option, nodes => nodes.find(node => ["简体中文", "Simplified Chinese"].includes(node.textContent.trim())).click());
  await page.waitForFunction(() => document.documentElement.lang === "zh-CN", {timeout: 8000});
};
let browser;
try {
  browser = await puppeteer.launch({ executablePath, headless: true, args: ["--no-first-run"] });
  const page = await browser.newPage();
  await page.setViewport({width: 1440, height: 900});
  await page.evaluateOnNewDocument(() => localStorage.setItem("eagler-touhou-first-use-notice-seen-v1", "1"));
  await page.setRequestInterception(true);
  page.on("request", request => request.url() === new URL("_eagler/ui-country", baseUrl).href
    ? request.respond({status: 200, contentType: "text/plain", body: "CN"})
    : request.continue());
  const failures = [];
  const fontRequests = new Set();
  if (!devServer) page.on("console", message => { if (message.type() === "error") failures.push(message.text()); });
  page.on("pageerror", error => failures.push(error.message));
  page.on("request", request => {
    if (/\.woff2(?:$|\?)/.test(request.url())) fontRequests.add(new URL(request.url()).pathname);
  });
  for (const [path, locale, title] of [
    ["", "zh-CN", "单人游戏 - TouhouOnline"],
    ["en.html", "en", "Single-player - TouhouOnline"],
    ["ja.html", "ja", "シングルプレイ - TouhouOnline"],
  ]) {
    await page.goto(new URL(path, baseUrl).href, { waitUntil: "domcontentloaded" });
    await page.waitForFunction(value => document.documentElement.lang === value && window.__eaglerBoot?.done, {}, locale);
    const state = await page.evaluate(() => ({
      lang: document.documentElement.lang,
      locale: document.documentElement.dataset.uiLocale,
      title: document.title,
      description: document.querySelector('meta[name="description"]')?.content,
      cards: document.querySelectorAll("#bookGameGrid [data-book-product]").length,
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
  await page.goto(new URL("en.html", baseUrl).href, {waitUntil: "domcontentloaded"});
  await page.waitForFunction(() => window.__eaglerBoot?.done);
  // Follow the actual Router link while retaining the English session/document.
  await page.evaluate(() => {
    window.__localeRouteDocument = 'same-document';
    document.querySelector('.book-lobby-link').click();
  });
  await page.waitForFunction(() => location.pathname.endsWith('/lobby.html'));
  await page.waitForSelector('.lobby-main');
  assert.deepEqual(await page.evaluate(() => ({lang:document.documentElement.lang,
    locale:document.documentElement.dataset.uiLocale,marker:window.__localeRouteDocument})),
    {lang:'en',locale:'en',marker:'same-document'});
  assert.match(await page.title(),/Multiplayer lobby/);
  await page.evaluate(() => history.back());
  await page.waitForFunction(() => location.pathname.endsWith('/en.html') && document.documentElement.lang === 'en');
  await selectChinese(page);
  await page.waitForFunction(() => location.pathname.endsWith("/"));
  await page.evaluate(() => window.dispatchEvent(new PointerEvent("pointerdown")));
  await page.waitForFunction(() => {
    const link = document.querySelector('link[data-deferred-ui-fonts]');
    return link instanceof HTMLLinkElement && !!link.sheet;
  });
  assert.deepEqual(failures, []);
  for (const [country, locale, entry] of [["JP", "ja", "ja.html"], ["US", "en", "en.html"], ["ZZ", "en", "en.html"]]) {
    const context = await browser.createBrowserContext();
    try {
      const region = await context.newPage();
      await region.setViewport({width: 1440, height: 900});
      await region.evaluateOnNewDocument(() => localStorage.setItem("eagler-touhou-first-use-notice-seen-v1", "1"));
      const errors = [];
      region.on("pageerror", error => errors.push(error.message));
      await region.setRequestInterception(true);
      let lookups = 0;
      region.on("request", request => {
        if (request.url() !== new URL("_eagler/ui-country", baseUrl).href) return request.continue();
        lookups++;
        return request.respond({status: 200, contentType: "text/plain", body: country});
      });
      await region.goto(new URL("lobby.html?keep=a%20b#rooms", baseUrl), {waitUntil: "domcontentloaded"});
      await region.waitForFunction(value => document.documentElement.lang === value && !!document.querySelector("#uiLanguageSelect"), {}, locale);
      assert.equal(region.url(), new URL(`lobby.html?keep=a%20b&lang=${locale}#rooms`, baseUrl).href);
      assert.equal(await region.$eval("#uiLanguageSelect", select => select.value), locale);
      await region.goto(new URL("?keep=a%20b#library", baseUrl), {waitUntil: "domcontentloaded"});
      await region.waitForFunction(value => document.documentElement.lang === value && window.__eaglerBoot?.done, {}, locale);
      assert.equal(region.url(), new URL(`${entry}?keep=a%20b#library`, baseUrl).href);
      assert.equal(lookups, country === "ZZ" ? 2 : 1, "only a resolved region may be reused");
      await selectChinese(region);
      await region.goto(new URL("lobby.html", baseUrl), {waitUntil: "domcontentloaded"});
      await region.waitForFunction(() => document.documentElement.lang === "zh-CN" && !!document.querySelector("#uiLanguageSelect"));
      assert.equal(await region.$eval("#uiLanguageSelect", select => select.value), "zh-CN");
      assert.equal(lookups, country === "ZZ" ? 2 : 1, "manual language suppresses region lookup");
      assert.deepEqual(errors, []);
    } finally {
      await context.close();
    }
  }
  console.log(JSON.stringify({ localizedEntries: "PASS", browser: "Chrome", languages: ["zh-CN", "en", "ja"], fonts: [...fontRequests] }));
} finally {
  await browser?.close();
}
