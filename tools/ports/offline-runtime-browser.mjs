import assert from 'node:assert/strict';
import {createHash,randomUUID} from 'node:crypto';
import {spawn} from 'node:child_process';
import {createServer} from 'node:net';
import {createRequire} from 'node:module';
import {readFile,mkdir,writeFile,stat} from 'node:fs/promises';
import {resolve,basename,relative,isAbsolute,dirname,sep} from 'node:path';
import {fileURLToPath,pathToFileURL} from 'node:url';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {PRODUCT_CONTENT} from '../../launcher/lib/content-definition.mjs';
import {extractGameDataLayout} from '../../launcher/lib/runtime-data-layout.mjs';
import {createPublicationHostSeed} from '../../launcher/lib/publication-host-seed.mjs';
import {PACKAGE_STORE_DB,PACKAGE_STORE_DB_VERSION,PACKAGE_INSTALLATIONS,PACKAGE_GENERATIONS,PACKAGE_OBJECTS} from '../../launcher/package/package-store.mjs';
import {selectOfflineRuntimeTargets,verifyPrivateData,verifyRuntimePackage,readVerifiedRuntimeFile} from './acceptance-contracts.mjs';
import {inspectRoomResources,verifyRoomResourcePostflight} from './room-resources.mjs';
import {loadRuntimeReplayFixture} from './replay-fixtures.mjs';
import {assertCliArguments} from '../testing/cli-options.mjs';
import {runOfflineRuntimeReport} from './offline-runtime-result.mjs';
import {installOfflinePackageStoreObserver} from './offline-package-observer.mjs';
import {classifyOfflineBrowserEvents} from './offline-runtime-errors.mjs';
import {auditBrowserFinalErrors} from '../browser/final-error-audit.mjs';
import {createOfflineStaticTlsProxy,captureOfflineSiteIdentity,watchOfflineSiteResponses,verifyOfflineSiteIdentity} from './offline-runtime-https.mjs';
import {loadOfflineUpdateManifest,inspectOfflineUpdateDescriptor,assertOfflineUpdateManifestIdentity,writeInterruptedOfflineUpdateArchive} from './offline-runtime-update.mjs';

const arg=name=>process.argv.find(value=>value.startsWith(`--${name}=`))?.slice(name.length+3);
const required=name=>{const value=arg(name);if(!value)throw Error(`Required: --${name}=VALUE`);return value;};
const root=resolve(import.meta.dirname,'../..'),launcher=resolve(root,'launcher');
const {parse:parseJavaScript}=createRequire(new URL('../../launcher/package.json',import.meta.url))('acorn');
const requestedOutput=arg('output'),requestedOutputPath=requestedOutput?resolve(requestedOutput):null;
let outputExists=false,outputIsDirectory=false,outputInspectionError=null;
if(requestedOutputPath){
  try{const info=await stat(requestedOutputPath);outputExists=true;outputIsDirectory=info.isDirectory();}
  catch(error){if(error.code!=='ENOENT'){outputExists=true;outputInspectionError=error;}}
}
const output=requestedOutputPath&&(!outputExists||outputIsDirectory)?requestedOutputPath:
  resolve(root,'artifacts/validation',`offline-runtime-browser-cli-${randomUUID()}`);
const digest=bytes=>createHash('sha256').update(bytes).digest('hex');
const appendTail=(current,chunk)=>`${current}${chunk}`.slice(-300_000);
const pause=ms=>new Promise(resolve=>setTimeout(resolve,ms));

async function configureOfflineMusic(page){
  const select=page.locator('#musicSelect');
  if(await page.locator('#musicOption').getAttribute('hidden')!==null)
    return{mode:await select.inputValue(),source:'Launcher default; this package has no music choice'};
  const advanced=page.locator('#advancedOptions');
  if(await advanced.getAttribute('open')===null)await advanced.locator('summary').click();
  const control=page.locator('.mizuki-select').filter({has:page.locator('#musicSelect')});
  await control.locator('.mizuki-select-trigger').click();
  await page.locator('.mizuki-select-menu:visible [data-value="none"]').click();
  assert.equal(await select.inputValue(),'none','Launcher must commit the visible music choice');
  return{mode:'none',source:'Launcher visible music menu'};
}

async function assertPortAvailable(port){
  const probe=createServer();
  await new Promise((accept,reject)=>{probe.once('error',reject);probe.listen(port,'127.0.0.1',accept);});
  await new Promise((accept,reject)=>probe.close(error=>error?reject(error):accept()));
}

async function runLogged(command,args,{cwd,stdoutPath,stderrPath,label}){
  const child=spawn(command,args,{cwd,windowsHide:true,stdio:['ignore','pipe','pipe']});
  let stdout='',stderr='';
  child.stdout.on('data',chunk=>{stdout=appendTail(stdout,chunk.toString());});
  child.stderr.on('data',chunk=>{stderr=appendTail(stderr,chunk.toString());});
  const [code,signal]=await new Promise((accept,reject)=>{
    child.once('error',reject);child.once('close',(exitCode,exitSignal)=>accept([exitCode,exitSignal]));
  });
  if(stdoutPath)await writeFile(stdoutPath,stdout);
  if(stderrPath)await writeFile(stderrPath,stderr);
  if(code!==0)throw Error(`${label} exited ${code??signal}: ${stderr.slice(-6000)||stdout.slice(-6000)}`);
  return{exitCode:code,stdoutTail:stdout.slice(-8000),stderrTail:stderr.slice(-8000)};
}

async function startStaticServer(site,port){
  const script=resolve(launcher,'scripts/serve-static.mjs');
  const child=spawn(process.execPath,[script,site,String(port)],{cwd:launcher,windowsHide:true,stdio:['ignore','pipe','pipe']});
  let stdout='',stderr='',closed=null;
  child.stdout.on('data',chunk=>{stdout=appendTail(stdout,chunk.toString());});
  child.stderr.on('data',chunk=>{stderr=appendTail(stderr,chunk.toString());});
  child.once('error',error=>{closed=error;});
  child.once('close',(code,signal)=>{closed=new Error(`static server exited ${code??signal}: ${stderr.slice(-4000)||stdout.slice(-4000)}`);});
  const baseUrl=`http://127.0.0.1:${port}/`,deadline=Date.now()+10_000;
  while(Date.now()<deadline){
    if(closed)throw closed;
    try{const response=await fetch(new URL('deployment.json',baseUrl),{redirect:'error',signal:AbortSignal.timeout(1500)});
      if(response.ok)return{child,baseUrl,stdout:()=>stdout,stderr:()=>stderr};
    }catch{}
    await pause(100);
  }
  child.kill();
  throw Error(`Local site did not become ready on ${baseUrl}: ${stderr.slice(-4000)||stdout.slice(-4000)}`);
}

async function stopChild(child){
  if(!child||child.exitCode!==null||child.signalCode!==null)return;
  child.kill();
  const closed=await Promise.race([new Promise(accept=>child.once('close',()=>accept(true))),pause(3000).then(()=>false)]);
  if(!closed)throw Error(`Static server process ${child.pid??'unknown'} did not exit after shutdown`);
}

function runtimeLayout(target,identity){
  const product=PRODUCT_GAMES[target.game];
  if(product.dataProvider==='retail-memory')return PRODUCT_CONTENT[target.game]?.dataLayout;
  if(product.dataProvider!=='emscripten-preload')throw Error(`${target.game}: unsupported DATA provider ${product.dataProvider}`);
  return null;
}

async function preloadLayout(target,identity){
  const product=PRODUCT_GAMES[target.game];
  if(product.dataProvider==='retail-memory'){
    const layout=runtimeLayout(target,identity);
    assert.ok(layout,`${target.game}: declared retail DATA layout is missing`);
    return layout;
  }
  for(const name of Object.keys(identity.files).filter(path=>/\.(?:js|mjs)$/i.test(path))){
    const bytes=await readVerifiedRuntimeFile(target,identity,name);
    try{return extractGameDataLayout(bytes.toString('utf8'),target.game).layout;}catch{}
  }
  throw Error(`${target.id}: packaged Runtime has no verifiable Emscripten DATA layout`);
}

function createSiteSeed(game,data,dataLayout){
  const seed=createPublicationHostSeed('web-validation-offline-desktop');
  seed.schema='eagler-touhou/site-manifest/1';
  seed.shared={resourceMode:'hosted',vanillaFont:'shared/msgothic.ttc?v=fixture',unicodeFont:'shared/unifont.otf?v=fixture'};
  const placeholderHash=digest(Buffer.from('unselected product seed'));
  for(const [id,product] of Object.entries(PRODUCT_GAMES)){
    const entry=seed.games[id];
    Object.assign(entry,{
      runtime:`runtime/${id}/${id}.html?hosted=1&v=seed`,
      ...(product.multiplayerRuntime?{multiplayerRuntime:`runtime/${id}/multiplayer/${id}.html?hosted=1&v=seed`}:{}),
      gameData:{path:product.package.dataTarget.slice(1),bytes:4,sha256:placeholderHash,
        version:`sha256-${placeholderHash}`,layout:'sha256-'+ '0'.repeat(64)},
      features:{thprac:false,focusHitbox:false},
      music:{midi:{files:[],...(product.musicCapabilities.midi?{}:{supported:false})}},
    });
  }
  seed.games[game].gameData={path:PRODUCT_GAMES[game].package.dataTarget.slice(1),bytes:data.bytes,
    sha256:data.sha256,version:`sha256-${data.sha256}`,layout:dataLayout};
  return seed;
}

function featureConfig(){
  return{schema:'eagler-touhou/server-features/1',resourceMode:'import',
    games:Object.fromEntries(Object.entries(PRODUCT_GAMES).map(([game,product])=>[game,{
      ...(product.features.languages?{languages:['ja']}:{}),...(product.features.thprac?{thprac:false}:{})
    }]))};
}

function offlineRuntimeInitScript({game}){
  const hash=async buffer=>Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',buffer)),value=>value.toString(16).padStart(2,'0')).join('');
  if(window===window.top){
    Object.defineProperty(window,'__offlineRuntimeEvents',{configurable:true,value:[]});
    const pending=new Set();
    Object.defineProperty(window,'__flushOfflineProtocolCaptures',{configurable:true,value:async()=>{
      while(pending.size)await Promise.all([...pending]);
      if(window.__offlineProtocolCaptureError)throw new Error(window.__offlineProtocolCaptureError);
    }});
    addEventListener('message',event=>{
      const frame=document.getElementById('gameFrame');
      const data=event.data;
      if(event.origin!==location.origin||event.source!==frame?.contentWindow||data?.protocol!=='eagler-touhou/1'||
        data.game!==game||!Number.isSafeInteger(data.epoch)||data.epoch<1)return;
      window.__offlineRuntimeEvents.push(data);
      if(data.event!=='error'&&data.event!=='abort'&&data.ok!==false)return;
      const operation=Promise.resolve(window.__captureOfflineRuntimeProtocolFailure({...data,origin:event.origin}))
        .catch(error=>{window.__offlineProtocolCaptureError=String(error?.stack||error);});
      pending.add(operation);void operation.finally(()=>pending.delete(operation));
    });
  }
  Object.defineProperty(window,'__offlineManagedDataReads',{configurable:true,value:window.__offlineManagedDataReads??[]});
  let current;
  Object.defineProperty(window,'__eaglerPrepareManagedRuntimeDataV1',{configurable:true,enumerable:true,
    get(){return current;},set(provider){
      if(typeof provider!=='function'){current=provider;return;}
      current=async request=>{
        const result=await provider(request),buffer=result?.buffer;
        if(!(buffer instanceof ArrayBuffer))throw new Error('Managed DATA provider returned no ArrayBuffer');
        window.__offlineManagedDataReads.push({game,epoch:request?.epoch??null,generation:request?.generation??null,
          fileId:result.fileId??null,bytes:buffer.byteLength,sha256:await hash(buffer)});
        return result;
      };
    }});
}

async function bindOfflineProtocolCapture(page,game,protocolErrors){
  await page.exposeBinding('__captureOfflineRuntimeProtocolFailure',(source,event)=>{
    const samePage=source.page===page,mainFrame=source.frame===page.mainFrame();
    if(!samePage||!mainFrame){protocolErrors.push({type:'untrusted-binding-source',samePage,mainFrame,event});return;}
    protocolErrors.push(event);
  });
  await page.addInitScript(offlineRuntimeInitScript,{game});
}

async function drainOfflineProtocolCapture(page){
  if(!page||page.isClosed())return;
  await page.evaluate(async()=>{
    if(typeof window.__flushOfflineProtocolCaptures==='function')await window.__flushOfflineProtocolCaptures();
  });
}

const HARNESS_ROOTS=[
  'tools/ports/offline-runtime-browser.mjs',
  'tools/ports/acceptance-contracts.mjs',
  'tools/ports/room-resources.mjs',
  'tools/ports/replay-fixtures.mjs',
  'tools/testing/cli-report.mjs',
  'tools/testing/cli-options.mjs',
  'tools/browser/launch.mjs',
  'tools/browser/playwright.mjs',
  'tools/browser/fixture-server.mjs',
  'tools/browser/final-error-audit.mjs',
  'tools/ports/offline-runtime-https.mjs',
  'tools/ports/offline-runtime-update.mjs',
  'tools/ports/offline-runtime-result.mjs',
  'launcher/scripts/package-server.mjs',
  'launcher/scripts/verify-server-build.mjs',
  'launcher/scripts/serve-static.mjs',
  'launcher/package/package-store.mjs',
  'launcher/lib/content-definition.mjs',
  'launcher/lib/runtime-data-layout.mjs',
  'launcher/lib/publication-host-seed.mjs',
  'common/contracts/product-catalog.mjs',
  'common/contracts/netplay-timing.mjs',
  'common/contracts/multiplayer-quick-chat.mjs',
  'common/contracts/public-nickname.mjs',
  'common/contracts/th15-chapter-reward.mjs',
  'launcher/package.json',
  'launcher/package-lock.json',
];

async function collectHarnessSources(){
  const pending=[...HARNESS_ROOTS],seen=new Map();
  while(pending.length){
    const relativePath=pending.pop().replaceAll('\\','/');
    if(seen.has(relativePath))continue;
    const absolutePath=resolve(root,relativePath),bytes=await readFile(absolutePath);
    seen.set(relativePath,{bytes:bytes.length,sha256:digest(bytes),contents:bytes});
    if(!/\.(?:mjs|js|cjs)$/.test(relativePath))continue;
    const ast=parseJavaScript(bytes.toString('utf8'),{ecmaVersion:'latest',sourceType:'module'}),specifiers=new Set();
    const visit=node=>{
      if(!node||typeof node!=='object')return;
      if((node.type==='ImportDeclaration'||node.type==='ExportNamedDeclaration'||node.type==='ExportAllDeclaration')&&node.source?.type==='Literal')
        specifiers.add(node.source.value);
      if(node.type==='ImportExpression'&&node.source?.type==='Literal')specifiers.add(node.source.value);
      for(const value of Object.values(node))if(Array.isArray(value)){for(const child of value)visit(child);}else if(value&&typeof value==='object'&&typeof value.type==='string')visit(value);
    };
    visit(ast);
    for(const specifier of specifiers){
      if(specifier==='node:fs'||specifier.startsWith('node:')||!specifier.startsWith('.'))continue;
      const imported=fileURLToPath(new URL(specifier,pathToFileURL(absolutePath)));
      const relativeImport=relative(root,imported).replaceAll('\\','/');
      if(isAbsolute(relativeImport)||relativeImport==='..'||relativeImport.startsWith(`..${sep}`)||
          relativeImport.split('/').includes('node_modules'))continue;
      let candidate=relativeImport;
      try{await stat(resolve(root,candidate));}
      catch(error){
        if(error.code!=='ENOENT')throw error;
        const alternatives=[`${candidate}.mjs`,`${candidate}.js`,`${candidate}.json`,`${candidate}/index.mjs`,`${candidate}/index.js`];
        candidate=null;
        for(const alternative of alternatives){try{await stat(resolve(root,alternative));candidate=alternative;break;}catch(nested){if(nested.code!=='ENOENT')throw nested;}}
        if(!candidate)throw Error(`${relativePath}: cannot resolve local import ${specifier}`);
      }
      pending.push(candidate);
    }
  }
  return seen;
}

function injectedHarnessSources(){
  return new Map([
    ['browser-injected/offline-runtime-init.js',Buffer.from(`${offlineRuntimeInitScript.toString()}\n`)],
    ['browser-injected/offline-package-observer.js',Buffer.from(`${installOfflinePackageStoreObserver.toString()}\n`)],
  ]);
}

async function captureHarnessSources(destination){
  const sources=await collectHarnessSources(),records={};
  for(const [relativePath,entry] of [...sources].sort(([left],[right])=>left.localeCompare(right))){
    records[relativePath]={bytes:entry.bytes,sha256:entry.sha256};
    const copyPath=resolve(destination,'harness-sources',relativePath);
    await mkdir(dirname(copyPath),{recursive:true});await writeFile(copyPath,entry.contents);
  }
  for(const [injectedPath,injected] of injectedHarnessSources()){
    records[injectedPath]={bytes:injected.length,sha256:digest(injected)};
    const injectedCopy=resolve(destination,'harness-sources',injectedPath);
    await mkdir(dirname(injectedCopy),{recursive:true});await writeFile(injectedCopy,injected);
  }
  return records;
}

const report=await runOfflineRuntimeReport({output},async(report,save)=>{
  assertCliArguments(process.argv.slice(2),{values:['output','manifest','packages','artwork-dir','game','port','update-manifest']});
  report.outputDirectory=output;
  report.requestedOutput=requestedOutputPath;
  if(!requestedOutputPath)throw Error('Required: --output=directory');
  if(outputInspectionError)throw Error(`Cannot inspect requested output path ${requestedOutputPath}: ${outputInspectionError.message}`);
  if(outputExists&&!outputIsDirectory)throw Error(`Refusing to replace existing non-directory output path: ${requestedOutputPath}`);
  if(outputExists)throw Error(`Refusing to reuse existing evidence directory: ${requestedOutputPath}`);
  report.harnessSources=await captureHarnessSources(output);
  const game=required('game'),manifestPath=resolve(required('manifest')),packagesPath=resolve(required('packages'));
  const updateManifestPath=arg('update-manifest')?resolve(arg('update-manifest')):null;
  const updateManifest=updateManifestPath?await loadOfflineUpdateManifest(updateManifestPath):null;
  const artworkPath=resolve(required('artwork-dir'));
  const portValue=arg('port')??'8143',port=Number(portValue);
  assert.ok(Number.isSafeInteger(port)&&port>0&&port<65536,'--port must be an available TCP port');
  const product=PRODUCT_GAMES[game];assert.ok(product,`Unsupported game ${game}`);
  const config=JSON.parse(await readFile(manifestPath,'utf8'));
  const selection=selectOfflineRuntimeTargets(config,{game});
  const normal=selection.targets.find(target=>target.variant==='normal');
  const packageMap=JSON.parse(await readFile(packagesPath,'utf8'));
  assert.equal(typeof packageMap[game],'string',`${game}: --packages file has no offline ZIP path`);
  await assertPortAvailable(port);
  await mkdir(output,{recursive:true});
  const runId=randomUUID(),site=resolve(output,`site-${runId}`);
  try{await stat(site);throw Error(`Refusing to replace existing packaged site: ${site}`);}catch(error){if(error.code!=='ENOENT')throw error;}
  report.scope={game,variant:'normal',checks:['PACKAGE-02','PACKAGE-04','PACKAGE-05','SAVE-02','SAVE-03','SAVE-04','SAVE-05',
      ...(updateManifest?['PACKAGE-03','SAVE-06']:[])],
    excluded:updateManifest?[]:['PACKAGE-03','SAVE-06 require an explicit second-generation update manifest'],
    statement:updateManifest?
      'HTTPS-served packaged Launcher, local DATA ZIP import, verified failed-import retention, complete second-generation UI import, user Replay recovery and offline startup from the new generation.'
      :'HTTPS-served packaged Launcher, local DATA ZIP import through the Launcher UI, real normal Runtime first frame, Runtime file sync and read after an offline page reopen.'};
  report.inputs={manifest:{path:manifestPath,sha256:digest(await readFile(manifestPath))},packages:{path:packagesPath,sha256:digest(await readFile(packagesPath))},
    ...(updateManifest?{updateManifest:{path:updateManifest.path,bytes:updateManifest.bytes,sha256:updateManifest.sha256}}:{}),
    offlinePackagePath:resolve(packageMap[game]),artworkPath,outputSite:site,port};
  report.selection={games:selection.games,variant:selection.variant,expected:selection.expected};
  report.startedAt=report.startedAt??new Date().toISOString();
  await save();

  const [normalPackage,dataIdentity]=await Promise.all([
    verifyRuntimePackage(normal,{repoRoot:root}),verifyPrivateData(normal.data),
  ]);
  if(normal.dataSha256)assert.equal(dataIdentity.sha256,normal.dataSha256,`${normal.id}: DATA differs from the acceptance manifest`);
  const packagePreflight=await inspectRoomResources({baseUrl:`http://127.0.0.1:${port}/`,targets:[normal],packageMap:{[game]:packageMap[game]}});
  assert.equal(packagePreflight.passed,true,`${game}: offline ZIP preflight failed: ${JSON.stringify(packagePreflight.games)}`);
  const resource=packagePreflight.games[0];
  assert.equal(resource.expectedData.sha256,dataIdentity.sha256,`${game}: ZIP preflight DATA differs from actual Runtime input`);
  const dataLayout=await preloadLayout(normal,normalPackage);
  assert.equal(dataLayout,PRODUCT_CONTENT[game]?.dataLayout??dataLayout,`${game}: Runtime DATA layout disagrees with product contract`);
  report.runtimePackages={normal:{packageSha256:normalPackage.packageSha256,wasmSha256:normalPackage.wasmSha256,
      sourceBuild:normalPackage.sourceBuild,packageBuild:normalPackage.packageBuild},
    data:dataIdentity,dataLayout};
  report.offlinePackagePreflight=resource;
  let updateInput=null,updateIdentity=null,interruptedArchive=null;
  if(updateManifest){
    const entry=updateManifest.games[game];
    const updatePath=resolve(entry?.to?.path??'');
    assert.ok(entry,`${game}: --update-manifest has no matching second package`);
    const updateResourceCheck=await inspectRoomResources({baseUrl:`http://127.0.0.1:${port}/`,targets:[normal],packageMap:{[game]:updatePath}});
    assert.equal(updateResourceCheck.passed,true,`${game}: second ZIP preflight failed: ${JSON.stringify(updateResourceCheck.games)}`);
    const updateResource=updateResourceCheck.games[0];
    assert.deepEqual(updateResource.expectedData,dataIdentity,`${game}: second ZIP DATA differs from the real Runtime DATA`);
    const updateDescriptor=await inspectOfflineUpdateDescriptor(updatePath);
    updateIdentity=assertOfflineUpdateManifestIdentity({game,manifestEntry:entry,basePath:packageMap[game],
      baseRevision:resource.package.descriptorRevision,baseArchiveSha256:resource.package.archiveSha256,
      baseData:dataIdentity,update:updateDescriptor});
    const interruptedPath=resolve(output,'update-attempts',`${game}-interrupted.zip`);
    await mkdir(dirname(interruptedPath),{recursive:true});
    interruptedArchive=await writeInterruptedOfflineUpdateArchive(updatePath,interruptedPath);
    updateInput={manifestEntry:entry,resource:updateResource,descriptor:updateDescriptor,identity:updateIdentity,interruptedArchive};
    report.packageUpdate={status:'preflighted',...updateIdentity,preflight:updateResource,interruptedArchive,
      boundary:'The UI receives a deterministic partial ZIP rejected before staging; the complete verified second ZIP then exercises the production local import commit. Network mid-file interruption is not part of this UI retry route.'};
  }else report.packageUpdate={status:'blocked',reason:`The package input maps ${game} to one current ZIP (${resource.package?.descriptorRevision??'unknown revision'}). No explicit second-generation update manifest was selected; PACKAGE-03 and SAVE-06 remain untested.`};
  await save();

  const seed=createSiteSeed(game,dataIdentity,dataLayout);
  const features=featureConfig();
  const seedPath=resolve(output,`site-seed-${runId}.json`),featuresPath=resolve(output,`features-${runId}.json`);
  await writeFile(seedPath,JSON.stringify(seed,null,2)+'\n');await writeFile(featuresPath,JSON.stringify(features,null,2)+'\n');
  const packArgs=['scripts/package-server.mjs',`--output=${site}`,`--site-manifest=${seedPath}`,
    `--feature-config=${featuresPath}`,'--profile=web-validation-offline-desktop','--runtime-variants=normal',`--games=${game}`,'--music=midi',
    `--artwork-dir=${artworkPath}`,
    `--${game}-build=${normal.runtime}`];
  report.packaging=await runLogged(process.execPath,packArgs,{cwd:launcher,stdoutPath:resolve(output,'package-server.stdout.log'),
    stderrPath:resolve(output,'package-server.stderr.log'),label:'Launcher package-server'});
  await runLogged(process.execPath,['scripts/verify-server-build.mjs',site],{cwd:launcher,
    stdoutPath:resolve(output,'verify-server.stdout.log'),stderrPath:resolve(output,'verify-server.stderr.log'),label:'verify-server-build'});
  const [siteManifest,deployment,runtimeManifest,releaseCatalog]=await Promise.all([
    readFile(resolve(site,'site-manifest.json'),'utf8').then(JSON.parse),
    readFile(resolve(site,'deployment.json'),'utf8').then(JSON.parse),
    readFile(resolve(site,'runtime-manifest.json'),'utf8').then(JSON.parse),
    readFile(resolve(site,'release-catalog.json'),'utf8').then(JSON.parse),
  ]);
  assert.equal(siteManifest.shared.resourceMode,'import');
  assert.deepEqual(deployment.games,[game]);
  assert.equal(deployment.appShell?.schema,'eagler-touhou/app-shell/2',`${game}: packaged Launcher does not declare its App Shell`);
  assert.deepEqual(releaseCatalog.games,{},'Import-mode site must not silently supply a second online package');
  const normalRoot=`runtime/${game}/`,runtimeGroup=runtimeManifest.groups.find(group=>group.root===normalRoot);
  assert.ok(runtimeGroup?.current?.generation,`${game}: packaged normal Runtime generation is missing`);
  report.packagedSite={profile:deployment.profile,resourceMode:siteManifest.shared.resourceMode,
    deploymentSha256:digest(await readFile(resolve(site,'deployment.json'))),siteManifestSha256:digest(await readFile(resolve(site,'site-manifest.json'))),
    appShell:deployment.appShell,normalRuntime:{root:runtimeGroup.root,generation:runtimeGroup.current.generation,
      files:runtimeGroup.current.files.length},releaseCatalogGames:Object.keys(releaseCatalog.games)};
  await save();

  let staticServer,tlsProxy,siteSnapshot,browser,context,page;
  const pageWatchers=[];
  const browserEvents=[];
  const protocolErrors=[],responseErrors=[],teardownErrors=[];
  try{
    staticServer=await startStaticServer(site,port);
    tlsProxy=await createOfflineStaticTlsProxy({backendBase:staticServer.baseUrl});
    siteSnapshot=await captureOfflineSiteIdentity(site);
    report.server={baseUrl:tlsProxy.base,protocol:tlsProxy.protocol,port:tlsProxy.port,backendBase:staticServer.baseUrl,
      tlsIdentity:tlsProxy.tlsIdentity,staticSite:siteSnapshot.identity};await save();
    const {launchBrowser}=await import('../browser/launch.mjs');
    browser=await launchBrowser({args:['--autoplay-policy=no-user-gesture-required',
      `--ignore-certificate-errors-spki-list=${tlsProxy.tlsIdentity.spkiSha256}`]});
    report.browser={engine:'Chromium',version:browser.version(),headless:true};
    context=await browser.newContext({viewport:{width:1280,height:900}});
    await context.addInitScript(installOfflinePackageStoreObserver,{name:PACKAGE_STORE_DB,version:PACKAGE_STORE_DB_VERSION,
      installations:PACKAGE_INSTALLATIONS,generations:PACKAGE_GENERATIONS,objects:PACKAGE_OBJECTS});
    page=await context.newPage();
    await bindOfflineProtocolCapture(page,game,protocolErrors);
    const onlineSiteWatcher=await watchOfflineSiteResponses(page,{base:tlsProxy.base,snapshot:siteSnapshot,output,label:'online-site'});
    pageWatchers.push(onlineSiteWatcher);
    page.on('console',message=>{if(message.type()==='error')browserEvents.push({type:'console',text:message.text()});});
    page.on('pageerror',error=>browserEvents.push({type:'pageerror',text:String(error.stack||error)}));
    page.on('requestfailed',request=>browserEvents.push({type:'requestfailed',url:request.url(),error:request.failure()?.errorText??null}));
    page.on('response',response=>{if(response.status()>=400)browserEvents.push({type:'http',url:response.url(),status:response.status()});});
    const appUrl=new URL(`?game=${encodeURIComponent(game)}`,tlsProxy.base).href;
    await page.goto(appUrl,{waitUntil:'load',timeout:60_000});
    await page.waitForFunction(()=>window.__eaglerBoot?.done===true,null,{timeout:60_000});
    const controller=async()=>page.evaluate(async()=>{
      const registration=await navigator.serviceWorker.ready;
      return{scope:registration.scope,worker:registration.active?.scriptURL??null,controlled:!!navigator.serviceWorker.controller};
    });
    let shell=await controller();
    if(!shell.controlled){await page.reload({waitUntil:'load',timeout:60_000});await page.waitForFunction(()=>window.__eaglerBoot?.done===true,null,{timeout:60_000});}
    await page.waitForFunction(()=>!!navigator.serviceWorker.controller,null,{timeout:30_000});
    shell=await controller();
    assert.equal(new URL(shell.scope).origin,new URL(tlsProxy.base).origin,'App Shell scope must belong to the browser HTTPS origin');
    assert.equal(new URL(shell.worker).origin,new URL(tlsProxy.base).origin,'App Shell worker must belong to the browser HTTPS origin');
    report.appShell={...shell,controlledBeforeImport:true};await save();
    const firstUse=page.locator('#firstUseNoticeDialog');
    if(await firstUse.count()&&await firstUse.evaluate(element=>element.open))await page.locator('#firstUseNoticeClose').click();
    await page.locator('#gamePackageImport').waitFor({state:'visible',timeout:20_000});
    await page.locator('#gamePackageImport').click();
    await page.locator('#gameDataImportWindow').waitFor({state:'visible',timeout:15_000});
    await page.locator('#gameDataImportInput').setInputFiles(resolve(packageMap[game]));
    try{
      await page.waitForFunction(()=>{
        const window=document.querySelector('#gameDataImportWindow');
        return !!window&&window.hidden&&window.getAttribute('aria-busy')!=='true';
      },null,{timeout:180_000});
    }catch(error){
      const state=await page.evaluate(()=>({reason:document.querySelector('#gameDataImportReason')?.textContent,
        busy:document.querySelector('#gameDataImportWindow')?.getAttribute('aria-busy'),
        status:document.querySelector('#status')?.textContent,toast:document.querySelector('#toast')?.textContent}));
      throw Error(`${game}: real Launcher ZIP import did not finish: ${JSON.stringify(state)}; ${error.message}`);
    }
    const installed=await page.evaluate(async game=>{
      const store=window.__offlinePackageStoreObserver;
      const result=await store.readCurrentPackageGeneration(game),generation=result.generation;
      if(!generation)throw Error(`${game}: current Package Store generation is absent after UI import`);
      const fileId=generation.descriptor.runtimeRequirement?.dataFile;
      const ref=fileId&&generation.files[fileId],declaration=fileId&&generation.descriptor.files[fileId];
      if(!fileId||!ref?.objectId||!declaration)throw Error(`${game}: imported Package Store has no declared Runtime DATA object`);
      const stored=await store.readPackageObject(ref.objectId);
      if(!stored?.blob)throw Error(`${game}: imported Package Store DATA object cannot be read`);
      const buffer=await stored.blob.arrayBuffer(),hash=Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',buffer)),value=>value.toString(16).padStart(2,'0')).join('');
      return{generationId:generation.id,revision:generation.descriptor.revision,gameVersion:generation.descriptor.gameVersion??'original',
        fileId,target:declaration.target,bytes:buffer.byteLength,sha256:hash,declaredBytes:declaration.bytes,declaredSha256:declaration.sha256};
    },game);
    assert.equal(installed.bytes,dataIdentity.bytes,`${game}: imported Package Store DATA byte count differs`);
    assert.equal(installed.sha256,dataIdentity.sha256,`${game}: imported Package Store DATA differs from current private DATA`);
    assert.equal(installed.declaredBytes,dataIdentity.bytes,`${game}: imported descriptor byte count differs`);
    assert.equal(installed.declaredSha256,dataIdentity.sha256,`${game}: imported descriptor SHA differs`);
    assert.equal(installed.target,product.package.dataTarget,`${game}: imported DATA target differs from product contract`);
    report.uiImport={path:resolve(packageMap[game]),source:'Launcher #gamePackageImport UI',store:'IndexedDB Package Store',generation:installed};await save();

    report.musicSelection={online:await configureOfflineMusic(page)};await save();
    await page.locator('#launch').click();
    const decision=page.locator('#decisionDialog');
    if(await decision.count()&&await decision.evaluate(element=>element.open))await page.locator('#decisionConfirm').click();
    await page.waitForFunction(()=>window.__offlineRuntimeEvents.some(event=>event.event==='ready'),null,{timeout:180_000});
    await page.waitForFunction(()=>window.__offlineRuntimeEvents.some(event=>event.event==='first-frame'),null,{timeout:180_000});
    const onlineRuntime=await page.evaluate(({game,generation,expectedData})=>{
      const frame=document.getElementById('gameFrame'),src=new URL(frame.src,location.href),events=window.__offlineRuntimeEvents;
      const ready=events.find(event=>event.event==='ready'),first=events.find(event=>event.event==='first-frame');
      const reads=window.__offlineManagedDataReads.filter(item=>item.game===game);
      const data=reads.find(item=>item.generation===generation&&item.bytes===expectedData.bytes&&item.sha256===expectedData.sha256);
      return{frameUrl:frame.src,pathname:src.pathname,query:Object.fromEntries(src.searchParams),ready,first,
        pageOnline:navigator.onLine,workerControlled:!!navigator.serviceWorker.controller,dataRead:data??null,dataReads:reads};
    },{game,generation:installed.generationId,expectedData:dataIdentity});
    assert.equal(onlineRuntime.pageOnline,true);
    assert.equal(onlineRuntime.workerControlled,true);
    assert.equal(onlineRuntime.ready?.game,game);assert.equal(onlineRuntime.ready?.protocol,'eagler-touhou/1');
    assert.equal(onlineRuntime.first?.game,game);
    assert.equal(onlineRuntime.query.managedData,'1');assert.equal(onlineRuntime.query.gameGeneration,installed.generationId);
    assert.ok(onlineRuntime.pathname.startsWith(`/runtime/${game}/`),`${game}: Launcher selected a nonlocal Runtime path`);
    assert.ok(!onlineRuntime.pathname.startsWith(`/runtime/${game}/multiplayer/`),`${game}: normal lane launched the MP Runtime`);
    assert.ok(onlineRuntime.pathname.includes(runtimeGroup.current.generation),`${game}: Runtime URL does not use packaged generation`);
    assert.ok(onlineRuntime.dataRead,`${game}: Runtime did not receive the exact imported DATA through the production Launcher provider`);
    report.onlineRuntime=onlineRuntime;
    const frameUrl=onlineRuntime.frameUrl;
    const runtimeEpoch=Number(new URL(frameUrl).searchParams.get('runtimeEpoch'));
    assert.ok(Number.isSafeInteger(runtimeEpoch)&&runtimeEpoch>0,`${game}: Runtime epoch is missing`);

    const runtimeCommand=async(command,payload={})=>page.evaluate(({game,command,payload})=>new Promise((accept,reject)=>{
      const frame=document.getElementById('gameFrame'),runtimeEpoch=Number(new URL(frame.src,location.href).searchParams.get('runtimeEpoch')),
        request=`offline-runtime-${crypto.randomUUID()}`;
      const timer=setTimeout(()=>{removeEventListener('message',receive);reject(Error(`Runtime ${command} timed out`));},30_000);
      function receive(event){
        if(event.origin!==location.origin||event.source!==frame?.contentWindow||event.data?.request!==request)return;
        clearTimeout(timer);removeEventListener('message',receive);
        if(event.data.ok)accept(event.data);else reject(Error(event.data.error||`Runtime ${command} rejected`));
      }
      addEventListener('message',receive);
      frame.contentWindow.postMessage({protocol:'eagler-touhou/1',game,epoch:runtimeEpoch,command,request,...payload},location.origin);
    }),{game,command,payload});
    const listBefore=await runtimeCommand('list');
    assert.ok(Array.isArray(listBefore.files),`${game}: Runtime file list response is invalid`);
    const storage=product.storage;
    const nativePaths=new Set([storage.scoreFile,...(storage.configFiles??[])]);
    const nativeFiles=[];
    for(const file of listBefore.files.filter(entry=>nativePaths.has(entry.path))){
      const value=await runtimeCommand('read',{path:file.path}),bytes=Buffer.from(value.bytes??[]);
      const path=resolve(output,'user-files',`online-${basename(file.path)}`);
      await mkdir(resolve(output,'user-files'),{recursive:true});await writeFile(path,bytes);
      nativeFiles.push({path:file.path,bytes:bytes.length,sha256:digest(bytes),evidencePath:path});
    }
    const replay=await loadRuntimeReplayFixture(normal),replayPath=`replay/${product.replay.prefix}_udtest.rpy`;
    const replayBytes=Array.from(replay.contents);
    const listBeforeWrite=await runtimeCommand('list');
    const rejected=[];
    for(const command of ['read','write','remove']){
      let error=null;
      try{await runtimeCommand(command,{path:'../outside-offline-acceptance.dat',bytes:replayBytes});}
      catch(reason){error=String(reason?.message??reason);}
      assert.ok(error&&!/timed out/i.test(error),`${command} must reject paths outside the declared storage`);
      rejected.push({command,error});
    }
    const listAfterRejected=await runtimeCommand('list');
    assert.deepEqual(listAfterRejected.files,listBeforeWrite.files,`${game}: rejected path operations changed the Runtime file list`);
    await runtimeCommand('write',{path:replayPath,bytes:replayBytes});
    await runtimeCommand('sync');
    const saved=await runtimeCommand('read',{path:replayPath});
    assert.deepEqual(saved.bytes,replayBytes,`${game}: Runtime bytes differ immediately after sync`);
    const savedIdentity={path:replayPath,bytes:saved.bytes.length,sha256:digest(Buffer.from(saved.bytes)),
      sourceFixture:replay.identity};
    assert.equal(savedIdentity.bytes,replay.identity.bytes);assert.equal(savedIdentity.sha256,replay.identity.sha256);
    const savedUserFile=resolve(output,'user-files',`${game}-user-replay.rpy`);
    await mkdir(resolve(output,'user-files'),{recursive:true});await writeFile(savedUserFile,replay.contents);
    report.userFiles={sync:{status:'passed',runtimePath:replayPath,...savedIdentity,evidencePath:savedUserFile},
      pathTraversal:{status:'passed',commands:rejected,unchangedFileList:true},
      nativeGameFiles:{status:nativeFiles.length?'observed':'not-created-during-title-session',files:nativeFiles}};
    report.persistence={onlineList:listBefore.files.map(file=>({path:file.path,bytes:file.bytes??null})),syncResponse:saved.ok??true};
    await save();

    let finalGeneration=installed,updateScenario=null,onlineRuntimeAfterUpdate=null;
    if(updateInput){
      await onlineSiteWatcher.flush();
      await page.reload({waitUntil:'load',timeout:60_000});
      await page.waitForFunction(()=>window.__eaglerBoot?.done===true,null,{timeout:60_000});
      await page.waitForFunction(()=>!!navigator.serviceWorker.controller,null,{timeout:30_000});
      const beforeUpdate=await page.evaluate(async game=>{
        const store=window.__offlinePackageStoreObserver,current=await store.readCurrentPackageGeneration(game);
        if(!current.generation)throw Error('Current package generation disappeared before update retry');
        return{generationId:current.generation.id,revision:current.generation.descriptor.revision};
      },game);
      assert.deepEqual(beforeUpdate,{generationId:installed.generationId,revision:installed.revision},`${game}: base generation changed before update UI attempt`);
      await page.locator('#gamePackageImport').click();
      const importWindow=page.locator('#gameDataImportWindow');
      await importWindow.waitFor({state:'visible',timeout:20_000});
      const initialImportReason=await page.locator('#gameDataImportReason').textContent();
      await page.locator('#gameDataImportInput').setInputFiles(updateInput.interruptedArchive.path);
      await page.waitForFunction(initial=>{
        const window=document.querySelector('#gameDataImportWindow'),reason=document.querySelector('#gameDataImportReason')?.textContent;
        return !!window&&!window.hidden&&window.getAttribute('aria-busy')!=='true'&&reason!==initial;
      },initialImportReason,{timeout:60_000});
      const rejectedImport=await page.locator('#gameDataImportReason').textContent();
      assert.ok(rejectedImport&&rejectedImport!==initialImportReason,`${game}: interrupted archive did not produce a Launcher import error`);
      const afterInterrupted=await page.evaluate(async game=>{
        const store=window.__offlinePackageStoreObserver,current=await store.readCurrentPackageGeneration(game);
        return current.generation?{generationId:current.generation.id,revision:current.generation.descriptor.revision}:null;
      },game);
      assert.deepEqual(afterInterrupted,beforeUpdate,`${game}: partial ZIP import changed the current package generation`);
      report.packageUpdate.interruptedAttempt={status:'rejected-before-store-staging',archive:updateInput.interruptedArchive,
        launcherReason:rejectedImport,oldGenerationRetained:afterInterrupted};
      await save();

      const updateSamples=[];let samplingUpdate=true;
      const sampleUpdateStore=(async()=>{
        while(samplingUpdate){
          updateSamples.push(await page.evaluate(async game=>{
            const store=window.__offlinePackageStoreObserver,current=await store.readCurrentPackageGeneration(game);
            return{busy:document.querySelector('#gameDataImportWindow')?.getAttribute('aria-busy')==='true',
              currentGeneration:current.installation?.currentGeneration??null,pendingGeneration:current.installation?.pendingGeneration??null};
          },game));
          await pause(50);
        }
      })();
      try{
        await page.locator('#gameDataImportInput').setInputFiles(updateInput.identity.updatePath);
        await page.waitForFunction(()=>{
          const window=document.querySelector('#gameDataImportWindow');
          return !!window&&window.hidden&&window.getAttribute('aria-busy')!=='true';
        },null,{timeout:240_000});
      }finally{samplingUpdate=false;await sampleUpdateStore.catch(()=>{});}
      const pendingSamples=updateSamples.filter(sample=>sample.pendingGeneration);
      assert.ok(updateSamples.some(sample=>sample.busy&&sample.currentGeneration===installed.generationId),
        `${game}: complete update UI import did not show the old generation during the active operation`);
      assert.ok(pendingSamples.every(sample=>sample.currentGeneration===installed.generationId),
        `${game}: current Package Store generation switched before the pending update completed`);
      const updated=await page.evaluate(async({game,expectedOgg})=>{
        const store=window.__offlinePackageStoreObserver,current=await store.readCurrentPackageGeneration(game),generation=current.generation;
        if(!generation)throw Error('Package Store has no current generation after complete update import');
        const hash=async blob=>Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',await blob.arrayBuffer())),value=>value.toString(16).padStart(2,'0')).join('');
        const fileId=generation.descriptor.runtimeRequirement?.dataFile,ref=fileId&&generation.files[fileId],declaration=fileId&&generation.descriptor.files[fileId];
        if(!ref?.objectId||!declaration)throw Error('Updated generation has no Runtime DATA object');
        const stored=await store.readPackageObject(ref.objectId);
        if(!stored?.blob)throw Error('Updated generation DATA object is unavailable');
        const dataBlob=stored.blob,data={bytes:dataBlob.size,sha256:await hash(dataBlob)};
        const ogg=[];
        for(const expected of expectedOgg){
          const id=`ogg:${expected.name}`,entry=generation.descriptor.files[id],oggRef=generation.files[id];
          if(!entry||!oggRef?.objectId)throw Error(`Updated generation is missing OGG object ${id}`);
          const object=await store.readPackageObject(oggRef.objectId);
          if(!object?.blob)throw Error(`Updated generation OGG object is unavailable: ${id}`);
          const value={name:expected.name,bytes:object.blob.size,sha256:await hash(object.blob)};
          if(value.bytes!==expected.bytes||value.sha256!==expected.sha256)throw Error(`Updated Package Store OGG identity mismatch: ${id}`);
          ogg.push(value);
        }
        return{generationId:generation.id,revision:generation.descriptor.revision,source:current.installation?.source??null,
          pendingGeneration:current.installation?.pendingGeneration??null,
          fileCount:Object.keys(generation.files).length,data,ogg};
      },{game,expectedOgg:updateInput.identity.ogg});
      assert.notEqual(updated.generationId,installed.generationId,`${game}: update did not create a new Package Store generation`);
      assert.equal(updated.revision,updateInput.identity.updateRevision,`${game}: Package Store committed a different update descriptor`);
      assert.equal(updated.pendingGeneration,null,`${game}: committed Package Store still has a pending update`);
      assert.deepEqual(updated.data,dataIdentity,`${game}: update changed or corrupted DATA identity`);
      finalGeneration={generationId:updated.generationId,revision:updated.revision};
      report.packageUpdate.retry={status:'committed',generation:updated,sourceZip:updateInput.identity.updatePath,
        archiveBytes:updateInput.identity.updateArchiveBytes,archiveSha256:updateInput.identity.updateArchiveSha256,
        oldGeneration:installed.generationId,atomicCurrentGeneration:updated.generationId,
        storeSamples:{count:updateSamples.length,pendingCount:pendingSamples.length,oldGenerationWhileBusy:updateSamples.some(sample=>sample.busy&&sample.currentGeneration===installed.generationId)}};
      await save();

      report.musicSelection.updated=await configureOfflineMusic(page);await save();
      await page.locator('#launch').click();
      const updateDecision=page.locator('#decisionDialog');
      if(await updateDecision.count()&&await updateDecision.evaluate(element=>element.open))await page.locator('#decisionConfirm').click();
      await page.waitForFunction(()=>window.__offlineRuntimeEvents.some(event=>event.event==='ready'),null,{timeout:180_000});
      await page.waitForFunction(()=>window.__offlineRuntimeEvents.some(event=>event.event==='first-frame'),null,{timeout:180_000});
      onlineRuntimeAfterUpdate=await page.evaluate(({game,generation,expectedData})=>{
        const frame=document.getElementById('gameFrame'),src=new URL(frame.src,location.href),events=window.__offlineRuntimeEvents;
        const reads=window.__offlineManagedDataReads.filter(item=>item.game===game);
        return{frameUrl:frame.src,pathname:src.pathname,query:Object.fromEntries(src.searchParams),ready:events.find(event=>event.event==='ready'),
          first:events.find(event=>event.event==='first-frame'),dataRead:reads.find(item=>item.generation===generation&&item.bytes===expectedData.bytes&&item.sha256===expectedData.sha256)??null};
      },{game,generation:finalGeneration.generationId,expectedData:dataIdentity});
      assert.equal(onlineRuntimeAfterUpdate.ready?.game,game);
      assert.equal(onlineRuntimeAfterUpdate.query.gameGeneration,finalGeneration.generationId);
      assert.ok(onlineRuntimeAfterUpdate.dataRead,`${game}: Runtime did not read DATA from the new Package Store generation`);
      const retainedReplay=await runtimeCommand('read',{path:replayPath});
      assert.deepEqual(retainedReplay.bytes,replayBytes,`${game}: user Replay changed after package generation update`);
      report.onlineRuntimeAfterUpdate=onlineRuntimeAfterUpdate;
      report.packageUpdate={...report.packageUpdate,status:'passed',scenario:{id:'package-update-and-user-data-recovery',status:'passed',passed:true,
        stages:['partial archive rejected with old generation retained','complete second ZIP committed a new generation','Replay restored from the new Runtime','new-generation Runtime reached first-frame']},
        replay:{path:replayPath,bytes:retainedReplay.bytes.length,sha256:digest(Buffer.from(retainedReplay.bytes))},
        coverageBoundary:'This UI retry verifies archive-validation rollback and complete local import. A network download interrupted after Package Store staging remains outside this scenario.'};
      updateScenario=report.packageUpdate.scenario;
      await save();
    }

    await onlineSiteWatcher.flush();
    try{await onlineSiteWatcher.stop();}catch(error){teardownErrors.push({phase:'online-response-probe.stop',error:String(error.stack||error)});throw error;}
    await drainOfflineProtocolCapture(page);
    try{await page.close();}catch(error){teardownErrors.push({phase:'online-page.close',error:String(error.stack||error)});throw error;}
    page=null;
    await context.setOffline(true);
    page=await context.newPage();
    await bindOfflineProtocolCapture(page,game,protocolErrors);
    const offlineSiteWatcher=await watchOfflineSiteResponses(page,{base:tlsProxy.base,snapshot:siteSnapshot,output,label:'offline-site'});
    pageWatchers.push(offlineSiteWatcher);
    page.on('console',message=>{if(message.type()==='error')browserEvents.push({type:'offline-console',text:message.text()});});
    page.on('pageerror',error=>browserEvents.push({type:'offline-pageerror',text:String(error.stack||error)}));
    page.on('requestfailed',request=>browserEvents.push({type:'offline-requestfailed',url:request.url(),error:request.failure()?.errorText??null}));
    page.on('response',response=>{if(response.status()>=400)browserEvents.push({type:'offline-http',url:response.url(),status:response.status()});});
    await page.goto(appUrl,{waitUntil:'load',timeout:60_000});
    await page.waitForFunction(()=>window.__eaglerBoot?.done===true,null,{timeout:60_000});
    await page.waitForFunction(()=>!!navigator.serviceWorker.controller,null,{timeout:30_000});
    const offlineBrowser=await page.evaluate(()=>({online:navigator.onLine,controlled:!!navigator.serviceWorker.controller,
      worker:navigator.serviceWorker.controller?.scriptURL??null,origin:location.origin}));
    assert.equal(offlineBrowser.online,false,`${game}: browser context did not enter offline mode`);
    assert.equal(offlineBrowser.controlled,true,`${game}: App Shell Service Worker does not control offline document`);
    const offlineFirstUse=page.locator('#firstUseNoticeDialog');
    if(await offlineFirstUse.count()&&await offlineFirstUse.evaluate(element=>element.open))await page.locator('#firstUseNoticeClose').click();
    report.musicSelection.offline=await configureOfflineMusic(page);await save();
    await page.locator('#launch').click();
    const offlineDecision=page.locator('#decisionDialog');
    if(await offlineDecision.count()&&await offlineDecision.evaluate(element=>element.open))await page.locator('#decisionConfirm').click();
    await page.waitForFunction(()=>window.__offlineRuntimeEvents.some(event=>event.event==='ready'),null,{timeout:180_000});
    await page.waitForFunction(()=>window.__offlineRuntimeEvents.some(event=>event.event==='first-frame'),null,{timeout:180_000});
    const offlineRuntime=await page.evaluate(({game,generation,expectedData})=>{
      const frame=document.getElementById('gameFrame'),src=new URL(frame.src,location.href),events=window.__offlineRuntimeEvents;
      const reads=window.__offlineManagedDataReads.filter(item=>item.game===game);
      return{frameUrl:frame.src,pathname:src.pathname,query:Object.fromEntries(src.searchParams),ready:events.find(event=>event.event==='ready'),
        first:events.find(event=>event.event==='first-frame'),pageOnline:navigator.onLine,workerControlled:!!navigator.serviceWorker.controller,
        dataRead:reads.find(item=>item.generation===generation&&item.bytes===expectedData.bytes&&item.sha256===expectedData.sha256)??null,
        dataReads:reads};
    },{game,generation:finalGeneration.generationId,expectedData:dataIdentity});
    assert.equal(offlineRuntime.pageOnline,false);assert.equal(offlineRuntime.workerControlled,true);
    assert.equal(offlineRuntime.ready?.game,game);assert.equal(offlineRuntime.ready?.protocol,'eagler-touhou/1');
    assert.equal(offlineRuntime.first?.game,game);
    assert.equal(offlineRuntime.query.managedData,'1');assert.equal(offlineRuntime.query.gameGeneration,finalGeneration.generationId);
    assert.ok(offlineRuntime.pathname.startsWith(`/runtime/${game}/`));
    assert.ok(!offlineRuntime.pathname.startsWith(`/runtime/${game}/multiplayer/`));
    assert.ok(offlineRuntime.pathname.includes(runtimeGroup.current.generation));
    assert.ok(offlineRuntime.dataRead,`${game}: offline Runtime did not receive the same Package Store DATA bytes`);
    const offlineRuntimeCommand=async(command,payload={})=>page.evaluate(({game,command,payload})=>new Promise((accept,reject)=>{
      const frame=document.getElementById('gameFrame'),src=new URL(frame.src,location.href),epoch=Number(src.searchParams.get('runtimeEpoch'));
      const request=`offline-runtime-${crypto.randomUUID()}`;
      const timer=setTimeout(()=>{removeEventListener('message',receive);reject(Error(`Offline Runtime ${command} timed out`));},30_000);
      function receive(event){if(event.origin!==location.origin||event.source!==frame?.contentWindow||event.data?.request!==request)return;
        clearTimeout(timer);removeEventListener('message',receive);if(event.data.ok)accept(event.data);else reject(Error(event.data.error||`Offline Runtime ${command} rejected`));}
      addEventListener('message',receive);frame.contentWindow.postMessage({protocol:'eagler-touhou/1',game,epoch,command,request,...payload},location.origin);
    }),{game,command,payload});
    const restored=await offlineRuntimeCommand('read',{path:replayPath});
    assert.deepEqual(restored.bytes,replayBytes,`${game}: offline Runtime did not restore synced user bytes`);
    const offlineList=await offlineRuntimeCommand('list');
    assert.ok(offlineList.files.some(file=>file.path===replayPath),`${game}: synced user file is absent from offline Runtime list`);
    await offlineRuntimeCommand('sync');
    const afterOfflineSync=await offlineRuntimeCommand('read',{path:replayPath});
    assert.deepEqual(afterOfflineSync.bytes,replayBytes,`${game}: offline sync changed saved user bytes`);
    const offlinePackageAfter=await verifyRoomResourcePostflight({target:normal,prepared:resource});
    const dataAfter=await verifyPrivateData(normal.data);
    assert.deepEqual(dataAfter,dataIdentity,`${game}: private DATA changed during the browser run`);
    const normalAfter=await verifyRuntimePackage(normal,{repoRoot:root});
    assert.deepEqual(normalAfter,normalPackage,`${game}: normal Runtime receipt, source plan or package identity changed during browser run`);
    assert.equal(digest(await readFile(manifestPath)),report.inputs.manifest.sha256,`${game}: Runtime input manifest changed during browser run`);
    assert.equal(digest(await readFile(packagesPath)),report.inputs.packages.sha256,`${game}: base package map changed during browser run`);
    if(updateManifestPath)
      assert.equal(digest(await readFile(updateManifestPath)),report.inputs.updateManifest.sha256,`${game}: update package manifest changed during browser run`);
    let updatePackageAfter=null,updateDescriptorAfter=null;
    if(updateInput){
      updatePackageAfter=await verifyRoomResourcePostflight({target:normal,prepared:updateInput.resource});
      updateDescriptorAfter=await inspectOfflineUpdateDescriptor(updateInput.identity.updatePath);
      assert.deepEqual(updateDescriptorAfter,updateInput.descriptor,`${game}: second ZIP identity changed during browser run`);
      assert.deepEqual(updatePackageAfter.data,dataIdentity,`${game}: second ZIP postflight DATA differs from the Runtime input`);
    }
    await offlineSiteWatcher.flush();
    const staticSiteAfter=await verifyOfflineSiteIdentity(siteSnapshot);
    report.browserSiteResponses=pageWatchers.map(watcher=>({capture:watcher.capture,count:watcher.responses.length,errors:watcher.errors,responses:watcher.responses}));
    report.staticSitePostflight=staticSiteAfter;
    report.offlineRuntime=offlineRuntime;
    report.browserEvents=browserEvents;
    report.offlineFailures=browserEvents.filter(event=>event.type==='offline-requestfailed'||event.type==='offline-http');
    const offlineEventClassification=classifyOfflineBrowserEvents(
      browserEvents.filter(event=>event.type.startsWith('offline-')),{origin:offlineBrowser.origin});
    report.offlineEventClassification={
      origin:offlineBrowser.origin,
      blockingEvents:offlineEventClassification.blockingEvents,
      sameOriginTransportEvents:offlineEventClassification.sameOriginTransportEvents,
      crossOriginTransportEvents:offlineEventClassification.crossOriginTransportEvents,
    };
    assert.deepEqual(offlineEventClassification.blockingEvents,[],
      `${game}: browser error or HTTP/request failure occurred while offline`);
    report.postflight={privateData:dataAfter,offlinePackage:offlinePackageAfter,
      normalRuntimePackage:normalAfter,
      ...(updateInput?{updatePackage:{resource:updatePackageAfter,descriptor:updateDescriptorAfter}}:{})};
    report.harnessSourcesPostflight=await collectHarnessSources().then(sources=>Object.fromEntries(
      [...sources].sort(([left],[right])=>left.localeCompare(right)).map(([name,entry])=>[name,{bytes:entry.bytes,sha256:entry.sha256}])
    ));
    for(const [name,bytes] of injectedHarnessSources())
      report.harnessSourcesPostflight[name]={bytes:bytes.length,sha256:digest(bytes)};
    assert.deepEqual(report.harnessSourcesPostflight,report.harnessSources,`${game}: offline driver or its judging dependencies changed during the run`);
    report.checks=[
      {id:'PACKAGE-02',status:'passed',complete:true,scope:'selected game, normal Runtime',evidence:'The offline ZIP, Package Store DATA object, actual managed Runtime provider and current private DATA share exact byte count and SHA-256; the imported target matches the product contract.'},
      {id:'PACKAGE-04',status:'partial',complete:false,scope:'selected game, local ZIP import',evidence:'One local ZIP was installed through the Launcher UI and verified in Package Store; online installation and an independent original-file import were not compared.'},
      {id:'PACKAGE-05',status:'passed',complete:true,scope:'selected game, normal Runtime and installed App Shell',evidence:'The selected packaged normal Runtime reached first-frame after the installed site was reopened offline; the same Package Store generation supplied the verified DATA bytes.'},
      {id:'SAVE-02',status:'partial',complete:false,scope:'Replay file only',evidence:'The Runtime accepted a hash-identified Replay at its allowed path; read, write and remove traversal requests failed without changing the file list. Score/configuration and other invalid-path forms were not exercised.'},
      {id:'SAVE-03',status:'partial',complete:false,scope:'Replay file only',evidence:'Runtime sync completed for the Replay file before document retirement and after offline restore; native score and settings writes were not produced or checked.'},
      {id:'SAVE-04',status:'partial',complete:false,scope:'Replay file only',evidence:'A new Runtime document restored the exact synced Replay bytes and path; native score/settings recovery was not checked.'},
      {id:'SAVE-05',status:'partial',complete:false,scope:'Replay file only',evidence:'The offline page, App Shell worker, selected Runtime first-frame and Replay file read/write/sync completed; other user-data classes are outside this run.'},
      updateInput?
        {id:'PACKAGE-03',status:'partial',complete:false,scope:'selected game, interrupted partial ZIP import and complete UI retry',evidence:'A partial archive was rejected before staging with the old generation unchanged; the full second ZIP committed a new Package Store generation and started. Network interruption after file staging remains outside this route.'}:
        {id:'PACKAGE-03',status:'blocked',reason:report.packageUpdate.reason},
      updateInput?
        {id:'SAVE-06',status:'partial',complete:false,scope:'selected game, synchronized Replay across package generations',evidence:'The second ZIP changed the package descriptor generation while retaining the same DATA identity; the synchronized Replay remained byte-identical through the new-generation Runtime and offline launch. Score/configuration and changed-DATA recovery were not exercised.'}:
        {id:'SAVE-06',status:'blocked',reason:report.packageUpdate.reason},
    ];
    report.result=updateInput?
      'HTTPS Launcher import, package generation retry, Replay recovery and offline Runtime startup passed. PACKAGE-03 and SAVE-06 adaptation coverage remains partial.'
      :'HTTPS Launcher import, Runtime startup and offline user-file recovery passed. PACKAGE-03 and SAVE-06 remain blocked until a second-generation update manifest is selected.';
    await save();
    return{scenarioPassed:true,updateScenario};
  }finally{
    const teardown=async(phase,operation)=>{
      try{await operation();}catch(error){teardownErrors.push({phase,error:String(error.stack||error)});}
    };
    if(page&&!page.isClosed())await teardown('runtime-protocol.drain',()=>drainOfflineProtocolCapture(page));
    for(let index=0;index<pageWatchers.length;index++){
      const watcher=pageWatchers[index];
      await teardown(`response-probe[${index}].flush-before-stop`,()=>watcher.flush());
      await teardown(`response-probe[${index}].stop`,()=>watcher.stop());
      await teardown(`response-probe[${index}].flush-after-stop`,()=>watcher.flush());
      for(const error of watcher.errors??[])responseErrors.push({watcher:index,error});
    }
    if(page&&!page.isClosed())await teardown('page.close',()=>page.close());
    if(context)await teardown('context.close',()=>context.close());
    if(browser)await teardown('browser.close',()=>browser.close());
    if(tlsProxy)await teardown('tls-proxy.close',()=>tlsProxy.close());
    if(staticServer)await teardown('static-server.close',()=>stopChild(staticServer.child));
    report.browserEvents=browserEvents;
    report.protocolErrors=protocolErrors;
    report.responseErrors=responseErrors;
    report.teardownErrors=teardownErrors;
    report.browserSiteResponses=pageWatchers.map(watcher=>({capture:watcher.capture,count:watcher.responses.length,errors:watcher.errors,responses:watcher.responses}));
    const finalErrorAudit=auditBrowserFinalErrors({
      pageErrors:browserEvents.filter(event=>event.type==='pageerror'||event.type==='offline-pageerror'),
      consoleErrors:browserEvents.filter(event=>event.type==='console'||event.type==='offline-console'),
      httpErrors:browserEvents.filter(event=>event.type==='http'||event.type==='offline-http'||event.type==='requestfailed'||event.type==='offline-requestfailed'),
      protocolErrors,responseErrors,teardownErrors,
    });
    report.finalErrorAudit=finalErrorAudit;
    await save();
  }
});

if(report.passed!==true||report.status!=='passed'){
  process.exitCode=1;
  console.error(`${report.status.toUpperCase()} report: ${resolve(output,'report.json')}`);
  if(report.failure)console.error(report.failure);
}
