// Verify the actual linked binaries, not only the build planner's source list.
import assert from 'node:assert/strict';
import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';

const root=resolve(import.meta.dirname,'../..');
const sha=path=>createHash('sha256').update(readFileSync(path)).digest('hex');
const report={passed:false,profiles:[]};
const output=resolve(root,'artifacts/multiplayer-tests/build-isolation.json');
try{
 for(const profile of ['sdl3','multiplayer','multiplayer-fixtures']){
  const folder=resolve(root,'th10_web/artifacts',profile);
  const build=JSON.parse(readFileSync(resolve(folder,'build.json')));
  const wasm=resolve(folder,'th10-sdl.wasm');
  assert.equal(build.profile,profile);assert.equal(sha(wasm),build.sha256);
  assert.equal(sha(resolve(folder,'th10-sdl.mjs')),build.loaderSha256);
  for(const [path,hash] of Object.entries(build.sourceFiles))
   assert.equal(sha(resolve(root,path)),hash,'Source/build mismatch: '+path);
  const names=WebAssembly.Module.exports(new WebAssembly.Module(readFileSync(wasm))).map(e=>e.name);
  const fixtures=names.filter(name=>name.startsWith('mp_fixture_'));
  const multiplayer=names.filter(name=>name.startsWith('multiplayer_'));
  if(profile==='multiplayer-fixtures'){
   for(const name of ['mp_fixture_prepare','mp_fixture_draw_state_oracle','mp_fixture_collision_oracle'])
    assert(fixtures.includes(name),'Missing fixture oracle '+name);
  }else assert.deepEqual(fixtures,[],'Fixture control exported by '+profile);
  assert.equal(multiplayer.length>0,profile!=='sdl3');
  report.profiles.push({profile,sha256:build.sha256,sourceDigest:build.sourceDigest,
   fixtureExports:fixtures,multiplayerExportCount:multiplayer.length});
 }
 report.passed=true;
}catch(error){report.error=String(error);process.exitCode=1;}
finally{
 mkdirSync(resolve(root,'artifacts/multiplayer-tests'),{recursive:true});
 writeFileSync(output,JSON.stringify(report,null,2)+'\n');
 console.log(JSON.stringify({...report,report:output}));
}
