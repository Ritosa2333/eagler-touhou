import assert from 'node:assert/strict';
import { select } from './test-shared-font-selection.mjs';
import {pathToFileURL} from 'node:url';
import {workspacePath} from '../lib/workspace-layout.mjs';
const host = process.env.EAGLER_TH11_HOST_MODULE || pathToFileURL(workspacePath('th11', 'th11_web/sdl-runtime/eagler-host.mjs')).href;
const {resourcePath, installResources} = await import(host);
assert.equal(resourcePath('/msgothic.ttc', 'th11'), true, 'TH11 accepts its shared original-text font');
const installed = [];
for (const language of ['ja', 'lang_zh-hans']) {
 const paths = await select({requiredShared: ['/msgothic.ttc','/unifont.otf']}, language, true);
 await installResources({FS: {mkdirTree() {}, writeFile: path => installed.push(path)}}, paths.map(path => ({path, url: 'https://mirror.invalid/font.otf', size: 2})), {game: 'th11', base: 'https://mirror.invalid/', fetcher: async () => new Response(new Uint8Array([1, 2]))});
}
assert.deepEqual(installed, ['/msgothic.ttc','/unifont.otf','/msgothic.ttc','/unifont.otf'], 'Japanese and localized selections satisfy the real TH11 resource installer');
console.log('TH11 shared-font Runtime installation: PASS');
