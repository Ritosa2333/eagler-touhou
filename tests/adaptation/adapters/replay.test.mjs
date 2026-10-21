import test from 'node:test';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {assessTh08ReplayInputConsumption,deriveTh08ReplayInputExpectation,replayTerminalPolicy} from './replay.mjs';

function packBits(bits){
  const bytes=[];
  for(let start=0;start<bits.length;start+=8){let byte=0;for(let i=0;i<8;i++)byte=(byte<<1)|(bits[start+i]??0);bytes.push(byte);}
  return Buffer.from(bytes);
}

function encodeLiteralPayload(payload){
  const bits=[];
  for(const byte of payload){bits.push(1);for(let shift=7;shift>=0;shift--)bits.push((byte>>shift)&1);}
  bits.push(0,...Array(13).fill(0));
  return packBits(bits);
}

function replayFixture({inputBytes=10,timingBytes=2,debugRecords=false}={}){
  const stageOffset=0x134,timingOffset=stageOffset+0x24+inputBytes;
  const timingEnd=timingOffset+timingBytes,payloadEnd=Math.max(0x134+0xcc,timingEnd+1);
  const payload=Buffer.alloc(payloadEnd-0x68);
  const putU32=(buffer,offset,value)=>buffer.writeUInt32LE(value>>>0,offset);
  for(let i=0;i<inputBytes;i++)payload[stageOffset-0x68+0x24+i]=(i*17)&0xff;
  for(let i=0;i<timingBytes;i++)payload[timingOffset-0x68+i]=(i*29)&0xff;
  const compressed=encodeLiteralPayload(payload);
  const plain=Buffer.alloc(0x68+compressed.length);
  putU32(plain,0,0x50523854);plain.writeUInt16LE(6,4);plain[6]=debugRecords?1:0;
  putU32(plain,12,plain.length);plain[21]=37;
  putU32(plain,24,compressed.length);putU32(plain,28,payload.length);
  putU32(plain,32,stageOffset);putU32(plain,68,timingOffset);
  // Mark the first timing block's exact end. An unpaired offset has no stage
  // gameplay block and is ignored by ReplayPlayback::load, but bounds this
  // stage's timing bytes exactly as the real format does.
  putU32(plain,72,timingEnd);
  compressed.copy(plain,0x68);
  let checksum=0x3f000318;for(let i=21;i<plain.length;i++)checksum=(checksum+plain[i])>>>0;
  putU32(plain,16,checksum);
  const raw=Buffer.from(plain);let key=raw[21];
  for(let i=24;i<raw.length;i++){raw[i]=(raw[i]+key)&0xff;key=(key+7)&0xff;}
  return raw;
}

test('th08 independent v6 parser derives the read capacity from exact stage blocks',()=>{
  const bytes=replayFixture(),sha256=createHash('sha256').update(bytes).digest('hex');
  const expected=deriveTh08ReplayInputExpectation(bytes,{stage:0,expectedSha256:sha256});
  assert.equal(expected.formatVersion,6);assert.equal(expected.stage,0);assert.equal(expected.debugRecords,false);
  assert.deepEqual(expected.blocks,{inputOffset:0x158,inputBytes:10,timingOffset:0x162,timingBytes:2});
  assert.deepEqual(expected.capacity,{maxSuccessfulFrames:5,inputCursorAtCapacity:10,timingCursorAtCapacity:1,
    inputFrameCapacity:5,timingFrameCapacity:30,limitingBlock:'input'});

  const timingBound=replayFixture({inputBytes:70,timingBytes:2});
  const timingHash=createHash('sha256').update(timingBound).digest('hex');
  const timingExpected=deriveTh08ReplayInputExpectation(timingBound,{stage:0,expectedSha256:timingHash});
  assert.deepEqual(timingExpected.capacity,{maxSuccessfulFrames:30,inputCursorAtCapacity:60,timingCursorAtCapacity:1,
    inputFrameCapacity:35,timingFrameCapacity:30,limitingBlock:'timing'});

  const laterTimingBound=replayFixture({inputBytes:200,timingBytes:4});
  const laterTimingHash=createHash('sha256').update(laterTimingBound).digest('hex');
  const laterTimingExpected=deriveTh08ReplayInputExpectation(laterTimingBound,{stage:0,expectedSha256:laterTimingHash});
  assert.deepEqual(laterTimingExpected.capacity,{maxSuccessfulFrames:90,inputCursorAtCapacity:180,timingCursorAtCapacity:3,
    inputFrameCapacity:100,timingFrameCapacity:90,limitingBlock:'timing'});

  const debug=replayFixture({inputBytes:26,timingBytes:2,debugRecords:true});
  const debugHash=createHash('sha256').update(debug).digest('hex');
  const debugExpected=deriveTh08ReplayInputExpectation(debug,{stage:0,expectedSha256:debugHash});
  assert.equal(debugExpected.stride,6);assert.equal(debugExpected.capacity.maxSuccessfulFrames,5);
  assert.equal(debugExpected.capacity.inputCursorAtCapacity,30);
});

test('th08 parser rejects corrupted bytes, wrong fixture identity and absent stage',()=>{
  const bytes=replayFixture(),sha256=createHash('sha256').update(bytes).digest('hex');
  assert.throws(()=>deriveTh08ReplayInputExpectation(bytes,{stage:0,expectedSha256:'f'.repeat(64)}),/verified fixture SHA/);
  assert.throws(()=>deriveTh08ReplayInputExpectation(bytes,{stage:1,expectedSha256:sha256}),/no stage 1/);
  const corrupted=Buffer.from(bytes);corrupted[corrupted.length-1]^=1;
  assert.throws(()=>deriveTh08ReplayInputExpectation(corrupted,{stage:0,expectedSha256:createHash('sha256').update(corrupted).digest('hex')}),/checksum|LZSS/);
});

test('th08 terminal counters enforce source relations and block capacity without inventing a natural endpoint',()=>{
  const bytes=replayFixture(),sha256=createHash('sha256').update(bytes).digest('hex');
  const expected=deriveTh08ReplayInputExpectation(bytes,{stage:0,expectedSha256:sha256});
  const actual={available:true,loaded:true,stage:0,frame:5,reportedFrame:5,inputCursor:10,inputBytes:10,
    timingCursor:1,timingBytes:2,stride:2,ended:true,inGame:false};
  const atCapacity=assessTh08ReplayInputConsumption({expected,actual,ended:true});
  assert.equal(atCapacity.status,'incomplete');assert.equal(atCapacity.matches.atFileDerivedCapacity,true);
  assert.match(atCapacity.reason,/no mapped natural ReplayStream cursor/);

  const earlyButConsistent=assessTh08ReplayInputConsumption({expected,actual:{...actual,frame:4,reportedFrame:4,inputCursor:8},ended:true});
  assert.equal(earlyButConsistent.status,'incomplete');
  assert.equal(earlyButConsistent.matches.withinStageCapacity,true);
  assert.equal(earlyButConsistent.matches.atFileDerivedCapacity,false);

  for(const [field,value] of [['frame',6],['reportedFrame',4],['inputCursor',9],['inputBytes',12],
    ['timingCursor',0],['timingBytes',4],['stride',6],['stage',1],['ended',false],['inGame',true]]){
    const result=assessTh08ReplayInputConsumption({expected,actual:{...actual,[field]:value},ended:true});
    assert.equal(result.status,'failed',`${field} mismatch must fail`);
    assert.ok(result.matches[field]===false||result.reason,`${field} mismatch must remain visible`);
  }
  assert.equal(assessTh08ReplayInputConsumption({expected,actual:{...actual,frame:-1,inputCursor:-2,timingCursor:0},ended:true}).status,'failed');
  assert.equal(assessTh08ReplayInputConsumption({expected,actual:{...actual,available:false,reason:'missing export'},ended:true}).status,'incomplete');
  assert.equal(assessTh08ReplayInputConsumption({expected,actual,ended:false}).status,'incomplete');
  assert.equal(assessTh08ReplayInputConsumption({expected,actual:{...actual,inputCursor:null},ended:true}).status,'incomplete');
});

test('hash-bound th08 demo0 capacity classifies natural endpoints without promoting EOF or EOF-minus-one',()=>{
  // Independently established by parsing the registered v6 demo0 SHA-256
  // 6de76d9f5d0ef8d58887bc74087fd566c246c676b6429850ea4702776f2533d8.
  // Block lengths provide an upper read capacity. The accepted original capture
  // does not map its natural terminal to ReplayStream cursors.
  const expected={stage:5,sha256:'6de76d9f5d0ef8d58887bc74087fd566c246c676b6429850ea4702776f2533d8',
    debugRecords:false,blocks:{inputBytes:22168,timingBytes:371},stride:2,
    capacity:{maxSuccessfulFrames:11084,inputCursorAtCapacity:22168,timingCursorAtCapacity:370,
      inputFrameCapacity:11084,timingFrameCapacity:11100,limitingBlock:'input'}};
  const terminal={available:true,loaded:true,stage:5,frame:11084,reportedFrame:11084,inputCursor:22168,inputBytes:22168,
    timingCursor:370,timingBytes:371,stride:2,ended:true,inGame:false};
  const atCapacity=assessTh08ReplayInputConsumption({expected,actual:terminal,ended:true});
  assert.equal(atCapacity.status,'incomplete');assert.equal(atCapacity.matches.atFileDerivedCapacity,true);

  for(const frame of [11047,11083]){
    const beforeCapacity=assessTh08ReplayInputConsumption({expected,actual:{...terminal,frame,reportedFrame:frame,
      inputCursor:frame*2,timingCursor:Math.ceil(frame/30)},ended:true});
    assert.equal(beforeCapacity.status,'incomplete',`natural terminal at frame ${frame} needs an independent endpoint reference`);
    assert.equal(beforeCapacity.matches.withinStageCapacity,true);
    assert.equal(beforeCapacity.matches.atFileDerivedCapacity,false);
  }

  const overCapacity=assessTh08ReplayInputConsumption({expected,actual:{...terminal,frame:11085,reportedFrame:11085,
    inputCursor:22170,timingCursor:370},ended:true});
  assert.equal(overCapacity.status,'failed');assert.equal(overCapacity.matches.withinStageCapacity,false);

  for(const [field,value] of [['stage',4],['inputBytes',22166],['timingBytes',370],['stride',6],['timingCursor',369]]){
    const shifted=assessTh08ReplayInputConsumption({expected,actual:{...terminal,[field]:value},ended:true});
    assert.equal(shifted.status,'failed',`${field} layout mismatch must fail`);
    assert.ok(shifted.matches[field]===false||shifted.reason,`${field} mismatch must remain attributed`);
  }
});

test('only th08 normal selects the parsed capacity diagnostic policy',()=>{
  assert.deepEqual(replayTerminalPolicy({game:'th08',variant:'normal'}),{
    kind:'parsed-capacity-diagnostic',requires:['native replay stream counters','natural terminal signal','independent natural-terminal cursor reference']});
  assert.equal(replayTerminalPolicy({game:'th08',variant:'multiplayer'}),null);
  assert.equal(replayTerminalPolicy({game:'th10',variant:'normal'}),null);
});

