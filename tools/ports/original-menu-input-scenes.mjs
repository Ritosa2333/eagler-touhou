import assert from 'node:assert/strict';
import {assertMenuStartupObservation,assertMenuStartupEvidence} from '../../tests/adaptation/adapters/menu-startup.mjs';

const SCHEMA='eagler-touhou/original-menu-input-scenes/1';
const MAX_TICKS=256;

class SceneIncomplete extends Error{}
function incomplete(reason,evidence){return{checkStatus:'incomplete',reason,evidence};}
function timeout(error){return error?.name==='TimeoutError'||error?.code==='ETIMEDOUT'||error?.timeout===true;}
function validMask(value){return Number.isSafeInteger(value)&&value>0&&value<=0x80000000&&Number.isInteger(Math.log2(value));}
function normalizedPoint(value,label){
  assert.ok(Number.isFinite(value?.x)&&value.x>=0&&value.x<=1&&Number.isFinite(value?.y)&&value.y>=0&&value.y<=1,
    `${label} must be a normalized Runtime-canvas point`);
  return{x:value.x,y:value.y};
}
function validate(options={}){
  assert.ok(['keyboard','gamepad','touch'].includes(options.ingress),`Unsupported original-menu input ingress ${String(options.ingress)}`);
  const expected=options.expected??{};
  for(const field of ['startScreen','startCursor','difficultyScreen','readyPhase','startPhase','difficultyCount'])
    assert.ok(Number.isSafeInteger(expected[field])&&expected[field]>=0,`Menu adapter must declare ${field}`);
  const masks=options.inputMasks??{};
  for(const name of ['confirm','up','down','cancel'])assert.ok(validMask(masks[name]),`Menu adapter must declare the single-bit ${name} mask`);
  assert.equal(new Set(['confirm','up','down','cancel'].map(name=>masks[name])).size,4,'Menu input masks must be distinct');
  const budgets=options.budgets??{};
  for(const name of ['readyTicks','transitionTicks','inputReleaseTicks'])
    assert.ok(Number.isSafeInteger(budgets[name])&&budgets[name]>0&&budgets[name]<=MAX_TICKS,`Menu scene needs a bounded ${name}`);
  assert.ok(typeof budgets.source==='string'&&budgets.source.length>0,'Menu tick budget needs its title source anchor');
  const swipeHold=options.ingress==='touch'?(options.geometry?.swipeHoldTicks??1):1;
  if(options.startup){
    assert.equal(options.startup.schema,'eagler-touhou/title-startup-navigation/1');
    assert.ok(Number.isInteger(options.startup.ticks)&&options.startup.ticks>0&&options.startup.ticks<=MAX_TICKS);
    assert.ok(Number.isFinite(options.startup.physicalWaitMs)&&options.startup.physicalWaitMs>0&&options.startup.physicalWaitMs<=5000);
  }
  assert.ok(Number.isSafeInteger(swipeHold)&&swipeHold>0&&swipeHold<=64,'Menu swipe hold must have a bounded native tick extent');
  const total=budgets.readyTicks*2+budgets.transitionTicks*2+budgets.inputReleaseTicks*4+10+swipeHold;
  assert.ok(total<=MAX_TICKS,`Original menu scene exceeds its ${MAX_TICKS}-tick bound`);
  assert.ok(typeof options.sources?.titleState==='string'&&options.sources.titleState.length>0,'Title-state observation needs its production source');
  assert.ok(typeof options.sources?.navigation==='string'&&options.sources.navigation.length>0,'Title navigation needs its production source');
  assert.ok(typeof options.sources?.cancel==='string'&&options.sources.cancel.length>0,'Title cancellation needs its production source');
  if(options.ingress==='touch'){
    const geometry=options.geometry??{};
    assert.ok(Number.isSafeInteger(geometry.pointerId)&&geometry.pointerId>=0,'Touch menu requires a stable pointer id');
    assert.ok(Number.isSafeInteger(geometry.cancelPointerId)&&geometry.cancelPointerId>=0&&geometry.cancelPointerId!==geometry.pointerId,
      'Touch menu cancel requires a second distinct pointer id');
    normalizedPoint(geometry.start,'Menu tap point');
    normalizedPoint(geometry.swipeFrom,'Menu swipe origin');
    normalizedPoint(geometry.swipeTo,'Menu swipe target');
    normalizedPoint(geometry.cancelPoint,'Menu cancel point');
    normalizedPoint(geometry.cancelSecondPoint,'Menu second cancel point');
    assert.ok(Number.isFinite(geometry.width)&&geometry.width>0&&Number.isFinite(geometry.height)&&geometry.height>0,
      'Touch menu needs the actual canvas dimensions');
    assert.ok(Number.isFinite(geometry.swipeThresholdPx)&&geometry.swipeThresholdPx>0,
      'Touch menu needs the title-declared swipe threshold');
    assert.ok(typeof geometry.source==='string'&&geometry.source.length>0,'Touch menu geometry needs its production source');
  }
  assert.ok(expected.difficultyCount>=2,'Menu navigation target must have at least two selectable positions');
  return{total};
}
function identityOf(value,label,expected){
  if(value?.available===false)throw new SceneIncomplete(value.reason??`${label}: title state is unavailable`);
  const runtime=value?.runtime;
  for(const key of ['generation','pointer','epoch'])
    if(!Number.isSafeInteger(runtime?.[key])||(key==='epoch'?runtime[key]<0:runtime[key]<=0))
      throw new SceneIncomplete(`${label}: Runtime ${key} identity is missing`);
  if(expected)assert.deepEqual(runtime,expected,`${label}: Runtime identity changed during menu input`);
  const title=value?.title;
  for(const key of ['screen','cursor','phase','stateTimer2'])
    if(!Number.isSafeInteger(title?.[key])||title[key]<0)throw new SceneIncomplete(`${label}: native title ${key} is unavailable`);
  if(typeof title.confirmationReady!=='boolean')throw new SceneIncomplete(`${label}: native title confirmation gate is unavailable`);
  if(!Number.isSafeInteger(title.input?.current)||!Number.isSafeInteger(title.input?.previous))
    throw new SceneIncomplete(`${label}: native title input consumer is unavailable`);
  if(value.scene!=='title')throw new SceneIncomplete(`${label}: Runtime left the title scene`);
  return runtime;
}
function assertEdge(title,mask,label){
  assert.notEqual(title.input.current&mask,0,`${label}: native title consumer did not receive the requested input`);
  assert.equal(title.input.previous&mask,0,`${label}: input was held before the requested fresh edge`);
}
/**
 * Exercise the production title menu with keyboard, mapped gamepad or touch.
 * `send` is a transport only; native title status supplies every verdict.
 * Touch swipe sampling occurs before pointer-up, matching TouchController's
 * menu gesture lifetime. A cancel touch uses the title's two-finger Escape.
 */
export async function runOriginalMenuInputScene(io,options={}){
  const evidence={schema:SCHEMA,ingress:options.ingress??null,expected:structuredClone(options.expected??null),
    inputMasks:structuredClone(options.inputMasks??null),geometry:structuredClone(options.geometry??null),
    budgets:structuredClone(options.budgets??null),sources:structuredClone(options.sources??null),events:[],observations:[],cleanup:{released:false}};
  const missing=['readTitle','send','step'].filter(name=>typeof io?.[name]!=='function');
  if(missing.length)return incomplete(`Original menu IO is missing ${missing.join(', ')}`,evidence);
  const {total}=validate(options);evidence.totalBudgetTicks=total;
  let runtime=null,result=null,failure=null,lastTouchCompletedAt=null;const held=new Map(),observationCounts=new Map();
  const call=async(label,fn)=>{try{return await fn();}catch(error){evidence.ioErrors??=[];evidence.ioErrors.push({label,name:error?.name??null,message:String(error?.message??error)});
    if(timeout(error))throw new SceneIncomplete(`${label}: native menu IO timed out`);throw error;}};
  const observe=async(label)=>{const value=await call(label,()=>io.readTitle());runtime=identityOf(value,label,runtime);
    const count=(observationCounts.get(label)??0)+1;observationCounts.set(label,count);
    evidence.observations.push({label:count===1?label:`${label}-${count}`,value:structuredClone(value)});return value.title;};
  const send=async(label,op)=>{
    const receipt=await call(label,()=>io.send({source:options.ingress,...op}));evidence.events.push({label,operation:structuredClone(op),receipt:structuredClone(receipt??null)});
    if(receipt?.available===false)throw new SceneIncomplete(receipt.reason??`${label}: input transport unavailable`);
    if(receipt?.acknowledged!==true)throw new SceneIncomplete(`${label}: input transport did not acknowledge the event`);
    if(options.ingress==='touch'){
      if(receipt.source!=='touch'||receipt.type!==op.type||receipt.id!==op.id||!Number.isFinite(receipt.startedAtMs)||
         !Number.isFinite(receipt.completedAtMs)||receipt.completedAtMs<receipt.startedAtMs)
        throw new SceneIncomplete(`${label}: touch transport receipt is missing or malformed`);
      if(lastTouchCompletedAt!==null&&receipt.startedAtMs<lastTouchCompletedAt)
        throw new SceneIncomplete(`${label}: touch transport receipts are not in submitted FIFO order`);
      lastTouchCompletedAt=receipt.completedAtMs;
    }else if(receipt.source!==options.ingress||receipt.action!==op.action||receipt.down!==op.down)
      throw new SceneIncomplete(`${label}: mapped input receipt does not match the requested ${options.ingress} action`);
    return receipt;
  };
  const stepObserve=async(label,ticks=1)=>{for(let i=0;i<ticks;i++){await call(`${label}-step`,()=>io.step(1));}return observe(label);};
  const pressMapped=async(label,action,mask)=>{
    await send(`${label}-down`,{action,down:true});held.set(`input:${action}`,{action});
    let title=await stepObserve(`${label}-consumer`);assertEdge(title,mask,label);
    await send(`${label}-up`,{action,down:false});held.delete(`input:${action}`);
    title=await stepObserve(`${label}-release`,options.budgets.inputReleaseTicks);
    assert.equal(title.input.current&mask,0,`${label}: native title input remained held after transport release`);
    return title;
  };
  const touchTap=async(label,point)=>{
    const id=options.geometry.pointerId;
    await send(`${label}-down`,{type:'down',id,x:point.x,y:point.y});held.set(`pointer:${id}`,{id,point});
    await send(`${label}-up`,{type:'up',id,x:point.x,y:point.y});held.delete(`pointer:${id}`);
    const title=await stepObserve(`${label}-consumer`);assertEdge(title,options.inputMasks.confirm,label);return title;
  };
  const touchSwipe=async(label)=>{
    const g=options.geometry,id=g.pointerId;
    await send(`${label}-down`,{type:'down',id,x:g.swipeFrom.x,y:g.swipeFrom.y});held.set(`pointer:${id}`,{id,point:g.swipeTo});
    await send(`${label}-move`,{type:'move',id,x:g.swipeTo.x,y:g.swipeTo.y});
    const dx=(g.swipeTo.x-g.swipeFrom.x)*g.width,dy=(g.swipeTo.y-g.swipeFrom.y)*g.height;
    assert.ok(Math.max(Math.abs(dx),Math.abs(dy))>g.swipeThresholdPx,`Swipe must exceed the title's ${g.swipeThresholdPx}px threshold`);
    const movedMask=Math.abs(dy)>Math.abs(dx)?(dy>0?options.inputMasks.down:options.inputMasks.up):null;
    if(!movedMask)throw new SceneIncomplete('Touch menu gesture is not a vertical one-item navigation trial');
    let title=await stepObserve(`${label}-held-consumer`);assertEdge(title,movedMask,label);
    const selectedCursor=title.cursor;
    for(let tick=1;tick<(g.swipeHoldTicks??1);tick++){
      title=await stepObserve(`${label}-held-stability-${tick}`);
      assert.equal(title.cursor,selectedCursor,'A single held swipe repeated native menu navigation');
      assert.equal(title.screen,options.expected.difficultyScreen,'A held swipe left the original menu');
    }
    await send(`${label}-up`,{type:'up',id,x:g.swipeTo.x,y:g.swipeTo.y});held.delete(`pointer:${id}`);
    title=await stepObserve(`${label}-release`,options.budgets.inputReleaseTicks);
    return{title,movedMask};
  };
  const touchCancel=async(label)=>{
    const g=options.geometry,a=g.pointerId,b=g.cancelPointerId;
    await send(`${label}-first-down`,{type:'down',id:a,x:g.cancelPoint.x,y:g.cancelPoint.y});held.set(`pointer:${a}`,{id:a,point:g.cancelPoint});
    await send(`${label}-second-down`,{type:'down',id:b,x:g.cancelSecondPoint.x,y:g.cancelSecondPoint.y});held.set(`pointer:${b}`,{id:b,point:g.cancelSecondPoint});
    await send(`${label}-first-up`,{type:'up',id:a,x:g.cancelPoint.x,y:g.cancelPoint.y});held.delete(`pointer:${a}`);
    await send(`${label}-second-up`,{type:'up',id:b,x:g.cancelSecondPoint.x,y:g.cancelSecondPoint.y});held.delete(`pointer:${b}`);
    const title=await stepObserve(`${label}-consumer`);assertEdge(title,options.inputMasks.cancel,label);return title;
  };
  const startup=async()=>{
    const contract=options.startup;if(!contract)return;
    if(typeof io.waitPhysical!=='function')throw new SceneIncomplete('Declared title startup needs a physical wait transport');
    const raw=evidence.startup={contract:structuredClone(contract),ticks:0,observations:[],physicalWaits:[]};
    let value,confirmed=false,waited=false;
    const read=async label=>{
      value=await call(label,()=>io.readTitle());
      runtime??=structuredClone(value.runtime);
      raw.observations.push({label,tick:raw.ticks,value:structuredClone(value)});
      assertMenuStartupObservation(value,contract,runtime);return value.title;
    };
    const advance=async label=>{
      if(raw.ticks>=contract.ticks)throw new SceneIncomplete('Native title startup exceeded its registered tick budget');
      await call(`${label}-step`,()=>io.step(1));raw.ticks++;return read(label);
    };
    let t=await read('startup-initial');
    while(!(value.nativeScene===contract.titleScene&&t.screen===options.expected.startScreen&&t.phase===options.expected.startPhase&&t.confirmationReady)){
      if(!confirmed)assert.equal(t.input.current,0,'Startup received input before prompt readiness');
      if(!waited&&value.nativeScene===contract.titleScene&&t.screen===contract.loadingScreen&&t.startupElapsed>=contract.minimumStartupFrames){
        t=await advance('startup-loading-boundary');
        if(t.screen!==contract.loadingScreen)continue;
        await read('startup-before-physical-wait');
        const wait=await call('startup-physical-wait',()=>io.waitPhysical(contract.physicalWaitMs));
        await read('startup-after-physical-wait');
        raw.physicalWaits.push({...structuredClone(wait),before:'startup-before-physical-wait',after:'startup-after-physical-wait'});waited=true;
      }
      if(value.nativeScene===contract.titleScene&&t.screen===contract.promptScreen&&t.phase===contract.promptReadyPhase&&t.confirmationReady){
        assert.equal(confirmed,false,'Startup returned to a ready prompt after confirmation');
        await read('startup-ready-prompt');
        if(options.ingress==='touch'){
          const {pointerId:id,start:p}=options.geometry;
          await send('startup-confirm-down',{type:'down',id,x:p.x,y:p.y});held.set(`pointer:${id}`,{id,point:p});
          await send('startup-confirm-up',{type:'up',id,x:p.x,y:p.y});held.delete(`pointer:${id}`);
        }else{
          await send('startup-confirm-down',{action:'confirm',down:true});held.set('input:confirm',{action:'confirm'});
        }
        t=await advance('startup-confirm-consumer');assertEdge(t,options.inputMasks.confirm,'startup confirm');confirmed=true;
        if(options.ingress!=='touch'){
          await send('startup-confirm-up',{action:'confirm',down:false});held.delete('input:confirm');
        }
        for(let i=0;i<options.budgets.inputReleaseTicks;i++)t=await advance(`startup-confirm-release-${i}`);
      }else t=await advance(`startup-progress-${raw.ticks+1}`);
    }
    assertMenuStartupEvidence(raw,contract,options.expected,runtime,options.inputMasks.confirm);
  };
  try{
    await startup();
    let title=await observe('title-menu-ready');
    assert.equal(title.screen,options.expected.startScreen,'Runtime did not begin on the adapter-declared title start menu');
    assert.equal(title.cursor,options.expected.startCursor,'Title start cursor differs from its declared safe entry');
    if(!title.confirmationReady){
      for(let tick=0;tick<options.budgets.readyTicks&&!title.confirmationReady;tick++)title=await stepObserve('wait-start-menu-confirmation-ready');
    }
    if(!title.confirmationReady)throw new SceneIncomplete('Title start menu did not become natively confirmable within its declared bound');
    assert.equal(title.phase,options.expected.startPhase,'Title start menu reached confirmation in an undeclared phase');

    title=options.ingress==='touch'?await touchTap('select-start-menu-entry',options.geometry.start):
      await pressMapped('select-start-menu-entry','confirm',options.inputMasks.confirm);
    if(options.ingress==='touch')assertEdge(title,options.inputMasks.confirm,'select-start-menu-entry');
    for(let tick=0;tick<options.budgets.transitionTicks&&title.screen!==options.expected.difficultyScreen;tick++){
      if(options.expected.confirmTransitionPhase!==undefined){
        assert.equal(title.screen,options.expected.startScreen,'Confirm transition left the declared start menu');
        assert.equal(title.phase,options.expected.confirmTransitionPhase,'Confirm transition changed its declared native phase');
      }
      title=await stepObserve('wait-real-difficulty-menu');
    }
    assert.equal(title.screen,options.expected.difficultyScreen,'Production title confirm did not enter the declared difficulty menu');
    for(let tick=0;tick<options.budgets.readyTicks&&!title.confirmationReady;tick++)title=await stepObserve('wait-difficulty-menu-ready');
    if(!title.confirmationReady)throw new SceneIncomplete('Difficulty menu did not expose its native confirmation gate within the declared bound');
    assert.equal(title.phase,options.expected.readyPhase,'Difficulty menu reached confirmation in an undeclared phase');
    const cursorBefore=title.cursor;evidence.difficultyCursorBefore=cursorBefore;

    let movedMask;
    if(options.ingress==='touch')({title,movedMask}=await touchSwipe('single-menu-swipe'));
    else{title=await pressMapped('single-menu-down','down',options.inputMasks.down);movedMask=options.inputMasks.down;}
    evidence.difficultyCursorAfter=title.cursor;
    assert.equal(title.screen,options.expected.difficultyScreen,'Navigation changed the title screen instead of moving its cursor');
    const expectedCursor=(cursorBefore+(movedMask===options.inputMasks.up?-1:1)+options.expected.difficultyCount)%options.expected.difficultyCount;
    assert.equal(title.cursor,expectedCursor,'One native menu-direction edge did not advance exactly one difficulty choice');
    assert.equal(title.input.current&movedMask,0,'Menu navigation input remained held after its release');
    if(options.ingress!=='touch'){
      title=await pressMapped('single-menu-up','up',options.inputMasks.up);
      const expectedAfterUp=(expectedCursor-1+options.expected.difficultyCount)%options.expected.difficultyCount;
      assert.equal(title.screen,options.expected.difficultyScreen,'Up navigation changed the title screen');
      assert.equal(title.cursor,expectedAfterUp,'One native menu-up edge did not move exactly one choice in reverse');
      assert.equal(title.cursor,cursorBefore,'One down and one up did not restore the original difficulty choice');
      assert.equal(title.input.current&options.inputMasks.up,0,'Menu-up input remained held after its release');
    }

    title=options.ingress==='touch'?await touchCancel('return-from-difficulty-menu'):
      await pressMapped('return-from-difficulty-menu','cancel',options.inputMasks.cancel);
    if(options.ingress==='touch')assertEdge(title,options.inputMasks.cancel,'return-from-difficulty-menu');
    for(let tick=0;tick<options.budgets.transitionTicks&&title.screen!==options.expected.startScreen;tick++)
      title=await stepObserve('wait-title-menu-return');
    assert.equal(title.screen,options.expected.startScreen,'Native cancel did not return to the title start menu');
    assert.equal(title.cursor,options.expected.startCursor,'Returning from difficulty did not restore the safe title entry');
    if(!title.confirmationReady){
      for(let tick=0;tick<options.budgets.readyTicks&&!title.confirmationReady;tick++)title=await stepObserve('wait-returned-start-menu-ready');
    }
    if(!title.confirmationReady)throw new SceneIncomplete('Returned title menu did not restore its native confirmation gate');
    assert.equal(title.phase,options.expected.startPhase,'Returned title menu is in an undeclared phase');
    assert.equal(title.input.current&options.inputMasks.cancel,0,'Cancel input remained held after the return transition');
    result={checkStatus:'passed',reason:null,evidence};
  }catch(error){
    if(error instanceof SceneIncomplete)result=incomplete(error.message,evidence);else failure=error;
  }finally{
    for(const [key,value] of [...held.entries()])try{
      if(value?.action)await send(`cleanup-${value.action}-release`,{action:value.action,down:false});
      else if(value?.id!==undefined)await send(`cleanup-pointer-${value.id}-release`,{type:'up',id:value.id,x:value.point.x,y:value.point.y});
      held.delete(key);
    }catch(error){evidence.cleanup.error=String(error?.message??error);}
    evidence.cleanup.heldCount=held.size;
    evidence.cleanup.released=held.size===0;
  }
  if(failure){failure.evidence=evidence;failure.originalMenuEvidence=evidence;throw failure;}
  if(held.size||evidence.cleanup.error){const error=new Error('Original menu input scene could not release every input owner');error.evidence=evidence;throw error;}
  return result;
}
