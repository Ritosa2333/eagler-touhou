import { readFile } from "node:fs/promises";
import { resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { resolveFrontendPackageSource } from "../lib/frontend-manifest.mjs";

const project = resolve(fileURLToPath(new URL("..", import.meta.url)));
const html = await readFile(resolveFrontendPackageSource("index.html"), "utf8");
const script = await readFile(resolve(project, "src", "launcher", "app.mts"), "utf8");

const htmlIds = new Set([...html.matchAll(/\bid=["']([^"']+)["']/g)].map(match => match[1]));
const scriptIds = new Set([...script.matchAll(/\$\(["']#([^"']+)["']\)/g)].map(match => match[1]));
const missing = [...scriptIds].filter(id => !htmlIds.has(id));

if (missing.length) {
  throw new Error(`src/launcher/app.mts 引用了 index.html 中不存在的 ID：${missing.join(", ")}`);
}

if (!/<input\b[^>]*\bid=["']fileInput["'][^>]*\btype=["']file["']/i.test(html)) {
  throw new Error("文件选择器 #fileInput 缺失或类型错误");
}
if (!script.includes("input.onchange =") || !script.includes("input.oncancel =")) {
  throw new Error("文件选择器必须分别处理选择和取消事件");
}
if (/window\.addEventListener\(["']focus["']\s*,\s*onFocus/.test(script)) {
  throw new Error("文件选择器不能用窗口 focus 判断选择结果");
}

const escapeControl = html.indexOf('id="touchEscape"');
const restartControl = html.indexOf('id="touchRestart"');
const thpracControl = html.indexOf('id="touchThpracInput"');
if (!(escapeControl >= 0 && restartControl > escapeControl && thpracControl > restartControl)) {
  throw new Error("按键布局顺序必须保持 ESC → R → thprac");
}
if (!/<button\b[^>]*\bid="touchRestart"[^>]*\bdata-touch-layout-control="restart"[^>]*\btitle="在暂停菜单按下后，会重开本局。"/i.test(html)) {
  throw new Error("R 按键缺失、未加入布局，或描述不正确");
}
const restartSetting = html.indexOf('id="restartButtonToggle"');
const thpracSetting = html.indexOf('id="thpracTouchControlsToggle"');
if (!(restartSetting >= 0 && thpracSetting > restartSetting)) {
  throw new Error("触控设置中的 R 开关必须位于 thprac 按键开关上方");
}
if (!/<button\b[^>]*\bid="restartButtonToggle"[^>]*\baria-checked="false"/i.test(html)) {
  throw new Error("R 按键开关必须默认关闭");
}

console.log(JSON.stringify({ htmlIds: htmlIds.size, referencedIds: scriptIds.size, missing: 0 }));
