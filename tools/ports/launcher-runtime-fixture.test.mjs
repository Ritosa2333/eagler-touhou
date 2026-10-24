import test from 'node:test';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {mkdtemp,mkdir,writeFile,rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {resolve} from 'node:path';
import {prepareLauncherRuntimeFixture,verifyLauncherFixtureMusic,verifyLauncherFixtureSources} from './launcher-runtime-fixture.mjs';
import {writeReleaseManifest,verifyReleaseManifest} from '../../launcher/lib/release-manifest.mjs';

test('produced Release Manifest must retain common and selected port sourcePath bindings',async()=>{
  const root=await mkdtemp(resolve(tmpdir(),'launcher-source-artifact-')),site=resolve(root,'site');
  try{
    await mkdir(resolve(root,'launcher/config'),{recursive:true});await mkdir(site);
    const repositories={launcher:'launcher',common:'common',th08:'ports/th08'};
    await writeFile(resolve(root,'launcher/config/workspace.json'),JSON.stringify({repositories}));
    const payload=Buffer.from('closed runtime bytes');await writeFile(resolve(site,'runtime.wasm'),payload);
    await writeFile(resolve(site,'deployment.json'),JSON.stringify({files:[{path:'runtime.wasm',bytes:payload.length,sha256:createHash('sha256').update(payload).digest('hex')}]}));
    const sources=Object.fromEntries(Object.values(repositories).map(sourcePath=>[sourcePath,{sourcePath,revision:'a'.repeat(40),sha256:'b'.repeat(64)}]));
    const write=sources=>writeReleaseManifest(site,{profile:'fixture',sources});
    await write(sources);
    const result=await verifyLauncherFixtureSources({site,root,game:'th08'});
    assert.equal(result.sources.common.sourcePath,'common');assert.equal(result.sources.th08.sourcePath,'ports/th08');
    const absent=structuredClone(sources);delete absent.common;await write(absent);
    await verifyReleaseManifest(site); // Valid artifact hashes alone do not prove the required owner.
    await assert.rejects(verifyLauncherFixtureSources({site,root,game:'th08'}),/common: packaged source identity is missing/);
    const rebound=structuredClone(sources);rebound.common.sourcePath='ports/th08';await write(rebound);
    await verifyReleaseManifest(site);
    await assert.rejects(verifyLauncherFixtureSources({site,root,game:'th08'}),/common: packaged sourcePath differs/);
    const portAbsent=structuredClone(sources);delete portAbsent['ports/th08'];await write(portAbsent);
    await assert.rejects(verifyLauncherFixtureSources({site,root,game:'th08'}),/th08: packaged source identity is missing/);
  }finally{await rm(root,{recursive:true,force:true});}
});

test('Launcher fixture rejects Practice requests outside its selected product before any preparation',async()=>{
  const options={root:resolve(import.meta.dirname,'../..'),output:resolve(tmpdir(),'unused-practice-fixture'),target:{game:'th08',variant:'normal'}};
  await assert.rejects(prepareLauncherRuntimeFixture({...options,practiceFeatures:{th06:true}}),/selected fixture product/);
  await assert.rejects(prepareLauncherRuntimeFixture({...options,practiceFeatures:{th08:'true'}}),/must be boolean/);
  await assert.rejects(prepareLauncherRuntimeFixture({...options,practiceFeatures:{absent:true}}),/Unknown Practice feature product/);
});

test('Launcher OGG declaration binds private source bytes to the real imported component',async()=>{
  const root=await mkdtemp(resolve(tmpdir(),'launcher-music-fixture-'));
  try{
    const sourceDirectory=resolve(root,'games/web-content/th08/bgm-ogg');await mkdir(sourceDirectory,{recursive:true});
    const bytes=Buffer.from('registered OGG fixture'),name='th08_01.ogg',path=resolve(sourceDirectory,name);
    await writeFile(path,bytes);
    const sha256=createHash('sha256').update(bytes).digest('hex');
    const input={sourceDirectory,mount:'/bgm-ogg',files:[{name,path,bytes:bytes.length,sha256}]};
    const descriptor={components:{ogg:{type:'ogg',files:['ogg:'+name]}},
      files:{['ogg:'+name]:{source:'music/ogg/'+name,target:'/bgm-ogg/'+name,bytes:bytes.length,sha256}}};
    assert.deepEqual(await verifyLauncherFixtureMusic({game:'th08',input,descriptor}),input);
    for(const change of [
      d=>d.components.ogg.files=[],d=>d.files['ogg:'+name].sha256='f'.repeat(64),
      d=>d.files['ogg:'+name].target='/different/'+name,
    ]){
      const changed=structuredClone(descriptor);change(changed);
      await assert.rejects(verifyLauncherFixtureMusic({game:'th08',input,descriptor:changed}));
    }
    await writeFile(path,Buffer.from('changed source bytes'));
    await assert.rejects(verifyLauncherFixtureMusic({game:'th08',input,descriptor}),/identity changed/);
    await writeFile(path,bytes);await writeFile(resolve(sourceDirectory,'unregistered.ogg'),bytes);
    await assert.rejects(verifyLauncherFixtureMusic({game:'th08',input,descriptor}),/inventory changed/);
  }finally{await rm(root,{recursive:true,force:true});}
});
