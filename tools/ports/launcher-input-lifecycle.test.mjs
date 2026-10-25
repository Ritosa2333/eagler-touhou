import test from 'node:test';
import assert from 'node:assert/strict';
import {assertLauncherInputLifecycle,readInputListeners} from './launcher-input-lifecycle.mjs';
import {nativeTabFixture} from '../../tests/adaptation/fixtures/native-tab-lifecycle.mjs';
import {summarizeRuntimePcmWindow} from './music-browser-helpers.mjs';

function fixture(){
  const listeners={host:{window:[{type:'keydown',line:1},{type:'keyup',line:2}],document:[]},runtime:{window:[],document:[]}};
  const sample=(input=0,x=0,epoch=1,frame=10)=>({owner:{protocol:'eagler-touhou/1',game:'next',epoch,app:64,active:true,timeOrigin:epoch*100,touchEnabled:true,touchFocusMode:'hold-button',url:'https://example.test/runtime?epoch='+epoch,
    held:[{code:'ArrowRight',down:Boolean(input&128)},{code:'KeyZ',down:false},{code:'ShiftLeft',down:false}]},
    observation:{input:{value:input},position:{x,y:0},render:{presentationFrame:frame}},listeners:structuredClone(listeners),
    listenerDocuments:{host:{frameId:'host',executionContextId:1},runtime:{url:'https://example.test/runtime?epoch='+epoch,frameId:'game',executionContextId:epoch+1}}});
  const identity={role:'mixer',available:true,contextIndex:0,tapIds:[0],sampleRate:48000};
  const audio=summarizeRuntimePcmWindow(Array.from({length:8},(_,i)=>({target:{...identity,state:'running',currentTime:1+i/10,stateChangeCount:0,stateChangeDrops:0,stateChanges:[]},
    captures:[{contextIndex:0,tapId:0,contextState:'running',sampleRate:48000,currentTime:1+i/10,samples:2048,nonZero:16,peakAbs:.2,rms:.1}]})),{role:'mixer',minimumWindows:8});
  const outgoing={identity:{epoch:1,timeOrigin:100},held:sample(132).owner.held,active:true,app:64,mappingAttached:true};
  return{schema:'eagler-touhou/launcher-input-lifecycle/1',touchHelp:{wasVisible:true,visibleAfter:false},autoFireSetup:{before:'true',after:'false',operation:'production-button-pointer-click'},initial:sample(),beforeBackground:sample(132,2),
    background:{nativeLifecycle:nativeTabFixture(),hidden:sample(),audio},afterBackground:sample(0,2,1,20),
    beforeExit:sample(132,4),outgoingBefore:structuredClone(outgoing),outgoingAfter:{...outgoing,held:sample().owner.held,active:false,app:0,mappingAttached:false,url:'about:blank'},
    exit:{source:'Launcher browser history back',hostBefore:{timeOrigin:1,origin:'https://example.test'},hostAfter:{timeOrigin:1,origin:'https://example.test'}},reopened:sample(0,0,2),
    singleInput:{held:sample(128,2,2),released:sample(0,2,2)},cleanupErrors:[]};
}
const check=e=>assertLauncherInputLifecycle(e,{directionMask:128,focusMask:4});
test('Launcher input lifecycle recomputes real background, output, retirement and fresh listener ownership',()=>check(fixture()));
for(const [name,mutate] of Object.entries({
  'keyboard never reached native movement':e=>e.beforeBackground.observation.position.x=0,
  'touch focus never reached native input':e=>e.beforeBackground.observation.input.value=128,
  'hidden native input remains held':e=>e.background.hidden.observation.input.value=128,
  'frame submission stalls after return':e=>e.afterBackground.observation.render.presentationFrame=10,
  'silent recovery disguised by passed flag':e=>e.background.audio.outputObserved=false,
  'PCM output source changed':e=>e.background.audio.windows[3].target.contextIndex=1,
  'outgoing key remains held':e=>e.outgoingAfter.held[0].down=true,
  'outgoing mapping remains owned':e=>e.outgoingAfter.active=true,
  'Host reloaded instead of closing through Launcher':e=>e.exit.hostAfter.timeOrigin=2,
  'Host changed origin':e=>e.exit.hostAfter.origin='https://other.test',
  'same document reused':e=>e.reopened.owner.timeOrigin=100,
  'same epoch reused':e=>e.reopened.owner.epoch=1,
  'duplicate Host keyboard listener':e=>e.reopened.listeners.host.window.push({type:'keydown',line:1}),
  'Runtime listeners read from Host':e=>e.reopened.listenerDocuments.runtime.frameId='host',
  'single key has no native effect':e=>e.singleInput.held.observation.position.x=0,
  'new native key never released':e=>e.singleInput.released.observation.input.value=128,
}))test(`Launcher lifecycle rejects ${name}`,()=>{const e=fixture();mutate(e);assert.throws(()=>check(e));});

test('in-process iframe listener reads select its default world through the parent session',async()=>{
  const page={url:()=> 'https://example.test/host'},frame={url:()=> 'https://example.test/game'},reads=[];
  const context={newCDPSession:async target=>{
    if(target===frame)throw Error('This frame does not have a separate CDP session');
    let emit;return{on:(_name,handler)=>{emit=handler;},detach:async()=>{},send:async(method,args)=>{
      if(method==='Runtime.enable'){for(const [frameId,id] of [['host',1],['game',2]])emit({context:{id,auxData:{frameId,isDefault:true}}});return{};}
      if(method==='Page.getFrameTree')return{frameTree:{frame:{id:'host',url:page.url()},childFrames:[{frame:{id:'game',url:frame.url()}}]}};
      if(method==='Runtime.evaluate'){reads.push(args);return{result:{objectId:String(args.contextId)}};}
      if(method==='DOMDebugger.getEventListeners')return{listeners:[{type:'keydown',lineNumber:Number(args.objectId)}]};
      return{};
    }};
  }};
  const result=await readInputListeners(context,page,frame);
  assert.deepEqual(reads.map(read=>read.contextId),[1,1,2,2]);assert.equal(result.listeners.host.window[0].line,1);
  assert.equal(result.listeners.runtime.window[0].line,2);assert.equal(result.documents.runtime.url,frame.url());
});
