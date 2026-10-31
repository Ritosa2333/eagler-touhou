import {createSharedFontSelector} from './support/shared-font-selection.mjs';
import {test} from 'vitest';
import assert from 'node:assert/strict';

test("test-shared-font-selection", async () => {
const {select,dispose}=await createSharedFontSelector();
try {
assert.deepEqual(await select({requiredShared:['/unifont.otf']}),[],'Legacy baked-font packages remain supported');
assert.deepEqual(await select({requiredShared:['/unifont.otf']},'ja',true),['/unifont.otf']);
assert.deepEqual(await select({requiredShared:['/unifont.otf']},'lang_zh-hans'),['/unifont.otf']);
assert.deepEqual(await select({requiredShared:['/msgothic.ttc','/unifont.otf']},'ja',true),['/msgothic.ttc','/unifont.otf']);
assert.deepEqual(await select({requiredShared:['/msgothic.ttc','/unifont.otf']},'ja'),['/msgothic.ttc']);
assert.deepEqual(await select({requiredShared:['/msgothic.ttc','/unifont.otf']},'lang_zh-hans'),['/msgothic.ttc','/unifont.otf'],'Localized games still need their original-text fallback font');
assert.deepEqual(await select({requiredShared:['/msgothic.ttc','/unifont.otf']},'lang_en',true,['/msgothic.ttc','/unifont.otf']),[],'Installed shared fonts must not download twice');
assert.deepEqual(await select({},'ja'),['/msgothic.ttc'],'Legacy Runtime selection remains supported');
assert.deepEqual(await select({requiredShared:[]}),[]);
assert.deepEqual(await select({requiredShared:['/unifont.otf']},'ja',true,['/unifont.otf']),[],'Installed font must not download twice');
console.log('Shared font selection: Japanese baked fonts, practice, localization, legacy and installed-font cases PASS');
} finally {await dispose();}
});
