import {test} from 'vitest';
import assert from "node:assert/strict";
import {
  SITE_NOTICE_DISMISSED_KEY,
  SITE_NOTICE_DURATION_MS,
  SITE_NOTICE_STORAGE_KEY,
  parseSiteNoticeText,
  siteNoticeBrandAsset,
} from "../.cache/build/browser/assets/launcher/site-notice-content.mjs";

test("test-site-notice", async () => {
const baseUrl = "https://test.example/eagler-touhou/";
assert.ok(Number.isSafeInteger(SITE_NOTICE_DURATION_MS) && SITE_NOTICE_DURATION_MS > 0,
  "notice countdown must have a finite positive duration");
assert.equal(siteNoticeBrandAsset("https://cloud.touhou.best/", baseUrl), "assets/notice-touhou-cloud.png",
  "the permission-cleared 车万云 provider mark must remain available offline");
assert.equal(siteNoticeBrandAsset("https://space.bilibili.com/1", baseUrl), "assets/notice-bilibili.svg");
assert.equal(siteNoticeBrandAsset("https://github.com/example/repo", baseUrl), "assets/notice-github.svg");
assert.equal(siteNoticeBrandAsset("https://qm.qq.com/q/example", baseUrl), "assets/notice-qq.svg");
assert.equal(siteNoticeBrandAsset("faq.html", baseUrl), "assets/favicon.ico");
assert.equal(siteNoticeBrandAsset("https://example.org/", baseUrl), "");

const parsed = parseSiteNoticeText(
  "反馈请看 [FAQ](faq.html) 或 [GitHub](https://github.com/example/repo)\n不要执行 [bad](javascript:alert(1))",
  baseUrl,
);
assert.equal(parsed.length, 2);
assert.deepEqual(parsed[0].filter(segment => segment.type === "link").map(segment => ({
  label: segment.label,
  href: segment.href,
  external: segment.external,
  asset: segment.asset,
})), [
  { label: "FAQ", href: "faq.html", external: false, asset: "assets/favicon.ico" },
  { label: "GitHub", href: "https://github.com/example/repo", external: true, asset: "assets/notice-github.svg" },
]);
assert.equal(parsed[1].map(segment => segment.type === "text" ? segment.text : segment.label).join(""),
  "不要执行 [bad](javascript:alert(1))", "non-http links must remain inert text");
assert.deepEqual(parseSiteNoticeText("[车万云](https://cloud.touhou.best/)", baseUrl)[0], [{
  type: "link",
  label: "车万云",
  href: "https://cloud.touhou.best/",
  resolvedHref: "https://cloud.touhou.best/",
  external: true,
  asset: "assets/notice-touhou-cloud.png",
}]);

console.log('Shared notice parser and link policy: PASS');
});
