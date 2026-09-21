import assert from "node:assert/strict";
import { discouragedBrowserId } from "../.cache/build/browser/assets/launcher/browser-support.mjs";

// One representative UA per vendor token. Detection is token-based, so these
// only pin the feature markers, not the surrounding version numbers.
const blacklisted = [
  ["baidu", "Mozilla/5.0 (Linux; Android 16; PLR110 Build/BP2A.250605.015; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/97.0.4692.98 Mobile Safari/537.36 T7/15.74 SP-engine/3.62.0 bd_dvt/0 *baiduboxapp/15.76.0.10* (Baidu; P1 16) NABar/1.0"],
  ["qq", "Mozilla/5.0 (Linux; U; Android 16; en-us; PLR110 Build/BP2A.250605.015) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/151.0.7922.199 *MQQBrowser/20.6* Mobile Safari/537.36"],
  ["quark", "Mozilla/5.0 (Linux; U; Android 16; en-US; PLR110 Build/BP2A.250605.015) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/100.0.4896.58 *Quark/10.18.2.1157* Mobile Safari/537.36"],
  ["toutiao", "Mozilla/5.0 (iPhone; CPU iPhone OS 15_4_1 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Mobile/15E148 *NewsArticle/8.7.6.20* JsSdk/2.0 NetType/WIFI (News 8.7.6 15.400000)"],
  ["toutiao-huawei", "Mozilla/5.0 (Linux; Android 10; ELE-AL00 Build/HUAWEIELE-AL00; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/75.0.3770.156 Mobile Safari/537.36 *TTWebView/0751130035403*"],
  ["oppo", "Mozilla/5.0 (Linux; U; Android 11; zh-cn; PDRM00 Build/RKQ1.200903.002) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/70.0.3538.80 Mobile Safari/537.36 *HeyTapBrowser/40.7.27.2*"],
  ["vivo", "Mozilla/5.0 (Linux; Android 8.1.0; vivo X20A Build/OPM1.171019.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/87.0.4280.141 Mobile Safari/537.36 *VivoBrowser/10.8.16.0*"],
  ["sogou", "Mozilla/5.0 (Linux; Android 10; G0515D Build/QKQ1.200913.002; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/68.0.3440.106 Mobile Safari/537.36 AWP/2.0 SogouMSE,*SogouMobileBrowser/5.23.57*"],
  ["uc", "Mozilla/5.0 (Linux; U; Android 11; zh-CN; M2011K2C Build/RKQ1.200928.002) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/78.0.3904.108 *UCBrowser/13.8.0.1161* Mobile Safari/537.36"],
];

const allowed = [
  ["chrome", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36"],
  ["firefox", "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:121.0) Gecko/20100101 Firefox/121.0"],
  ["safari-ios", "Mozilla/5.0 (iPhone; CPU iPhone OS 17_0 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/17.0 Mobile/15E148 Safari/604.1"],
  ["edge", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36 Edg/120.0.0.0"],
  ["samsung", "Mozilla/5.0 (Linux; Android 14; SM-S918B) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Mobile Safari/537.36 SamsungBrowser/23.0"],
  ["miui", "Mozilla/5.0 (Linux; U; Android 13; zh-cn) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/100.0.0.0 Mobile Safari/537.36 XiaoMi/MiuiBrowser/17.3.8"],
  ["huawei", "Mozilla/5.0 (Linux; U; Android 12) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/100.0.0.0 HuaweiBrowser/13.0.0.302 Mobile Safari/537.36"],
];

for (const [id, ua] of blacklisted) {
  assert.equal(discouragedBrowserId(ua), id, `${id}: vendor token must match`);
}
for (const [name, ua] of allowed) {
  assert.equal(discouragedBrowserId(ua), null, `${name}: must not be blacklisted`);
}
assert.equal(discouragedBrowserId(""), null);

console.log(JSON.stringify({ browserSupport: "PASS", blacklisted: blacklisted.length, allowed: allowed.length }));
