import test from 'node:test';
import assert from 'node:assert/strict';
import {execFileSync,spawnSync} from 'node:child_process';
import {resolve} from 'node:path';

const root=resolve(import.meta.dirname,'../..');
function plan(...args){return JSON.parse(execFileSync(process.execPath,
 ['portable/build.mjs',...args,'--print-plan'],{cwd:root,encoding:'utf8',windowsHide:true}));}

test('fixture setup never enters the ordinary or production multiplayer source set',()=>{
 const ordinary=plan(),production=plan('--multiplayer');
 const fixture=plan('--multiplayer','--multiplayer-fixtures');
 for(const value of [ordinary,production])
  assert(!value.sources.some(path=>path.endsWith('/FixtureExports.cpp')));
 assert.equal(ordinary.variant,'normal');
 assert.equal(production.profile,'multiplayer');
 assert.equal(fixture.variant,'multiplayer');
 assert.equal(fixture.profile,'multiplayer-fixtures');
 assert.notEqual(fixture.outputDirectory,production.outputDirectory);
 assert.deepEqual(fixture.flags,production.flags);
 assert.deepEqual(fixture.sources.filter(path=>!production.sources.includes(path)),
                  ['../portable/multiplayer/FixtureExports.cpp']);
 assert.deepEqual(production.sources.filter(path=>!fixture.sources.includes(path)),[]);
});

test('fixture mode requires the explicit TH10 multiplayer variant',()=>{
 for(const args of [['--multiplayer-fixtures'],['--th08','--multiplayer','--multiplayer-fixtures']]){
  const result=spawnSync(process.execPath,['portable/build.mjs',...args,'--print-plan'],
                        {cwd:root,encoding:'utf8',windowsHide:true});
  assert.notEqual(result.status,0);
  assert.match(result.stderr,/TH10 fixture builds require/);
 }
});
