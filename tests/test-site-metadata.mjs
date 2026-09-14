import assert from "node:assert/strict";
import { mkdtemp, readFile, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join, resolve } from "node:path";
import { normalizeSiteUrl, writeSiteMetadata } from "../lib/site-metadata.mjs";

assert.equal(normalizeSiteUrl("https://touhou.vip/"), "https://touhou.vip/");
assert.throws(() => normalizeSiteUrl("https://touhou.vip/path"), /directory URL/);
assert.throws(() => normalizeSiteUrl("javascript:alert(1)"), /http/);

const root = await mkdtemp(join(tmpdir(), "eagler-site-metadata-"));
try {
  const shell = locale => `<!doctype html><html lang="${locale}"><head><!-- site-metadata:start --><!-- site-metadata:end --></head><body></body></html>`;
  await Promise.all([
    writeFile(resolve(root, "index.html"), shell("zh-CN")),
    writeFile(resolve(root, "en.html"), shell("en")),
    writeFile(resolve(root, "faq.html"), shell("zh-CN")),
    writeFile(resolve(root, "about.html"), shell("zh-CN")),
  ]);
  await writeSiteMetadata(root, "https://touhou.vip/");
  const [zh, en, robots, sitemap] = await Promise.all([
    readFile(resolve(root, "index.html"), "utf8"),
    readFile(resolve(root, "en.html"), "utf8"),
    readFile(resolve(root, "robots.txt"), "utf8"),
    readFile(resolve(root, "sitemap.xml"), "utf8"),
  ]);
  assert.match(zh, /rel="canonical" href="https:\/\/touhou\.vip\/"/);
  assert.match(en, /rel="canonical" href="https:\/\/touhou\.vip\/en\.html"/);
  assert.match(zh, /hreflang="en" href="https:\/\/touhou\.vip\/en\.html"/);
  assert.match(en, /hreflang="zh-CN" href="https:\/\/touhou\.vip\/"/);
  assert.match(robots, /Sitemap: https:\/\/touhou\.vip\/sitemap\.xml/);
  assert.match(sitemap, /xmlns:xhtml=/);
  assert.equal((zh.match(/rel="canonical"/g) || []).length, 1);
  console.log(JSON.stringify({ siteMetadata: "PASS", languages: ["zh-CN", "en"], sitemap: true }));
} finally {
  await rm(root, { recursive: true, force: true });
}
