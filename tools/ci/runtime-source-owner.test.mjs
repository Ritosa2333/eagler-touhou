import test from 'node:test';
import assert from 'node:assert/strict';
import {resolve} from 'node:path';
import {runtimeSourceOwner} from '../../launcher/tests/support/runtime-source-owner.mjs';
const root=resolve(import.meta.dirname,'../..');
for(const game of ['th08','th10','th11','th15'])test(`${game} ordinary and MP source stay bound on integrated main`,()=>{
  const ordinary=resolve(root,`ports/${game}`),mp=resolve(root,`ports/${game}-mp`);
  assert.equal(runtimeSourceOwner({source:ordinary,game}).component,`ports/${game}`);
  assert.equal(runtimeSourceOwner({source:mp,game,multiplayer:true}).component,`ports/${game}-mp`);
  assert.throws(()=>runtimeSourceOwner({source:mp,game}),/requires declared source/);
  assert.throws(()=>runtimeSourceOwner({source:ordinary,game,multiplayer:true}),/requires declared source/);
});
