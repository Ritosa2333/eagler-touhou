import assert from 'node:assert/strict';
import { select } from './test-shared-font-selection.mjs';
const host = process.env.EAGLER_TH11_HOST_MODULE || new URL('../../th11-eagler/th11_web/sdl-runtime/eagler-host.mjs', import.meta.url).href;
const {resourcePath, installResources} = await import(host);
assert.equal(resourcePath('/msgothic.ttc', 'th11'), false, 'Reproduce the reported TH11 path rejection');
const installed = [];
for (const language of ['ja', 'lang_zh-hans']) {
 const paths = await select({requiredShared: ['/unifont.otf']}, language, true);
 await installResources({FS: {mkdirTree() {}, writeFile: path => installed.push(path)}}, paths.map(path => ({path, url: 'https://mirror.invalid/font.otf', size: 2})), {game: 'th11', base: 'https://mirror.invalid/', fetcher: async () => new Response(new Uint8Array([1, 2]))});
}
assert.deepEqual(installed, ['/unifont.otf', '/unifont.otf'], 'Japanese and localized selections satisfy the real TH11 resource installer');
console.log('TH11 shared-font Runtime installation: PASS');
