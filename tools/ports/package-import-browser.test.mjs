import test from 'node:test';
import assert from 'node:assert/strict';
import {runPackageImportRoutes} from './package-import-browser.mjs';

test('independent source failures retain their error and continue the remaining queue with owned cleanup',async()=>{
  let opened=0,closed=0,prepared=0;const saves=[];
  const browser={newContext:async()=>{opened++;return{newPage:async()=>({isClosed:()=>true}),close:async()=>{closed++;}};}};
  const evidence=await runPackageImportRoutes({browser,game:'th08',localBase:'https://zip.test/',onlineBase:'https://online.test/',originalBase:'https://raw.test/',
    setupPage:async()=>{prepared++;throw Error('preparation rejected');},drainPage:async()=>{},save:async e=>saves.push(structuredClone(e))});
  assert.equal(opened,3);assert.equal(closed,3);assert.equal(prepared,3);assert.equal(saves.length,3);
  assert.deepEqual(evidence.routes.map(row=>row.source),['zip','online','original-file']);
  assert.ok(evidence.routes.every(row=>row.status==='failed'&&row.failure.includes('preparation rejected')));assert.deepEqual(evidence.cleanupErrors,[]);
});
