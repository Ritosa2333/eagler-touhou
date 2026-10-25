import {createHash} from 'node:crypto';
import {existsSync} from 'node:fs';
import {readFile} from 'node:fs/promises';
import {dirname, isAbsolute, relative, resolve, join} from 'node:path';
import {pathToFileURL} from 'node:url';
import {assetPath,loadGoldenManifest,readJsonLinesArray,readJsonLinesMaybeGzip} from '../../../../ports/th08/tools/replay-verifier/golden.mjs';
import {loadLegacyOriginal,recordsFromLongJsonLines,recordsFromRows} from '../../../../ports/th08/tools/replay-verifier/legacy-oracle-adapter.mjs';
import {inspectTh08ReplayFixture} from './th08-replay-format.mjs';
import {TH08_PRESENTATION_LAB_ABI} from '../../../../ports/th08/portable/presentation-lab/native-abi.mjs';
import {createReplayLedgerInspector} from '../../../../ports/th08/portable/presentation-lab/replay-capture.mjs';

const REPO_ROOT=resolve(import.meta.dirname,'../../../..');

const sha256Pattern=/^[a-f0-9]{64}$/;

function pathInside(root,candidate,label){
  const absolute=resolve(candidate),outside=relative(resolve(root),absolute);
  if(outside==='..'||outside.startsWith(`..${process.platform==='win32'?'\\':'/'}`)||isAbsolute(outside))
    throw new Error(`${label} escapes the selected capture root`);
  return absolute;
}

function validDiagnosticBuild(build){
  return Boolean(build&&build.schema==='th08/presentation-lab-build/1'&&build.instrumented===true&&
    build.game==='th08'&&build.adapterVersion==='th08-presentation-lab/5'&&build.nativeAbi===TH08_PRESENTATION_LAB_ABI&&
    Number.isSafeInteger(build.runtimeEpoch)&&build.runtimeEpoch>0&&
    typeof build.commit==='string'&&build.commit.length>0&&['wasm','loader','sourceDigest','toolDigest'].every(key=>sha256Pattern.test(build[key]??''))&&
    build.toolSources&&typeof build.toolSources==='object'&&!Array.isArray(build.toolSources)&&Object.keys(build.toolSources).length>0);
}

async function readJson(path){return JSON.parse(await readFile(path,'utf8'));}

async function* countedRows(rows,expected,label){
  let count=0;
  for await(const row of rows){count++;yield row;}
  if(count!==expected)throw new Error(`${label} contains ${count} rows; its metadata declares ${expected}`);
}

export function assessTh08ReplayCapture(capture,{lane,id,expectedSha256,expectedBytes,expectedResourceSha256,allowPrefix=false}={}){
  const expectedSchema=lane==='quick'?'th08/current-replay-capture/v3':'th08/current-replay-tick-capture/v3';
  if(capture?.schema!==expectedSchema)return{status:'incomplete',reason:`${id}: capture does not use ${expectedSchema}.`};
  if((capture.complete!==true||capture.stopReason!=='returned-to-title'||capture.terminalScene!==1)&&!allowPrefix)
    return{status:'incomplete',reason:`${id}: capture did not observe a natural return to the title.`};
  if(!Array.isArray(capture.errors)||capture.errors.length&&!allowPrefix)return{status:'incomplete',reason:`${id}: capture reports runtime errors.`};
  if(!validDiagnosticBuild(capture.build))return{status:'incomplete',reason:`${id}: diagnostic build identity is incomplete.`};
  if(!capture.replay||capture.replay.sha256!==expectedSha256||capture.replay.bytes!==expectedBytes)
    return{status:'incompatible',reason:`${id}: captured Replay identity does not match the registered original bytes.`};
  if(expectedResourceSha256&&capture.build.inputs?.data?.sha256!==expectedResourceSha256)
    return{status:'incompatible',reason:`${id}: actual Runtime DATA identity does not match the accepted reference.`};
  if(capture.build.inputs?.replay&&(capture.build.inputs.replay.sha256!==expectedSha256||capture.build.inputs.replay.bytes!==expectedBytes))
    return{status:'incompatible',reason:`${id}: Runtime-imported Replay differs from the selected file.`};
  return null;
}

export async function inspectTh08CaptureJournal(capture,{root,metadataPath,rows}={}){
  const inspector=createReplayLedgerInspector(),iterator=rows[Symbol.asyncIterator]?.()??rows[Symbol.iterator]();
  const ledger=Array.isArray(capture.cursorLedger)?capture.cursorLedger:
    readJsonLinesMaybeGzip(resolveTh08CaptureRowsPath(root,metadataPath,capture.cursorLedger?.path,'TH08 cursor ledger'));
  let count=0,last=null;
  for await(const sample of ledger){
    const row=sample.captured?(await iterator.next()).value:undefined;
    inspector.push(sample,row);count++;last=sample;
  }
  const result=inspector.finish(capture.journal);
  if(!(await iterator.next()).done||capture.applicationTicks!==count||
    !Array.isArray(capture.cursorLedger)&&capture.cursorLedger.rows!==count){
    result.status='incomplete';result.problems.push({reason:'Cursor ledger, trace rows and application tick counts differ'});
  }
  if(capture.complete===true&&(last?.scene!==1||capture.terminalScene!==1||capture.stopReason!=='returned-to-title')){
    result.status='incomplete';result.problems.push({reason:'Claimed natural return is not the final observed title scene'});
  }
  return result;
}

export function resolveTh08CaptureRowsPath(captureRoot,metadataPath,rowsPath,label='TH08'){
  if(typeof rowsPath!=='string'||!rowsPath)return null;
  const candidate=isAbsolute(rowsPath)?rowsPath:resolve(dirname(metadataPath),rowsPath);
  return pathInside(captureRoot,candidate,`${label} rows`);
}

/** Build one complete immutable-golden lane for the shared Replay comparator. */
export async function createTh08ReplaySuite({lane,captureRoot,inputRoot,replayFiles={},goldenRoot,commonRoot,oracleRoot}={}){
  if(!['quick','daily'].includes(lane))throw new TypeError('TH08 Replay lane must be quick or daily');
  if(!captureRoot)throw new TypeError('TH08 Replay suite needs a capture root');
  const root=resolve(captureRoot),golden=await loadGoldenManifest(goldenRoot),corpus=await readJson(join(REPO_ROOT,'ports/th08/tools/replay-verifier/corpus.json'));
  const expectedIds=[...golden.manifest.suites[lane]],cases=[],fixtures=new Map((corpus.fixtures??[]).map(item=>[item.id,item]));
  const dailyByKind=new Map((corpus.cases??[]).filter(item=>item.kind==='lunatic'||item.kind==='extra').map(item=>[item.kind,item]));
  for(const id of expectedIds){
    const asset=golden.manifest.assets?.[id];
    if(!asset||asset.kind==null){cases.push({id,status:'incompatible',reason:`${id}: selected lane is absent from the immutable golden manifest.`});continue;}
    const inputFixture=lane==='quick'?fixtures.get(id):dailyByKind.get(id);
    if(!inputFixture){cases.push({id,status:'incompatible',reason:`${id}: selected lane is absent from the TH08 Replay corpus.`});continue;}
    const expectedSha256=inputFixture.replaySha256;
    if(!sha256Pattern.test(expectedSha256??'')||expectedSha256!==asset.replaySha256){
      cases.push({id,status:'incompatible',reason:`${id}: corpus and golden Replay identities do not agree.`});continue;
    }
    if(lane==='quick'&&inputFixture.expectedTicks!==asset.comparedTicks){
      cases.push({id,status:'incompatible',reason:`${id}: quick fixture count does not agree with its immutable golden.`});continue;
    }
    const sourcePath=lane==='quick'?join(resolve(inputRoot??join(REPO_ROOT,'ports/th08/artifacts/presentation-lab/input')),`${id}.rpy`):
      (replayFiles[id]??join(REPO_ROOT,'ports/th08',inputFixture.source));
    let replayBytes;
    try{replayBytes=Uint8Array.from(await readFile(sourcePath));}
    catch(error){cases.push({id,status:'missing-replay',reason:`${id}: Replay input is unavailable (${String(error?.code??error)}).`});continue;}
    const metadataPath=join(root,`${id}.json`);
    let capture;
    try{capture=await readJson(metadataPath);}
    catch(error){cases.push({id,status:error?.code==='ENOENT'?'missing-capture':'incompatible',reason:`${id}: candidate capture metadata is unavailable or invalid.`});continue;}
    const captureProblem=assessTh08ReplayCapture(capture,{lane,id,expectedSha256:asset.replaySha256,expectedBytes:replayBytes.byteLength,
      expectedResourceSha256:golden.manifest.identity.resourceSha256,allowPrefix:true});
    if(captureProblem){cases.push({id,...captureProblem});continue;}
    if(lane==='quick'){
      if(!Array.isArray(capture.rows)||!capture.rows.length){
        cases.push({id,status:'incomplete',reason:`${id}: capture contains no comparable raw tick rows.`});continue;
      }
    }else{
      if(!Number.isSafeInteger(capture.rows?.ticks)||capture.rows.ticks<=0){
        cases.push({id,status:'incomplete',reason:`${id}: capture does not declare its raw row count.`});continue;
      }
      try{capture.rows.path=resolveTh08CaptureRowsPath(root,metadataPath,capture.rows.path,id);}
      catch(error){cases.push({id,status:'incompatible',reason:error.message});continue;}
      if(!capture.rows.path||!existsSync(capture.rows.path)){
        cases.push({id,status:'incomplete',reason:`${id}: captured Replay rows are unavailable.`});continue;
      }
    }
    let journal;
    try{journal=await inspectTh08CaptureJournal(capture,{root,metadataPath,
      rows:lane==='quick'?capture.rows:countedRows(readJsonLinesMaybeGzip(capture.rows.path),capture.rows.ticks,`${id} journal trace`)});}
    catch(error){cases.push({id,status:'incomplete',reason:`${id}: cursor journal is unavailable or invalid (${error.message}).`});continue;}
    cases.push({id,status:'ready',replayBytes,replaySha256:asset.replaySha256,goldenAsset:asset,
      inputFixture,capture,journal,metadataPath,oracleRoot:oracleRoot?resolve(oracleRoot):null});
  }
  const common=await import(pathToFileURL(join(resolve(commonRoot??join(REPO_ROOT,'common')),'testkit','replay-verifier','adapter.mjs')));
  const originalProfile=lane==='quick'?'jp-1.00d/historical-oracle-v1':'jp-1.00d/ordinary-replay-v1';
  const identity={...golden.manifest.identity,profile:originalProfile};
  const adapter={
    describe:()=>({adapterApiVersion:1,game:'th08',adapterVersion:'th08-replay-lab/v1',features:{original:true,candidate:true}}),
    inspectReplay:async(bytes,{fixture})=>inspectTh08ReplayFixture(bytes,{expectedSha256:fixture.replaySha256}),
    validateCandidate:async({fixture})=>{
      if(lane!=='daily')return null;
      const capture=fixture.capture,asset=fixture.goldenAsset;
      if(!Array.isArray(capture.stages)||capture.stages.some(stage=>!Number.isInteger(stage))||!Number.isSafeInteger(capture.finalScore)||typeof capture.terminalMatch!=='boolean')
        return{status:'INCOMPLETE',reason:'natural terminal stage and score evidence is unavailable'};
      const expected={stages:asset.stages,finalScore:asset.finalScore};
      const actual={stages:capture.stages,finalScore:capture.finalScore};
      const matches=actual.stages.length===expected.stages.length&&actual.stages.every((stage,index)=>stage===expected.stages[index])&&actual.finalScore===expected.finalScore;
      if(capture.terminalMatch!==matches)return{status:'INCOMPATIBLE',reason:'terminal metadata match flag is inconsistent with its observed values',expected,actual,terminalMatch:capture.terminalMatch};
      return null;
    },
    validateCandidateEnd:async({fixture})=>{
      if(!Array.isArray(fixture.capture.errors)||fixture.capture.errors.length)return{status:'INCOMPLETE',reason:'capture-runtime-errors',errors:fixture.capture.errors??null};
      if(fixture.journal?.status!=='passed')return{status:'INCOMPLETE',reason:'capture-cursor-journal',journal:fixture.journal};
      if(lane!=='daily')return null;
      const {capture,goldenAsset:asset}=fixture;
      const expected={stages:asset.stages,finalScore:asset.finalScore};
      const actual={stages:capture.stages,finalScore:capture.finalScore};
      return capture.terminalMatch?null:{status:'DIVERGED',reason:'natural-terminal-state',expected,actual};
    },
    async openOriginal({fixture}){
      const asset=fixture.goldenAsset;
      if(fixture.oracleRoot&&lane==='quick'){
        const source=await loadLegacyOriginal(fixture.oracleRoot,{...fixture.inputFixture,expectedTicks:asset.comparedTicks,replaySha256:fixture.replaySha256});
        return{records:recordsFromRows(fixture,source.rows,'th08-original-jit/advanced-source',{...common,identity}),
        provenance:{provider:'th08-original-jit/advanced-source',oracleRoot:fixture.oracleRoot},close:async()=>{}};
      }
      if(lane==='quick'){
        const rows=await readJsonLinesArray(assetPath(golden.root,asset));
        if(rows.length!==asset.rawTicks||rows.length!==asset.comparedTicks)throw new Error(`${fixture.id}: immutable quick golden row count is inconsistent`);
        return{records:recordsFromRows(fixture,rows,'th08/published-golden-v1',{...common,identity}),
        provenance:{provider:'th08/published-golden-v1',goldenSha256:asset.sha256},close:async()=>{}};
      }
      const rows=countedRows(readJsonLinesMaybeGzip(assetPath(golden.root,asset)),asset.rawTicks,`${fixture.id} golden`);
      return{records:recordsFromLongJsonLines({fixture,rows,provider:'th08/published-golden-v1',identity,complete:true,
        provenance:{goldenSha256:asset.sha256}},common),
      provenance:{provider:'th08/published-golden-v1',goldenSha256:asset.sha256},close:async()=>{}};
    },
    async openCandidate({fixture}){
      const capture=fixture.capture;
      if(!capture)throw new Error(`${fixture.id}: candidate capture was not prevalidated`);
      const provider=`th08-eagler/presentation-lab/${capture.build.wasm}`;
      const complete=capture.complete===true&&capture.stopReason==='returned-to-title'&&capture.terminalScene===1&&
        capture.errors.length===0&&fixture.journal.status==='passed';
      if(lane==='quick')return{records:recordsFromRows(fixture,capture.rows,provider,{...common,identity,complete,
        provenance:{diagnosticBuild:capture.build,stopReason:capture.stopReason,terminalScene:capture.terminalScene}}),
      provenance:{provider,diagnosticBuild:capture.build,stopReason:capture.stopReason,terminalScene:capture.terminalScene},close:async()=>{}};
      const asset=fixture.goldenAsset;
      if(!Array.isArray(capture.stages)||capture.stages.some(stage=>!Number.isInteger(stage))||!Number.isSafeInteger(capture.finalScore))
        throw new Error(`${fixture.id}: natural terminal stage/score evidence is malformed`);
      const rows=countedRows(readJsonLinesMaybeGzip(fixture.capture.rows.path),capture.rows.ticks,`${fixture.id} candidate`);
      return{records:recordsFromLongJsonLines({fixture,rows,provider,identity,complete,
        provenance:{diagnosticBuild:capture.build,stopReason:capture.stopReason,terminalScene:capture.terminalScene}},common),
      provenance:{provider,diagnosticBuild:capture.build,stopReason:capture.stopReason,terminalScene:capture.terminalScene},close:async()=>{}};
    },
  };
  const manifestSha256=createHash('sha256').update(await readFile(join(golden.root,'manifest.json'))).digest('hex');
  return{adapter,cases,expectedIds,requiredCategories:['rng','player','world'],golden:{profile:golden.manifest.profile,manifestSha256,
    identity:golden.manifest.identity,assets:Object.fromEntries(expectedIds.map(id=>{const asset=golden.manifest.assets?.[id];return[id,asset?{
      sha256:asset.sha256,replaySha256:asset.replaySha256,rawTicks:asset.rawTicks,comparedTicks:asset.comparedTicks}:null];}))}};
}

export async function runTh08ReplaySuite(options={}){
  const suite=await createTh08ReplaySuite(options);
  const commonRoot=resolve(options.commonRoot??join(REPO_ROOT,'common'));
  const {runReplaySuite}=await import(pathToFileURL(join(commonRoot,'testkit','replay-verifier','suite.mjs')));
  return{...(await runReplaySuite({...suite,methodControls:true})),lane:options.lane,golden:suite.golden};
}

/** The shared offline method review supplies the original lane's declared inputs. */
export async function createReplayReviewSuite({lane,captureRoot,inputs}={}){
  const options=lane==='quick'?{inputRoot:dirname(inputs.quickInputs[0].path)}:
    {replayFiles:Object.fromEntries(Object.entries(inputs.dailyReplays).map(([key,row])=>[key,row.path]))};
  return createTh08ReplaySuite({lane,captureRoot,...options});
}
