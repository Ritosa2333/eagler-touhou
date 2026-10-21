import assert from 'node:assert/strict';
import {readdir,readFile,stat} from 'node:fs/promises';
import {dirname,isAbsolute,relative,resolve} from 'node:path';
import {dependencySnapshot,fileIdentity,identityDigest} from '../../tools/testing/dependency-snapshot.mjs';
import {selectOfflineRuntimeTargets,verifyPrivateData,verifyPublishedRuntime,verifyRuntimePackage} from '../../tools/ports/acceptance-contracts.mjs';
import {loadRuntimeReplayFixture} from '../../tools/ports/replay-fixtures.mjs';
import {verifyMusicInputs} from '../../tools/ports/music-browser-helpers.mjs';
import {desktopExecutionBrowserMode,selectDesktopExecutionMode} from '../../tools/ports/runtime-desktop-lifecycle.mjs';
import {inspectDiagnosticProduct,loadDiagnosticInputs} from '../../tools/ports/diagnostic-inputs.mjs';
import {resolveDiagnosticProfile} from './adapters/diagnostics.mjs';
import {selectLanguageInputs,inspectLanguageInputs,languageInputReceipt} from '../../tools/ports/language-inputs.mjs';
import {inspectExistingBuildAcceptance} from '../../tools/ports/build-acceptance.mjs';
import {inspectSourceReview} from '../../tools/ports/adaptation-source-review.mjs';
import {inspectMinimalRuntimeInputs} from '../../tools/ports/minimal-runtime-browser.mjs';

async function identityTree(path,file=fileIdentity,exclude=()=>false){
  const rows=[];
  for(const entry of await readdir(path,{withFileTypes:true})){
    if(exclude(entry.name))continue;
    const child=resolve(path,entry.name);
    if(entry.isDirectory())for(const row of await identityTree(child,file,exclude))rows.push({...row,path:`${entry.name}/${row.path}`});
    else{assert.ok(entry.isFile(),`Preparation input must be a regular file: ${child}`);rows.push({path:entry.name,...await file(child)});}
  }
  return rows.sort((a,b)=>a.path.localeCompare(b.path,'en'));
}

/** Resume binds the selected diagnostic inputs and current build sources, without starting a server. */
export async function diagnosticCaseIdentity({root,manifestPath,game,variant,lanes,profile=resolveDiagnosticProfile(game,variant),pairedReplay=null}){
  assert.ok(profile,`Missing diagnostic profile ${game}/${variant}`);
  assert.ok(Array.isArray(lanes)&&lanes.length&&new Set(lanes).size===lanes.length&&
    lanes.every(lane=>['presentation','quick','daily'].includes(lane)),'Select explicit diagnostic lanes');
  const inside=path=>{
    const absolute=resolve(path),name=relative(root,absolute);
    assert.ok(!isAbsolute(name)&&name!=='..'&&!name.startsWith('../')&&!name.startsWith('..\\'),
      `Diagnostic dependency leaves the repository: ${path}`);
    return absolute;
  };
  const inputs=await loadDiagnosticInputs(resolve(root,manifestPath),{game,variant});
  const files=await inspectDiagnosticProduct(inputs.product,{...profile,lanes,pairedReplay});
  const receiptPath=inside(resolve(root,profile.buildReceipt)),receipt=JSON.parse(await readFile(receiptPath,'utf8'));
  assert.equal(receipt.diagnostic,true,'Diagnostic build receipt must identify an instrumented build');
  assert.ok(receipt.sourceFiles&&Object.keys(receipt.sourceFiles).length,'Diagnostic receipt has no source identity closure');
  const sources=[];
  for(const [name,sha256] of Object.entries(receipt.sourceFiles)){
    const path=inside(resolve(root,profile.sourceRoot,name)),identity=await fileIdentity(path);
    assert.equal(identity.sha256,sha256,`Rebuild modified diagnostic source: ${name}`);
    sources.push({path:relative(root,path).replaceAll('\\','/'),...identity});
  }
  const entries=['serverScript',...lanes.flatMap(lane=>lane==='presentation'?['browserVerifier']:
    lane==='quick'?['quickCapture','quickGate']:['dailyCapture','dailyGate'])].map(key=>profile[key]);
  const presentationTools=await identityTree(inside(resolve(root,profile.presentationDirectory)),fileIdentity,
    name=>name==='__pycache__'||name.endsWith('.pyc'));
  entries.push(...presentationTools.filter(file=>/\.[cm]?[jt]s$/.test(file.path)).map(file=>
    resolve(root,profile.presentationDirectory,file.path)));
  const decisionPaths=new Set();
  for(const lane of lanes){
    const paths=profile.decisionInputs?.[lane]??[];
    assert.ok(Array.isArray(paths),`Diagnostic ${lane} decision inputs must be a file list`);
    for(const path of paths)decisionPaths.add(path);
  }
  const decisionInputs=await Promise.all([...decisionPaths].sort().map(async name=>{
    const path=inside(resolve(root,name));return{path:relative(root,path).replaceAll('\\','/'),...await fileIdentity(path)};
  }));
  return{profile,manifest:{path:inputs.path,selection:{game,variant}},files,
    receipt:await fileIdentity(receiptPath),sources:sources.sort((a,b)=>a.path.localeCompare(b.path,'en')),
    runtime:await identityTree(inside(resolve(root,profile.runtimeDirectory))),
    presentationTools,
    tools:await dependencySnapshot(root,entries),decisionInputs,
    ...((lanes.includes('quick')||lanes.includes('daily'))?
      {golden:await identityTree(inside(dirname(resolve(root,profile.goldenManifest))))}:{}),
  };
}

/** Fingerprint the executable selected by the actual driver flags. */
export function adaptationBrowserMode(testCase){
  const args=testCase.task?.args??[];
  if(!args[0]?.endsWith('runtime-desktop-probes.mjs'))return'headless';
  return desktopExecutionBrowserMode(selectDesktopExecutionMode({
    inputOnly:args.includes('--input-only'),touchOnly:args.includes('--touch-only'),
    actionsOnly:args.includes('--actions-only'),
      replayRecordingOnly:args.includes('--replay-recording-only'),
    backgroundOnly:args.includes('--background-only'),audioLatencyOnly:args.includes('--audio-latency-only'),
  }));
}

export const adaptationBrowserImplementation=testCase=>testCase.task?.args[0]?.endsWith('diagnostic-browser.mjs')?
  'python-playwright':'node-playwright';

/** Hash every installed Python distribution declared by Playwright's dependency metadata. */
export async function fingerprintPythonPlaywrightDependencies(dependencies,identify=fileIdentity){
  assert.ok(Array.isArray(dependencies)&&dependencies.length,'Python Playwright dependency inventory is empty');
  const names=new Set();
  const rows=await Promise.all(dependencies.map(async dependency=>{
    assert.ok(dependency&&typeof dependency.name==='string'&&dependency.name.trim()&&
      typeof dependency.version==='string'&&dependency.version.trim()&&Array.isArray(dependency.files)&&dependency.files.length,
    'Python distributions require a name, version and file inventory');
    const key=dependency.name.toLowerCase().replace(/[-_.]+/g,'-');
    assert.ok(!names.has(key),`Duplicate Python distribution ${dependency.name}`);names.add(key);
    const files=await Promise.all(dependency.files.map(async path=>{
      assert.ok(typeof path==='string'&&isAbsolute(path),`Python distribution file must be absolute: ${path}`);
      return{path:path.replaceAll('\\','/'),...await identify(path)};
    }));
    files.sort((a,b)=>a.path.localeCompare(b.path,'en'));
    return{name:dependency.name,version:dependency.version,files};
  }));
  return rows.sort((a,b)=>a.name.localeCompare(b.name,'en'));
}

/** One preflight cache per invocation; a new reader performs an independent postflight. */
export function createCaseIdentityReader({root,environment,inputOnly=false,inspectBuildEvidence=inspectExistingBuildAcceptance}){
  const cache=new Map();
  const once=(key,operation)=>{if(!cache.has(key))cache.set(key,Promise.resolve().then(operation));return cache.get(key);};
  const file=path=>once(`file:${resolve(root,path)}`,()=>fileIdentity(resolve(root,path)));
  const json=path=>once(`json:${resolve(root,path)}`,async()=>JSON.parse(await readFile(resolve(root,path),'utf8')));
  const tree=path=>identityTree(path,file);
  const toolIdentity=(desktop,implementation)=>once(`browser-tools:${desktop}:${implementation}`,async()=>{
    if(implementation==='python-playwright'){
      const {identifyPythonPlaywright}=await import('../../tools/browser/playwright.mjs');
      const python=identifyPythonPlaywright({environment});
      return{implementation,...python,
        interpreter:{path:python.interpreter,...await file(python.interpreter)},
        driverExecutable:{path:python.driverExecutable,...await file(python.driverExecutable)},
        executable:{path:python.executable,...await file(python.executable)},
        dependencies:await fingerprintPythonPlaywrightDependencies(python.dependencies,file),
        implementationDigest:identityDigest(await identityTree(python.packageRoot,file,
          name=>name==='__pycache__'||name.endsWith('.pyc'))),
      };
    }
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
    if(['source-review','single-game'].includes(testCase.group)){
      const name=testCase.group==='source-review'?'EAGLER_TEST_SOURCE_REVIEW':'EAGLER_TEST_MINIMAL_RUNTIME';
      const manifest=environment[name];assert.ok(manifest,`Required input: ${name}`);
      assert.ok(testCase.task.args.includes(`--manifest=${manifest}`),'Source journey task and selected input manifest differ');
      const selected={manifest,game:testCase.game,variant:testCase.variant};
      const inspected=testCase.group==='source-review'?await inspectSourceReview({...selected,checks:testCase.checks}):
        await inspectMinimalRuntimeInputs(selected);
      if(inputOnly)return{inputPreflight:{status:'passed',targetIds:[`${testCase.game}-${testCase.variant}`],inputs:[name]}};
      const dependencies=await dependencySnapshot(root,[testCase.task.args[0],
        'tests/adaptation/catalog.mjs','tests/adaptation/results.mjs','tests/adaptation/complete-evidence.mjs',
        'tests/adaptation/identity.mjs','tools/testing/runner.mjs']);
      const identity={version:1,game:testCase.game,variant:testCase.variant,group:testCase.group,checks:testCase.checks,
        command:testCase.task.command,args:testCase.task.args.map(arg=>arg.startsWith('--output=')?'--output=<CASE>':arg),
        dependencies,inputs:inspected.inputs};
      return{fingerprint:identityDigest(identity),identity};
    }
    if(testCase.group==='build'){
      const manifestPath=environment.EAGLER_TEST_RUNTIME_MANIFEST;
      assert.ok(typeof manifestPath==='string'&&manifestPath.trim(),'Required input: EAGLER_TEST_RUNTIME_MANIFEST');
      const option=testCase.task?.args?.find(argument=>argument.startsWith('--runtime-manifest='));
      assert.ok(option,'Build evidence task has no Runtime manifest argument');
      assert.equal(resolve(root,option.slice('--runtime-manifest='.length)),resolve(root,manifestPath),
        'Build evidence task and selected Runtime manifest differ');
      const buildEvidence=await inspectBuildEvidence({root,manifestPath,game:testCase.game,variant:testCase.variant});
      if(inputOnly)return{inputPreflight:{status:'passed',targetIds:[`${testCase.game}-${testCase.variant}`],
        inputs:['EAGLER_TEST_RUNTIME_MANIFEST','sourceBuildReport','packageReport']}};
      const dependencies=await once(`build-dependencies:${testCase.task.args[0]}`,()=>dependencySnapshot(root,[
        testCase.task.args[0],'tools/ports/build-acceptance.mjs','tools/ports/acceptance-contracts.mjs',
        'launcher/lib/runtime-build-evidence.mjs','launcher/lib/runtime-package-evidence.mjs',
        'tests/run.mjs','tests/adaptation/catalog.mjs','tests/adaptation/execution.mjs',
        'tests/adaptation/results.mjs','tests/adaptation/complete-evidence.mjs','tests/adaptation/identity.mjs',
        'tools/testing/runner.mjs','tools/testing/cli-report.mjs',
      ]));
      const identity={version:1,game:testCase.game,variant:testCase.variant,group:'build',checks:testCase.checks,
        command:testCase.task.command,args:testCase.task.args.map(arg=>arg.startsWith('--output=')?'--output=<CASE>':arg),
        dependencies,buildEvidence};
      const stableBuildEvidence=structuredClone(buildEvidence);
      if(stableBuildEvidence.verification&&typeof stableBuildEvidence.verification==='object')
        delete stableBuildEvidence.verification.checkedAt;
      return{fingerprint:identityDigest({...identity,buildEvidence:stableBuildEvidence}),identity};
    }
    const manifestPath=environment.EAGLER_TEST_RUNTIME_MANIFEST;
    assert.ok(manifestPath,'Required input: EAGLER_TEST_RUNTIME_MANIFEST');
    const manifest=await json(manifestPath),isOffline=testCase.task.args[0]?.endsWith('offline-runtime-browser.mjs');
    const isLanguage=testCase.task.args[0]?.endsWith('language-browser.mjs'),isLauncherJourney=isOffline||isLanguage;
    const targets=isOffline?selectOfflineRuntimeTargets(manifest,{game:testCase.game}).targets:
      (manifest.targets??[]).filter(target=>target.game===testCase.game&&target.variant===testCase.variant);
    assert.ok(targets.some(target=>target.variant===testCase.variant),`Missing Runtime target ${testCase.game}/${testCase.variant}`);
    assert.equal(new Set(targets.map(target=>target.variant)).size,targets.length,'Duplicate selected Runtime variant');
    const products=await Promise.all(targets.map(product));
    const shared=Object.fromEntries(await Promise.all(Object.entries(manifest.shared??{}).map(async([name,path])=>[name,{path,...await file(path)}])));
    const dependencies=inputOnly?null:await once(`dependencies:${testCase.task.args[0]}`,()=>dependencySnapshot(root,[
      testCase.task.args[0],'tests/run.mjs','tests/adaptation/catalog.mjs','tests/adaptation/execution.mjs',
      'tests/adaptation/results.mjs','tests/adaptation/identity.mjs','tools/testing/runner.mjs','tools/testing/supervise.py',
      'launcher/package.json','launcher/package-lock.json','docs/adaptation/checklist.json',
      'launcher/src/launcher/gamepad-mapping.mts','launcher/app/services/player-gamepad.ts','launcher/tests/fixtures/virtual-gamepads.mjs',
      ...(isLauncherJourney?['launcher/scripts/build-launcher.mjs']:[]),
    ]));
    const inputs={products,shared};
    if(isLauncherJourney&&!inputOnly)inputs.launcherSources=await once('launcher-sources',async()=>{
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
      }else if(name==='EAGLER_TEST_DIAGNOSTIC_MANIFEST'){
        inputs.diagnostics=await once(`diagnostic:${testCase.game}:${testCase.variant}:${testCase.diagnosticLanes?.join(',')}:${testCase.diagnosticPairedReplay?'paired':'regular'}:${value}`,
          ()=>diagnosticCaseIdentity({root,manifestPath:value,game:testCase.game,variant:testCase.variant,lanes:testCase.diagnosticLanes,
            pairedReplay:testCase.diagnosticPairedReplay?resolveDiagnosticProfile(testCase.game,testCase.variant)?.presentation?.pairedReplay:null}));
      }else if(name==='EAGLER_TEST_LANGUAGE_MANIFEST'){
        inputs.language=await once(`language:${testCase.game}:${testCase.variant}:${value}`,async()=>{
          const selected=selectLanguageInputs(await json(value),{game:testCase.game,variant:testCase.variant});
          const inspected=await inspectLanguageInputs(selected);
          return{receipt:languageInputReceipt(inspected),...(selected.assets?{assets:await tree(selected.assets)}:{})};
        });
      }else if(name==='EAGLER_TEST_ARTWORK')inputs.artwork=await once(`tree:${value}`,()=>tree(value));
      else if(name==='EAGLER_TEST_LAUNCHER_URL'){
        if(inputOnly){
          const url=new URL(value);
          assert.ok(['http:','https:'].includes(url.protocol),'Launcher URL must use HTTP or HTTPS');
          inputs.launcher={url:url.href,remoteIdentity:'deferred-to-execution'};
        }else{
          const target=targets.find(item=>item.variant==='multiplayer');
          inputs.launcher=await once(`published:${testCase.game}:${value}`,()=>verifyPublishedRuntime(value,testCase.game,'multiplayer',
            {expectedPackageSha256:target.packageSha256}));
        }
      }else inputs[name]=await file(value);
    }
    if(inputOnly)return{inputPreflight:{status:'passed',targetIds:products.map(item=>item.target.id),inputs:Object.keys(inputs).sort()}};
    const browserMode=adaptationBrowserMode(testCase),browserImplementation=adaptationBrowserImplementation(testCase);
    const identity={version:3,game:testCase.game,variant:testCase.variant,group:testCase.group,checks:testCase.checks,browserMode,browserImplementation,
      command:testCase.task.command,args:testCase.task.args.map(arg=>arg.startsWith('--output=')?'--output=<CASE>':arg),
      dependencies,inputs,tools:await toolIdentity(browserMode==='headed',browserImplementation)};
    return{fingerprint:identityDigest(identity),identity};
  };
}
