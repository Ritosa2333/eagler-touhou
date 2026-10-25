import {createHash} from 'node:crypto';
import {decodeTh08Lzss} from './th08-data-codec.mjs';

const REPLAY_HEADER_BYTES=0x68;
const REPLAY_METADATA_BYTES=0x134;
const REPLAY_STAGE_BYTES=0x24;
const REPLAY_PRACTICE_OFFSET=0x7b;
const REPLAY_PROCESSING_DROP_OFFSET=0x118;
const MAX_REPLAY_PAYLOAD_BYTES=8*1024*1024;

function requireRange(bytes,offset,size,label){
  if(!Number.isSafeInteger(offset)||!Number.isSafeInteger(size)||offset<0||size<0||offset+size>bytes.length)
    throw new Error(`th08 Replay ${label} is outside the decoded file`);
}

function readU16(bytes,offset,label){requireRange(bytes,offset,2,label);return bytes[offset]|(bytes[offset+1]<<8);}
function readU32(bytes,offset,label){requireRange(bytes,offset,4,label);return(bytes[offset]|(bytes[offset+1]<<8)|(bytes[offset+2]<<16)|(bytes[offset+3]<<24))>>>0;}

export function decodeTh08ReplayFile(input){
  const raw=Buffer.from(input);
  if(raw.length<REPLAY_HEADER_BYTES)throw new Error('th08 Replay is shorter than its 0x68-byte header');
  if(readU32(raw,0,'magic')!==0x50523854||readU16(raw,4,'version')!==6)
    throw new Error('th08 Replay is not version 6');
  const fileSize=readU32(raw,12,'file size');
  if(fileSize<REPLAY_HEADER_BYTES||fileSize>raw.length)throw new Error('th08 Replay file size is invalid');
  const plain=Buffer.from(raw.subarray(0,fileSize));
  let key=plain[21];
  for(let index=24;index<fileSize;index++){
    plain[index]=(plain[index]-key)&0xff;
    key=(key+7)&0xff;
  }
  let checksum=0x3f000318;
  for(let index=21;index<fileSize;index++)checksum=(checksum+plain[index])>>>0;
  if(readU32(plain,16,'checksum')!==checksum)throw new Error('th08 Replay checksum does not match its decoded bytes');
  const compressedSize=readU32(plain,24,'compressed size');
  const payloadSize=readU32(plain,28,'payload size');
  if(compressedSize>fileSize-REPLAY_HEADER_BYTES||payloadSize<0xcc||payloadSize>MAX_REPLAY_PAYLOAD_BYTES)
    throw new Error('th08 Replay compressed or payload size is invalid');
  const compressed=plain.subarray(REPLAY_HEADER_BYTES,REPLAY_HEADER_BYTES+compressedSize);
  const payload=decodeTh08Lzss(compressed,payloadSize);
  const decoded=Buffer.concat([plain.subarray(0,REPLAY_HEADER_BYTES),payload]);
  return{raw,decoded,fileSize,compressedSize,payloadSize,debugRecords:decoded[6]!==0};
}

export function inspectTh08ReplayFixture(input,{expectedSha256}={}){
  if(!/^[a-f0-9]{64}$/.test(expectedSha256??''))throw new Error('th08 Replay inspection requires the registered fixture SHA-256');
  const parsed=decodeTh08ReplayFile(input),sha256=createHash('sha256').update(parsed.raw).digest('hex');
  if(sha256!==expectedSha256)throw new Error('th08 Replay bytes do not match the registered fixture SHA-256');
  const stages=[];
  for(let stage=0;stage<9;stage++){
    const offset=readU32(parsed.decoded,32+stage*4,'stage block offset');
    const timingOffset=readU32(parsed.decoded,68+stage*4,'timing block offset');
    if(offset)stages.push({stage,timingPresent:Boolean(timingOffset)});
  }
  return{schema:'th08/replay-inspection/v1',sha256,bytes:parsed.raw.length,formatVersion:6,
    debugRecords:parsed.debugRecords,stages};
}

/** Independently read the canonical lag and practice fields from a saved v6 Replay. */
export function inspectTh08ReplayMetadata(input,{expectedSha256}={}){
  if(!/^[a-f0-9]{64}$/.test(expectedSha256??''))throw new Error('th08 Replay metadata inspection requires the saved-byte SHA-256');
  const parsed=decodeTh08ReplayFile(input),sha256=createHash('sha256').update(parsed.raw).digest('hex');
  if(sha256!==expectedSha256)throw new Error('th08 Replay metadata bytes do not match their saved-byte SHA-256');
  requireRange(parsed.decoded,0,REPLAY_METADATA_BYTES,'metadata');
  const lagView=new DataView(parsed.decoded.buffer,parsed.decoded.byteOffset+REPLAY_PROCESSING_DROP_OFFSET,4);
  const lag=lagView.getFloat32(0,true);
  if(!Number.isFinite(lag))throw new Error('th08 Replay metadata processing-drop value is not finite');
  return{schema:'th08/replay-metadata-inspection/v1',sha256,bytes:parsed.raw.length,formatVersion:6,
    metadata:{lag,practice:parsed.decoded[REPLAY_PRACTICE_OFFSET]!==0},
    source:'ReplayMetadata::lag and ReplayMetadata::practice from the independently decoded v6 header'};
}

/** Original v6 permits its USER information block after the encoded payload. */
export function inspectTh08OriginalReplay(input,{expectedSha256}={}){
  const parsed=decodeTh08ReplayFile(input),metadata=inspectTh08ReplayMetadata(input,{expectedSha256});
  let originalBytes=parsed.fileSize,userBytes=0;
  if(parsed.raw.length>originalBytes){
    requireRange(parsed.raw,originalBytes,12,'USER header');
    if(parsed.raw.toString('ascii',originalBytes,originalBytes+4)!=='USER')throw new Error('Original th08 Replay has an appended extension instead of USER information');
    userBytes=readU32(parsed.raw,originalBytes+4,'USER length');
    if(userBytes<12||userBytes%2||readU32(parsed.raw,originalBytes+8,'USER reserved')!==0)throw new Error('Original th08 USER information header is invalid');
    originalBytes+=userBytes;
    if(originalBytes!==parsed.raw.length)throw new Error('Original th08 Replay has an appended extension or invalid USER length');
  }
  if(metadata.metadata.practice)throw new Error('Original th08 Replay must describe an ordinary run');
  return{schema:'eagler-touhou/original-replay-inspection/1',format:'th08-v6',version:6,
    sha256:metadata.sha256,bytes:parsed.raw.length,originalBytes,userBytes,extensionBytes:0};
}

/** Read the exact u16 values ReplayStream can fetch from one verified v6 stage. */
export function inspectTh08ReplayInputFrames(input,{stage,expectedSha256,firstFrame=0,frameCount}={}){
  const expected=deriveTh08ReplayInputExpectation(input,{stage,expectedSha256});
  if(!Number.isSafeInteger(firstFrame)||firstFrame<0)throw new Error('th08 Replay firstFrame must be a nonnegative safe integer');
  if(!Number.isSafeInteger(frameCount)||frameCount<1)throw new Error('th08 Replay frameCount must be a positive safe integer');
  if(firstFrame+frameCount>expected.capacity.maxSuccessfulFrames)
    throw new Error('th08 Replay frame window exceeds its independently parsed successful-read capacity');
  const parsed=decodeTh08ReplayFile(input),stageOffset=readU32(parsed.decoded,32+stage*4,'selected stage offset');
  const inputOffset=stageOffset+REPLAY_STAGE_BYTES,stride=expected.stride;
  const frames=Array.from({length:frameCount},(_,index)=>{
    const frame=firstFrame+index,offset=inputOffset+frame*stride;
    return{frame,cursor:frame*stride,byteOffset:offset,buttons:readU16(parsed.decoded,offset,'input frame')};
  });
  return{schema:'eagler-touhou/th08-replay-input-frames/1',stage,sha256:expected.sha256,
    formatVersion:6,stride,inputOffset,capacityFrames:expected.capacity.maxSuccessfulFrames,frames,
    source:'independent v6 decode; each value is read at ReplayStream::play input_cursor and stride'};
}

const MOTION_MAGIC='THMOTION';
const MOTION_FOOTER_BYTES=24;
const MOTION_ENTRY_LIMIT=600000;
const MAX_MOTION_VELOCITY=10000000;

function readI32(bytes,offset,label){return readU32(bytes,offset,label)|0;}
function readF32(bytes,offset,label){
  requireRange(bytes,offset,4,label);
  const value=new DataView(bytes.buffer,bytes.byteOffset+offset,4).getFloat32(0,true);
  if(!Number.isFinite(value))throw new Error(`th08 THMOTION ${label} must be finite`);
  return value;
}
function fnv1a(bytes){let hash=2166136261;for(const byte of bytes)hash=Math.imul(hash^byte,16777619)>>>0;return hash;}

/** Parse and verify the appended THMOTION footer independently of MotionTrack::load. */
export function parseTh08MotionTrailer(input,{expectedSha256,expectedGame=8}={}){
  if(!/^[a-f0-9]{64}$/.test(expectedSha256??''))throw new Error('th08 THMOTION parse requires the verified Replay SHA-256');
  const raw=Buffer.from(input),sha256=createHash('sha256').update(raw).digest('hex');
  if(sha256!==expectedSha256)throw new Error('th08 THMOTION bytes do not match the verified Replay SHA-256');
  const parsed=decodeTh08ReplayFile(raw);
  if(raw.length<parsed.fileSize+MOTION_FOOTER_BYTES)throw new Error('th08 Replay has no complete THMOTION footer');
  const footerOffset=raw.length-MOTION_FOOTER_BYTES;
  if(raw.toString('ascii',footerOffset,footerOffset+8)!==MOTION_MAGIC)throw new Error('th08 Replay has no terminal THMOTION footer');
  const game=readU32(raw,footerOffset+8,'game id'),version=readU32(raw,footerOffset+12,'version');
  const payloadBytes=readU32(raw,footerOffset+16,'payload length'),expectedHash=readU32(raw,footerOffset+20,'checksum');
  if(game!==expectedGame)throw new Error(`th08 THMOTION game id ${game} does not match ${expectedGame}`);
  if(version!==1&&version!==2)throw new Error(`Unsupported th08 THMOTION version ${version}`);
  if(payloadBytes<36||payloadBytes>footerOffset-parsed.fileSize)
    throw new Error('th08 THMOTION payload length is outside the appended Replay tail');
  const payloadOffset=footerOffset-payloadBytes,payload=raw.subarray(payloadOffset,footerOffset);
  if(fnv1a(payload)!==expectedHash)throw new Error('th08 THMOTION payload checksum does not match');
  const movementCounts=Array.from({length:9},(_,stage)=>readU32(payload,stage*4,`movement count stage ${stage}`));
  const movementTotal=movementCounts.reduce((sum,count)=>sum+count,0);
  if(movementTotal>MOTION_ENTRY_LIMIT)throw new Error('th08 THMOTION movement count exceeds its production limit');
  let offset=36;
  const movement=movementCounts.map((count,stage)=>{
    if(count>(payload.length-offset)/12)throw new Error(`th08 THMOTION movement entries for stage ${stage} are truncated`);
    const entries=[];
    for(let index=0;index<count;index++){
      const tick=readU32(payload,offset,'movement tick'),x=readF32(payload,offset+4,'movement x'),y=readF32(payload,offset+8,'movement y');
      if(index&&tick<=entries[index-1].tick)throw new Error(`th08 THMOTION movement ticks are not strictly increasing for stage ${stage}`);
      if(Math.abs(x)>MAX_MOTION_VELOCITY||Math.abs(y)>MAX_MOTION_VELOCITY)
        throw new Error(`th08 THMOTION movement velocity exceeds its production limit for stage ${stage}`);
      entries.push({tick,x,y});offset+=12;
    }
    return{stage,entries};
  });
  let touch=[];
  if(version===2){
    if(payload.length-offset<36)throw new Error('th08 THMOTION v2 touch counts are truncated');
    const touchCounts=Array.from({length:9},(_,stage)=>readU32(payload,offset+stage*4,`touch count stage ${stage}`));
    offset+=36;const touchTotal=touchCounts.reduce((sum,count)=>sum+count,0);
    if(touchTotal>MOTION_ENTRY_LIMIT)throw new Error('th08 THMOTION touch count exceeds its production limit');
    touch=touchCounts.map((count,stage)=>{
      if(count>(payload.length-offset)/20)throw new Error(`th08 THMOTION touch entries for stage ${stage} are truncated`);
      const entries=[];
      for(let index=0;index<count;index++){
        const tick=readU32(payload,offset,'touch tick'),id=readI32(payload,offset+4,'touch id');
        const x=readF32(payload,offset+8,'touch x'),y=readF32(payload,offset+12,'touch y'),action=readU32(payload,offset+16,'touch action');
        if(index&&tick<entries[index-1].tick)throw new Error(`th08 THMOTION touch ticks are out of order for stage ${stage}`);
        if(action>3)throw new Error(`th08 THMOTION touch action is invalid for stage ${stage}`);
        entries.push({tick,id,x,y,action});offset+=20;
      }
      return{stage,entries};
    });
  }
  if(offset!==payload.length)throw new Error('th08 THMOTION payload contains unclaimed bytes');
  return{schema:'eagler-touhou/th08-thmotion/1',game,version,sha256,bytes:raw.length,
    payload:{offset:payloadOffset,bytes:payloadBytes,checksum:expectedHash},
    movement,touch,totals:{movement:movementTotal,touch:touch.reduce((sum,item)=>sum+item.entries.length,0)},
    source:'independent THMOTION footer, FNV-1a checksum, count, tick and entry validation'};
}

/** Title-owned semantics consumed by generic finite-window Replay scenarios. */
export const TH08_REPLAY_SEMANTICS=Object.freeze({
  schema:'eagler-touhou/replay-title-semantics/1',game:'th08',variant:'normal',
  nativeTrace:{schema:2,wordCount:36,controlExport:'sdl_replay_trace_control_v1',readExport:'sdl_replay_trace_status_v1',
    statusFields:{schema:29,nextSequence:30,firstAvailable:31,count:32,capacity:33,samplesStart:34},
    sample:{sequence:0,boundary:1,stage:2,gameFlags:3,pollSequence:4,liveInput:5,
      recordingActive:6,recordingStage:7,recordingFrame:8,recordingInputCursor:9,recordingPhysicalInput:10,
      playbackLoaded:11,playbackStage:12,playbackFrame:13,playbackInputCursor:14,playbackCurrent:15,playbackPrevious:16,
      motionPlaying:17,motionStage:18,motionTick:19,movementCursor:20,movementCount:21,touchCursor:22,touchCount:23,pointCount:24,
      consumerPhase:25,consumerValidMask:26,consumerSequence:27,consumerControlFrame:28,consumerStage:29,
      consumerCurrent:30,consumerPrevious:31,pointId:32,pointXFloat32Bits:33,pointYFloat32Bits:34,pointReleased:35},
    values:{postUpdateBoundary:1,consumerRunPhase:1,consumedInputValidBit:1<<1},
    pointCoordinates:'Float32 normalized [0,1]',pointTolerance:1e-6,
    inputReceipts:{touchControl:{export:'sdl_game_status',wordCount:10,
      fields:{context:{index:5,type:'i32'},fire:{index:7,type:'bool01'}},
      source:'ports/th08/web/cpp/sdl/GameHost.cpp | touch_state | sdl_game_status'}},
    source:'ports/th08/web/cpp/platform/BrowserRuntime.cpp | append_replay_trace | status'},
  format:{id:'th08-v6',version:6,frameValue:'little-endian-u16',stride:'2-or-6-from-debug-record-flag',
    source:'ports/th08/web/cpp/game/ReplayFile.hpp | ReplayStream.cpp'},
  finalizer:{export:'save_replay',app:true,allocateExport:'allocate',freeExport:'deallocate',
    slot:1,name:'ADAPTEST',path:'replay/th8_01.rpy',
    source:'ports/th08/web/cpp/platform/BrowserExports.cpp | browser_save_replay | GameApplication::finalize_replay'},
  opener:{export:'eagler_replay_open',app:false,allocateExport:'allocate',freeExport:'deallocate',
    saveRoot:'/savesth08',source:'ports/th08/web/cpp/sdl/LocalReplay.inc | eagler_replay_open'},
  recording:{sampleBoundary:'GameplayScene::prepare_frame receives SupervisorFrame input',
    commitBoundary:'GameplayScene::update_recording | priority 17 | ReplayStream::record',
    source:'ports/th08/web/cpp/game/GameplayScene.cpp | bind_jobs | update_recording',
    initialRawFrames:1,initialRawFrameValue:0,frameToRawIndex:'recorded-frame-number',
    consumer:{frameOffset:-1,initialInput:0,
      source:'ports/th08/web/cpp/game/GameplayScene.cpp | update_player priority 9 | update_recording priority 17 publishes next movement input'}},
  playback:{readBoundary:'ReplayStream::play | priority 6',consumerBoundary:'GameplayScene::update_player | priority 9',
    rawIndexOffset:-1,source:'ports/th08/web/cpp/game/GameplayScene.cpp | bind_jobs | update_replay | update_player',
    frameToRawIndex:'post-update-playback-frame-minus-one'},
  inputs:[
    {source:'keyboard',operation:{kind:'keyboard-key',code:'KeyZ',pressed:true},release:{kind:'keyboard-key',code:'KeyZ',pressed:false},
      logicalMask:1,anchor:'ports/th08/web/cpp/game/InputController.cpp | keyboard | VK_Z => Shoot'},
    {source:'virtual-gamepad',operation:{kind:'browser-gamepad-bit',bit:0,pressed:true},release:{kind:'browser-gamepad-bit',bit:0,pressed:false},
      logicalMask:1,anchor:'common/browser/browser-gamepad-input.hpp | ApplyGamepadVirtualKeys | bit 0 => VK_Z'},
    {source:'touch-controls',operation:{kind:'touch-fire-control',enabled:true},release:{kind:'touch-fire-control',enabled:false},
      logicalMask:1,anchor:'ports/th08/portable/input/TouchController.hpp | sample | fire => VK_Z'},
  ],
  pointerExtension:{id:'THMOTION',versions:[1,2],requiredVersion:2,
    rejection:{versionOffsetFromEnd:12,unsupportedVersion:0xffffffff,path:'replay/th8_udvers.rpyx',
      error:'Error: 触摸录像数据损坏或不属于这款游戏',source:'ports/th08/portable/browser/motion-replay.mjs | validateMotionReplay | version'},
    trajectory:[
      {kind:'pointer',action:'down',id:7101,x:0.42,y:0.64},
      {kind:'pointer',action:'move',id:7101,x:0.48,y:0.61},
      {kind:'pointer',action:'up',id:7101,x:0.52,y:0.58},
    ],
    consumedAt:'MotionTrack::playback | apply_touch_events',
    appliedPoint:{source:'MotionTrack::playback_points.begin()',selection:'single active pointer in signed-id map order',
      coordinates:'Float32 normalized [0,1]',tolerance:1e-6,
      traceWords:{count:24,id:32,xFloat32Bits:33,yFloat32Bits:34,released:35}},
    source:'ports/th08/portable/input/MotionTrack.hpp | trailer | load | apply_touch_events'},
  isolation:{liveInputs:['direction','shoot'],probes:[
      {name:'keyboard-direction',kind:'keyboard-key',code:'ArrowRight',pressed:true,logicalMask:128,
        covers:'direction',release:{kind:'keyboard-key',code:'ArrowRight',pressed:false},anchor:'ports/th08/web/cpp/game/InputController.cpp | keyboard | VK_RIGHT => Right'},
      {name:'keyboard-shoot',kind:'keyboard-key',code:'KeyZ',pressed:true,logicalMask:1,
        covers:'shoot',release:{kind:'keyboard-key',code:'KeyZ',pressed:false},anchor:'ports/th08/web/cpp/game/InputController.cpp | keyboard | VK_Z => Shoot'},
      {name:'virtual-gamepad-shoot',kind:'browser-gamepad-bit',bit:0,pressed:true,logicalMask:1,
        covers:'shoot',release:{kind:'browser-gamepad-bit',bit:0,pressed:false},anchor:'common/browser/browser-gamepad-input.hpp | bit 0 => VK_Z'},
      {name:'touch-fire',kind:'touch-fire-control',enabled:true,logicalMask:1,
        covers:'shoot',release:{kind:'touch-fire-control',enabled:false},anchor:'ports/th08/portable/input/TouchController.hpp | sample | fire => VK_Z',
        ingress:{strategy:'native-touch-control-suppressed',receipt:'touchControl',contextField:'context',contextValue:3,
          activeField:'fire',releasedValue:0,activeValue:1,
          source:'ports/th08/web/cpp/sdl/GameHost.cpp | touch_state | gameplay context 3 | sdl_game_status fields 5 and 7'}}
    ],playbackControlMask:8,
    source:'ports/th08/web/cpp/platform/BrowserRuntime.cpp | poll_input | GameplayScene::update_replay'},
  terminal:{kind:'block-exhaustion',hasIndependentFinishedEvent:false,
    source:'ports/th08/web/cpp/game/ReplayStream.cpp | play | ports/th08/web/cpp/game/GameplayScene.cpp | update_replay'},
});

function blockEnd(decoded,start){
  const end=REPLAY_HEADER_BYTES+readU32(decoded,28,'payload size');
  let boundary=end;
  for(const offsetBase of [32,68])for(let stage=0;stage<9;stage++){
    const offset=readU32(decoded,offsetBase+stage*4,'stage block offset');
    if(offset>start&&offset<boundary)boundary=offset;
  }
  return boundary;
}

/** Derive the maximum successful ReplayStream reads allowed by the imported v6 stage blocks. */
export function deriveTh08ReplayInputExpectation(input,{stage,expectedSha256}={}){
  if(!Number.isInteger(stage)||stage<0||stage>8)throw new Error('th08 Replay stage must be an integer from 0 through 8');
  if(!/^[a-f0-9]{64}$/.test(expectedSha256??''))throw new Error('th08 Replay capacity requires the verified fixture SHA-256');
  const parsed=decodeTh08ReplayFile(input);
  const sha256=createHash('sha256').update(parsed.raw).digest('hex');
  if(sha256!==expectedSha256)throw new Error('th08 Replay bytes do not match the verified fixture SHA-256');
  const stageOffset=readU32(parsed.decoded,32+stage*4,'selected stage offset');
  const timingOffset=readU32(parsed.decoded,68+stage*4,'selected timing offset');
  if(stageOffset<REPLAY_METADATA_BYTES||timingOffset<REPLAY_METADATA_BYTES)
    throw new Error(`th08 Replay has no stage ${stage} input/timing blocks`);
  const inputBoundary=blockEnd(parsed.decoded,stageOffset);
  const timingBoundary=blockEnd(parsed.decoded,timingOffset);
  if(inputBoundary-stageOffset<REPLAY_STAGE_BYTES||timingBoundary<=timingOffset)
    throw new Error(`th08 Replay stage ${stage} has an invalid input/timing block`);
  const inputBytes=inputBoundary-stageOffset-REPLAY_STAGE_BYTES;
  const timingBytes=timingBoundary-timingOffset;
  const stride=parsed.debugRecords?6:2;
  const inputFrames=inputBytes<2?0:Math.floor((inputBytes-2)/stride)+1;
  // ReplayStream::play advances timing_cursor by one byte at each 30-frame
  // boundary, while requiring a two-byte lookahead at that boundary.
  const timingFrames=Math.max(0,timingBytes-1)*30;
  const frames=Math.min(inputFrames,timingFrames);
  if(frames<=0)throw new Error(`th08 Replay stage ${stage} has no readable input capacity`);
  const limitingBlock=inputFrames<=timingFrames?'input':'timing';
  return{
    schema:'eagler-touhou/th08-replay-input-capacity/2',stage,sha256,formatVersion:6,
    debugRecords:parsed.debugRecords,stride,
    blocks:{inputOffset:stageOffset+REPLAY_STAGE_BYTES,inputBytes,
      timingOffset,timingBytes},
    capacity:{maxSuccessfulFrames:frames,inputCursorAtCapacity:frames*stride,timingCursorAtCapacity:Math.ceil(frames/30),
      inputFrameCapacity:inputFrames,timingFrameCapacity:timingFrames,limitingBlock},
    source:'independent th08 v6 file decryption, LZSS payload decoding, and stage/timing block-bound calculation',
  };
}

/** Check Runtime self-consistency against file-derived capacity without inventing a natural endpoint. */
export function assessTh08ReplayInputConsumption({expected,actual,ended}={}){
  const unavailable=(reason)=>({status:'incomplete',reason,expected:expected??null,actual:actual??null});
  if(!expected)return unavailable('No independent th08 Replay capacity is attached.');
  if(!actual||actual.available!==true)return unavailable(actual?.reason??'Native Replay stream consumption observation is unavailable.');
  if(actual.loaded!==true)return unavailable('No th08 Replay stage is loaded in the Runtime.');
  const required=['stage','frame','reportedFrame','inputCursor','inputBytes','timingCursor','timingBytes','stride'];
  const missing=required.filter(field=>!Number.isSafeInteger(actual[field]));
  if(missing.length)return unavailable(`Native Replay stream observation is missing ${missing.join(', ')}.`);
  if(ended!==true)return unavailable('The Runtime has not reported natural Replay completion.');
  const capacity=expected.capacity;
  if(!capacity||!Number.isSafeInteger(capacity.maxSuccessfulFrames)||capacity.maxSuccessfulFrames<=0||
    !Number.isSafeInteger(capacity.inputFrameCapacity)||!Number.isSafeInteger(capacity.timingFrameCapacity)||
    ![2,6].includes(expected.stride)||!Number.isSafeInteger(expected.stage)||
    !Number.isSafeInteger(expected.blocks?.inputBytes)||!Number.isSafeInteger(expected.blocks?.timingBytes))
    return{status:'failed',reason:'Independent th08 Replay capacity has an invalid shape.',expected,actual:{...actual}};
  const inputFrameCapacity=expected.blocks.inputBytes<2?0:Math.floor((expected.blocks.inputBytes-2)/expected.stride)+1;
  const timingFrameCapacity=Math.max(0,expected.blocks.timingBytes-1)*30;
  if(expected.debugRecords!== (expected.stride===6)||capacity.inputFrameCapacity!==inputFrameCapacity||
    capacity.timingFrameCapacity!==timingFrameCapacity||capacity.maxSuccessfulFrames!==Math.min(inputFrameCapacity,timingFrameCapacity)||
    capacity.inputCursorAtCapacity!==capacity.maxSuccessfulFrames*expected.stride||
    capacity.timingCursorAtCapacity!==Math.ceil(capacity.maxSuccessfulFrames/30))
    return{status:'failed',reason:'Independent th08 Replay capacity does not match the parsed block lengths and stride.',expected,actual:{...actual}};

  const observedMatches={
    stage:actual.stage===expected.stage,
    inputBytes:actual.inputBytes===expected.blocks.inputBytes,
    timingBytes:actual.timingBytes===expected.blocks.timingBytes,
    stride:actual.stride===expected.stride,
    reportedFrame:actual.reportedFrame===actual.frame,
    inputCursor:actual.inputCursor===actual.frame*actual.stride,
    timingCursor:actual.timingCursor===Math.ceil(actual.frame/30),
    ended:actual.ended===true,
    inGame:actual.inGame===false,
    nonnegativeCursors:actual.frame>=0&&actual.inputCursor>=0&&actual.timingCursor>=0,
    withinInputCapacity:actual.frame<=capacity.inputFrameCapacity,
    withinTimingCapacity:actual.frame<=capacity.timingFrameCapacity,
    withinStageCapacity:actual.frame<=capacity.maxSuccessfulFrames,
  };
  const mismatch=Object.entries(observedMatches).filter(([,match])=>!match).map(([field])=>field);
  const matches={...observedMatches,
    atFileDerivedCapacity:actual.frame===capacity.maxSuccessfulFrames,
    naturalTerminalCursorReference:false,
  };
  if(mismatch.length){
    return{status:'failed',reason:`Native Replay cursor or terminal state violates the file-derived capacity: ${mismatch.join(', ')}.`,
      expected,actual:{...actual},matches,capacity,proof:'ReplayStream::play advances frame/input cursor only after a successful input read, and timing cursor once every 30 successful frames.'};
  }
  return{
    status:'incomplete',
    reason:actual.frame===capacity.maxSuccessfulFrames?
      'Native Replay reached the maximum reads permitted by the file blocks, but the accepted original capture has no mapped natural ReplayStream cursor for this fixture. The block capacity does not establish the original natural endpoint.':
      'Native Replay ended within the file-derived read capacity. The accepted original capture has no mapped natural ReplayStream cursor for this fixture, so this terminal cannot be compared with an independent natural endpoint.',
    expected,actual:{...actual},matches,capacity,
    proof:'The parser establishes an upper bound from input/timing block lengths. Runtime frame and cursors are checked for source-defined consistency; a separate natural-end reference is required to prove the expected terminal.',
  };
}

export function replayTerminalPolicy({game,variant}={}){
  if(game==='th08'&&variant==='normal')return{kind:'parsed-capacity-diagnostic',requires:['native replay stream counters','natural terminal signal','independent natural-terminal cursor reference']};
  return null;
}
