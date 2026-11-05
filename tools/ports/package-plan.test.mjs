import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp,mkdir,writeFile,rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {resolve} from 'node:path';
import {resolvePackagePlan as th08Plan} from '../../ports/th08-mp/portable/package-eagler.mjs';
import {resolvePackagePlan as th10Plan} from '../../ports/th10-mp/portable/package-eagler.mjs';

test('TH08 and TH10 MP package identities survive common native module names',async()=>{
  const root=await mkdtemp(resolve(tmpdir(),'eagler-package-owner-'));
  try{
    for(const commonModule of [false,true]){
      if(commonModule){await mkdir(resolve(root,'web/cpp/game'),{recursive:true});await writeFile(resolve(root,'web/cpp/game/AnmRenderer.cpp'),'// unrelated module name');}
      assert.equal(th08Plan(root,['--multiplayer']).game,'th08');
      assert.equal(th10Plan(root,['--multiplayer']).game,'th10');
      assert.equal(th08Plan(root,['--multiplayer']).variant,'multiplayer');
      assert.equal(th10Plan(root,['--multiplayer']).variant,'multiplayer');
    }
  }finally{await rm(root,{recursive:true,force:true});}
});
