import test from 'node:test';
import assert from 'node:assert/strict';
import {resolve} from 'node:path';
import {existsSync} from 'node:fs';
import {runtimeSourceOwner} from '../../launcher/tests/support/runtime-source-owner.mjs';
const root=resolve(import.meta.dirname,'../..');
for(const game of ['th08','th10','th11','th15'])test(`${game} ordinary and MP source stay bound on integrated main`,{skip:!existsSync(resolve(root,`runtimes/${game}`))},()=>{
  const ordinary=resolve(root,`runtimes/${game}`),mp=resolve(root,`runtimes/${game}-mp`);
  assert.equal(runtimeSourceOwner({source:ordinary,game}).component,`runtimes/${game}`);
  assert.equal(runtimeSourceOwner({source:mp,game,multiplayer:true}).component,`runtimes/${game}-mp`);
  assert.throws(()=>runtimeSourceOwner({source:mp,game}),/requires declared source/);
  assert.throws(()=>runtimeSourceOwner({source:ordinary,game,multiplayer:true}),/requires declared source/);
});
