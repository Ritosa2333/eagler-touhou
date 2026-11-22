import assert from "node:assert/strict";
import test from "node:test";
import { mergeSettingIni, setIniValue } from "../../wwwroot/host/storage/setting-ini.mjs";

test("configure volumes and the 60 Hz switch land in Setting.INI", () => {
  const text = mergeSettingIni("", {
    bgmVolume: 40,
    sfxVolume: 15,
    limitPresentationTo60: false,
  });
  assert.match(text, /BGMVolume=40/);
  assert.match(text, /SEVolume=15/);
  assert.match(text, /UnlockFPS=1/);
  assert.match(text, /FullWindow=0/);
  assert.match(text, /UseDirectInput=0/);
});

test("an existing clear flag and unmapped keys stay put", () => {
  const prior = setIniValue(mergeSettingIni("", {}), "Mode", "Clear", "yes");
  const text = mergeSettingIni(prior, {
    limitPresentationTo60: true,
    keys: { Shoot: "C", Left: "not a key" },
    fontType: "SimHei",
  });
  assert.match(text, /Clear=yes/);
  assert.match(text, /UnlockFPS=0/);
  assert.match(text, /Shoot=C/);
  assert.match(text, /Left=Left/);
  assert.match(text, /FontType=SimHei/);
});
