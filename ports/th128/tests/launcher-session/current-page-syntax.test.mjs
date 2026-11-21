import test from 'node:test';
import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';
test('complete changed browser modules parse without evaluating the game or graphics',()=>{
 for(const name of ['site/play.mjs','site/import-entry.mjs','site/runtime/page-launch-context.mjs']){
  const file=fileURLToPath(new URL('../../'+name,import.meta.url)),result=spawnSync(process.execPath,['--check',file],{encoding:'utf8',windowsHide:true});
  assert.equal(result.status,0,result.stderr);assert.equal(result.stdout,'');assert.equal(result.stderr,'');
 }
});
