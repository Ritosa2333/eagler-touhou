import fs from 'node:fs';
import path from 'node:path';
import {parseReplay,replayInputEnded} from '../../src/replay.mjs';
import {stageResources} from '../../src/stage-loader.mjs';
import {assertHash,containedFile,jsonFile,prerequisite,sha256} from './config.mjs';
import {difference} from './trace-stream.mjs';
// Retail title idle rotation; also retained by src/title-scenes.mjs.
export const demoOrder=['demo2.rpy','demo1.rpy','demo0.rpy'];
export const stateSchemaSha256=sha256(fs.readFileSync(new URL('state-schema.json',import.meta.url)));
export function routeFor(replay){
  return replay.stages.map(stage=>{
    let inputTicks=stage.frames,sentinel=false;
    for(let i=0;i<stage.frames;i++)if(replayInputEnded(replay.input(stage.number,i))){
      if(i!==stage.frames-1)throw Error('Replay has a sentinel before its declared tail');
      inputTicks=i;sentinel=true;
    }
    if(inputTicks<1)throw Error('Empty replay stage');
    return {number:stage.number,seed:stage.seed,frames:stage.frames,inputTicks,sentinel};
  });
}
export function fixtureIdentity(fixture,replay){
  return {id:fixture.id,character:replay.character,shot:replay.shot,difficulty:replay.difficulty,
    practice:replay.practice,route:routeFor(replay)};
}
export function loadFixture(corpus,fixture,paths){
  if(!fixture||typeof fixture.id!=='string'||!fixture.provenance)throw Error('Corpus fixture has no id/provenance');
  assertHash(fixture.sha256,'Replay '+fixture.id);
  const bytes=fs.readFileSync(prerequisite(containedFile(paths.replays,fixture.file??fixture.id),'Replay '+fixture.id));
  if(sha256(bytes)!==fixture.sha256||fixture.bytes!==bytes.length)throw Error('Replay identity mismatch: '+fixture.id);
  const replay=parseReplay(bytes);if(replay.extension)throw Error('Retail golden suites reject assisted/extended input: '+fixture.id);
  if(difference([fixture.character,fixture.shot,fixture.difficulty],[replay.character,replay.shot,replay.difficulty]))throw Error('Replay loadout/difficulty mismatch');
  if(!Array.isArray(fixture.stages)||fixture.stages.length!==replay.stages.length)throw Error('Corpus route is missing/cropped');
  for(let i=0;i<replay.stages.length;i++)for(const field of ['number','seed','frames'])if(fixture.stages[i][field]!==replay.stages[i][field])throw Error('Corpus stage order/bounds mismatch: '+fixture.id);
  const completion=fixture.completion;
  if(!completion||typeof completion!=='object'||completion.record!=='completion'||completion.complete!==true||!Array.isArray(completion.stages)||typeof completion.outcome!=='string')throw Error('Missing prerequisite: reviewed original completion rules for '+fixture.id);
  const identity=fixtureIdentity(fixture,replay),ticks=identity.route.reduce((sum,s)=>sum+s.inputTicks,0);
  if(completion.ticks!==ticks||completion.stages.length!==identity.route.length)throw Error('Corpus completion is cropped or incomplete');
  for(let i=0;i<identity.route.length;i++){
    const s=completion.stages[i],route=identity.route[i];
    if(s.number!==route.number||s.inputTicks!==route.inputTicks||!Number.isInteger(s.ended)||typeof s.flowStatus!=='string')throw Error('Corpus has incomplete stage terminal rules');
  }
  return {fixture,replay,identity,completion,resources:corpus.original?.resources};
}
export function selectSuite(corpus,suite){
  if(!['th12-replay-corpus/1','th12-replay-corpus/2'].includes(corpus.schema)||corpus.game!=='th12'||corpus.version!=='1.00b'||!Array.isArray(corpus.fixtures))throw Error('Incompatible corpus');
  const ids=corpus.fixtures.map(f=>f.id);if(new Set(ids).size!==ids.length)throw Error('Duplicate corpus fixture');
  let selected;
  if(suite==='quick'){
    if(difference(corpus.quick,demoOrder))throw Error('Quick requires every retail Demo in rotation order: '+demoOrder.join(', '));
    selected=corpus.quick;
  }else if(suite==='daily'){
    const daily=corpus.daily;
    if(difference(daily?.standardLoadout,{character:1,shot:1})||difference(daily?.requiredDifficulties,[3,4])||!Array.isArray(daily.fixtures)||!daily.fixtures.length)throw Error('Missing prerequisite: daily MarisaB Lunatic and Extra fixtures');
    selected=daily.fixtures;
  }else throw Error('Select quick or daily');
  if(new Set(selected).size!==selected.length)throw Error('Duplicate suite fixture');
  const fixtures=selected.map(id=>{const f=corpus.fixtures.find(f=>f.id===id);if(!f)throw Error('Unknown suite fixture '+id);return f;});
  if(suite==='quick'&&fixtures.some(f=>f.provenance!=='built-in-demo'))throw Error('Quick fixture provenance differs from retail Demo');
  if(suite==='daily')for(const difficulty of [3,4])if(!fixtures.some(f=>f.character===1&&f.shot===1&&f.difficulty===difficulty))throw Error('Daily coverage is missing difficulty '+difficulty);
  if(suite==='daily')for(const f of fixtures){
    if(f.character!==1||f.shot!==1||![3,4].includes(f.difficulty))throw Error('Daily standard loadout/difficulty differs from the declared profile');
    if(difference(f.stages?.map(s=>s.number),f.difficulty===4?[7]:[1,2,3,4,5,6]))throw Error('Daily requires the complete standard Lunatic/Extra route: '+f.id);
  }
  return fixtures;
}
export function resourceIdentity(corpus,paths,selected){
  const pin=corpus.original;if(!pin||!Array.isArray(pin.resources)||!pin.resources.length)throw Error('Missing prerequisite: original executable/resource identities');
  assertHash(pin.executableSha256,'original executable');
  const files=pin.resources.map(resource=>{
    const file=containedFile(paths.assets,resource.file),bytes=fs.readFileSync(prerequisite(file,'resource '+resource.file));
    if(bytes.length!==resource.bytes||sha256(bytes)!==assertHash(resource.sha256,resource.file))throw Error('Original resource identity mismatch: '+resource.file);
    return {file:resource.file,bytes:resource.bytes,sha256:resource.sha256};
  }).sort((a,b)=>a.file<b.file?-1:a.file>b.file?1:0);
  if(new Set(files.map(f=>f.file)).size!==files.length)throw Error('Duplicate original resource identity');
  const atlasNames={};for(let stage=1;stage<=7;stage++){const name=`stgenm${String(stage).padStart(2,'0')}m`;if(fs.existsSync(path.join(paths.assets,name+'.anm')))atlasNames[name]=true;}
  const required=new Set(selected.flatMap(f=>f.stages.flatMap(s=>stageResources(s.number,atlasNames).map(r=>r.file))));
  for(const file of required)if(!files.some(f=>f.file===file))throw Error('Resource identity does not cover loaded stage input: '+file);
  return {executableSha256:pin.executableSha256,resourceSha256:sha256(Buffer.from(JSON.stringify(files))),stateSchemaSha256};
}
export function goldenEntry(index,id){
  if(index?.schema!=='th12-gameplay-golden-index/1'||!Array.isArray(index.fixtures))throw Error('Incompatible gameplay golden index');
  if(new Set(index.fixtures.map(f=>f.id)).size!==index.fixtures.length)throw Error('Duplicate golden index fixture');
  const entry=index.fixtures.find(f=>f.id===id);if(!entry)throw Error('Missing prerequisite: full-state original golden for '+id);
  assertHash(entry.sha256,id);if(!Number.isSafeInteger(entry.bytes)||entry.bytes<1||path.basename(entry.file??'')!==entry.sha256+'.jsonl')throw Error('Golden must be an immutable content-addressed object');
  return entry;
}
export function loadCorpus(paths){return jsonFile(prerequisite(paths.corpus,'corpus manifest'));}
