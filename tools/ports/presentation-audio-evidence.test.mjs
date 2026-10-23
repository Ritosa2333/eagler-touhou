import test from 'node:test';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import vm from 'node:vm';
import {PRESENTATION_AUDIO_SCHEMA,presentationAudioFrameClockPlan,decodePresentationAudioClockBlock,
  presentationAudioPcmProcessorSource,installPresentationAudioProbe} from './presentation-audio-browser.mjs';
import {assessPresentationAudioOutput,assertPresentationAudioEqual,assertPresentationGainPcm} from './presentation-audio-evidence.mjs';

const clone=value=>JSON.parse(JSON.stringify(value));
const seal=raw=>({schema:PRESENTATION_AUDIO_SCHEMA,raw,rawSha256:createHash('sha256').update(JSON.stringify(raw)).digest('hex')});
function clockBlock(plan,frame,size=128,{segmentLength=1,currentFrame=frame,sequence=1}={}){
  const local=[],segment=[];
  for(let i=0;i<size;i++){const f=frame+i-plan.anchorFrame;local.push(plan.localBase+f%plan.segmentFrames);segment.push(Math.floor(f/plan.segmentFrames));}
  return {processSequence:sequence,currentFrame,quantumFrames:size,validOffset:0,frame,frames:size,local,
    segment:segmentLength===1?[segment[0]]:segment};
}
function pcmBlock(plan,frame,onset,{sequence=1}={}){
  const block=clockBlock(plan,frame,128,{sequence});const left=new Float32Array(128),right=new Float32Array(128);
  for(let i=0;i<128;i++)if(frame+i>=onset){left[i]=.25;right[i]=.125;}
  return {...block,inputChannels:2,leftWords:Array.from(new Uint32Array(left.buffer)),rightWords:Array.from(new Uint32Array(right.buffer))};
}
function fixture(){
  const plan=presentationAudioFrameClockPlan({sampleRate:48000,anchorTime:2}),frame=plan.anchorFrame+1024,time=frame/plan.sampleRate;
  const uploads=Array.from({length:46},(_,index)=>({serial:index+1,handle:index+1,voiceKey:String(100+index),sourceRate:44100,sourceFrames:4410,channels:1,uploadSuccess:true,
    contextSampleRate:48000,contextTime:.5,contextFrame:24000}));
  const taps=[],bindings=uploads.map((upload,index)=>{
    const ids=[];for(let n=0;n<3;n++){const id=taps.length+1;ids.push(id);const block=clockBlock(plan,plan.anchorFrame);
      taps.push({id,voiceKey:upload.voiceKey,kind:n?'source':'output',slot:n?n-1:null,
        configured:{startedAt:1,finishedAt:1.1,automationRate:{localFrame:'a-rate',segment:'a-rate'}},
        configuredAck:{kind:'configured',clockSchema:plan.schema,anchorFrame:plan.anchorFrame},
        final:{kind:'flushed',tag:1,processSequence:100,previousEnd:plan.anchorFrame+4096,failures:0,pending:0,currentFrame:plan.anchorFrame+4096},
        ready:{kind:'ready',frame:block.frame,processSequence:block.processSequence,currentFrame:block.currentFrame,block}});}
    return {logicalIndex:index,handle:upload.handle,voiceKey:upload.voiceKey,upload,
      baseline:{contextTime:2.001,leftGain:.5,rightGain:.5,volume:.5,pan:0,speed:1,playing:false,nodePresent:false},outputTap:ids[0],sourceTaps:ids.slice(1)};
  });
  const event={serial:47,logicalIndex:0,handle:1,voiceKey:'100',queueFrames:2048,queueRate:44100,sourceRate:44100,
    contextSampleRate:48000,contextTime:time,contextFrame:time*48000,queueClockBeforeTime:time,queueClockAfterTime:time,
    queueClockBeforeFrame:time*48000,queueClockAfterFrame:time*48000,queueSampleOrder:'cpp-sdl-queue-clock-bracket',routeSerial:48};
  const route={...event,serial:48,eventSerial:47,direct:true,fallback:false,fallbackStarted:false,pending:false};delete route.routeSerial;delete route.sourceRate;
  const hook={...event};delete hook.routeSerial;
  const sourcePcm={kind:'pcm',eventSerial:47,mode:'onset',tapId:2,voiceKey:'100',startFrame:frame,onsetFrame:frame+16,endFrame:frame+144,
    blocks:[pcmBlock(plan,frame,frame+16),pcmBlock(plan,frame+128,frame+16,{sequence:2})]};
  const pcm=clone(sourcePcm);Object.assign(pcm,{mode:'range',tapId:1,startFrame:frame+16,onsetFrame:null});
  for(const block of pcm.blocks)for(const side of ['leftWords','rightWords']){const words=Uint32Array.from(block[side]),samples=new Float32Array(words.buffer);for(let i=0;i<samples.length;i++)samples[i]*=.5;block[side]=Array.from(words);}
  const dropped={events:0,routes:0,uploads:0,errors:0};
  const interval={available:true,enabled:true,sequence:48,dropped:{...dropped},events:[event],routes:[route],uploads:[],errors:[]};
  const raw={contract:'th08/presentation-audio/1',epoch:1,rateHz:60,clock:plan,
    window:{startNativeSequence:46,endNativeSequence:48,startedAtTime:2.01,finishedAtTime:2.1,sampleRate:48000,historySeconds:2,prefixFrames:128},
    bindings,taps,hookEvents:[hook],hookRoutes:[route],hookUploads:[],starts:[{id:1,eventSerial:47,logicalIndex:0,handle:1,voiceKey:'100',sourceTap:2,outputTap:1,
      when:0,offset:0,duration:null,beforeTime:time,afterTime:time,gainSequence:8,bufferSame:true,sourceRate:44100,sourceFrames:4410,channels:2,playbackRate:1,loop:false}],
    markReceipts:[2,1].map(tapId=>({kind:'armed',eventSerial:47,tapId,processSequence:1,currentFrame:plan.anchorFrame})),
    gainCalls:['left','right','left','right','left','right','left','right'].map((side,index)=>({sequence:index+1,voiceKey:'100',side,value:.5,time,beforeTime:time,afterTime:time,
      hadActiveSource:false,controlStoppedActive:false,volume:.5,pan:0})),intervals:[interval],sourcePcm:[sourcePcm],pcm:[pcm],
    native:{sequence:48,recordLimit:512,dropped,uploads,events:[event],routes:[route],errors:[]},errors:[],gaps:[],unassociatedStarts:[],dropped:0};
  const expected={epoch:1,rateHz:60,startNativeSequence:46,expectedEvents:1,logicalIntervals:[{events:[0],routes:[{logicalIndex:0,direct:true,fallback:false,fallbackStarted:false,pending:false}],uploads:0,errors:0}]};
  return {raw,expected};
}
function verdict(mutate=()=>{}){const {raw,expected}=fixture();mutate(raw,expected);return assessPresentationAudioOutput(seal(raw),expected);}

test('installer validates the actual Runtime realm Map without accepting a parent-realm replacement',async()=>{
  const host=vm.runInNewContext('({Map, voices:new Map()})');
  host.__th08SfxLatencyProbeEnabled=true;host.__th08SfxLatencyTrace={uploads:[]};host.AudioWorkletNode=function(){};
  const context={state:'running',audioWorklet:{}},pool={context,voices:host.voices};
  const runtime={Module:{canvas:{ownerDocument:{defaultView:host}},eaglerSfxOutput:pool,SDL3:{audioContext:context}}};
  assert.equal(pool.voices instanceof Map,false);
  await assert.rejects(installPresentationAudioProbe(runtime,{epoch:1,rateHz:60}),/retain all actual eager SFX uploads/);
  pool.voices=new Map();
  await assert.rejects(installPresentationAudioProbe(runtime,{epoch:1,rateHz:60}),/Actual WebSfx AudioContext and voice pool/);
});

test('synthetic classifier fixture binds native play, actual source, gain and both PCM paths',()=>{
  const result=verdict();assert.equal(result.status,'passed',result.reason);assert.equal(result.comparedPcmFrames,128);
});
test('fixed AudioHost pan/volume controls retain four gain calls per channel, rejecting missing and extra calls',()=>{
  for(const count of [2,3,5]){
    const result=verdict(raw=>{const template=raw.gainCalls[0];
      raw.gainCalls=Array.from({length:count*2},(_,index)=>({...template,sequence:index+1,side:index%2?'right':'left'}));
      raw.starts[0].gainSequence=raw.gainCalls.length;
    });
    assert.equal(result.status,'failed');assert.match(result.reason,/four production AudioHost\/WebSfx gain calls/);
  }
});
test('scheduled source and native route without real PCM cannot pass',()=>{
  assert.equal(verdict(raw=>{raw.pcm=[];}).status,'failed');
  assert.equal(verdict(raw=>{raw.sourcePcm=[];}).status,'failed');
  assert.equal(verdict(raw=>{raw.starts=[];}).status,'failed');
});
test('actual silence, disconnected output and wrong gain fail while source remains nonzero',()=>{
  for(const mode of ['silence','disconnect','gain']){
    const result=verdict(raw=>{for(const block of raw.pcm[0].blocks){if(mode==='disconnect')block.inputChannels=0;
      for(const side of ['leftWords','rightWords']){const words=Uint32Array.from(block[side]),samples=new Float32Array(words.buffer);
        for(let i=0;i<samples.length;i++)samples[i]=mode==='gain'?samples[i]*.25:0;block[side]=Array.from(words);}}});
    assert.equal(result.status,'failed',mode);
  }
});
test('raw SHA and semantic checks both reject tampered PCM',()=>{
  const {raw,expected}=fixture(),output=seal(raw);raw.pcm[0].blocks[0].leftWords[16]++;
  assert.match(assessPresentationAudioOutput(output,expected).reason,/hash/);
  assert.equal(assessPresentationAudioOutput(seal(raw),expected).status,'failed');
});
test('native events, route, uploaded key, actual source and interval order are independent obligations',()=>{
  for(const mutate of [raw=>{raw.hookEvents[0].logicalIndex=4;},raw=>{raw.native.events[0].handle=2;},
    raw=>{raw.native.routes[0].direct=false;},raw=>{raw.starts[0].bufferSame=false;},raw=>{raw.starts[0].sourceTap=5;},
    raw=>{raw.intervals[0].events=[];},raw=>{raw.gainCalls.splice(0,1);},raw=>{raw.hookRoutes=[];}])
    assert.equal(verdict(mutate).status,'failed');
});
test('actual dropped/error/gap metadata cannot be hidden by nonzero PCM',()=>{
  for(const mutate of [raw=>{raw.dropped=1;},raw=>{raw.native.dropped.events=1;},raw=>{raw.errors.push({message:'clock'});},
    raw=>{raw.gaps.push({eventSerial:47,reason:'missing'});},raw=>{raw.native.errors.push({message:'callback'});}])
    assert.equal(verdict(mutate).status,'failed');
});
test('real worklet marker and final error acknowledgements are mandatory',()=>{
  for(const mutate of [raw=>{raw.markReceipts.pop();},raw=>{raw.taps[0].configuredAck=null;},raw=>{raw.taps[0].final=null;},
    raw=>{raw.taps[1].final.failures=1;},raw=>{raw.taps[0].final.pending=1;}])assert.equal(verdict(mutate).status,'failed');
});
test('same PCM with another epoch, rate or finite clock horizon cannot pass',()=>{
  assert.equal(verdict((raw,expected)=>{expected.epoch++;}).status,'failed');
  assert.equal(verdict((raw,expected)=>{expected.rateHz=144;}).status,'failed');
  assert.equal(verdict(raw=>{raw.window.finishedAtTime=raw.clock.endTime;}).status,'failed');
});
test('retained onset cannot skip a nonzero prefix or invent leading silence',()=>{
  assert.equal(verdict(raw=>{raw.sourcePcm[0].onsetFrame++;}).status,'failed');
  assert.equal(verdict(raw=>{raw.sourcePcm[0].blocks[0].leftWords[0]=0x3e800000;}).status,'failed');
});
test('missing, duplicated and reordered raw PCM blocks remain failures',()=>{
  for(const mutate of [raw=>{raw.sourcePcm[0].blocks.pop();},raw=>{raw.pcm[0].blocks.reverse();},
    raw=>{raw.pcm[0].blocks[1].processSequence++;},raw=>{raw.sourcePcm[0].blocks[1].frame++;}])assert.equal(verdict(mutate).status,'failed');
});
test('segmented local frame clock stays exact after 804 seconds and keeps original segment array lengths',()=>{
  for(const rate of [44100,48000]){const plan=presentationAudioFrameClockPlan({sampleRate:rate,anchorTime:3});
    for(const elapsed of [0,255,257,804,837,7199]){const frame=plan.anchorFrame+elapsed*rate;
      const block=clockBlock(plan,frame,128,{segmentLength:128});assert.equal(decodePresentationAudioClockBlock(block,plan).firstFrame,frame);
      assert.ok(Math.max(...block.local)<2**24);}
    const frame=plan.anchorFrame+plan.segmentFrames-60,block=clockBlock(plan,frame,128,{segmentLength:128});
    assert.equal(decodePresentationAudioClockBlock(block,plan).endFrame,frame+128);
    assert.throws(()=>decodePresentationAudioClockBlock({...block,segment:[block.segment[0]]},plan),/Noncontinuous/);
  }
});
test('one-sample anchor drift, missing step, missing reset and float32 precision loss are rejected exactly',()=>{
  const plan=presentationAudioFrameClockPlan({sampleRate:44100,anchorTime:3}),frame=plan.anchorFrame+plan.segmentFrames-64;
  for(const mutation of [b=>{b.segment[64]=0;},b=>{b.segment[63]=1;},b=>{b.local[64]+=plan.segmentFrames;},
    b=>{b.local[65]=b.local[64];},b=>{b.local[80]=2**24;},b=>{b.local=[];}]){
    const block=clockBlock(plan,frame,128,{segmentLength:128});mutation(block);assert.throws(()=>decodePresentationAudioClockBlock(block,plan));
  }
});
test('same-time command order is ramp then local reset; segment remains a separate real parameter',()=>{
  const plan=presentationAudioFrameClockPlan({sampleRate:48000,anchorTime:3});
  assert.deepEqual(plan.parameters.localFrame.slice(0,4),[['setValueAtTime',1048576,3],['linearRampToValueAtTime',2584576,35],
    ['setValueAtTime',1048576,35],['linearRampToValueAtTime',2584576,67]]);
  assert.deepEqual(plan.parameters.segment.slice(0,2),[['setValueAtTime',0,3],['setValueAtTime',1,35]]);
  assert.equal(plan.endTime-plan.anchorTime,7200);
});
function processorFixture(){let Processor;const messages=[],context={currentFrame:0,Float32Array,Uint32Array,Number,Array,
  AudioWorkletProcessor:class{constructor(){this.port={postMessage:message=>messages.push(message)};}},registerProcessor:(_name,value)=>{Processor=value;}};
  vm.runInNewContext(presentationAudioPcmProcessorSource(),context);const processor=new Processor();return {processor,messages,context};}
function render(fixture,plan,frame,{segmentLength=1,samples=.25}={}){const block=clockBlock(plan,frame,128,{segmentLength});fixture.context.currentFrame=frame;
  return fixture.processor.process([[new Float32Array(128).fill(samples),new Float32Array(128).fill(samples)]],
    [[new Float32Array(128),new Float32Array(128)]],{localFrame:Float32Array.from(block.local),segment:Float32Array.from(block.segment)});}
test('the real processor source preserves arrays and silent branch while capturing actual input',()=>{
  const f=processorFixture(),plan=presentationAudioFrameClockPlan({sampleRate:48000,anchorTime:2}),frame=plan.anchorFrame;
  f.processor.port.onmessage({data:{kind:'configure',plan,historyFrames:96000}});
  f.processor.port.onmessage({data:{kind:'mark',mode:'onset',eventSerial:47,startFrame:frame,prefixFrames:128,budgetFrames:48000}});
  render(f,plan,frame);assert.equal(f.messages.filter(row=>row.kind==='pcm').length,1);
  const pcm=f.messages.find(row=>row.kind==='pcm');assert.equal(pcm.blocks[0].segment.length,1);assert.equal(pcm.blocks[0].local.length,128);
  assert.equal(pcm.blocks[0].leftWords[0],0x3e800000);
});
test('the real processor fails a frame gap and does not reset continuity for a new marker',()=>{
  const f=processorFixture(),plan=presentationAudioFrameClockPlan({sampleRate:48000,anchorTime:2}),frame=plan.anchorFrame;
  f.processor.port.onmessage({data:{kind:'configure',plan,historyFrames:96000}});render(f,plan,frame);
  f.processor.port.onmessage({data:{kind:'mark',mode:'onset',eventSerial:48,startFrame:frame+256,prefixFrames:128,budgetFrames:48000}});
  render(f,plan,frame+256);assert.ok(f.messages.some(row=>row.reason==='noncontinuous-clock-blocks'));assert.ok(!f.messages.some(row=>row.kind==='pcm'));
});
test('worklet currentFrame is retained without replacing the independent per-sample clock',()=>{
  const f=processorFixture(),plan=presentationAudioFrameClockPlan({sampleRate:48000,anchorTime:2}),frame=plan.anchorFrame;
  f.processor.port.onmessage({data:{kind:'configure',plan,historyFrames:96000}});render(f,plan,frame);
  const block=clockBlock(plan,frame+128);f.context.currentFrame=frame;
  f.processor.process([[new Float32Array(128),new Float32Array(128)]],[[new Float32Array(128),new Float32Array(128)]],
    {localFrame:Float32Array.from(block.local),segment:Float32Array.from(block.segment)});
  assert.ok(!f.messages.some(row=>row.kind==='error'));assert.equal(f.processor.history.at(-1).currentFrame,frame);
  assert.equal(f.processor.history.at(-1).frame,frame+128);
});
test('bounded equality keeps strict deep semantics without constructing a giant diagnostic diff',()=>{
  const left=Array.from({length:20000},(_,i)=>({i,value:'pcm'})),right=clone(left);right.at(-1).value='changed';
  assert.throws(()=>assertPresentationAudioEqual(left,right,'native rows'),error=>error.message==='native rows: retained values differ'&&error.message.length<100);
  assertPresentationAudioEqual(left,clone(left),'same');
});
test('gain transitions must follow the exact observed timing bracket and cannot reverse or drift',()=>{
  const source=Float32Array.from([1,1,1,1]),call={value:.5,time:2/48000,beforeTime:2/48000,afterTime:2/48000,hadActiveSource:true,controlStoppedActive:true};
  assertPresentationGainPcm({source,output:Float32Array.from([1,1,.5,.5]),startFrame:0,sampleRate:48000,baseline:1,calls:[call],firstPullFrame:0});
  for(const output of [[1,1,1,1],[.5,.5,1,1],[1,.5,.5,.5],[1,1,.5,.49]])assert.throws(()=>assertPresentationGainPcm({source,
    output:Float32Array.from(output),startFrame:0,sampleRate:48000,baseline:1,calls:[call],firstPullFrame:0}));
});
test('an idle first-pull clamp is finite and is forbidden for a previously active production voice',()=>{
  const source=new Float32Array(8).fill(1),output=new Float32Array(8).fill(.5),call={value:.5,time:0,beforeTime:0,afterTime:0,hadActiveSource:false,controlStoppedActive:false};
  assertPresentationGainPcm({source,output,startFrame:128,sampleRate:48000,baseline:1,calls:[call],firstPullFrame:128});
  const delayed=Float32Array.from([1,1,.5,.5,.5,.5,.5,.5]);
  assert.throws(()=>assertPresentationGainPcm({source,output:delayed,startFrame:128,sampleRate:48000,baseline:1,calls:[call],firstPullFrame:128}));
});
test('capture beyond clock horizon is a real failure even if old errors were removed',()=>{
  assert.equal(verdict(raw=>{raw.window.finishedAtTime=raw.clock.endTime+1;raw.errors=[];raw.gaps=[];}).status,'failed');
});
