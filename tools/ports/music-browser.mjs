import assert from 'node:assert/strict';
import {createHash,randomUUID} from 'node:crypto';
import {execFileSync} from 'node:child_process';
import {mkdir,readFile,stat,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {createRuntimeFixtureSite} from './runtime-fixture-site.mjs';
import {readVerifiedRuntimeFile,selectRuntimeTargets,verifyPrivateData,verifyRuntimePackage} from './acceptance-contracts.mjs';
import {sourceSnapshot} from '../testing/source-snapshot.mjs';
import {runWithReport} from '../testing/cli-report.mjs';
import {assertCliArguments} from '../testing/cli-options.mjs';
import {readRuntimeObservation} from './runtime-observations.mjs';
import {musicScenarioErrors,musicTitleSceneOutcome} from './music-browser-result.mjs';
import {MUSIC_BROWSER_GAMES,MUSIC_PCM_PROBE_SOURCE,describeNativeMusic,describeNoneMusicControl,mapTrackToInput,
  nativeAudioStatsShape,readNativeAudioState,runtimeMusicResources,selectedMusicMappingGames,summarizeMusicPcm,verifyMusicInputs} from './music-browser-helpers.mjs';

const arg=name=>process.argv.find(value=>value.startsWith(`--${name}=`))?.slice(name.length+3);
const outputArgument=arg('output'),requestedOutput=outputArgument?resolve(outputArgument):null;
const root=resolve(import.meta.dirname,'../..');
let outputExists=false,outputIsDirectory=false,outputInspectionError=null;
if(requestedOutput){
  try{const info=await stat(requestedOutput);outputExists=true;outputIsDirectory=info.isDirectory();}
  catch(error){if(error.code!=='ENOENT'){outputExists=true;outputInspectionError=error;}}
}
const output=requestedOutput&&(!outputExists||outputIsDirectory)?requestedOutput:
  resolve(root,'artifacts/validation',`music-browser-cli-${randomUUID()}`);
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const titleTrack={th11:'th11_00.ogg',th15:'th15_01.ogg'};

function trackMapFromTh11(source) {
  return Object.fromEntries([...source.matchAll(/\{\s*(\d+)\s*,\s*"([A-Za-z0-9_]+)"\s*\}/g)].map(([,cue,stem])=>[cue,stem]));
}

function gameplayReached(game,observation) {
  if(game==='th08')return observation.worldProgress?.comparable===true;
  if(game==='th10')return observation.status?.screen===7;
  if(game==='th11'||game==='th15')return observation.phase?.name==='stage';
  if(game==='th20')return observation.phase?.name==='stage';
  return false;
}

async function pressStart(page) {
  await page.evaluate(()=>rpc('keyboard',{code:'KeyZ',down:true,inputSource:'physical'}));
  try{await page.waitForTimeout(120);}finally{
    await page.evaluate(()=>rpc('keyboard',{code:'KeyZ',down:false,inputSource:'physical'}));
  }
  await page.waitForTimeout(180);
}

const result=await runWithReport({output,schema:'eagler-touhou/music-browser/1'},async(report,save)=>{
  assertCliArguments(process.argv.slice(2),{values:['output','manifest','game','music-manifest','variant'],flags:['plan-only']});
  assert.ok(requestedOutput,'Required: --output=report-directory');
  if(outputInspectionError)throw outputInspectionError;
  assert.ok(!outputExists||outputIsDirectory,'--output must name a directory');
  assert.equal(arg('variant'),'normal','music-browser requires --variant=normal and real normal Runtime packages');
  const manifestArgument=arg('manifest'),musicArgument=arg('music-manifest'),gameArgument=arg('game');
  assert.ok(manifestArgument,'Required: --manifest=accepted-private-inputs.json');
  assert.ok(musicArgument,'Required: --music-manifest=private-local-music-inputs.json');
  assert.ok(gameArgument,'Required: --game=explicit-comma-separated-game-ids');
  const games=gameArgument.split(',');
  assert.ok(games.length&&games.every(Boolean),'--game must contain comma-separated game IDs');
  assert.equal(new Set(games).size,games.length,'--game must not contain duplicate game IDs');
  for(const game of games)assert.ok(MUSIC_BROWSER_GAMES.includes(game),`Unsupported music acceptance game ${game}`);
  const manifestPath=resolve(manifestArgument),musicManifestPath=resolve(musicArgument);
  const manifestBytes=await readFile(manifestPath),config=JSON.parse(manifestBytes.toString('utf8'));
  const selection=selectRuntimeTargets(config,{game:games.join(','),variant:'normal'});
  const musicManifestBytes=await readFile(musicManifestPath),musicManifest=JSON.parse(musicManifestBytes.toString('utf8'));
  const musicInputs=await verifyMusicInputs(musicManifest,{repoRoot:root,games});
  const packageBuilds=selection.targets.map(async target=>({target,identity:{
    runtime:await verifyRuntimePackage(target,{repoRoot:root}),data:await verifyPrivateData(target.data)}}));
  const verifiedPackages=await Promise.all(packageBuilds);
  const mappingGames=selectedMusicMappingGames(games);
  let th11Tracks={},th15Tracks=[];
  const musicMapEvidence={};
  const guardedSources={
    productCatalog:{path:'common/contracts/product-catalog.mjs',file:resolve(root,'common/contracts/product-catalog.mjs')},
    acceptanceContracts:{path:'tools/ports/acceptance-contracts.mjs',file:resolve(root,'tools/ports/acceptance-contracts.mjs')},
    nativeObservationModule:{path:'tools/ports/runtime-observations.mjs',file:resolve(root,'tools/ports/runtime-observations.mjs')},
    responseCapture:{path:'tools/browser/response-body-capture.mjs',file:resolve(root,'tools/browser/response-body-capture.mjs')},
    cliReport:{path:'tools/testing/cli-report.mjs',file:resolve(root,'tools/testing/cli-report.mjs')},
    cliOptions:{path:'tools/testing/cli-options.mjs',file:resolve(root,'tools/testing/cli-options.mjs')},
    browserLaunch:{path:'tools/browser/launch.mjs',file:resolve(root,'tools/browser/launch.mjs')},
    resultModel:{path:'tools/ports/music-browser-result.mjs',file:resolve(root,'tools/ports/music-browser-result.mjs')},
  };
  if(mappingGames.includes('th11')){
    const path=resolve(root,'ports/th11/web/cpp/game/MusicCatalog.hpp'),bytes=await readFile(path);
    th11Tracks=trackMapFromTh11(bytes.toString('utf8'));
    assert.ok(Object.keys(th11Tracks).length>0,'TH11 native music cue table was not found');
    guardedSources.th11MusicCatalog={path:'ports/th11/web/cpp/game/MusicCatalog.hpp',file:path};
    musicMapEvidence.th11={source:'ports/th11/web/cpp/game/MusicCatalog.hpp',sha256:hash(bytes),cueCount:Object.keys(th11Tracks).length};
  }
  if(mappingGames.includes('th15')){
    const path=resolve(root,'ports/th15/web/assets/music/manifest.json'),bytes=await readFile(path);
    const catalog=JSON.parse(bytes.toString('utf8'));
    th15Tracks=catalog.tracks.map(track=>track.filename.replace(/\.wav$/i,'.ogg'));
    guardedSources.th15MusicManifest={path:'ports/th15/web/assets/music/manifest.json',file:path};
    musicMapEvidence.th15={source:'ports/th15/web/assets/music/manifest.json',sha256:hash(bytes),layoutSha256:catalog.layoutSha256,trackCount:th15Tracks.length};
  }
  for(const source of Object.values(guardedSources))source.bytes=await readFile(source.file);
  const maps={th11Tracks,th15Tracks};
  for(const game of games)if(titleTrack[game])
    assert.ok(musicInputs[game].files.some(file=>file.name===titleTrack[game]),`${game}: title music input ${titleTrack[game]} is missing`);
  const sourceSnapshotStart=sourceSnapshot(root),driverBytes=await readFile(new URL(import.meta.url));
  const helperBytes=await readFile(new URL('./music-browser-helpers.mjs',import.meta.url));
  const fixtureBytes=await readFile(new URL('./runtime-fixture-site.mjs',import.meta.url));
  const fixtureServerBytes=await readFile(resolve(root,'tools/browser/fixture-server.mjs'));
  const observationBytes=Buffer.from(`export const readRuntimeObservation = (${readRuntimeObservation.toString()});\n`,'utf8');
  const artifacts={
    harness:{path:'music-browser.mjs',sha256:hash(driverBytes)},
    helper:{path:'music-browser-helpers.mjs',sha256:hash(helperBytes)},
    runtimeFixture:{path:'runtime-fixture-site.mjs',sha256:hash(fixtureBytes)},
    fixtureServer:{path:'fixture-server.mjs',sha256:hash(fixtureServerBytes)},
    nativeObservationReader:{path:'runtime-observation-reader.mjs',sha256:hash(observationBytes)},
    audioProbe:{path:'audio-pcm-probe.js',sha256:hash(Buffer.from(MUSIC_PCM_PROBE_SOURCE,'utf8'))},
  };
  const artifactBytes={harness:driverBytes,helper:helperBytes,runtimeFixture:fixtureBytes,
    fixtureServer:fixtureServerBytes,
    nativeObservationReader:observationBytes,audioProbe:Buffer.from(MUSIC_PCM_PROBE_SOURCE,'utf8')};
  for(const [key,source] of Object.entries(guardedSources)){
    artifacts[key]={path:`source/${source.path.replaceAll('/','__')}`,sha256:hash(source.bytes)};
    artifactBytes[key]=source.bytes;
  }
  await mkdir(resolve(output,'source'),{recursive:true});
  await Promise.all(Object.entries(artifactBytes).map(([key,bytes])=>writeFile(resolve(output,artifacts[key].path),bytes)));
  Object.assign(report,{commit:execFileSync('git',['rev-parse','HEAD'],{cwd:root,encoding:'utf8'}).trim(),
    sourceSnapshotStart,manifest:{path:manifestPath,sha256:hash(await readFile(manifestPath))},
    musicManifest:{path:musicManifestPath,sha256:hash(musicManifestBytes),games:Object.keys(musicInputs)},
    musicInputs,musicMapEvidence,artifacts,
    selection:{games:selection.games,variant:selection.variant,expectedTargets:selection.expected},
    scope:'Chromium | formal Runtime configure and launch | product-catalog OGG mounts | actual Runtime WebAudio output tap | title output scenes. AUDIO-01 full track switching, gameplay scene changes, looping and lifecycle coverage remains itemized separately. TH11 and TH15 expose mapped native track identity; TH08, TH10 and TH20 report decoder and PCM evidence without asserting a track ID. No Launcher UI import, MIDI, cursor-continuity claim or physical listening claim.',
    protocols:{runtime:'eagler-touhou/1',audio:'Per-Runtime-realm AudioNode AnalyserNode tap immediately before AudioDestinationNode',
      nativeStats:Object.fromEntries(games.map(game=>[game,nativeAudioStatsShape(game)]))},
    targets:[]});
  await save();
  const verifyArtifacts=async()=>{
    const current={
      harness:await readFile(new URL(import.meta.url)),
      helper:await readFile(new URL('./music-browser-helpers.mjs',import.meta.url)),
      runtimeFixture:await readFile(new URL('./runtime-fixture-site.mjs',import.meta.url)),
      fixtureServer:await readFile(resolve(root,'tools/browser/fixture-server.mjs')),
      nativeObservationReader:Buffer.from(`export const readRuntimeObservation = (${readRuntimeObservation.toString()});\n`,'utf8'),
      audioProbe:Buffer.from(MUSIC_PCM_PROBE_SOURCE,'utf8'),
    };
    for(const [key,source] of Object.entries(guardedSources))current[key]=await readFile(source.file);
    for(const [key,bytes] of Object.entries(current)){
      assert.equal(hash(bytes),artifacts[key].sha256,`${key} changed during music acceptance`);
      const saved=await readFile(resolve(output,artifacts[key].path));
      assert.equal(hash(saved),artifacts[key].sha256,`${key} evidence copy changed during music acceptance`);
    }
  };
  const snapshotMusicFiles=async game=>{
    const files=new Map();
    for(const entry of musicInputs[game].files){
      const bytes=await readFile(entry.path);
      assert.equal(bytes.length,entry.bytes,`${game}/${entry.name}: OGG changed while preparing fixture`);
      assert.equal(hash(bytes),entry.sha256,`${game}/${entry.name}: OGG changed while preparing fixture`);
      files.set(`/music/${game}/${entry.name}`,bytes);
    }
    return files;
  };
  if(process.argv.slice(2).includes('--plan-only')){
    for(const {target,identity} of verifiedPackages){
      const game=target.game;
      const files=await snapshotMusicFiles(game);
      const shared=Object.fromEntries(Object.entries(config.shared??{}).map(([name,path])=>[name,resolve(path)]));
      const fixture=await createRuntimeFixtureSite({repoRoot:root,targets:[target],shared,files,protocol:'https'});
      try{
        const fixtureIdentity=await fixture.verify(target);
        assert.deepEqual(fixtureIdentity,identity,`${game}: preflight Runtime/DATA identity mismatch`);
        const sharedIdentity=await fixture.verifyShared();
        const resources=runtimeMusicResources(game,musicInputs,{base:fixture.base});
        const delivered=[];
        for(const resource of resources){
          const response=await fixture.fetch(resource.url);assert.ok(response.ok,`${game}: OGG route returned HTTP ${response.status}`);
          const bytes=Buffer.from(await response.arrayBuffer());
          const expected=musicInputs[game].files.find(file=>file.name===resource.path.slice(resource.path.lastIndexOf('/')+1));
          assert.ok(expected,`${game}: OGG resource has no manifest identity`);
          assert.equal(bytes.length,resource.bytes,`${game}/${expected.name}: fixture route byte count mismatch`);
          assert.equal(hash(bytes),expected.sha256,`${game}/${expected.name}: fixture route SHA-256 mismatch`);
          delivered.push({name:expected.name,path:resource.path,url:new URL(resource.url).pathname,bytes:bytes.length,sha256:hash(bytes)});
        }
        assert.equal(fixture.httpFailures.length,0,`${game}: local OGG fixture reported an HTTP failure`);
        assert.deepEqual(await fixture.verify(target),fixtureIdentity,`${game}: package or DATA identity changed during preflight`);
        assert.deepEqual(await fixture.verifyShared(),sharedIdentity,`${game}: shared Runtime inputs changed during preflight`);
        report.targets.push({game,variant:'normal',status:'preflight-only',passed:false,
          runtimeIdentity:fixtureIdentity.runtime,dataIdentity:fixtureIdentity.data,
          sharedIdentity,fixture:{protocol:fixture.protocol,tlsIdentity:fixture.tlsIdentity},musicResources:delivered,nativeStats:nativeAudioStatsShape(game),
          trackIdentity:titleTrack[game]?'native-stat-map-available':'not-exposed-by-native-stats'});
      }finally{await fixture.close();}
    }
    const postflight=await verifyMusicInputs(musicManifest,{repoRoot:root,games});
    assert.deepEqual(postflight,musicInputs,'OGG resource identities changed during music preflight');
    assert.equal(hash(await readFile(manifestPath)),hash(manifestBytes),'Runtime input manifest changed during music preflight');
    assert.equal(hash(await readFile(musicManifestPath)),hash(musicManifestBytes),'Music input manifest changed during music preflight');
    await verifyArtifacts();
    Object.assign(report,{musicInputPostflight:postflight,sourceSnapshotPostflight:sourceSnapshot(root),
      preflight:{status:'passed',browserStarted:false,runtimePlayback:'not-run'},
      status:'incomplete',passed:false,incompleteReason:'Plan-only verifies inputs and delivery routes without starting a Runtime'});
    await save();return;
  }
  for(const {target,identity} of verifiedPackages){
      const game=target.game,record={game,variant:'normal',id:target.id,status:'running',passed:false,modes:[],checks:[],errors:[],consoleErrors:[],httpErrors:[]};
      report.targets.push(record);await save();
      const shared={...Object.fromEntries(Object.entries(config.shared??{}).map(([name,path])=>[name,resolve(path)]))};
      const files=await snapshotMusicFiles(game);
      const fixture=await createRuntimeFixtureSite({repoRoot:root,targets:[target],shared,files,protocol:'https'});
      try{
      const fixtureIdentity=await fixture.verify(target);
      assert.deepEqual(fixtureIdentity,identity,`${game}: Runtime or DATA identity changed during preflight`);
      const sharedIdentity=await fixture.verifyShared();
      const {launchBrowser}=await import('../browser/launch.mjs');
      const browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required',
        `--ignore-certificate-errors-spki-list=${fixture.tlsIdentity.spkiSha256}`]});
      record.fixture={base:fixture.base,protocol:fixture.protocol,tlsIdentity:fixture.tlsIdentity};
      try{
        for(const mode of ['none','ogg']){
          const modeRecord={mode,status:'running',runtimeResources:[],title:null,start:null};record.modes.push(modeRecord);
          const context=await browser.newContext({viewport:{width:1100,height:820}});
          await context.addInitScript({content:MUSIC_PCM_PROBE_SOURCE});
          const page=await context.newPage();let frame=null,fixtureWatcher=null;
          page.on('pageerror',error=>record.errors.push({mode,error:String(error)}));
          page.on('crash',()=>record.errors.push({mode,error:'Runtime browser page crashed'}));
          page.on('console',message=>{if(message.type()==='error')record.consoleErrors.push({mode,text:message.text()});});
          page.on('response',response=>{if(response.status()>=400)record.httpErrors.push({mode,url:response.url(),status:response.status()});});
          page.on('requestfailed',request=>record.httpErrors.push({mode,url:request.url(),failure:request.failure()?.errorText??'request failed'}));
          try{
            fixtureWatcher=await fixture.watchPage(page,{output,label:`${game}-${mode}`});
            await page.goto(fixture.hostUrl(target,mode==='none'?1:2));
            await page.waitForFunction(()=>events.some(event=>event.event==='ready'),null,{timeout:120000});
            frame=page.frames().find(candidate=>candidate.url().includes(`/runtime/${target.id}/`));assert.ok(frame,`${game}: Runtime frame missing`);
            const sharedResources=Object.keys(config.shared??{}).map(name=>({path:`/${name}`,url:`${fixture.base}/shared/${name}`}));
            const resources=musicInputs[game].files;
            const runtimeResources=mode==='ogg'?runtimeMusicResources(game,musicInputs,{base:fixture.base}):[];
            modeRecord.runtimeResources=runtimeResources.map(resource=>({path:resource.path,bytes:resource.bytes}));
            await page.evaluate(({sharedResources,runtimeResources,mode,options})=>rpc('configure',{language:'ja',music:mode,
              resources:[],runtimeResources,sharedResources,thpracCatalog:[],options:{alwaysHitbox:false,touchEnabled:false,
                thpracEnabled:false,limitPresentationTo60:false,...options}}),
              {sharedResources,runtimeResources,mode,options:target.options??{}});
            await page.evaluate(()=>rpc('launch'));
            await page.waitForFunction(()=>events.some(event=>event.event==='first-frame'),null,{timeout:120000});
            await page.waitForTimeout(1800);
            const collectScene=async()=>{
              const captures=[];let raw,native;
              for(let sample=0;sample<(mode==='none'?6:20);sample++){
                raw=await frame.evaluate(readNativeAudioState,game);native=describeNativeMusic(game,raw,maps);
                captures.push(...await frame.evaluate(()=>window.__eaglerMusicPcmProbe?.capture()??[]));
                if(mode==='ogg'&&native.trackFile&&summarizeMusicPcm(captures).outputObserved)break;
                await page.waitForTimeout(mode==='none'?100:250);
              }
              const pcm=summarizeMusicPcm(captures);
              raw=await frame.evaluate(readNativeAudioState,game);native=describeNativeMusic(game,raw,maps);
              const input=mapTrackToInput(native.trackFile,resources);
              return {native,decoder:raw.decoder,trackInput:input,pcm};
            };
            modeRecord.title=await collectScene();
            modeRecord.title.observation=await frame.evaluate(readRuntimeObservation,{game,variant:'normal'});
            modeRecord.title.audioWindow={settleMs:1800,captureWindowMs:mode==='none'?600:null};
            await fixtureWatcher.flush();
            const oggResponses=fixtureWatcher.responses.filter(response=>new URL(response.url).pathname.startsWith(`/music/${game}/`));
            if(mode==='ogg'){
              assert.ok(oggResponses.length,`${game}: Runtime did not request any verified OGG resource`);
              if(titleTrack[game])assert.ok(oggResponses.some(response=>new URL(response.url).pathname===`/music/${game}/${titleTrack[game]}`),
                `${game}: Runtime did not request the native title track OGG`);
            }
            modeRecord.fixtureResponses={capture:fixtureWatcher.capture,count:fixtureWatcher.responses.length,responses:fixtureWatcher.responses};
            modeRecord.requestedOggResources=oggResponses.map(response=>({url:response.url,expected:response.expected,actual:response.actual}));
            await page.screenshot({path:resolve(output,`${game}-${mode}-title.png`)});
            if(mode==='ogg'&&titleTrack[game]){
              assert.equal(modeRecord.title.native.trackFile,titleTrack[game],`${game}: native title track does not map to the declared title OGG`);
              assert.ok(modeRecord.title.trackInput,`${game}: native title track does not bind to a verified OGG input`);
              assert.ok(modeRecord.title.pcm.outputObserved,`${game}: native title track has no observed Runtime PCM output`);
            }
            if(mode==='ogg'&&!titleTrack[game]){
              assert.ok(modeRecord.title.pcm.outputObserved,`${game}: OGG mode produced no observed Runtime PCM output`);
            }
            const canObserveStart=['th08','th10','th11','th15'].includes(game);
            if(canObserveStart){
              let observation=modeRecord.title.observation,inputsSent=0;
              for(let press=0;press<28&&!gameplayReached(game,observation);press++){
                await pressStart(page);inputsSent++;observation=await frame.evaluate(readRuntimeObservation,{game,variant:'normal'});
              }
              modeRecord.start={status:gameplayReached(game,observation)?'gameplay-observed':'start-input-sent',
                observation,inputsSent};
              if(gameplayReached(game,observation)){
                modeRecord.start.audio=await collectScene();
                modeRecord.start.audio.observation=observation;
                await page.screenshot({path:resolve(output,`${game}-${mode}-gameplay.png`)});
              }
            }else modeRecord.start={status:'not-run',reason:'TH20 native phase and UI observations are recorded; this driver does not yet implement title-specific navigation'};
            modeRecord.identityCoverage=titleTrack[game]?'native-track-id-and-ogg-input':'decoder-and-pcm-only';
            modeRecord.status=mode==='ogg'&&modeRecord.title.pcm.outputObserved?'observed':'recorded';
            if(mode==='none'){
              modeRecord.control=describeNoneMusicControl(modeRecord.title.pcm);
              modeRecord.passed=modeRecord.control.status==='silent-after-settling';
            }else modeRecord.passed=modeRecord.title.pcm.outputObserved;
          }catch(error){modeRecord.status='failed';modeRecord.passed=false;modeRecord.error=String(error.stack||error);record.errors.push(modeRecord.error);}
          finally{
            if(fixtureWatcher){
              try{await fixtureWatcher.flush();}catch(error){modeRecord.responseAuditError=String(error.stack||error);}
              await fixtureWatcher.stop();
              modeRecord.fixtureResponses??={capture:fixtureWatcher.capture,count:fixtureWatcher.responses.length,responses:fixtureWatcher.responses,errors:fixtureWatcher.errors};
            }
            modeRecord.events=await page.evaluate(()=>{
              if(!Array.isArray(window.events))throw Error('Runtime protocol event history is unavailable');
              return window.events;
            }).catch(error=>{
              modeRecord.protocolTailError=String(error.stack||error);
              record.errors.push({mode,error:modeRecord.protocolTailError});return[];
            });
            await context.close();await save();
          }
        }
        const ogg=record.modes.find(mode=>mode.mode==='ogg');
        const pcmOutput=Boolean(ogg?.title?.pcm?.outputObserved);
        const noneControl=record.modes.find(mode=>mode.mode==='none')?.control;
        record.dataIdentity=fixtureIdentity.data;record.packageIdentity=fixtureIdentity.runtime;
        record.scenarioErrors=musicScenarioErrors(record,{fixtureFailures:fixture.httpFailures});
        const sceneOutcome=musicTitleSceneOutcome({oggPcmObserved:pcmOutput,
          expectedTrack:titleTrack[game]??null,observedTrack:ogg?.title?.native?.trackFile??null,
          trackInput:ogg?.title?.trackInput??null,
          errors:record.scenarioErrors});
        record.scenario=sceneOutcome.scenario;record.checks=[sceneOutcome.coverage];
        record.audio01={...sceneOutcome.coverage,trackIdentity:sceneOutcome.trackIdentity,runtimePcm:pcmOutput,
          noneControl,titleTrack:ogg?.title?.native?.trackFile??null,titleInput:ogg?.title?.trackInput??null};
        record.status=sceneOutcome.scenario.status;record.passed=sceneOutcome.scenario.passed;
        const post=await fixture.verify(target);
        assert.deepEqual(post,fixtureIdentity,`${game}: Runtime or DATA identity changed during music acceptance`);
        assert.deepEqual(await fixture.verifyShared(),sharedIdentity,`${game}: shared Runtime inputs changed during music acceptance`);
        record.identityPostflight=post;
      }catch(error){record.status='failed';record.passed=false;record.failure=String(error.stack||error);}
      finally{await browser.close();}
      }catch(error){record.status='failed';record.passed=false;record.failure=String(error.stack||error);}
      finally{record.httpFailures=fixture.httpFailures;await fixture.close();await save();}
      console.log(`${game}: ${record.status.toUpperCase()} ${record.failure?.split('\n')[0]??record.audio01?.trackIdentity}`);
    }
  const musicInputsPostflight=await verifyMusicInputs(musicManifest,{repoRoot:root,games});
  assert.deepEqual(musicInputsPostflight,musicInputs,'OGG resource identities changed during acceptance');
  assert.equal(hash(await readFile(manifestPath)),hash(manifestBytes),'Runtime input manifest changed during music acceptance');
  assert.equal(hash(await readFile(musicManifestPath)),hash(musicManifestBytes),'Music input manifest changed during music acceptance');
  await verifyArtifacts();
  report.musicInputPostflight=musicInputsPostflight;report.sourceSnapshotPostflight=sourceSnapshot(root);
  const failed=report.targets.some(target=>target.scenario?.passed!==true||target.status==='failed');
  report.scenario={id:'music-title-ogg-scenes',status:failed?'failed':'passed',passed:!failed,
    targetCount:report.targets.length};
  report.coverage={status:report.targets.every(target=>target.audio01?.complete===true)?'complete':'partial',
    complete:report.targets.length>0&&report.targets.every(target=>target.audio01?.complete===true),
    checks:report.targets.map(target=>({game:target.game,...target.audio01}))};
  report.status=report.scenario.status;report.passed=report.scenario.passed;
  report.result=failed?'At least one required title OGG output scene failed.':'Selected title OGG output scenes passed; AUDIO-01 full-scope coverage remains partial.';
  await save();
});

if(!result.passed){process.exitCode=1;if(result.failure)console.error(result.failure);}
