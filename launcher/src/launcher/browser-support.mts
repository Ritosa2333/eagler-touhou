// Browser-support blacklist for the launch gate.
//
// Only vendor-specific UA tokens are matched here, never full UA strings, so
// unrelated browsers and future versions of the listed browsers stay
// unaffected. A match only raises a warning the player can still dismiss.
const discouragedBrowserTokens: ReadonlyArray<readonly [string, RegExp]> = [
  ["baidu", /baiduboxapp/i],
  ["qq", /mqqbrowser/i],
  ["quark", /quark/i],
  ["toutiao", /newsarticle/i],
  ["toutiao-huawei", /ttwebview/i],
  ["oppo", /heytapbrowser/i],
  ["vivo", /vivobrowser/i],
  ["sogou", /sogoumobilebrowser/i],
  ["uc", /ucbrowser/i],
];

export function discouragedBrowserId(userAgent: string): string | null {
  for (const [id, token] of discouragedBrowserTokens) {
    if (token.test(userAgent)) return id;
  }
  return null;
}
