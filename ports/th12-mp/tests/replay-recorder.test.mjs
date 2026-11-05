import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {createHash} from 'node:crypto';
import {encodeReplay,ReplayRecorder,encodeReplayName,defaultReplayConfig,defaultReplaySpellTimes,encryptReplayBytes,compressReplayLiterals} from '../src/replay-recorder.mjs';
import {parseReplay,decryptBytes,inflateReplay,replayInputEnded} from '../src/replay.mjs';

const native=JSON.parse(fs.readFileSync(new URL('./fixtures/replay-export-native.json',import.meta.url)));
const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
const ended={held:0xffff,pressed:0xffff,released:0xffff};
const initial={score:1234567,power:143,pointValue:12345000,lives:6,lifeFragments:4,bombs:2,bombFragments:2,ufoColors:[1,2,0],rank:42,x:768,y:49920,nativeGlobals:[314,3,1,9876]};
const options={name:'RECORDER',character:0,shot:1,difficulty:2,timestamp:1770000000};
function makeStage(number=1,count=3,end=true){return {number,seed:0x3456,initial:structuredClone(initial),inputs:Array.from({length:count},(_,i)=>({held:i?0x81:0,pressed:i===1?0x81:0,released:0})),fps:Array(Math.ceil(count/30)).fill(60),end};}
const encode=stage=>encodeReplay({...options,stages:[stage]});

test('fresh exports pass actual43c350 signature/version/decrypt/inflate/stage registration',()=>{
  assert.equal(native.syntheticExportLoaded,true);assert.equal(native.originalReplayLoaded,false);
  assert.equal(native.wholeGameOracle,false);assert.equal(native.originalFilesModified,false);
  assert.equal(native.cryptoSamples.length,32);
  for(const sample of native.samples){
    const before=structuredClone(sample.options),file=encodeReplay(sample.options),view=new DataView(file.buffer);
    assert.deepEqual(sample.options,before,'encoder preserves caller stage records');
    assert.equal(file.length,sample.fileSize,sample.name);assert.equal(sha(file),sample.fileSha256,sample.name);
    assert.equal(Buffer.from(file.subarray(0,36)).toString('hex'),sample.fileHeaderHex);
    assert.equal(sample.native.returnCode,0);assert.deepEqual(sample.native.entered,['0043c350','0044c710','004639d0']);
    assert.equal(sample.badSignatureReturn,0xffffffff);assert.equal(sample.badVersionReturn,0xffffffff);
    assert.equal(sample.native.bytesRead,36+view.getUint32(28,true),'loader excludes optional USER blocks');
    const parsed=parseReplay(file),dataView=new DataView(parsed.data.buffer);
    assert.equal(sha(parsed.data),sample.native.rawSha256,'web decoder agrees with original LZ output');
    assert.deepEqual([parsed.character,parsed.shot,parsed.difficulty],[sample.options.character,sample.options.shot,sample.options.difficulty]);
    assert.equal(parsed.name,sample.options.name);
    assert.equal(parsed.practice,!!sample.options.practice);
    assert.equal(dataView.getBigUint64(12,true),BigInt(sample.options.timestamp));
    assert.equal(dataView.getInt32(20,true),sample.options.scoreStored);
    assert.equal(dataView.getFloat32(84,true),sample.options.slowdown);
    assert.equal(dataView.getUint32(104,true),sample.options.cleared?8:sample.options.stages.at(-1).number);
    assert.equal(parsed.data[10],sample.options.practice?1:0);
    const packed=file.subarray(36,36+view.getUint32(28,true));
    const compressed=decryptBytes(decryptBytes(packed,0x5e,0xe1,0x800),0x7d,0x3a,0x40);
    assert.equal(sha(compressed),sample.compressedSha256);
    for(const [index,stage] of parsed.stages.entries()){
      const expected=sample.native.stages[index],supplied=sample.options.stages[index];
      assert.deepEqual(stage.initial,supplied.initial);assert.deepEqual(stage.spellTimes,supplied.spellTimes);
      assert.deepEqual([stage.number,stage.frames,stage.payload,stage.offset,stage.inputOffset,stage.fpsOffset],
        [expected.number,expected.frames,expected.payload,expected.headerOffset,expected.inputOffset,expected.fpsOffset]);
      assert.equal(Buffer.from(parsed.data.subarray(stage.offset,stage.offset+160)).toString('hex'),expected.stageHeaderHex);
      assert.equal(sha(parsed.data.subarray(stage.inputOffset,stage.fpsOffset)),expected.inputSha256);
      assert.deepEqual(Array.from(parsed.data.subarray(stage.fpsOffset,stage.offset+160+stage.payload)),supplied.fps);
      assert.deepEqual(supplied.fps,expected.fps);
      assert.equal(stage.fpsCount,expected.fps.length,'31 playable frames include the second FPS sample with or without an end marker');
      for(const [frame,input] of supplied.inputs.entries())assert.deepEqual(parsed.input(stage.number,frame),input);
      assert.deepEqual(parsed.input(stage.number,stage.frames-1),supplied.end?ended:supplied.inputs.at(-1));
    }
  }
});

test('encryption matches actual463af0 at odd, quarter-block, block and small-tail boundaries',()=>{
  for(const sample of native.cryptoSamples){
    const source=Uint8Array.from({length:sample.length},(_,i)=>(i*73+19)&255),before=source.slice();
    assert.equal(sha(source),sample.sourceSha256);
    const encrypted=encryptReplayBytes(source,sample.key,sample.step,sample.block);
    assert.equal(sha(encrypted),sample.encryptedSha256,`native block${sample.block} length${sample.length}`);
    assert.deepEqual(decryptBytes(encrypted,sample.key,sample.step,sample.block),source);
    assert.deepEqual(source,before,'crypto preserves caller bytes');
  }
});

test('literal compression includes legal MSB-first bytes and a zero dictionary terminator',()=>{
  assert.deepEqual(compressReplayLiterals(Uint8Array.of(255,0)),Uint8Array.of(255,192,0,0));
  assert.deepEqual(compressReplayLiterals(new Uint8Array()),Uint8Array.of(0,0));
  for(const length of [112,113,159,160,8192,8193]){
    const source=Uint8Array.from({length},(_,i)=>(i*37+41)&255),compressed=compressReplayLiterals(source);
    assert.equal(compressed.length,Math.ceil((length*9+14)/8));assert.deepEqual(inflateReplay(compressed,length),source);
  }
});

test('default config and spell marker words agree with actual423250/43b080',()=>{
  assert.equal(Buffer.from(defaultReplayConfig()).toString('hex'),native.defaultConfigHex);
  assert.deepEqual(defaultReplaySpellTimes(),native.defaultSpellTimes);
  const fullscreen=defaultReplaySpellTimes(true);assert.deepEqual(fullscreen.slice(0,20),native.defaultSpellTimes.slice(0,20));assert.equal(fullscreen[20],1);
  const config=defaultReplayConfig();config[0]=0;assert.equal(defaultReplayConfig()[0],1,'fresh mutable defaults');
});

test('file header uses native reserved zeros and aligned optional USER records',()=>{
  const file=encode(makeStage()),view=new DataView(file.buffer);
  assert.equal(view.getUint32(0,true),0x72323174);assert.equal(view.getUint16(4,true),4);assert.equal(view.getUint32(16,true),0x100);
  for(const offset of [8,20,24])assert.equal(view.getUint32(offset,true),0);
  assert.equal(view.getUint16(6,true),0);
  let at=view.getUint32(12,true),types=[];
  while(at<file.length){assert.equal(view.getUint32(at,true),0x52455355);const size=view.getUint32(at+4,true);assert.equal(size%4,0);assert.ok(size>=16);types.push(file[at+8]);assert.equal(file[at+9]|file[at+10]|file[at+11],0);at+=size;}
  assert.equal(at,file.length);assert.deepEqual(types,[0,1]);
  const without=encodeReplay({...options,stages:[makeStage()],includeUserBlocks:false}),v=new DataView(without.buffer);
  assert.equal(v.getUint32(12,true),without.length);assert.equal(without.length,36+v.getUint32(28,true));
  assert.deepEqual(parseReplay(file).data,parseReplay(without).data);
});

test('names are padded native ASCII or explicit nonzero Shift-JIS bytes',()=>{
  assert.equal(Buffer.from(encodeReplayName('A')).toString(),'A       ');
  assert.equal(Buffer.from(encodeReplayName('123456789')).toString(),'12345678');
  assert.equal(Buffer.from(encodeReplayName('A\0ignored')).toString(),'A       ');
  const source=Uint8Array.of(0x82,0x60);assert.deepEqual(encodeReplayName(source),Uint8Array.of(0x82,0x60,32,32,32,32,32,32));
  for(const invalid of ['博丽','\n',Uint8Array.of(0),new Uint8Array(9).fill(65)])assert.throws(()=>encodeReplayName(invalid),/名字/);
});

test('recording stores computed press/release edges and rounded FPS every30 inputs',()=>{
  const recorder=new ReplayRecorder(options);const source=makeStage();recorder.recordStage(source);
  source.initial.lives=0;source.initial.ufoColors[0]=9;
  recorder.pushInput({held:0x81},59.6);recorder.pushInput({held:0xc1});recorder.pushInput({held:0x40});
  for(let i=3;i<31;i++)recorder.pushInput({held:0},i===30?300:1);
  recorder.setSpellTime(3,-12345678);
  const recorded=recorder.finishStage({end:true});assert.equal(recorded.inputs.length,32);
  assert.deepEqual(recorded.fps,[60,255]);assert.deepEqual(recorded.initial,initial);
  assert.deepEqual(recorded.inputs.slice(0,3),[{held:0x81,pressed:0x81,released:0},{held:0xc1,pressed:0x40,released:0},{held:0x40,pressed:0,released:0x81}]);
  assert.deepEqual(recorded.inputs.at(-1),ended);assert.equal(recorded.spellTimes[3],-12345678);
  const parsed=parseReplay(recorder.result({end:false,scoreStored:7654321}));assert.equal(parsed.stages[0].frames,32);
  assert.equal(new DataView(parsed.data.buffer).getInt32(20,true),7654321);
});

test('stage transitions reset edge history and result writes one end marker',()=>{
  const recorder=new ReplayRecorder(options);recorder.recordStage(makeStage(1));recorder.pushInput({held:0x81});recorder.finishStage();
  recorder.recordStage(makeStage(2));recorder.pushInput({held:0x81});
  const parsed=parseReplay(recorder.result({cleared:true,continues:2,slowdown:3.5}));
  assert.deepEqual(parsed.stages.map(s=>s.frames),[1,2]);assert.deepEqual(parsed.input(2,0),{held:0x81,pressed:0x81,released:0});
  assert.deepEqual(parsed.input(2,1),ended);assert.equal(replayInputEnded(parsed.input(1,0)),false);
  assert.equal(new DataView(parsed.data.buffer).getUint32(104,true),8);assert.equal(new DataView(parsed.data.buffer).getInt32(108,true),2);
  assert.equal(recorder.finishStage(),null);assert.deepEqual(parseReplay(recorder.result()).data,parsed.data);
  const endedStage=makeStage();endedStage.inputs.push({...ended});assert.equal(parseReplay(encode(endedStage)).stages[0].frames,4);
});

test('held-onlyffff remains playable and signed native fields survive export',()=>{
  const stage=makeStage();stage.inputs[1]={held:0xffff,pressed:0,released:0};
  Object.assign(stage.initial,{score:-10,power:-143,lives:-1,lifeFragments:-2,bombs:-3,bombFragments:-4,rank:-42});
  const parsed=parseReplay(encode(stage));assert.deepEqual(parsed.stages[0].initial,stage.initial);
  assert.equal(replayInputEnded(parsed.input(1,1)),false);assert.equal(replayInputEnded(parsed.input(1,3)),true);
});

test('encoder rejects inconsistent metadata, stage sizes and misplaced end markers',()=>{
  const base={...options,stages:[makeStage()]};
  for(const bad of [{character:-1},{character:3},{shot:2},{difficulty:5},{stages:[]},{stages:Array(8).fill(makeStage())},{stages:[makeStage(),makeStage()]},
    {timestamp:-1},{timestamp:NaN},{slowdown:-1},{slowdown:101},{continues:-1},{finalStage:9},{configBytes:new Uint8Array(59)}])assert.throws(()=>encodeReplay({...base,...bad}),/录像/);
  for(const change of [{number:0},{seed:65536},{initial:{...initial,power:32768}},{initial:{...initial,lives:1.5}},{initial:{...initial,ufoColors:[0,1]}},
    {initial:{...initial,nativeGlobals:[0,1,2]}},{spellTimes:Array(20).fill(0)},{inputs:[]},{inputs:[ended,{held:0,pressed:0,released:0}]},{fps:[]},{fps:[256]},
    {inputs:[{held:65536,pressed:0,released:0}]}])assert.throws(()=>encode({...makeStage(),...change}),/录像/);
});

test('recorder fails explicitly for invalid lifecycle and sentinel writes',()=>{
  const recorder=new ReplayRecorder(options);assert.throws(()=>recorder.pushInput({held:1}),/尚未开始/);assert.throws(()=>recorder.setSpellTime(0,0),/尚未开始/);
  assert.throws(()=>recorder.result(),/关卡数量/);assert.equal(recorder.finishStage(),null);
  recorder.recordStage(makeStage());assert.throws(()=>recorder.recordStage(makeStage(2)),/尚未结束/);
  assert.throws(()=>recorder.pushInput(ended),/结束标记/);assert.throws(()=>recorder.pushInput({held:1},Infinity),/帧率/);
  assert.throws(()=>recorder.setSpellTime(20,0),/符卡序号/);assert.throws(()=>recorder.finishStage({spellTimes:[]}),/符卡时间/);
  recorder.pushInput({held:1,pressed:0, released:2});assert.deepEqual(recorder.active.inputs[0],{held:1,pressed:0,released:2});
  recorder.finishStage();assert.throws(()=>recorder.recordStage(makeStage()),/重复/);
});
