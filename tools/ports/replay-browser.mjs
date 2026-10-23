import assert from 'node:assert/strict';
import {createHash,randomBytes} from 'node:crypto';
import {extname,relative,resolve} from 'node:path';
import {execFileSync} from 'node:child_process';
import {mkdir,readFile,writeFile} from 'node:fs/promises';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {readVerifiedRuntimeFile,selectRuntimeTargets,verifyPrivateData,verifyRuntimePackage} from './acceptance-contracts.mjs';
import {loadRuntimeReplayFixture} from './replay-fixtures.mjs';
import {runWithReport} from '../testing/cli-report.mjs';
import {assertCliArguments} from '../testing/cli-options.mjs';
import {RUNTIME_BUILD_CONTROL_FILES,RUNTIME_BUILD_HELPER_FILES} from '../../launcher/lib/runtime-build-evidence.mjs';
import {RUNTIME_PACKAGE_CONTROL_FILES} from '../../launcher/lib/runtime-package-evidence.mjs';
import {createFixtureServer} from '../browser/fixture-server.mjs';
import {createFixtureResponseProbe} from './runtime-fixture-site.mjs';
import {assessReplayTerminalSignal,createReplayProgressTracker,gateReplayRecordOnFinalErrors,hasHashBoundNaturalReplayEndpoint,playbackObservation,
  replayOriginalImportCheck,replayProtocolFailures,replayScenarioPassedTargetIds} from './replay-browser-semantics.mjs';
import {assessTh08ReplayInputConsumption,deriveTh08ReplayInputExpectation,replayTerminalPolicy} from '../../tests/adaptation/adapters/replay.mjs';
import {readRuntimeObservation,runtimeObservationRequest} from './runtime-observations.mjs';

const arg=name=>process.argv.find(value=>value.startsWith(`--${name}=`))?.slice(name.length+3);
const flag=name=>process.argv.includes(`--${name}`);
// Output is explicit so a new run cannot reuse a stale report directory.
// All semantic parsing runs inside runWithReport below.
const outputArgument=arg('output');
if(!outputArgument)throw new Error('Required: --output=<ignored report directory>');
const output=resolve(outputArgument);
const repoRoot=resolve(import.meta.dirname,'../..');
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const decisionInputs=new Map();
const decisionSourcePaths=[...new Set([
  'tools/ports/replay-browser.mjs','tools/ports/acceptance-contracts.mjs','tools/ports/replay-fixtures.mjs','tools/ports/replay-browser-semantics.mjs','tools/ports/replay-browser-semantics.test.mjs','tools/ports/runtime-fixture-site.mjs','tools/browser/fixture-server.mjs','tools/browser/response-body-capture.mjs','tools/browser/final-error-audit.mjs','tools/browser/final-error-audit.test.mjs','tools/browser/launch.mjs','tools/browser/playwright.mjs',
  'tools/ports/runtime-observations.mjs','tools/ports/runtime-observations.test.mjs','tests/adaptation/adapters/replay.mjs','tests/adaptation/adapters/replay.test.mjs',
  'tools/testing/cli-report.mjs','tools/testing/cli-options.mjs','common/contracts/product-catalog.mjs',
  'common/browser/local-replay.mjs','common/browser/local-replay-native.mjs','common/browser/playback-clock.mjs',
  'launcher/lib/contracts/runtime-generations.mjs','launcher/lib/contracts/site-manifest.mjs',
  'launcher/lib/contracts/load-compiled-contract.mjs','launcher/lib/contracts-build.mjs',
  'launcher/src/contracts/runtime-generations.mts','launcher/src/contracts/site-manifest.mts',
  // Native inputs are bound by each selected Runtime's verified source receipt.
  // Reference-specific source anchors are captured by candidateCounterReferenceFor.
  ...RUNTIME_BUILD_CONTROL_FILES,...RUNTIME_BUILD_HELPER_FILES,...RUNTIME_PACKAGE_CONTROL_FILES,
])].sort((a,b)=>a<b?-1:a>b?1:0);

async function saveDecisionInput({key,path,absolutePath,copyPath,bytes}){
  const sha256=hash(bytes),existing=decisionInputs.get(key);
  if(existing){assert.equal(existing.sha256,sha256,`Decision input changed during preflight: ${path}`);return existing;}
  const copyAbsolute=resolve(output,copyPath);
  await mkdir(resolve(copyAbsolute,'..'),{recursive:true});
  await writeFile(copyAbsolute,bytes);
  const item={key,path,absolutePath,copyPath,bytes:bytes.length,sha256};
  decisionInputs.set(key,item);return item;
}

async function captureDecisionFile(path){
  const absolutePath=resolve(repoRoot,path),bytes=await readFile(absolutePath);
  return saveDecisionInput({key:`source:${path}`,path,absolutePath,copyPath:`evidence/decision-sources/${path}`,bytes});
}

async function readDecisionSource(path){
  const absolutePath=resolve(repoRoot,path),bytes=await readFile(absolutePath);
  await saveDecisionInput({key:`source:${path}`,path,absolutePath,copyPath:`evidence/decision-sources/${path}`,bytes});
  return bytes;
}

function publicDecisionInputs(){
  return [...decisionInputs.values()].map(({path,copyPath,bytes,sha256})=>({path,copyPath,bytes,sha256}));
}

async function verifyDecisionInputs(){
  const verified=[];
  for(const item of decisionInputs.values()){
    const current=await readFile(item.absolutePath);
    assert.equal(current.length,item.bytes,`Decision input size changed during Replay acceptance: ${item.path}`);
    assert.equal(hash(current),item.sha256,`Decision input changed during Replay acceptance: ${item.path}`);
    const copy=await readFile(resolve(output,item.copyPath));
    assert.equal(copy.length,item.bytes,`Saved decision input copy has the wrong size: ${item.path}`);
    assert.equal(hash(copy),item.sha256,`Saved decision input copy has the wrong hash: ${item.path}`);
    verified.push({path:item.path,bytes:item.bytes,sha256:item.sha256});
  }
  return{status:'passed',verifiedAt:new Date().toISOString(),inputs:verified};
}

function hostPageHtml(target,epoch,runId){
  const runtime=`/runtime/${target.id}/${target.entry}?hosted=1&managedData=1&gameGeneration=replay-desktop&runtimeEpoch=${epoch}&runId=${encodeURIComponent(runId)}`;
  return `<!doctype html><meta charset="utf-8"><style>html,body{margin:0;background:#171717}iframe{width:960px;height:720px;border:0}</style><script>
  const runtimeFrame=()=>document.querySelector('iframe');
  window.events=[];
  window.__eaglerPrepareManagedRuntimeDataV1=async()=>{const response=await fetch('/data/${target.id}');if(!response.ok)throw Error('Missing private DATA');const buffer=await response.arrayBuffer();return{buffer,bytes:buffer.byteLength}};
  const belongs=event=>event.origin===location.origin&&event.source===runtimeFrame()?.contentWindow&&event.data?.protocol==='eagler-touhou/1'&&event.data?.game===${JSON.stringify(target.game)}&&event.data?.epoch===${epoch};
  addEventListener('message',event=>{if(belongs(event))window.events.push(event.data)});
  window.send=(command,fields={})=>runtimeFrame().contentWindow.postMessage({...fields,protocol:'eagler-touhou/1',game:${JSON.stringify(target.game)},epoch:${epoch},command},location.origin);
  window.rpc=(command,fields={})=>new Promise((accept,reject)=>{const request=crypto.randomUUID(),timer=setTimeout(()=>{removeEventListener('message',receive);reject(Error('Timeout: '+command))},180000);function receive(event){if(!belongs(event)||event.data?.request!==request)return;clearTimeout(timer);removeEventListener('message',receive);event.data.ok?accept(event.data):reject(Error(event.data.error||'Command rejected'))}addEventListener('message',receive);send(command,{...fields,request})});
  </script><iframe src=${JSON.stringify(runtime)}></iframe>`;
}

// Preserve original/candidate counters without treating them as native Replay
// cursors unless the source proves equivalent semantics and binds the bytes.
const candidateCounterReferenceFor=async(target,replaySha256)=>{
  if(target.variant==='multiplayer')return null;
  if(target.game==='th08'){
    const corpusPath='ports/th08/tools/replay-verifier/corpus.json';
    const goldenPath='ports/th08/tools/replay-verifier/golden/manifest.json';
    const localStatePath='ports/th08/web/cpp/sdl/LocalReplay.inc',streamPath='ports/th08/web/cpp/game/ReplayStream.cpp';
    const [corpusBytes,goldenBytes,localStateBytes,streamBytes]=await Promise.all([
      readDecisionSource(corpusPath),readDecisionSource(goldenPath),readDecisionSource(localStatePath),readDecisionSource(streamPath)]);
    const corpus=JSON.parse(corpusBytes),golden=JSON.parse(goldenBytes);
    const corpusEntry=corpus.fixtures?.find(item=>item.replaySha256===replaySha256);
    const goldenEntry=Object.values(golden.assets??{}).find(item=>item.replaySha256===replaySha256);
    const expectedRows=corpusEntry?.expectedTicks??goldenEntry?.rawTicks;
    if(expectedRows&&goldenEntry?.rawTicks===expectedRows){
      const rowsPath=`ports/th08/tools/replay-verifier/golden/${goldenEntry.path}`;
      const rowsBytes=await readDecisionSource(rowsPath);
      return{kind:'original-executable-presentation-row-count',counter:{name:'presentationRows',value:expectedRows},replaySha256,counterBoundToSelectedReplay:true,
        comparableToNativeCursor:false,
        countDefinition:'capture-original-replay samples the original executable on D3D present and deduplicates stage:frame identities. ReplayStream::play increments its cursor only when it consumes an input block. The captured row count is not mapped to that cursor.',
        evidence:[{path:corpusPath,sha256:hash(corpusBytes)},{path:goldenPath,sha256:hash(goldenBytes)},{path:rowsPath,sha256:hash(rowsBytes)},
          {path:localStatePath,sha256:hash(localStateBytes)},{path:streamPath,sha256:hash(streamBytes)}]};
    }
  }
  if(target.game==='th09'){
    const replayPath='ports/th09/web/reference/assets/demorpy0.rpy';
    const testPath='ports/th09/web/tests/cpp/session-replays.test.mjs';
    const resultPath='ports/th09/web/artifacts/cpp/verification/session-replays.json';
    const managedPath='ports/th09/web/sdl-runtime/managed.mjs',playerPath='ports/th09/web/sdl-runtime/replay-player.mjs',clockPath='common/browser/playback-clock.mjs';
    const replayBytes=await readDecisionSource(replayPath);
    if(hash(replayBytes)!==replaySha256)return null;
    const [testBytes,managedBytes,playerBytes,clockBytes]=await Promise.all([
      readDecisionSource(testPath),readDecisionSource(managedPath),readDecisionSource(playerPath),readDecisionSource(clockPath)]);
    let resultBytes;
    try{resultBytes=await readFile(resolve(repoRoot,resultPath));}
    catch(error){if(error.code==='ENOENT')return null;throw error;}
    await saveDecisionInput({key:`source:${resultPath}`,path:resultPath,absolutePath:resolve(repoRoot,resultPath),
      copyPath:`evidence/decision-sources/${resultPath}`,bytes:resultBytes});
    const report=JSON.parse(resultBytes),entry=report.verifier?.results?.find(item=>item.id==='demo0');
    if(report.verifier?.passed===true&&entry?.status==='PASS'&&entry.comparedTicks>0)return{
      kind:'original-process-and-cpp-session-replay-comparison',counter:{name:'comparedTicks',value:entry.comparedTicks},replaySha256,
      persistedReplaySha256:entry.replaySha256??null,counterBoundToSelectedReplay:false,comparableToNativeCursor:false,
      countDefinition:'th09 test code hashes the raw input and includes that hash in both comparison identities while it runs. The persisted ignored session-replays.json result does not carry the Replay hash, so this PASS count is not bound to the selected bytes. Official inspect is not implemented; playback open constructs the ReplayPlayer.',
      evidence:[{path:replayPath,sha256:hash(replayBytes)},{path:testPath,sha256:hash(testBytes)},{path:resultPath,sha256:hash(resultBytes)},
        {path:managedPath,sha256:hash(managedBytes)},{path:playerPath,sha256:hash(playerBytes)},{path:clockPath,sha256:hash(clockBytes)}]};
  }
  if(target.game==='th11'){
    const goldenPath='ports/th11/tools/replay-verifier/golden/manifest.json';
    const [goldenBytes,worldTestBytes,candidateBytes,runtimeStateBytes,replayReaderBytes]=await Promise.all([
      readDecisionSource(goldenPath),
      readDecisionSource('ports/th11/web/tests/cpp/world-replays.test.mjs'),
      readDecisionSource('ports/th11/tools/replay-verifier/capture-candidate.mjs'),
      readDecisionSource('ports/th11/web/cpp/sdl/Application.cpp'),
      readDecisionSource('ports/th11/web/cpp/game/Replay.cpp'),
    ]);
    const golden=JSON.parse(goldenBytes);
    const entry=Object.values(golden.assets??{}).find(item=>item.replaySha256===replaySha256);
    if(entry?.ticks>0&&entry.completion?.checks===entry.ticks&&entry.completion?.replaySha256===replaySha256)
      return{kind:'original-retail-replay-golden',counter:{name:'gameplayUpdatesBeforeTerminalMarker',value:entry.ticks},replaySha256,counterBoundToSelectedReplay:true,
      comparableToNativeCursor:false,
      countDefinition:'The golden trace counts complete gameplay updates before the all-FFFF terminal marker. The native cursor adds prior-stage metadata and the current-stage reader cursor. Existing evidence does not map the trace update count to that cross-stage cursor, so no terminal cursor is derived from this count.',
        finalState:entry.completion.finalState,evidence:[{path:goldenPath,sha256:hash(goldenBytes)},
          {path:'ports/th11/web/tests/cpp/world-replays.test.mjs',sha256:hash(worldTestBytes)},
          {path:'ports/th11/tools/replay-verifier/capture-candidate.mjs',sha256:hash(candidateBytes)},
          {path:'ports/th11/web/cpp/sdl/Application.cpp',sha256:hash(runtimeStateBytes)},
          {path:'ports/th11/web/cpp/game/Replay.cpp',sha256:hash(replayReaderBytes)}]};
  }
  return null;
};

const result=await runWithReport({output,schema:'eagler-touhou/replay-browser/1'},async(report,save)=>{
  assertCliArguments(process.argv.slice(2),{values:['manifest','output','game','variant','port','replay-timeout-ms','stall-timeout-ms'],flags:['plan-only']});
  const manifest=arg('manifest');
  assert.ok(manifest,'Required: --manifest=inputs.json');
  for(const path of decisionSourcePaths)await captureDecisionFile(path);
  report.decisionInputs=publicDecisionInputs();await save();
  const manifestAbsolute=resolve(manifest),manifestBytes=await readFile(manifestAbsolute);
  await saveDecisionInput({key:`manifest:${manifestAbsolute}`,path:manifestAbsolute,absolutePath:manifestAbsolute,
    copyPath:'evidence/decision-sources/input-manifest.json',bytes:manifestBytes});
  report.decisionInputs=publicDecisionInputs();await save();
  const config=JSON.parse(manifestBytes.toString('utf8'));
  assert.ok(Array.isArray(config.targets)&&config.targets.length>0,'Input manifest has no Runtime targets');
  const games=arg('game');
  if(!games)throw new Error('Required: --game=<explicit comma-separated games>');
  const variant=arg('variant')??'normal';
  assert.ok(['normal','multiplayer','both'].includes(variant),`Unsupported --variant=${variant}`);
  let selection;
  if(variant==='both'){
    const gameList=games.split(',').map(game=>game.trim()).filter(Boolean);
    assert.ok(gameList.length>0,'--game selected no titles');
    const normal=selectRuntimeTargets(config,{game:gameList.join(','),variant:'normal'});
    const multiplayerGames=gameList.filter(game=>PRODUCT_GAMES[game]?.multiplayerRuntime);
    const multiplayer=multiplayerGames.length?selectRuntimeTargets(config,{game:multiplayerGames.join(','),variant:'multiplayer'}):{expected:[],targets:[]};
    selection={games:gameList,variant,expected:[...normal.expected,...multiplayer.expected],targets:[...normal.targets,...multiplayer.targets]};
  }else selection=selectRuntimeTargets(config,{game:games,variant});
  const port=Number(arg('port')??8142),planOnly=flag('plan-only');
  assert.ok(Number.isInteger(port)&&port>=1024&&port<=65535,'--port must be an integer TCP port from 1024 to 65535');
  const replayTimeout=Number(arg('replay-timeout-ms')??1800000);
  assert.ok(Number.isSafeInteger(replayTimeout)&&replayTimeout>=60000,'--replay-timeout-ms must be at least 60000');
  const stallTimeout=Number(arg('stall-timeout-ms')??120000);
  assert.ok(Number.isSafeInteger(stallTimeout)&&stallTimeout>=10000&&stallTimeout<=replayTimeout,'--stall-timeout-ms must be from 10000 through --replay-timeout-ms');
  report.playbackMonitor={checkpointIntervalMs:2000,stallTimeoutMs:stallTimeout,replayTimeoutMs:replayTimeout,
    stallCriterion:'No change in the reported native frame cursor or stage for the configured stall interval.'};
  const sharedInputs=new Map();
  for(const [name,path] of Object.entries(config.shared??{})){
    assert.match(name,/^[A-Za-z0-9_.-]+$/,'Shared resource name must be a basename');
    const absolutePath=resolve(path),bytes=await readFile(absolutePath);
    assert.ok(bytes.length>0,`Shared resource must not be empty: ${name}`);
    sharedInputs.set(`/shared/${name}`,{name,path:absolutePath,bytes:bytes.length,sha256:hash(bytes)});
  }
  report.decisionInputs=publicDecisionInputs();
  report.sharedInputs=Object.fromEntries([...sharedInputs].map(([url,identity])=>[url,{
    path:identity.path,bytes:identity.bytes,sha256:identity.sha256}]));
  await save();
  report.commit=execFileSync('git',['rev-parse','HEAD'],{cwd:repoRoot,encoding:'utf8',windowsHide:true}).trim();
  report.driverSha256=hash(await readFile(new URL(import.meta.url)));
  const decisionsByPath=new Map(publicDecisionInputs().map(item=>[item.path,item]));
  report.playbackControlEvidence={path:'common/browser/local-replay.mjs',sha256:decisionsByPath.get('common/browser/local-replay.mjs').sha256,
    nativeStepPath:'common/browser/local-replay-native.mjs',nativeStepSha256:decisionsByPath.get('common/browser/local-replay-native.mjs').sha256,
    pacing:'Official rate 1 for the first 120 logical frames; official rate 0 then runs the same native replay step in bounded sequential batches until the native terminal signal is observed.'};
  report.selection={games:selection.games,variant,expectedTargets:selection.expected};
  report.scope='Each playable target records official Shell storage/import, 120 rate-1 native frames, and the Runtime native terminal signal without browser or Runtime errors. th08 normal derives the maximum successful reads from the exact imported version-6 stage blocks and records Runtime cursor consistency at natural termination. REPLAY-01 completion additionally requires a hash-bound reference mapping the original natural endpoint to ReplayStream cursors. Other targets retain their title-specific proof scope.';
  report.multiplayerScope='A single-player Replay fixture is tested in an MP Runtime only when the official Shell and native format admit it. MP-11 requires a Replay recorded by a real multiplayer session; an unavailable MP fixture remains blocked and is not reported as out of scope.';
  report.port=port;report.targets=[];
  const records=new Map(),verifiedPackages=new Map(),verifiedData=new Map(),fixtures=new Map();
  for(const target of selection.targets){
    const record={id:target.id,game:target.game,variant:target.variant,checks:[],errors:[],consoleErrors:[],httpErrors:[]};
    report.targets.push(record);records.set(target.id,record);
    try{
      const packageIdentity=await verifyRuntimePackage(target,{repoRoot});
      const dataIdentity=await verifyPrivateData(target.data);
      if(target.dataSha256)assert.equal(dataIdentity.sha256,target.dataSha256,`${target.id}: private DATA hash mismatch`);
      const fixture=await loadRuntimeReplayFixture(target);
      const counterReference=await candidateCounterReferenceFor(target,fixture.identity.sha256);
      record.packageIdentity=packageIdentity;record.dataIdentity=dataIdentity;record.fixture=fixture.identity;
      if(counterReference)counterReference.selectedReplaySha256=fixture.identity.sha256;
      record.candidateCounterReference=counterReference;
      record.compatibility={complete:false,status:'incomplete',reason:'A hash-bound original terminal reference with matching counter semantics is not attached to this target.'};
      if(target.variant==='multiplayer'){
        record.scope='The supplied single-player Replay is checked through the current multiplayer Runtime only when its native import format is compatible; this does not cover MP-11 recording or synchronized player lanes.';
        record.fixtureAdmission=multiplayerFixtureAdmission(target,fixture.contents);
      }
      verifiedPackages.set(target.id,packageIdentity);verifiedData.set(target.id,dataIdentity);fixtures.set(target.id,fixture);
      record.checks.push({id:'verified-current-package-data-and-replay',status:'passed',packageIdentity,dataIdentity,fixture:fixture.identity,
        candidateCounterReference:counterReference});
      if(target.game==='th20'){
        record.status='incomplete';record.compatibility={complete:false,status:'blocked',reason:'The official TH20 Shell has storage read/write/list commands but no production playback command or Replay viewer option.'};
        record.playback={status:'unsupported',scenarioStatus:'blocked',reason:record.compatibility.reason};
      }else if(record.fixtureAdmission?.status==='requires-multiplayer-replay'){
        record.status='incomplete';record.compatibility={complete:false,status:'blocked',reason:record.fixtureAdmission.reason};
        record.playback={status:'pending-multiplayer-fixture',scenarioStatus:'blocked',reason:record.fixtureAdmission.reason,evidence:record.fixtureAdmission.evidence};
      }
    }catch(error){record.status='failed';record.error=String(error.stack||error);record.checks.push({id:'verified-current-package-data-and-replay',status:'failed',error:record.error});}
    report.decisionInputs=publicDecisionInputs();
    await save();
  }
  const prepared=selection.targets.filter(target=>verifiedPackages.has(target.id)&&records.get(target.id)?.playback?.status!=='pending-multiplayer-fixture');
  report.decisionInputsPreflight=publicDecisionInputs().map(({path,bytes,sha256})=>({path,bytes,sha256}));
  if(planOnly){
    report.status=report.targets.some(record=>record.status==='failed')?'failed':'incomplete';report.passed=false;
    report.reason='Plan-only mode verified package, DATA, Replay, and any available original counter references; no browser playback was run.';
    for(const target of prepared){const record=records.get(target.id);if(!record.playback)record.playback={status:'not-run'};}
    report.decisionInputsPostflight=await verifyDecisionInputs();
    await save();return;
  }

  const hostDocuments=new Map(),runtimeHtmlDocuments=new Map();
  report.runtimeHtmlDocuments={};
  for(const target of prepared){
    const bytes=Buffer.from(hostPageHtml(target,1,report.runId),'utf8');
    const copyPath=`evidence/host-documents/${target.id}.html`,absolutePath=resolve(output,copyPath);
    await mkdir(resolve(absolutePath,'..'),{recursive:true});await writeFile(absolutePath,bytes);
    hostDocuments.set(target.id,{target:target.id,path:`/host/${target.id}`,copyPath,bytes:bytes.length,sha256:hash(bytes)});
    const documents=new Map(),identity=verifiedPackages.get(target.id);
    for(const name of Object.keys(identity.files).filter(name=>extname(name).toLowerCase()==='.html')){
      const expected=identity.files[name],body=await readVerifiedRuntimeFile(target,identity,name);
      assert.equal(body.length,expected.bytes,`${target.id}: Runtime HTML expected byte size differs from package inventory: ${name}`);
      assert.equal(hash(body),expected.sha256,`${target.id}: Runtime HTML expected SHA differs from package inventory: ${name}`);
      const documentPath=`evidence/runtime-html-documents/${target.id}/${name}`,documentAbsolute=resolve(output,documentPath);
      await mkdir(resolve(documentAbsolute,'..'),{recursive:true});await writeFile(documentAbsolute,body);
      documents.set(name,{target:target.id,path:name,requestPath:`/runtime/${target.id}/${name}`,copyPath:documentPath,bytes:body.length,sha256:hash(body)});
    }
    runtimeHtmlDocuments.set(target.id,documents);report.runtimeHtmlDocuments[target.id]=[...documents.values()];
  }
  report.hostDocuments=Object.fromEntries(hostDocuments);
  await save();
  const fixtureServer=await createFixtureServer(async(request,response)=>{
    try{
      const url=new URL(request.url,'https://127.0.0.1');
      if(url.pathname==='/favicon.ico'){response.writeHead(204).end();return;}
      const selected=config.targets.find(target=>url.pathname===`/host/${target.id}`);
      if(selected){
        const epoch=Number(url.searchParams.get('epoch')??1);assert.equal(epoch,1,'Replay Runtime host requires its recorded epoch 1');
        assert.equal(url.searchParams.get('runId'),report.runId,'Replay host request must use this report runId');
        const bytes=Buffer.from(hostPageHtml(selected,epoch,report.runId),'utf8'),expected=hostDocuments.get(selected.id);
        assert.ok(expected,`${selected.id}: host document was not captured before serving`);
        assert.equal(bytes.length,expected.bytes,`${selected.id}: generated host document size changed`);
        assert.equal(hash(bytes),expected.sha256,`${selected.id}: generated host document changed`);
        response.writeHead(200,{'Content-Type':'text/html','Content-Length':bytes.length,'Cache-Control':'no-store, no-cache, must-revalidate','Pragma':'no-cache','Expires':'0'});response.end(bytes);return;
      }
      const shared=sharedInputs.get(url.pathname);
      if(shared){
        const bytes=await readFile(shared.path);
        assert.equal(bytes.length,shared.bytes,`Shared resource size changed after preflight: ${shared.name}`);
        assert.equal(hash(bytes),shared.sha256,`Shared resource changed after preflight: ${shared.name}`);
        response.writeHead(200,{'Content-Type':mime(shared.path),'Content-Length':bytes.length,'Cache-Control':'no-store, no-cache, must-revalidate','Pragma':'no-cache','Expires':'0'});response.end(bytes);return;
      }
      let file=null,dataTarget=null;
      for(const target of selection.targets){
        if(url.pathname===`/data/${target.id}`){file=target.data;dataTarget=target;}
        const prefix=`/runtime/${target.id}/`;
        if(url.pathname.startsWith(prefix)){
          const identity=verifiedPackages.get(target.id);assert.ok(identity,`${target.id}: package was not verified before serving`);
          const tail=decodeURIComponent(url.pathname.slice(prefix.length));
          const bytes=await readVerifiedRuntimeFile(target,identity,tail);
          response.writeHead(200,{'Content-Type':mime(tail),'Content-Length':bytes.length,'Cache-Control':'no-store, no-cache, must-revalidate','Pragma':'no-cache','Expires':'0'});response.end(bytes);return;
        }
      }
      if(!file){response.writeHead(404).end('Unknown acceptance resource');return;}
      const bytes=await readFile(file);
      if(dataTarget){const expected=verifiedData.get(dataTarget.id);assert.ok(expected,`${dataTarget.id}: DATA not preflighted`);
        assert.equal(bytes.length,expected.bytes,`${dataTarget.id}: DATA changed after preflight`);assert.equal(hash(bytes),expected.sha256,`${dataTarget.id}: DATA changed after preflight`);}
      response.writeHead(200,{'Content-Type':mime(file),'Content-Length':bytes.length,'Cache-Control':'no-store, no-cache, must-revalidate','Pragma':'no-cache','Expires':'0'});response.end(bytes);
    }catch(error){response.writeHead(404).end(String(error.message||error));}
  },{protocol:'https',port});
  let browser;
  try{
    report.fixtureServer={protocol:fixtureServer.protocol,host:fixtureServer.host,port:fixtureServer.port,
      tlsIdentity:fixtureServer.tlsIdentity,trustScope:'Chromium trusts the ephemeral fixture certificate SPKI pin; BrowserContext certificate validation remains enabled.'};
    await save();
    const {launchBrowser}=await import('../browser/launch.mjs');
    browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required',
      `--ignore-certificate-errors-spki-list=${fixtureServer.tlsIdentity.spkiSha256}`]});
    report.browser=browser.version();
    for(const target of prepared){
      const record=records.get(target.id);
      if(target.game==='th20')continue;
      const context=await browser.newContext({viewport:{width:1100,height:820}}),page=await context.newPage();
      record.runtimeProtocolEvents=[];record.runtimeProtocolErrors=[];record.protocolCaptureErrors=[];
      await page.exposeBinding('__captureReplayProtocolFailure',(_source,event)=>record.runtimeProtocolEvents.push(event));
      await page.addInitScript(({game,epoch})=>{
        if(window!==window.top)return;
        const pending=new Set(),runtimeFrame=()=>document.querySelector('iframe');
        window.__flushReplayProtocolCaptures=async()=>{while(pending.size)await Promise.all([...pending]);return pending.size;};
        addEventListener('message',event=>{
          const data=event.data;
          if(event.origin!==location.origin||event.source!==runtimeFrame()?.contentWindow||data?.protocol!=='eagler-touhou/1'||
            data.game!==game||data.epoch!==epoch||!(data.event==='error'||data.event==='abort'||data.ok===false))return;
          const capture=Promise.resolve(window.__captureReplayProtocolFailure(data));pending.add(capture);
          void capture.then(()=>pending.delete(capture),()=>pending.delete(capture));
        });
      },{game:target.game,epoch:1});
      const responseEvidenceDirectory=resolve(output,'evidence/browser-response-bodies',target.id);
      await mkdir(responseEvidenceDirectory,{recursive:true});
      const identity=verifiedPackages.get(target.id);
      const responseProbe=await createFixtureResponseProbe(page,{base:fixtureServer.base,label:`replay-${target.id}`,output:responseEvidenceDirectory,
        expectedResponse:async url=>{
          if(url.pathname===`/host/${target.id}`)return hostDocuments.get(target.id);
          const shared=sharedInputs.get(url.pathname);if(shared)return shared;
          if(url.pathname===`/data/${target.id}`)return verifiedData.get(target.id);
          const prefix=`/runtime/${target.id}/`;
          if(url.pathname.startsWith(prefix)){
            const name=decodeURIComponent(url.pathname.slice(prefix.length)),expected=identity.files[name];
            return expected?{...expected,path:name}:null;
          }
          return null;
        }});
      report.fixtureResponseCapture=responseProbe.capture;
      record.responseCapture={...responseProbe.capture,evidenceDirectory:relative(output,responseEvidenceDirectory).replaceAll('\\','/')};
      record.responseIntegrityErrors=responseProbe.errors;record.fixtureResponses=responseProbe.responses;
      record.responsePolicyErrors=[];record.responseErrors=[];record.teardownErrors=[];
      record.sharedResponses=[];record.runtimeHtmlResponses=[];record.hostDocumentResponse=null;
      const updateResponseSummary=()=>{
        record.fixtureResponses=responseProbe.responses;
        record.hostDocumentResponse=responseProbe.responses.find(item=>new URL(item.url).pathname===`/host/${target.id}`)??null;
        record.sharedResponses=responseProbe.responses.filter(item=>new URL(item.url).pathname.startsWith('/shared/'));
        record.runtimeHtmlResponses=responseProbe.responses.filter(item=>{
          const url=new URL(item.url),prefix=`/runtime/${target.id}/`;
          return url.pathname.startsWith(prefix)&&extname(decodeURIComponent(url.pathname.slice(prefix.length))).toLowerCase()==='.html';
        });
      };
      const collectFixtureResponsePolicyErrors=()=>{
        updateResponseSummary();
        const errors=[];
        for(const item of responseProbe.responses){
          const headers=item.headers??{},cacheControl=Object.entries(headers).find(([name])=>name.toLowerCase()==='cache-control')?.[1]??'';
          if(!/\bno-store\b/i.test(cacheControl))errors.push({url:item.url,rule:'cache-control-no-store',observed:cacheControl});
          if(item.fromServiceWorker!==false)errors.push({url:item.url,rule:'direct-fixture-response',observed:item.fromServiceWorker});
          const url=new URL(item.url),isHost=url.pathname===`/host/${target.id}`;
          if(isHost||url.pathname===`/runtime/${target.id}/${target.entry}`)
            if(url.searchParams.get('runId')!==report.runId)
              errors.push({url:item.url,rule:'report-run-id',observed:url.searchParams.get('runId'),expected:report.runId});
        }
        return errors;
      };
      const assertFixtureResponsePolicies=()=>{
        const errors=collectFixtureResponsePolicyErrors();
        assert.deepEqual(errors,[],`${target.id}: fixture response cache, service-worker, or runId policy failed`);
      };
      const captureProtocolErrors=async()=>{
        const capture=await page.evaluate(async()=>{
          const pending=await window.__flushReplayProtocolCaptures?.();
          return{events:window.events??[],pending:Number.isSafeInteger(pending)?pending:null};
        });
        record.protocolCaptureDrain={pending:capture.pending,observedAt:new Date().toISOString()};
        assert.equal(capture.pending,0,`${target.id}: same-origin iframe protocol binding calls were not drained`);
        record.runtimeProtocolEventsFromWindow=replayProtocolFailures(capture.events);
        mergeReplayProtocolFailures();
        return record.runtimeProtocolErrors;
      };
      const mergeReplayProtocolFailures=()=>{
        const failures=[...(record.runtimeProtocolEventsFromWindow??[]),...replayProtocolFailures(record.runtimeProtocolEvents)];
        record.runtimeProtocolErrors=failures;
      };
      page.on('pageerror',error=>record.errors.push(String(error)));
      page.on('console',message=>{if(message.type()==='error')record.consoleErrors.push(message.text());});
      page.on('response',response=>{if(response.status()>=400)record.httpErrors.push({url:response.url(),status:response.status()});});
      page.on('requestfailed',request=>record.httpErrors.push({url:request.url(),failure:request.failure()?.errorText??'request failed'}));
      const step=async(id,operation)=>{
        try{const value=await operation();record.checks.push({id,status:'passed',value});await save();return value;}
        catch(error){record.checks.push({id,status:'failed',error:String(error.stack||error)});record.status='failed';record.error=String(error.stack||error);await save();throw error;}
      };
      try{
        await step('official-shell-ready',async()=>{
          const hostUrl=`${fixtureServer.base}/host/${target.id}?epoch=1&runId=${encodeURIComponent(report.runId)}`;
          record.hostDocumentRequestUrl=hostUrl;
          await page.goto(hostUrl);
          await responseProbe.flush();updateResponseSummary();assertFixtureResponsePolicies();
          assert.deepEqual(record.responseIntegrityErrors,[],`${target.id}: host document raw response identity mismatch`);
          assert.ok(record.hostDocumentResponse,`${target.id}: browser did not receive and preserve the host document response`);
          await page.waitForFunction(()=>events.some(event=>event.event==='ready'),null,{timeout:120000});
          const ready=await page.evaluate(()=>events.find(event=>event.event==='ready'));
          assert.equal(ready.protocol,'eagler-touhou/1');assert.equal(ready.game,target.game);assert.equal(ready.epoch,1);
          record.runtimeEntryUrl=page.frames().find(frame=>frame.url().includes(`/runtime/${target.id}/`))?.url()??null;
          assert.ok(record.runtimeEntryUrl,`${target.id}: official packaged entry iframe was not loaded`);
          await responseProbe.flush();updateResponseSummary();assertFixtureResponsePolicies();
          assert.deepEqual(record.responseIntegrityErrors,[],`${target.id}: Runtime HTML raw response identity mismatch before Replay import`);
          const entryDocument=runtimeHtmlDocuments.get(target.id)?.get(target.entry);
          assert.ok(entryDocument,`${target.id}: verified package entry is not a captured HTML document`);
          assert.ok(record.runtimeHtmlResponses.some(item=>new URL(item.url).pathname===entryDocument.requestPath&&item.status===200),
            `${target.id}: browser did not receive the recorded Runtime entry HTML response`);
          return ready;
        });
        await step('configure-replay-viewer-and-launch',async()=>{
          const sharedResources=Object.keys(config.shared??{}).map(name=>({path:`/${name}`,url:`/shared/${name}`}));
          await page.evaluate(({sharedResources,options})=>rpc('configure',{language:'ja',music:'none',resources:[],runtimeResources:[],sharedResources,
            thpracCatalog:[],options:{alwaysHitbox:false,touchEnabled:false,thpracEnabled:false,limitPresentationTo60:false,...options,replayViewer:true}}),
            {sharedResources,options:target.options??{}});
          await page.evaluate(()=>rpc('launch'));
          await page.waitForFunction(()=>events.some(event=>event.event==='first-frame'),null,{timeout:180000});
          return page.evaluate(()=>events.filter(event=>event.event==='first-frame').at(-1));
        });
        const fixture=fixtures.get(target.id),prefix=PRODUCT_GAMES[target.game].replay.prefix;
        const chars='abcdefghijklmnopqrstuvwxyz0123456789',suffix=Array.from(randomBytes(4),byte=>chars[byte%chars.length]).join('');
        const replayExtension=fixture.identity.path.toLowerCase().endsWith('.rpyx')?'rpyx':'rpy';
        const replayPath=`replay/${prefix}_ud${suffix}.${replayExtension}`;
        await step('import-current-replay-through-shell',async()=>{
          const bytes=Array.from(fixture.contents);
          await page.evaluate(({path,bytes})=>rpc('write',{path,bytes}),{path:replayPath,bytes});
          const stored=await page.evaluate(path=>rpc('read',{path}),replayPath);
          assert.deepEqual(stored.bytes,bytes,`${target.id}: stored Replay bytes differ from fixture`);
          const listed=await page.evaluate(()=>rpc('list'));
          assert.ok(listed.files.some(file=>file.path===replayPath),`${target.id}: imported Replay is not listed by its official Shell`);
          return{path:replayPath,bytes:fixture.identity.bytes,sha256:hash(fixture.contents),listed:listed.files.filter(file=>file.path===replayPath)};
        });
        const description=await step('native-replay-inspection',async()=>{
          if(target.game==='th09'){
            record.replayDescription={method:'official-playback-open',inspection:'not-implemented',
              reason:'th09 creates its ReplayPlayer through playback action open; the official shell has no inspect action.'};
            return null;
          }
          const inspected=await page.evaluate(path=>rpc('playback',{action:'inspect',path}),replayPath);
          assert.ok(inspected.description&&Array.isArray(inspected.description.stages)&&inspected.description.stages.length,
            `${target.id}: native Replay inspector did not return any selectable stages`);
          record.replayDescription=inspected.description;
          return inspected.description;
        });
        const selectedStage=description?.stages?.[0]?.id??null;
        const strictConsumptionPolicy=replayTerminalPolicy({game:target.game,variant:target.variant});
        let replayInputExpectation=null;
        if(strictConsumptionPolicy){
          await step('derive-independent-replay-read-capacity',async()=>{
            assert.ok(Number.isInteger(selectedStage),`${target.id}: th08 normal Replay inspector did not select a numeric stage`);
            replayInputExpectation=deriveTh08ReplayInputExpectation(fixture.contents,{stage:selectedStage,expectedSha256:fixture.identity.sha256});
            record.replayInputExpectation=replayInputExpectation;
            return{source:replayInputExpectation.source,stage:replayInputExpectation.stage,sha256:replayInputExpectation.sha256,
              blocks:replayInputExpectation.blocks,stride:replayInputExpectation.stride,capacity:replayInputExpectation.capacity};
          });
        }
        await step('native-replay-plays-and-advances',async()=>{
          const opened=await page.evaluate(({path,stage})=>rpc('playback',{action:'open',path,...(stage===null?{}:{stage})}),{path:replayPath,stage:selectedStage});
          const naturalFrames=120,started=Date.now(),progress=[];let last=null,latest=opened.playback??opened;
          record.playbackOpenState=playbackObservation(latest);
          const naturalStartFrame=Number.isSafeInteger(latest.frame)?latest.frame:
            Number.isSafeInteger(latest.startFrame)?latest.startFrame:Number.isSafeInteger(latest.start)?latest.start:0;
          while(Date.now()-started<60000){
            latest=await page.evaluate(()=>events.filter(event=>event.event==='runtime-info'&&event.playback?.kind==='replay').at(-1)?.playback??null)??latest;
            if(latest&&Number.isSafeInteger(latest.frame)&&latest.frame!==last){progress.push({...playbackObservation(latest),observedAt:new Date().toISOString()});last=latest.frame;}
            if(latest&&Number.isSafeInteger(latest.frame)&&latest.frame>=naturalStartFrame+naturalFrames)break;
            await page.waitForTimeout(80);
          }
          assert.ok(latest&&Number.isSafeInteger(latest.frame)&&latest.frame>=naturalStartFrame+naturalFrames,
            `${target.id}: official Replay did not advance ${naturalFrames} frames at its default rate: ${JSON.stringify(latest)}`);
          assert.equal(latest.rate,1,`${target.id}: natural-window measurement requires the official default rate 1`);
          record.playbackProgress=progress;
          await page.screenshot({path:resolve(output,`${target.id}-replay-playing.png`)});
          record.playbackPacing={naturalRate:1,naturalFrames,acceleratedRate:0,
            acceleration:'Official local Replay bridge rate 0 keeps the native step path and runs bounded sequential batches.'};
          return{stage:selectedStage,startFrame:naturalStartFrame,naturalFrames,observed:progress.slice(-8),frame:latest.frame};
        });
        await step('native-terminal-signal-observed',async()=>{
          await page.evaluate(()=>rpc('playback',{action:'rate',value:0}));
          const startedAt=Date.now(),initialState=await page.evaluate(()=>events.filter(event=>event.event==='runtime-info'&&event.playback?.kind==='replay').at(-1)?.playback??null);
          const naturalMaximumFrame=Math.max(record.playbackOpenState?.frame??-Infinity,...(record.playbackProgress??[]).map(item=>item.frame??-Infinity));
          const tracker=createReplayProgressTracker({startedAt,initialState,initialMaximumFrame:Number.isFinite(naturalMaximumFrame)?naturalMaximumFrame:null}),checkpoints=[];
          record.playbackTerminalSignalProgress=checkpoints;
          let latest=initialState,finalState=null,lastCheckpointAt=startedAt;
          while(Date.now()-startedAt<replayTimeout){
            latest=await page.evaluate(()=>events.filter(event=>event.event==='runtime-info'&&event.playback?.kind==='replay').at(-1)?.playback??null)??latest;
            const observedAt=Date.now(),sample=tracker.sample(latest,observedAt);
            checkpoints.push(sample);
            record.playbackTerminalSignalMonitor={startedAt:new Date(startedAt).toISOString(),elapsedMs:observedAt-startedAt,
              lastAdvanceAt:new Date(sample.lastAdvanceAt).toISOString(),lastAdvanceAgeMs:sample.lastAdvanceAgeMs,
              checkpointIntervalMs:2000,stallTimeoutMs:stallTimeout,lastState:sample};
            if(sample.ended){finalState=latest;await save();break;}
            if(sample.lastAdvanceAgeMs>=stallTimeout)
              throw new Error(`${target.id}: native Replay cursor/stage stalled for ${sample.lastAdvanceAgeMs}ms at ${JSON.stringify(sample)}`);
            if(observedAt-lastCheckpointAt>=2000){lastCheckpointAt=observedAt;await save();}
            await page.waitForTimeout(2000);
          }
          if(!finalState)throw new Error(`${target.id}: native Replay did not report its terminal signal within ${replayTimeout}ms; last observation ${JSON.stringify(record.playbackTerminalSignalMonitor?.lastState??null)}`);
          const runtimeFrame=page.frames().find(frame=>frame.url().includes(`/runtime/${target.id}/`));
          const nativeRuntimeObservation=runtimeFrame?await runtimeFrame.evaluate(readRuntimeObservation,runtimeObservationRequest(target)):null;
          record.nativeReplayObservation=nativeRuntimeObservation?.replay??null;
          const nativeInputConsumption=strictConsumptionPolicy?assessTh08ReplayInputConsumption({
            expected:replayInputExpectation,actual:nativeRuntimeObservation?.replay?.stream,ended:finalState.ended===true,
          }):null;
          const assessment=assessReplayTerminalSignal({game:target.game,variant:target.variant,state:finalState,candidateCounterReference:record.candidateCounterReference,
            inspectionStages:description?.stages??[],maximumFrame:tracker.maximumFrame,nativeInputConsumption});
          record.playbackTerminalSignalAssessment=assessment;
          assert.notEqual(assessment.status,'failed',`${target.id}: ${assessment.reason} ${JSON.stringify(assessment.comparison??assessment.observation)}`);
          record.playback={status:assessment.status,path:replayPath,fixture:fixture.identity,stage:selectedStage,
            state:playbackObservation(finalState),progress:record.playbackTerminalSignalMonitor,
            nativeReplayObservation:record.nativeReplayObservation,
            terminalSignalObserved:assessment.terminalSignalObserved,
            fullInputConsumption:assessment.fullInputConsumption,
            nativeFormatConsistency:assessment.nativeFormatConsistency??null,
            nativeCounterObservation:assessment.nativeCounterObservation??null,
            candidateCounterObservation:assessment.candidateCounterObservation??null,
            terminalSignalScope:assessment.terminalSignalScope??'Native Runtime reported its terminal signal; full Replay input consumption and original-format compatibility remain unproven.'};
          return record.playback;
        });
        assert.deepEqual(record.errors,[],`${target.id}: page errors during Replay playback`);
        assert.deepEqual(record.consoleErrors,[],`${target.id}: browser Console errors during Replay playback`);
        assert.deepEqual(await captureProtocolErrors(),[],`${target.id}: Runtime protocol error, abort, or rejected command during Replay playback`);
        assert.deepEqual(record.httpErrors,[],`${target.id}: HTTP errors during Replay playback`);
        await responseProbe.flush();updateResponseSummary();assertFixtureResponsePolicies();
        assert.deepEqual(record.responseIntegrityErrors,[],`${target.id}: fixture response identity mismatch`);
        assert.ok(record.hostDocumentResponse,`${target.id}: browser did not receive the recorded host document`);
        const observedShared=new Set(record.sharedResponses.map(item=>new URL(item.url).pathname));
        const missingShared=[...sharedInputs.keys()].filter(path=>!observedShared.has(path));
        assert.deepEqual(missingShared,[],`${target.id}: configured shared resources were not observed in browser responses`);
        const latestPackage=await verifyRuntimePackage(target,{repoRoot}),latestData=await verifyPrivateData(target.data),latestFixture=await loadRuntimeReplayFixture(target);
        assert.deepEqual(latestPackage,verifiedPackages.get(target.id),`${target.id}: Runtime package changed during playback`);
        assert.deepEqual(latestData,verifiedData.get(target.id),`${target.id}: private DATA changed during playback`);
        assert.deepEqual(latestFixture.identity,fixture.identity,`${target.id}: Replay input changed during playback`);
        record.postflight={packageIdentity:latestPackage,dataIdentity:latestData,fixture:latestFixture.identity};
        assert.ok(record.playback&&['observed','incomplete'].includes(record.playback.status),`${target.id}: Replay scenario lacks a native terminal signal assessment`);
        const scenarioPassed=record.playback.status==='observed'&&
          (!strictConsumptionPolicy||hasHashBoundNaturalReplayEndpoint(record.playback));
        record.playback.scenarioStatus=scenarioPassed?'passed':'incomplete';
        record.playback.scenarioEvidence={officialShellImport:'passed',nativeFramesAtDefaultRate:record.playbackPacing.naturalFrames,
          terminalSignalObserved:record.playback.terminalSignalObserved,fullInputConsumption:record.playback.fullInputConsumption,
          replayInputExpectation:record.replayInputExpectation??null,
          browserAndRuntimeErrors:'pending-final-audit',rawHtmlResponseIntegrity:'passed'};
        if(scenarioPassed){
          record.status='passed';
          record.compatibility=strictConsumptionPolicy?
            {complete:true,status:'passed',reason:'Native th08 Replay counters match the hash-bound original natural endpoint and independently parsed stage capacity.'}:
            {complete:false,status:'incomplete',reason:'This target has no exact native terminal count with a source-attested same-counter mapping bound to this fixture.'};
        }else{
          const reason=record.playback.fullInputConsumption?.reason??'Native Replay input consumption is unavailable.';
          record.status='incomplete';record.compatibility={complete:false,status:'incomplete',reason};
        }
      }catch(error){record.status='failed';record.error=String(error.stack||error);
        await captureProtocolErrors().catch(captureError=>record.protocolCaptureErrors.push(String(captureError.stack||captureError)));
        record.failureState=await page.evaluate(()=>({events:window.events?.slice(-20)??[],body:document.body.innerText})).catch(()=>null);
        await page.screenshot({path:resolve(output,`${target.id}-replay-failure.png`)}).catch(()=>{});
      }finally{
        let responseProbeFlushed=false,responseProbeFlushFailed=false,responseProbeStopped=false,contextClosed=false,pageClosed=false;
        try{await responseProbe.flush();responseProbeFlushed=true;}
        catch(error){responseProbeFlushFailed=true;record.teardownErrors.push({phase:'responseProbe.flush',error:String(error.stack||error)});}
        try{await responseProbe.stop();responseProbeStopped=true;}
        catch(error){record.teardownErrors.push({phase:'responseProbe.stop',error:String(error.stack||error)});}
        try{await responseProbe.flush();responseProbeFlushed=true;}
        catch(error){responseProbeFlushFailed=true;record.teardownErrors.push({phase:'responseProbe.finalFlush',error:String(error.stack||error)});}
        updateResponseSummary();record.responsePolicyErrors=collectFixtureResponsePolicyErrors();
        try{await captureProtocolErrors();}
        catch(error){record.protocolCaptureErrors.push(String(error.stack||error));}
        const pageClosedEvent=page.isClosed()?Promise.resolve():new Promise(resolveClosed=>page.once('close',resolveClosed));
        try{await context.close();contextClosed=true;await pageClosedEvent;pageClosed=true;}
        catch(error){record.teardownErrors.push({phase:'context.close',error:String(error.stack||error)});}
        // Binding callbacks synchronously append their payloads before the page-close event;
        // context.close plus the observed close event is the drain boundary for late messages.
        mergeReplayProtocolFailures();
        updateResponseSummary();
        record.responseErrors=[
          ...record.responseIntegrityErrors.map(error=>({kind:'identity',error})),
          ...record.responsePolicyErrors.map(error=>({kind:'policy',...error})),
        ];
        const teardownErrors=[...record.teardownErrors,...record.protocolCaptureErrors.map(error=>({phase:'protocolCapture',error}))];
        gateReplayRecordOnFinalErrors(record,{pageErrors:record.errors,consoleErrors:record.consoleErrors,
          httpErrors:record.httpErrors,protocolErrors:record.runtimeProtocolErrors,responseErrors:record.responseErrors,teardownErrors});
        record.finalErrorAudit.lifecycle={responseProbeFlushed:responseProbeFlushed&&!responseProbeFlushFailed?'passed':'failed',
          responseProbeStopped:responseProbeStopped?'passed':'failed',
          protocolCaptureDrainedBeforeClose:record.protocolCaptureErrors.length===0&&record.protocolCaptureDrain?.pending===0,
          protocolCaptureDrain:record.protocolCaptureDrain??null,bindingEventsAtClose:record.runtimeProtocolEvents.length,
          contextClosed,pageClosed};
        record.checks.push(replayOriginalImportCheck(record,{game:target.game,variant:target.variant}));
        await save();
      }
    }
  }finally{await browser?.close();await fixtureServer.close();}
  for(const target of selection.targets){
    const record=records.get(target.id);
    if(!record||record.postflight||!verifiedPackages.has(target.id))continue;
    const latestPackage=await verifyRuntimePackage(target,{repoRoot}),latestData=await verifyPrivateData(target.data),latestFixture=await loadRuntimeReplayFixture(target);
    assert.deepEqual(latestPackage,verifiedPackages.get(target.id),`${target.id}: Runtime package changed after Replay acceptance`);
    assert.deepEqual(latestData,verifiedData.get(target.id),`${target.id}: private DATA changed after Replay acceptance`);
    assert.deepEqual(latestFixture.identity,fixtures.get(target.id).identity,`${target.id}: Replay input changed after Replay acceptance`);
    record.postflight={packageIdentity:latestPackage,dataIdentity:latestData,fixture:latestFixture.identity};
  }
  const sharedPostflight={};
  for(const [url,identity] of sharedInputs){
    const bytes=await readFile(identity.path);
    assert.equal(bytes.length,identity.bytes,`Shared resource size changed during Replay acceptance: ${identity.name}`);
    assert.equal(hash(bytes),identity.sha256,`Shared resource changed during Replay acceptance: ${identity.name}`);
    sharedPostflight[url]={path:identity.path,bytes:bytes.length,sha256:hash(bytes)};
  }
  report.sharedPostflight=sharedPostflight;
  report.decisionInputsPostflight=await verifyDecisionInputs();
  report.decisionInputs=publicDecisionInputs();
  const failures=report.targets.some(record=>record.status==='failed');
  const incomplete=report.targets.some(record=>record.status!=='passed'||record.compatibility?.complete!==true);
  const multiplayerTargets=report.targets.filter(record=>record.variant==='multiplayer');
  report.coverage={selectedTargets:report.targets.length,expectedTargets:selection.expected.length,
    scenarioPassedTargets:replayScenarioPassedTargetIds(report.targets),
    compatibilityCompleteTargets:report.targets.filter(record=>record.compatibility?.complete===true).map(record=>record.id),
    nativeTerminalSignalObservedTargets:report.targets.filter(record=>record.playback?.status==='observed').map(record=>record.id),
    blockedTargets:report.targets.filter(record=>record.playback?.scenarioStatus==='blocked').map(record=>record.id),
    unsupportedPlaybackTargets:report.targets.filter(record=>record.playback?.status==='unsupported').map(record=>record.id),
    pendingMultiplayerFixtureTargets:report.targets.filter(record=>record.playback?.status==='pending-multiplayer-fixture').map(record=>record.id),
    mp11:{status:multiplayerTargets.length?'incomplete':'not-selected',complete:false,
      reason:multiplayerTargets.length?'This driver imports configured Replay files and does not record one from a real multiplayer session.':null,
      targets:multiplayerTargets.map(record=>({id:record.id,fixtureAdmission:record.fixtureAdmission?.status??'not-assessed'}))}};
  report.status=failures?'failed':incomplete?'incomplete':'passed';report.passed=report.status==='passed';
  if(report.status!=='passed')process.exitCode=1;
  report.finishedAt=new Date().toISOString();await save();
});
if(result.status!=='passed')process.exitCode=1;

function mime(path){
  const extension=extname(path).toLowerCase();
  return({'.html':'text/html','.mjs':'text/javascript','.js':'text/javascript','.wasm':'application/wasm','.json':'application/json',
    '.ttc':'font/ttf',
    '.css':'text/css','.png':'image/png','.ttf':'font/ttf','.otf':'font/otf','.mp3':'audio/mpeg','.ogg':'audio/ogg'})[extension]??'application/octet-stream';
}

function multiplayerFixtureAdmission(target,bytes){
  const signature=Buffer.from(bytes.subarray(0,8)).toString('ascii');
  if(target.game==='th08'){
    if(signature==='T8MPRPY1')return{status:'candidate-multiplayer-format'};
    return{status:'requires-multiplayer-replay',reason:'th08 MP import requires a .rpyx multiplayer Replay archive. The configured original .rpy has a different format.',evidence:['ports/th08-mp/web/sdl-runtime/shell.mjs:478','ports/th08-mp/web/cpp/multiplayer/ReplayArchive.cpp']};
  }
  if(['th10','th11','th15'].includes(target.game)){
    if(signature==='EAGLRPY1')return{status:'candidate-multiplayer-format'};
    const evidence=target.game==='th10'?['ports/th10-mp/portable/browser/replay-file-policy.mjs:3-26','ports/th10-mp/web/sdl-runtime/shell.mjs:315-320']:
      target.game==='th11'?['ports/th11-mp/web/sdl-runtime/shell.mjs:310,361-365','ports/th11-mp/web/cpp/multiplayer/ReplayArchive.cpp:13-30']:
      ['ports/th15-mp/web/sdl-runtime/shell.mjs:266-269','ports/th15-mp/web/cpp/multiplayer/ReplayArchive.cpp:17-29'];
    return{status:'requires-multiplayer-replay',reason:`TH${target.game.slice(2)} MP import validates a title-specific EAGLRPY1 multiplayer archive; the configured original .rpy does not use that format.`,evidence};
  }
  if(target.game==='th06')return{status:'single-player-format-compatible',evidence:['ports/th06/resources/shell.html:1108-1110','ports/th06/src/ReplayExtension.cpp:345']};
  if(target.game==='th07')return{status:'single-player-format-compatible',evidence:['ports/th07/resources/shell.html:1153','ports/th07/src/ReplayExtension.cpp:362','ports/th07/src/ReplayManager.cpp:700']};
  if(target.game==='th09')return{status:'single-player-format-compatible',evidence:['ports/th09/web/sdl-runtime/managed.mjs:199-203']};
  return{status:'requires-official-shell-admission',reason:'The inspected Runtime has no matching multiplayer-only signature rule for this fixture. Official Shell import and native playback determine admission; single-player Replay playback does not establish MP-11.'};
}
