// Local TH11 Hosted smoke: Launcher language selection must reach native Runtime.
import puppeteer from "puppeteer-core";
import { existsSync, readdirSync } from "node:fs";
import { resolve } from "node:path";

const browserRoot = resolve(process.env.LOCALAPPDATA || "", "ms-playwright");
const installations = existsSync(browserRoot)
  ? readdirSync(browserRoot).filter(name => /^chromium-\d+$/.test(name)).sort().reverse()
  : [];
const executablePath = process.env.EAGLER_CHROMIUM ||
  resolve(browserRoot, installations[0] || "", "chrome-win64/chrome.exe");
if (!existsSync(executablePath)) throw Error(`Chromium not found: ${executablePath}`);
const site = process.env.EAGLER_TEST_URL || "http://127.0.0.1:8135/";
const packagePath = process.env.EAGLER_TEST_PACKAGE;
if (packagePath && !existsSync(packagePath)) throw Error(`Package not found: ${packagePath}`);
const browser = await puppeteer.launch({ executablePath, headless: true, args: ["--no-sandbox"] });
try {
  for (const language of packagePath ? ["ja", "lang_zh-hans", "lang_en"] : ["lang_zh-hans", "lang_en"]) {
    const context = await browser.createBrowserContext();
    const page = await context.newPage();
    if(process.env.EAGLER_TEST_GAP === "1")await page.setViewport({width:430,height:932,isMobile:true,hasTouch:true});
    const errors = [];
    page.on("pageerror", error => errors.push(error.message));
    await page.goto(new URL("?game=th11", site).href, { waitUntil: "networkidle2", timeout: 30000 });
    if (await page.$eval("#firstUseNoticeDialog", dialog => dialog.open)) {
      await page.evaluate(() => document.querySelector("#firstUseNoticeClose").click());
    }
    const hostedResourceRequests = [];
    if (packagePath) {
      await page.evaluate(() => document.querySelector("#gamePackageImport").click());
      await page.waitForFunction(() => !document.querySelector("#gameDataImportWindow").hidden);
      await (await page.$("#gameDataImportInput")).uploadFile(resolve(packagePath));
      try {
        await page.waitForFunction(() => document.querySelector("#gameDataImportWindow").hidden, { timeout: 30000 });
      } catch {
        const state = await page.evaluate(() => ({ reason: document.querySelector("#gameDataImportReason")?.textContent,
          status: document.querySelector("#status")?.textContent,
          decisions: [...document.querySelectorAll("dialog[open]")].map(item => ({ id: item.id, text: item.textContent })) }));
        throw Error(`TH11 package import failed: ${JSON.stringify({ state, errors })}`);
      }
      await page.setRequestInterception(true);
      page.on("request", request => {
        const path = new URL(request.url()).pathname;
        if (/^\/shared\/(?:msgothic\.ttc|unifont\.otf)$/.test(path) || /^\/games\/th11\/language\//.test(path)) {
          hostedResourceRequests.push(path); void request.abort();
        } else void request.continue();
      });
    }
    await page.select("#languageSelect", language);
    await page.select("#musicSelect", "ogg-stream");
    if(process.env.EAGLER_TEST_GAP === "1") {
      await page.evaluate(()=>{const toggle=document.querySelector("#touchToggle");if(toggle.getAttribute("aria-checked")!=="true")toggle.click();});
      await page.waitForFunction(()=>document.querySelector("#decisionDialog").open||document.querySelector("#touchToggle").getAttribute("aria-checked")==="true");
      if(await page.$eval("#decisionDialog",item=>item.open))await page.evaluate(()=>document.querySelector("#decisionConfirm").click());
      await page.waitForFunction(()=>document.querySelector("#touchToggle").getAttribute("aria-checked")==="true");
    }
    await page.evaluate(() => document.querySelector("#launch").click());
    if (packagePath) {
      await page.waitForFunction(() => document.querySelector("#decisionDialog")?.open ||
        [...document.querySelectorAll("iframe")].some(item => !!item.src), { timeout: 15000 });
      if (await page.$eval("#decisionDialog", item => item.open)) {
        await page.evaluate(() => document.querySelector("#decisionCancel").click());
      }
    }
    try { await page.waitForFunction(language => {
      const frame = [...document.querySelectorAll("iframe")].find(item => item.contentWindow?.core?.th11_phase);
      const error = document.querySelector("#startupErrorText")?.textContent || "";
      return (frame?.contentWindow?.core?.th11_frame?.() > 120 &&
        (language === "ja" || !!frame.contentWindow.FS?.analyzePath("/thcrap/th11/localization/options.json").exists)) || !!error;
    }, { timeout: 30000 }, language); } catch {
      const state = await page.evaluate(() => ({ status: document.querySelector("#status")?.textContent,
        reason: document.querySelector("#gameDataImportReason")?.textContent,
        startup: document.querySelector("#startupErrorText")?.textContent,
        decision: { hidden: document.querySelector("#decisionDialog")?.hidden, text: document.querySelector("#decisionDialog")?.textContent },
        dialogs: [...document.querySelectorAll("dialog[open]")].map(item => ({ id: item.id, text: item.textContent })),
        frames: [...document.querySelectorAll("iframe")].map(item => ({ url: item.src, frame: item.contentWindow?.core?.th11_frame?.(), error: item.contentDocument?.querySelector("#error")?.textContent })) }));
      throw Error(`TH11 startup failed: ${JSON.stringify({ language, state, errors, hostedResourceRequests })}`);
    }
    const result = await page.evaluate(() => {
      const frame = [...document.querySelectorAll("iframe")].find(item => item.contentWindow?.core?.th11_phase);
      const fs = frame?.contentWindow?.FS;
      const exists = path => !!fs?.analyzePath(path).exists;
      return {
        selected: document.querySelector("#languageSelect")?.value,
        phase: frame?.contentWindow?.core?.th11_phase(),
        frame: frame?.contentWindow?.core?.th11_frame(),
        error: document.querySelector("#startupErrorText")?.textContent ||
          frame?.contentDocument?.querySelector("#error")?.textContent || "",
        files: {
          options: exists("/thcrap/th11/localization/options.json"),
          strings: exists("/thcrap/th11/localization/strings.etl"),
          dialogue: exists("/thcrap/th11/st01_00a.msg"),
          ending: exists("/thcrap/th11/e00.msg"),
          image: exists("/thcrap/th11/ascii/pause.png"),
          font: exists("/thcrap/th11/fonts/unifont-15.1.05-subset.otf"),
        },
        sharedFonts: { japanese: exists("/msgothic.ttc"), unicode: exists("/unifont.otf") },
      };
    });
    console.log(JSON.stringify({ language, imported: !!packagePath, ...result, errors, hostedResourceRequests }));
    if(process.env.EAGLER_TEST_GAP === "1") {
      await page.evaluate(()=>{if(!document.querySelector("#touchHelp").hidden)document.querySelector("#touchHelpClose").click();});
      const gap = await page.evaluate(() => {
        const button=document.querySelector("#touchGap"),runtime=[...document.querySelectorAll("iframe")].find(item=>item.contentWindow?.core?.th11_phase);
        if(button.hidden||!runtime.contentWindow.core.sdl_touch_gap)throw Error("TH11 gap control/export unavailable: "+JSON.stringify({hidden:button.hidden,export:typeof runtime.contentWindow.core.sdl_touch_gap,touch:document.querySelector('#touchToggle').getAttribute('aria-checked'),dialogs:[...document.querySelectorAll('dialog[open]')].map(d=>({id:d.id,text:d.textContent.slice(0,200)}))}));
        button.dispatchEvent(new PointerEvent("pointerdown",{bubbles:true,cancelable:true,pointerId:87,pointerType:"touch"}));
        const pressed=button.getAttribute("aria-pressed");
        button.dispatchEvent(new PointerEvent("pointerup",{bubbles:true,cancelable:true,pointerId:87,pointerType:"touch"}));
        const released=button.getAttribute("aria-pressed");
        button.dispatchEvent(new PointerEvent("pointerdown",{bubbles:true,cancelable:true,pointerId:88,pointerType:"touch"}));
        window.dispatchEvent(new Event("blur"));
        return {pressed,released,blurred:button.getAttribute("aria-pressed")};
      });
      if(gap.pressed!=="true"||gap.released!=="false"||gap.blurred!=="false")throw Error(JSON.stringify(gap));
      console.log(JSON.stringify({language,gap}));
      if(process.env.EAGLER_TEST_GAP_SCREENSHOT)await page.screenshot({path:resolve(process.env.EAGLER_TEST_GAP_SCREENSHOT.replace('{language}',language))});
    }
    if (result.selected !== language || result.phase == null || result.frame <= 120 || result.error ||
        (language !== "ja" && Object.values(result.files).some(value => !value)) || errors.length ||
        hostedResourceRequests.length || (packagePath && Object.values(result.sharedFonts).some(value => !value))) {
      throw Error(`TH11 ${language} localization smoke failed`);
    }
    if (language === "lang_zh-hans" && process.env.EAGLER_TEST_SCREENSHOT) {
      await page.screenshot({ path: resolve(process.env.EAGLER_TEST_SCREENSHOT) });
    }
    if (language === "lang_zh-hans" && process.env.EAGLER_TEST_STORY === "1") {
      const frame = page.frames().find(item => item.url().includes("th11.html"));
      if (!frame) throw Error("TH11 Runtime frame not found");
      await frame.evaluate(async () => {
        const wait = ms => new Promise(resolve => setTimeout(resolve, ms));
        for (let index = 0; index < 4; index++) {
          core.th11_key(44, 1); await wait(70); core.th11_key(44, 0); await wait(800);
        }
      });
      await page.waitForFunction(() => [...document.querySelectorAll("iframe")].some(item =>
        item.contentWindow?.core?.th11_phase?.() === 1) ||
        !!document.querySelector("#startupErrorText")?.textContent, { timeout: 30000 });
      const story = await frame.evaluate(() => ({ phase: core.th11_phase(), frame: core.th11_frame(), error: document.querySelector("#error")?.textContent || "" }));
      console.log(JSON.stringify({ language, story }));
      if (story.phase !== 1 || story.error) throw Error(`TH11 story: ${story.error || "not started"}`);
      if (process.env.EAGLER_TEST_STORY_SCREENSHOT) {
        await new Promise(resolve => setTimeout(resolve, 5000));
        const laterError = await frame.evaluate(() => document.querySelector("#error")?.textContent || "");
        if (laterError) throw Error(`TH11 story after 5s: ${laterError}`);
        await page.screenshot({ path: resolve(process.env.EAGLER_TEST_STORY_SCREENSHOT) });
      }
    }
    if (language === "lang_zh-hans" && process.env.EAGLER_TEST_MUSIC_ROOM === "1") {
      const frame = page.frames().find(item => item.url().includes("th11.html"));
      if (!frame) throw Error("TH11 Runtime frame not found");
      await frame.evaluate(async () => {
        const wait = ms => new Promise(resolve => setTimeout(resolve, ms));
        for (let index = 0; index < 4; index++) {
          core.th11_key(208, 1); await wait(70); core.th11_key(208, 0); await wait(100);
        }
        core.th11_key(44, 1); await wait(70); core.th11_key(44, 0);
        await wait(4000);
      });
      const music = await frame.evaluate(() => {
        const pointer=core.th11_error(),end=Module.HEAPU8.indexOf(0,pointer);
        return { phase: core.th11_phase(), frame: core.th11_frame(),
          error: document.querySelector("#error")?.textContent || "",
          coreError: new TextDecoder().decode(Module.HEAPU8.subarray(pointer,end<0?pointer+256:end)) };
      });
      console.log(JSON.stringify({ language, musicRoom: music }));
      if (music.error || music.coreError) throw Error(`TH11 music room: ${music.error || music.coreError}`);
      if (process.env.EAGLER_TEST_MUSIC_ROOM_SCREENSHOT) {
        await page.screenshot({ path: resolve(process.env.EAGLER_TEST_MUSIC_ROOM_SCREENSHOT) });
      }
    }
    await context.close();
  }
} finally {
  await browser.close();
}
