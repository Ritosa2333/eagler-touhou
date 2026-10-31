import {test} from 'vitest';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {parse} from 'parse5';
import {resolveFrontendPackageSource} from '../lib/frontend-manifest.mjs';

test("test-dom-contract", async () => {
/** Static React document identity. Mounted UI ownership is checked by the rewrite suite. */




for(const file of ['index.html','en.html','lobby.html']) {
 const html=await readFile(resolveFrontendPackageSource(file),'utf8');
 const ids=[];const visit=node=>{for(const a of node.attrs??[])if(a.name==='id')ids.push(a.value);for(const child of node.childNodes??[])visit(child);};visit(parse(html));
 assert.equal(ids.length,new Set(ids).size,`${file}: duplicate initial document IDs`);
 assert.match(html,/window\.__reactRouterContext/);
 assert.doesNotMatch(html,/src=["'](?:\.\/)?app\.js["']/);
 assert.doesNotMatch(html,/assets\/launcher\/(?:app|lobby)\.mjs/);
}
console.log('React entry documents: unique IDs and one hydration owner PASS');
});
