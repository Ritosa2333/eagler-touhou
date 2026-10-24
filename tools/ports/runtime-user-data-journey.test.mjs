import test from 'node:test';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {TH08_STORAGE_CONTRACT} from '../../tests/adaptation/adapters/titles/th08-storage.mjs';
import {captureNativeUserData,assertNativeUserDataRestored,changeNativeConfiguration,assertNativeConfigurationAuthored,assessNativeUserDataJourney,readNativeStorageSelection} from './runtime-user-data-journey.mjs';

const config=()=>{const bytes=Buffer.alloc(60);bytes.writeUInt32LE(0x80001,20);bytes[40]=80;return bytes;};
const contract={...TH08_STORAGE_CONTRACT,files:TH08_STORAGE_CONTRACT.files.filter(file=>file.kind==='configuration')};

test('persistence compares reopened artifacts and rejects missing, changed or forged bytes',async()=>{
  const artifacts=new Map(),original=config();
  const capture=stage=>captureNativeUserData(contract,{stage,read:async()=>original,
    saveBytes:async(path,bytes)=>{const key=stage+'/'+path;artifacts.set(key,Buffer.from(bytes));return key;}});
  const before=await capture('saved'),after=await capture('reopened'),readEvidence=async path=>artifacts.get(path);
  assert.equal((await assertNativeUserDataRestored(contract,before,after,{readEvidence})).status,'passed');
  assert.equal(typeof before.files[0].bytes,'number');
  await assert.rejects(assertNativeUserDataRestored(contract,before,{files:[]},{readEvidence}));
  artifacts.get('reopened/th08.cfg')[40]=75;
  await assert.rejects(assertNativeUserDataRestored(contract,before,after,{readEvidence}));
});

test('complete persistence evidence is recomputed from bytes and real lifecycle boundaries',async()=>{
  const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
  const c={schema:'eagler-touhou/native-storage/1',source:'fixture/native-save',
    menu:TH08_STORAGE_CONTRACT.menu,
    selection:{wordCount:7,fields:{difficulty:1,character:2,practice:3,replay:4,cheats:5,assisted:6},ordinaryFlags:{mask:0x400f,value:4}},
    configuration:{path:'config',field:'volume',minimum:0,maximum:100,step:5},files:[
      {path:'config',kind:'configuration',inspect:bytes=>({values:{volume:bytes[0]},semanticSha256:hash(bytes)})},
      {path:'score',kind:'score',inspect:bytes=>({values:{statistics:Array.from({length:7},(_,i)=>({total:(i===0||i===6)?bytes[0]:0,
        characters:Array.from({length:12},(_,shot)=>(i===0||i===6)&&shot===0?bytes[0]:0)}))},semanticSha256:hash(bytes)})},
    ]};
  const snapshot=async(stage,volume,count)=>captureNativeUserData(c,{stage,includeContents:true,
    read:async path=>Buffer.from([path==='config'?volume:count]),saveBytes:async path=>stage+'/'+path});
  const replayBytes=Buffer.from('retained Replay bytes'),sha=hash(replayBytes),replayIdentity={bytes:replayBytes.length,sha256:sha,contentBase64:replayBytes.toString('base64')},
    replay={...replayIdentity,sourceFixture:{...replayIdentity}};
  const journey={schema:'eagler-touhou/native-storage-journey/1',source:c.source,
    before:await snapshot('before',80,0),authored:await snapshot('authored',80,1),reopened:await snapshot('reopened',75,1),
    offline:{...await snapshot('offline',75,1),pageOnline:false},offlineSaved:await snapshot('offline-saved',75,1),updated:await snapshot('updated',75,1),
    configuration:{path:'config',field:'volume',before:80,after:75},selection:{difficulty:0,character:0},
    selectionObservations:{difficulty:{scene:1,page:4,phase:1,cursor:0},character:{scene:1,page:5,phase:1,cursor:0}},
    entered:{schema:'eagler-touhou/runtime-observation/1',game:'th08',variant:'normal',phase:{name:'stage'},capabilities:{phase:true,input:true,position:true},input:{value:0},position:{x:1,y:1},pause:{active:false}},
    enteredSelection:{difficulty:0,character:0,practice:0,replay:0,cheats:0,assisted:0,gameFlags:4,rawWords:[0,0,0,0,0,0,0]},
    lifecycle:{beforeEpoch:1,afterEpoch:2,exitAbsentBeforeStop:true,stopError:null,exit:{event:'exit',epoch:1,code:0,status:'success'},first:{event:'first-frame',epoch:2}},
    replay,replayRestored:Object.fromEntries(['reopened','offline','offlineSaved','updated'].map(stage=>[stage,{...replayIdentity}])),generations:{before:'first',after:'second'}};
  for(const id of ['SAVE-03','SAVE-04','SAVE-05','SAVE-06'])assert.equal(assessNativeUserDataJourney(c,journey,id).status,'passed');
  for(const mutate of [j=>j.reopened.files[0].contentBase64='AA==',j=>j.configuration.after=80,
    j=>j.lifecycle.afterEpoch=1,j=>j.lifecycle.exit.epoch=2,j=>delete j.offline,j=>j.generations.after='first',
    j=>j.replayRestored.offline.sha256='b'.repeat(64),j=>j.offline.pageOnline=true,j=>j.lifecycle.exitAbsentBeforeStop=false,
    j=>j.lifecycle.stopError='Actual save failed',j=>j.selection.character=1,j=>j.selection.difficulty=1,
    j=>delete j.entered,j=>delete j.enteredSelection,j=>delete j.selectionObservations,
    j=>j.selectionObservations.difficulty.cursor=1,j=>j.selectionObservations.character.page=4,
    j=>j.enteredSelection.gameFlags=8,j=>j.enteredSelection.character=1,j=>j.entered.phase.name='title',
    j=>j.lifecycle.exit.code=1,j=>j.lifecycle.exit.status='failed',j=>delete j.replayRestored.offline.contentBase64,
    j=>{j.replay.sha256='b'.repeat(64);j.replay.sourceFixture.sha256=j.replay.sha256;for(const value of Object.values(j.replayRestored))value.sha256=j.replay.sha256;}]){
    const changed=structuredClone(journey);mutate(changed);assert.throws(()=>assessNativeUserDataJourney(c,changed,'SAVE-06'));
  }
});

test('native storage selection copies only the declared existing ABI memory and rejects invalid owners or ranges',()=>{
  const memory=new WebAssembly.Memory({initial:1}),words=[0,1,0,0,0,0,0];
  new Int32Array(memory.buffer,16,words.length).set(words);
  const runtime={global:'__storageSelectionFixture',core:'core',module:'Module',app:'app'},
    selection={readExport:'practice_status',wordCount:7,fields:{difficulty:1,character:2,practice:3,replay:4,cheats:5,assisted:6},gameFlagsField:19},menu={readExport:'status'};
  const owner={app:9,Module:{HEAPU8:new Uint8Array(memory.buffer)},core:{memory,practice_status:()=>16,status:(app,index)=>{assert.equal(app,9);assert.equal(index,19);return 4;}}};
  globalThis.__storageSelectionFixture=owner;
  try{
    const read=()=>readNativeStorageSelection({runtime,selection,menu});
    assert.deepEqual(read(),{difficulty:1,character:0,practice:0,replay:0,cheats:0,assisted:0,gameFlags:4,rawWords:words});
    const raw=read().rawWords;raw[1]=9;assert.equal(read().difficulty,1);
    for(const pointer of [0,-4,17,memory.buffer.byteLength-4]){owner.core.practice_status=()=>pointer;assert.throws(read,/range/);}
    owner.core.practice_status=()=>16;owner.Module.HEAPU8=new Uint8Array(new ArrayBuffer(65536));assert.throws(read,/owners differ/);
    owner.Module.HEAPU8=new Uint8Array(memory.buffer);delete owner.core.practice_status;assert.throws(read,/unavailable/);
  }finally{delete globalThis.__storageSelectionFixture;}
});

test('configuration journey uses native Options input and proves the saved change',async()=>{
  let state={scene:1,page:0,phase:1,cursor:0};const keys=[];
  const before={files:[{path:'th08.cfg',inspection:{values:{soundVolume:80}}}]};
  const change=await changeNativeConfiguration(contract,{before,observe:async()=>({...state}),
    waitUntil:async(label,predicate)=>{assert.ok(predicate(state),label);return {...state};},
    press:async code=>{keys.push(code);
      if(code==='ArrowDown')state.cursor++;
      if(code==='KeyZ'){state.page=2;state.cursor=0;}
      if(code==='Escape'&&state.page===2){if(state.cursor===9){state.page=0;state.cursor=7;}else state.cursor=9;}
    }});
  assert.equal(change.expected,75);assert.ok(keys.includes('ArrowLeft'));
  assert.equal(keys.filter(key=>key==='Escape').length,2);
  assert.throws(()=>assertNativeConfigurationAuthored(contract,change,before));
  assert.equal(assertNativeConfigurationAuthored(contract,change,{files:[{path:'th08.cfg',inspection:{values:{soundVolume:75}}}]}).status,'passed');
});
