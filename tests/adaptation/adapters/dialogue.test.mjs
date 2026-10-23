import test from 'node:test';
import assert from 'node:assert/strict';
import {runInNewContext} from 'node:vm';
import {dialogueTouchSceneRequest,readDialogueTouchWords,requestDialogueTouchMessage,
  normalizeDialogueTouchManifest,normalizeDialogueTouchObservation} from './dialogue.mjs';
import {TH08_DIALOGUE_SCENE_CONTRACT} from './titles/th08-dialogue.mjs';

const runtimeMap={global:'__th08Runtime',core:'core',module:'Module',app:'app'};
const contract={...TH08_DIALOGUE_SCENE_CONTRACT,runtime:runtimeMap};
const nextRuntimeMap={global:'__nextRuntime',core:'exports',module:'WasmModule',app:'instance'};
const nextContract={schema:'eagler-touhou/dialogue-touch-scene/1',game:'next',variant:'normal',stage:2,
  runtime:nextRuntimeMap,
  manifest:{export:'next_dialogue_words',magic:0x4e585431,version:2,wordCount:8},
  observation:{export:'next_dialogue_state',magic:0x4e585432,version:3,wordCount:12},
  request:{export:'next_open_dialogue',appArgument:true,acceptedStatus:7,statusCodes:{rejected:0,accepted:7}}};
const manifestWords=()=>{
  const words=Array(344).fill(0);
  words.splice(0,24,0x44544d31,1,344,1,0,7,0x1000,3,0,0,0,1,400,0x89abcdef,0x01234567,
    2,4,300,0x76543210,0xfedcba98,1,1,2,10);
  words.splice(24,20,0,0,18,1,44,1,72,0,80,5,3,1,99,1,120,2,150,12,64,9);
  return words;
};
const observationWords=()=>{
  const words=Array(45).fill(0),values={0:0x44544f31,1:1,2:45,3:1,4:0,5:7,6:0x1000,7:3,8:90,
    9:1,10:1,11:0,12:0,13:0,14:1,15:1,16:1,17:3,18:1,19:0,20:0,21:1,22:3,23:150,24:4,
    25:12,26:8,27:6,28:6,29:1,30:0,31:1,32:0,33:1,34:80,35:90,36:99,37:1,38:2,39:1,
    40:1,41:1,42:3,43:1,44:1};
  for(const [index,value]of Object.entries(values))words[Number(index)]=value;
  return words;
};
function makeRuntime(words,{pointer=64,exportName,kind='manifest',moduleMemory=true}={}){
  const memory=new WebAssembly.Memory({initial:2});new Uint32Array(memory.buffer,pointer,words.length).set(words);
  const calls=[];const name=exportName??(kind==='manifest'?contract.manifest.export:contract.observation.export);
  const core={memory,[name](...args){calls.push({owner:this,args});return pointer;}};
  const Module=moduleMemory?{HEAPU8:new Uint8Array(memory.buffer)}:{};
  const runtime={core,Module,app:{mustNotBePassed:true}};
  return{runtime,memory,calls,pointer};
}
function serialized(fn,request,sandbox){return runInNewContext(`(${fn.toString()})(request)`,{request,...sandbox});}

test('registry request resolves an explicitly registered title contract and Runtime mapping',()=>{
  const adapterRegistry={resolve(game,variant){assert.deepEqual({game,variant},{game:'th08',variant:'normal'});
    return{dialogueTouchScene:TH08_DIALOGUE_SCENE_CONTRACT,runtime:runtimeMap};}};
  const request=dialogueTouchSceneRequest({game:'th08',variant:'normal',adapterRegistry});
  assert.deepEqual(request,{game:'th08',variant:'normal',contract:{...TH08_DIALOGUE_SCENE_CONTRACT,runtime:runtimeMap}});
  assert.doesNotThrow(()=>JSON.stringify(request));
  assert.equal(dialogueTouchSceneRequest({game:'th10',variant:'normal',adapterRegistry:{resolve:()=>null}}).contract,null);
  assert.throws(()=>dialogueTouchSceneRequest({game:'next',variant:'normal',adapterRegistry:{resolve:()=>({
    dialogueTouchScene:TH08_DIALOGUE_SCENE_CONTRACT,runtime:runtimeMap})}}),/explicitly registered for its product/);
});

test('serialized manifest reader invokes the no-app export in the selected Runtime and returns the full native buffer',()=>{
  const words=manifestWords(),world=makeRuntime(words);
  const request={contract,operation:'manifest'};
  const raw=serialized(readDialogueTouchWords,request,{__th08Runtime:world.runtime});
  assert.equal(raw.available,true);assert.equal(raw.pointer,world.pointer);assert.deepEqual(Array.from(raw.words),words);
  assert.equal(world.calls.length,1);assert.deepEqual(world.calls[0].args,[]);
  assert.equal(world.calls[0].owner,world.runtime.core);
  const manifest=normalizeDialogueTouchManifest(raw,contract);
  assert.equal(manifest.available,true);
  assert.equal(manifest.source.ecl.fnv64,'0123456789abcdef');
  assert.equal(manifest.source.message.fnv64,'fedcba9876543210');
  assert.equal(manifest.source.ecl.asset,'ecldata1.ecl');
  assert.equal(manifest.source.message.asset,'msg1a.dat');
  assert.deepEqual(manifest.candidates.map(candidate=>candidate.entry),[0,3]);
  assert.equal(manifest.candidates[1].message.waitOffset,150);
  assert.equal(manifest.candidates[1].rawWords.length,10);
});

test('serialized observation reader decodes the fixed DTO1 ABI and validates fresh-edge evidence',()=>{
  const words=observationWords(),world=makeRuntime(words,{kind:'observation'});
  const raw=serialized(readDialogueTouchWords,{contract,operation:'observation'},{__th08Runtime:world.runtime});
  assert.deepEqual(world.calls[0].args,[]);
  const observation=normalizeDialogueTouchObservation(raw,contract);
  assert.equal(observation.available,true);
  assert.deepEqual(observation.runtime,{generation:7,pointer:0x1000,epoch:3});
  assert.equal(observation.dialogue.freshConfirmEdge,true);
  assert.deepEqual(observation.dialogue.consumer,{valid:true,sequence:80});
  assert.deepEqual(observation.fixture,{used:true,gateEnabled:true,updateStarted:true,withheldUpdates:3,
    writeLocked:true,loadScope:false,committedBytes:0});
  assert.deepEqual(observation.request,{serial:1,statusCode:1,status:'accepted',entry:3,timeline:1});
  const altered=observationWords();altered[33]=0;
  assert.throws(()=>normalizeDialogueTouchObservation({available:true,words:altered},contract),/disagrees with its native/);
  const stopped=observationWords();stopped[33]=0;stopped[44]=0;
  const stoppedObservation=normalizeDialogueTouchObservation({available:true,words:stopped},contract);
  assert.deepEqual(stoppedObservation.dialogue.consumer,{valid:false,sequence:80});
  const short=observationWords().slice(0,44);
  assert.throws(()=>normalizeDialogueTouchObservation({available:true,words:short},contract),/exactly 45 native words/);
  const inactive=observationWords();inactive[3]=0;inactive[4]=1;inactive[5]=7;inactive[6]=0;inactive[7]=3;
  const unavailable=normalizeDialogueTouchObservation({available:true,words:inactive},contract);
  assert.equal(unavailable.available,false);assert.match(unavailable.reason,/noRuntime/);
  assert.equal(unavailable.runtime.pointer,0);
});

test('manifest normalization rejects malformed identity, word layout, source mapping and candidate rows',()=>{
  const raw={available:true,words:manifestWords()};
  for(const mutate of [
    words=>{words[0]=0;},words=>{words[2]=343;},words=>{words[5]=0;},words=>{words[8]=1;},
    words=>{words[10]=3;},words=>{words[11]=0;},words=>{words[21]=5;},words=>{words[22]=33;},
    words=>{words[24+10]=words[24];},words=>{words[24+3]=2;},words=>{words[24+6]=300;},
  ]){const words=manifestWords();mutate(words);assert.throws(()=>normalizeDialogueTouchManifest({available:true,words},contract));}
  const unavailable=manifestWords();unavailable[3]=0;unavailable[4]=9;
  const result=normalizeDialogueTouchManifest({available:true,words:unavailable},contract);
  assert.equal(result.available,false);assert.equal(result.reason,'noConfirmEntry');
});

test('a second title reuses the serialized word reader through its explicit data contract and different mount',()=>{
  const memory=new WebAssembly.Memory({initial:1}),pointer=32,words=[0x4e585431,2,8,91,92,93,94,95];
  new Uint32Array(memory.buffer,pointer,words.length).set(words);
  const calls=[],exports={memory,next_dialogue_words(...args){calls.push({owner:this,args});return pointer;}};
  const runtime={exports,WasmModule:{HEAPU8:new Uint8Array(memory.buffer)},instance:{id:'next'}};
  const request={contract:nextContract,operation:'manifest'};
  const raw=serialized(readDialogueTouchWords,request,{__nextRuntime:runtime});
  assert.deepEqual(Array.from(raw.words),words);assert.deepEqual(calls[0].args,[]);assert.equal(calls[0].owner,exports);
  assert.equal(raw.operation,'manifest');assert.equal(raw.pointer,pointer);
});

test('the fixed manifest normalizer consumes source identity and asset maps from each title contract',()=>{
  const nextManifest={...contract.manifest,eclAssets:{77:'levels/stage-a.ecl'},messageAssets:{88:'dialogue/team-a.msg'},
    teamByCharacter:[0]};
  const next={...contract,game:'next',stage:2,manifest:nextManifest};
  const words=manifestWords();words[8]=2;words[20]=77;words[21]=88;
  const receipt=normalizeDialogueTouchManifest({available:true,words},next);
  assert.equal(receipt.source.variant,'normal');assert.equal(receipt.source.stage,2);
  assert.equal(receipt.source.ecl.asset,'levels/stage-a.ecl');assert.equal(receipt.source.message.asset,'dialogue/team-a.msg');
});

test('reader fails closed for an absent declared Runtime and never borrows a window heap',()=>{
  const fallback={HEAPU8:new Uint8Array(new ArrayBuffer(8192))};
  const request={contract,operation:'manifest'};
  const result=serialized(readDialogueTouchWords,request,{Module:fallback,HEAPU8:fallback.HEAPU8});
  assert.equal(result.available,false);assert.match(result.reason,/Declared Runtime mount/);
});

test('reader rejects competing memories within the declared Runtime with evidence',()=>{
  const words=manifestWords(),world=makeRuntime(words),otherMemory=new WebAssembly.Memory({initial:2});
  world.runtime.Module.HEAPU8=new Uint8Array(otherMemory.buffer);
  assert.throws(()=>serialized(readDialogueTouchWords,{contract,operation:'manifest'},{__th08Runtime:world.runtime}),error=>{
    assert.match(error.message,/two different memories/);assert.equal(error.evidence.operation,'manifest');return true;
  });
});

test('native request export receives the entry only and preserves the status receipt',()=>{
  const calls=[],core={sdl_dialogue_touch_request_v1(...args){calls.push({owner:this,args});return 1;}};
  const runtime={core,Module:{},app:{mustNotBePassed:true}};
  const result=serialized(requestDialogueTouchMessage,{contract,entry:3},{__th08Runtime:runtime});
  assert.deepEqual(calls[0].args,[3]);assert.equal(calls[0].owner,core);
  assert.equal(result.available,true);assert.equal(result.accepted,true);assert.equal(result.statusCode,1);
  assert.equal(result.status,'accepted');assert.equal(result.entry,3);
});

test('native request failure returns a useful rejected receipt and unknown statuses carry error evidence',()=>{
  const core={sdl_dialogue_touch_request_v1(){return 8;}};
  const rejected=serialized(requestDialogueTouchMessage,{contract,entry:3},{__th08Runtime:{core,Module:{}}});
  assert.equal(rejected.accepted,false);assert.equal(rejected.status,'entryNotReferenced');
  core.sdl_dialogue_touch_request_v1=()=>99;
  assert.throws(()=>serialized(requestDialogueTouchMessage,{contract,entry:3},{__th08Runtime:{core,Module:{}}}),error=>{
    assert.match(error.message,/unknown status/);assert.equal(error.evidence.nativeStatus,99);return true;
  });
  const invalid=serialized(requestDialogueTouchMessage,{contract,entry:-1},{__th08Runtime:{core,Module:{}}});
  assert.equal(invalid.available,false);
});

test('another title supplies its own request name, app argument and acceptance status',()=>{
  const calls=[],exports={next_open_dialogue(...args){calls.push({owner:this,args});return 7;}};
  const instance={id:'next'};const runtime={exports,WasmModule:{},instance};
  const result=serialized(requestDialogueTouchMessage,{contract:nextContract,entry:6},{__nextRuntime:runtime});
  assert.equal(calls[0].owner,exports);assert.deepEqual(calls[0].args,[instance,6]);
  assert.equal(result.accepted,true);assert.equal(result.status,'accepted');assert.equal(result.statusCode,7);
});
