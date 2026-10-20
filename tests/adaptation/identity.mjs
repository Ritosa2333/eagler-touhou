import assert from 'node:assert/strict';
import {readdir,readFile,stat} from 'node:fs/promises';
import {resolve} from 'node:path';
import {dependencySnapshot,fileIdentity,identityDigest} from '../../tools/testing/dependency-snapshot.mjs';
import {verifyPrivateData,verifyPublishedRuntime,verifyRuntimePackage} from '../../tools/ports/acceptance-contracts.mjs';
import {loadRuntimeReplayFixture} from '../../tools/ports/replay-fixtures.mjs';
import {verifyMusicInputs} from '../../tools/ports/music-browser-helpers.mjs';

/** One preflight cache per invocation; a new reader performs an independent postflight. */
export function createCaseIdentityReader({root,environment}){
  const cache=new Map();
  const once=(key,operation)=>{if(!cache.has(key))cache.set(key,Promise.resolve().then(operation));return cache.get(key);};
  const file=path=>once(`file:${resolve(root,path)}`,()=>fileIdentity(resolve(root,path)));
  const json=path=>once(`json:${resolve(root,path)}`,async()=>JSON.parse(await readFile(resolve(root,path),'utf8')));
  const tree=async path=>{
    const rows=[];
    for(const entry of await readdir(path,{withFileTypes:true})){
      const child=resolve(path,entry.name);
      if(entry.isDirectory())for(const row of await tree(child))rows.push({...row,path:`${entry.name}/${row.path}`});
      else{assert.ok(entry.isFile(),`Preparation input must be a regular file: ${child}`);rows.push({path:entry.name,...await file(child)});}
    }
    return rows.sort((a,b)=>a.path.localeCompare(b.path,'en'));
  };
  const toolIdentity=desktop=>once(`browser-tools:${desktop}`,async()=>{
    const {resolveChromiumExecutable}=await import('../../tools/browser/launch.mjs');
    const {loadPlaywright}=await import('../../tools/browser/playwright.mjs');
    const {identifyPlaywrightSources,resolveFullChromeExecutable}=await import('../../tools/browser/cdp-focus-gateway.mjs');
    const executable=desktop?await resolveFullChromeExecutable((await loadPlaywright()).chromium,{env:environment}):
      environment.PLAYWRIGHT_CHROMIUM_EXECUTABLE??resolveChromiumExecutable();
    assert.ok(executable,'Chromium executable was not found');
    const playwright=await identifyPlaywrightSources({workspaceRoot:root});
    const implementationDigest=identityDigest(await tree(resolve(playwright.packageRoot,'lib')));
    return{node:process.versions.node,platform:process.platform,arch:process.arch,
      executable:{path:executable,...await file(executable)},playwright,implementationDigest,
      flags:Object.fromEntries(['NATIVE_GPU','PIXEL_GPU','PLAYWRIGHT_BROWSERS_PATH'].map(name=>[name,environment[name]??null]))};
  });
  const product=async target=>once(`product:${target.id}`,async()=>{
    const {replay,...configuration}=target;
    return{target:configuration,packageDigest:identityDigest(await verifyRuntimePackage(target,{repoRoot:root})),
      data:await verifyPrivateData(target.data)};
  });
  return async testCase=>{
    const manifestPath=environment.EAGLER_TEST_RUNTIME_MANIFEST;
    assert.ok(manifestPath,'Required input: EAGLER_TEST_RUNTIME_MANIFEST');
    const manifest=await json(manifestPath),isOffline=testCase.task.args[0]?.endsWith('offline-runtime-browser.mjs');
    const targets=(manifest.targets??[]).filter(target=>target.game===testCase.game&&(isOffline||target.variant===testCase.variant));
    assert.ok(targets.some(target=>target.variant===testCase.variant),`Missing Runtime target ${testCase.game}/${testCase.variant}`);
    assert.equal(new Set(targets.map(target=>target.variant)).size,targets.length,'Duplicate selected Runtime variant');
    const products=await Promise.all(targets.map(product));
    const shared=Object.fromEntries(await Promise.all(Object.entries(manifest.shared??{}).map(async([name,path])=>[name,{path,...await file(path)}])));
    const dependencies=await once(`dependencies:${testCase.task.args[0]}`,()=>dependencySnapshot(root,[
      testCase.task.args[0],'tests/run.mjs','tests/adaptation/catalog.mjs','tests/adaptation/execution.mjs',
      'tests/adaptation/results.mjs','tests/adaptation/identity.mjs','tools/testing/runner.mjs','tools/testing/supervise.py',
      'launcher/package.json','launcher/package-lock.json','docs/adaptation/checklist.json',
      'launcher/src/launcher/gamepad-mapping.mts','launcher/app/services/player-gamepad.ts','launcher/tests/fixtures/virtual-gamepads.mjs',
      ...(isOffline?['launcher/scripts/build-launcher.mjs']:[]),
    ]));
    const inputs={products,shared};
    if(isOffline)inputs.launcherSources=await once('launcher-sources',async()=>{
      const launcher=resolve(root,'launcher'),directories=await Promise.all(
        ['app','src','public','lib','scripts','config'].map(async directory=>[directory,identityDigest(await tree(resolve(launcher,directory)))]));
      const rootFiles=await Promise.all((await readdir(launcher,{withFileTypes:true}))
        .filter(entry=>entry.isFile()&&/\.(?:[cm]?[jt]s|jsonc?|css|html)$/.test(entry.name))
        .map(async entry=>[entry.name,await file(resolve(launcher,entry.name))]));
      return{directories:Object.fromEntries(directories),rootFiles:Object.fromEntries(rootFiles)};
    });
    if(/(?:runtime-browser|replay-browser|offline-runtime-browser)\.mjs$/.test(testCase.task.args[0]))
      inputs.replays=await Promise.all(targets.filter(target=>!isOffline||target.variant==='normal').map(target=>
        once(`replay:${target.id}`,async()=>(await loadRuntimeReplayFixture(target)).identity)));
    const names=testCase.inputs??testCase.requiredInputs??[];
    for(const name of names){
      const value=environment[name];if(!value||name==='EAGLER_TEST_RUNTIME_MANIFEST')continue;
      if(name==='EAGLER_TEST_PACKAGES'){
        const mapping=await json(value),path=mapping[testCase.game];
        assert.ok(path,`${testCase.game}: offline package is missing`);inputs.packages={path,...await file(path)};
      }else if(name==='EAGLER_TEST_UPDATE_PACKAGES'){
        const updates=await json(value),entry=updates.games?.[testCase.game];
        if(entry){inputs.update={entry};for(const [kind,item] of Object.entries(entry))if(item?.path)inputs.update[kind]={path:item.path,...await file(item.path)};}
      }else if(name==='EAGLER_TEST_MUSIC_MANIFEST'){
        inputs.music=await once(`music:${testCase.game}:${value}`,()=>json(value).then(config=>verifyMusicInputs(config,{repoRoot:root,games:[testCase.game]})));
      }else if(name==='EAGLER_TEST_ARTWORK')inputs.artwork=await once(`tree:${value}`,()=>tree(value));
      else if(name==='EAGLER_TEST_LAUNCHER_URL'){
        const target=targets.find(item=>item.variant==='multiplayer');
        inputs.launcher=await once(`published:${testCase.game}:${value}`,()=>verifyPublishedRuntime(value,testCase.game,'multiplayer',
          {expectedPackageSha256:target.packageSha256}));
      }else inputs[name]=await file(value);
    }
    const identity={version:1,game:testCase.game,variant:testCase.variant,group:testCase.group,checks:testCase.checks,
      command:testCase.task.command,args:testCase.task.args.map(arg=>arg.startsWith('--output=')?'--output=<CASE>':arg),
      dependencies,inputs,tools:await toolIdentity(testCase.group.startsWith('desktop-'))};
    return{fingerprint:identityDigest(identity),identity};
  };
}
