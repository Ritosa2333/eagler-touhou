import test from 'node:test';
import assert from 'node:assert/strict';
import {runOriginalMenuInputScene} from './original-menu-input-scenes.mjs';
import {TH10_MENU_CONTRACT} from '../../tests/adaptation/adapters/titles/th10-menu.mjs';
import {assertMenuStartupEvidence} from '../../tests/adaptation/adapters/menu-startup.mjs';
import {readOriginalMenuState} from '../../tests/adaptation/adapters/menu.mjs';

const masks={confirm:1,up:16,down:32,cancel:8};
function options(ingress){return{ingress,expected:{startScreen:0,startCursor:0,difficultyScreen:4,readyPhase:1,startPhase:1,difficultyCount:4},inputMasks:masks,
  geometry:{pointerId:51,cancelPointerId:52,start:{x:.5,y:.5},swipeFrom:{x:.5,y:.3},swipeTo:{x:.5,y:.4},cancelPoint:{x:.4,y:.5},cancelSecondPoint:{x:.6,y:.5},
    width:640,height:480,swipeThresholdPx:24,source:'TouchController::pointer and sample'},
  budgets:{readyTicks:4,transitionTicks:8,inputReleaseTicks:1,source:'TitleMenus::OnUpdateStartMenu and OnUpdateDifficultySelect'},
  sources:{titleState:'BrowserRuntime::status fields 8/9/11/25/26/27/28',navigation:'TitleMenus::MoveCursorVertical',cancel:'TitleMenus::OnUpdateDifficultySelect'}};}

function makeIO({ignoreDown=false,ignoreUp=false,duplicateDown=false,wrongConfirmTransition=false,wrongCancel=false,badTouchOwner=false}={}){
  const state={runtime:{generation:1,pointer:0x1234,epoch:2},scene:'title',title:{screen:0,cursor:0,phase:1,stateTimer2:12,confirmationReady:true,input:{current:0,previous:0}}};
  const keys=new Map(),fingers=new Map();let pending=0,timer=0,changing=false,touchMs=10;
  const direction=()=>{
    for(const [id,p]of fingers){if(p.move){const dx=(p.move.x-p.start.x)*640,dy=(p.move.y-p.start.y)*480;if(Math.abs(dx)>24||Math.abs(dy)>24)return Math.abs(dy)>Math.abs(dx)?dy>0?masks.down:masks.up:0;}}
    return 0;
  };
  const keyMask=action=>({confirm:masks.confirm,up:masks.up,down:masks.down,cancel:masks.cancel})[action]??0;
  async function step(){
    state.title.input.previous=state.title.input.current;
    let current=0;for(const [action,down]of keys)if(down)current|=keyMask(action);
    current|=pending;pending=0;
    const swipe=direction();if(swipe)current|=swipe;
    state.title.input.current=current;
    const pressed=mask=>(current&mask)&&!(state.title.input.previous&mask);
    if(state.title.screen===0&&pressed(masks.confirm)){
      state.title.screen=wrongConfirmTransition?7:4;state.title.phase=0;state.title.stateTimer2=0;state.title.confirmationReady=false;state.title.cursor=0;timer=0;
    }else if(state.title.screen===4&&state.title.phase===1){
      if(pressed(masks.down)){state.title.cursor=(state.title.cursor+1)%4;if(duplicateDown)state.title.cursor=(state.title.cursor+1)%4;}
      if(pressed(masks.up))state.title.cursor=(state.title.cursor+3)%4;
      if(pressed(masks.cancel)){if(wrongCancel)state.title.screen=7;else{state.title.phase=3;state.title.confirmationReady=false;changing=true;timer=0;}}
    }
    if(changing){timer++;state.title.stateTimer2=timer;if(timer>=2){changing=false;state.title.screen=0;state.title.phase=1;state.title.stateTimer2=12;state.title.confirmationReady=true;state.title.cursor=0;}}
    else if(state.title.screen===4&&!state.title.confirmationReady){timer++;state.title.stateTimer2=timer;if(timer>=2){state.title.phase=1;state.title.confirmationReady=true;}}
  }
  return{
    async readTitle(){return structuredClone(state);},
    async send(op){
      if(op.source==='touch'){
        if(badTouchOwner)return{available:true,acknowledged:true,source:'touch',type:op.type,id:op.id+1,startedAtMs:1,completedAtMs:2};
        const now=touchMs++;
        if(op.type==='down')fingers.set(op.id,{start:{x:op.x,y:op.y},move:null});
        else if(op.type==='move'){const p=fingers.get(op.id);if(p)p.move={x:op.x,y:op.y};}
        else if(op.type==='up'){
          const p=fingers.get(op.id);if(p){
            if(fingers.size>=2&&state.title.screen===4)pending|=masks.cancel;
            else if(!p.move&&state.title.screen===0)pending|=masks.confirm;
            fingers.delete(op.id);
          }
        }
        return{available:true,acknowledged:true,source:'touch',type:op.type,id:op.id,startedAtMs:now,completedAtMs:now+1};
      }
      if(!['keyboard','gamepad'].includes(op.source))return{available:false,reason:'wrong source'};
      if(op.down){if(!((ignoreDown&&op.action==='down')||(ignoreUp&&op.action==='up')))keys.set(op.action,true);}
      else{keys.delete(op.action);}
      return{available:true,acknowledged:true,source:op.source,action:op.action,down:op.down};
    },
    async step(){await step();},
  };
}

for(const ingress of ['keyboard','gamepad','touch'])test(`${ingress} drives a real title navigation, one difficulty step and native return`,async()=>{
  const result=await runOriginalMenuInputScene(makeIO(),options(ingress));
  assert.equal(result.checkStatus,'passed');
  assert.deepEqual(result.evidence.sources,options(ingress).sources);
  assert.ok(Number.isSafeInteger(result.evidence.totalBudgetTicks)&&result.evidence.totalBudgetTicks<=256);
  assert.equal(result.evidence.difficultyCursorBefore,0);
  assert.equal(result.evidence.difficultyCursorAfter,1);
  assert.equal(result.evidence.observations.some(row=>row.label==='single-menu-up-consumer'),ingress!=='touch');
  assert.equal(result.evidence.observations.some(row=>row.label==='single-menu-swipe-held-consumer'),ingress==='touch');
  assert.equal(result.evidence.observations.at(-1).value.title.screen,0);
});

test('an acknowledged swipe that does not reach the native title consumer fails',async()=>{
  await assert.rejects(runOriginalMenuInputScene(makeIO({ignoreDown:true}),options('keyboard')),
    /native title consumer did not receive/);
});

test('a native readiness wait retains distinct samples and a numeric interaction budget',async()=>{
  const io=makeIO(),readTitle=io.readTitle;let reads=0;
  io.readTitle=async()=>{const value=await readTitle();if(reads++<3)value.title.confirmationReady=false;return value;};
  const result=await runOriginalMenuInputScene(io,options('keyboard'));
  assert.equal(result.checkStatus,'passed');
  const labels=result.evidence.observations.map(row=>row.label);
  assert.equal(new Set(labels).size,labels.length);
  assert.ok(labels.includes('wait-start-menu-confirmation-ready-2'));
  assert.ok(Number.isSafeInteger(result.evidence.totalBudgetTicks));
});

test('a keyboard up edge with no native cursor change fails the directional baseline',async()=>{
  await assert.rejects(runOriginalMenuInputScene(makeIO({ignoreUp:true}),options('keyboard')),
    /native title consumer did not receive/);
});

test('a repeated cursor movement from one held menu input fails the one-step assertion',async()=>{
  await assert.rejects(runOriginalMenuInputScene(makeIO({duplicateDown:true}),options('keyboard')),/exactly one difficulty choice/);
});

test('a mismatched touch pointer owner is rejected even when the event transport responds',async()=>{
  const result=await runOriginalMenuInputScene(makeIO({badTouchOwner:true}),options('touch'));
  assert.equal(result.checkStatus,'incomplete');
  assert.match(result.reason,/touch transport receipt is missing or malformed/);
});

test('a confirm that opens an unrelated screen cannot be rescued by later observations',async()=>{
  await assert.rejects(runOriginalMenuInputScene(makeIO({wrongConfirmTransition:true}),options('keyboard')),/did not enter the declared difficulty menu/);
});

test('a cancel that leaves the title flow cannot be reported as a successful return',async()=>{
  await assert.rejects(runOriginalMenuInputScene(makeIO({wrongCancel:true}),options('keyboard')),/did not return to the title start menu/);
});

function startupIO({wrongScene=false,changedIdentity=false,stuck=false,shortWait=false}={}){
  const base=makeIO();let phase='loading',steps=0,waited=false,pressed=false,released=false,transition=0;
  const runtime={generation:1,pointer:0x1234,epoch:2};
  const loading={screen:-2,cursor:0,phase:0,stateTimer2:0,confirmationReady:false,startupElapsed:2,input:{current:0,previous:0}};
  let title=structuredClone(loading);
  return{
    async readTitle(){
      if(phase==='main'){
        const value=await base.readTitle();value.available=true;value.nativeScene=4;value.title.screen=value.title.screen===0?2:value.title.screen===4?6:value.title.screen;
        value.title.phase=value.title.phase===1?2:value.title.phase;value.title.startupElapsed=-1;
        for(const name of ['current','previous'])if(value.title.input[name]&8)value.title.input[name]=(value.title.input[name]&~8)|2;return value;
      }
      return{available:true,runtime:{...runtime,...(changedIdentity&&steps>1?{epoch:3}:{})},nativeScene:wrongScene?7:4,scene:wrongScene?'other':'title',title:structuredClone(title)};
    },
    async waitPhysical(requestedMs){waited=true;return{requestedMs,startedAtMs:100,completedAtMs:100+(shortWait?20:requestedMs)};},
    async send(op){
      if(phase==='main')return base.send(op);
      if(op.action==='confirm'){if(op.down)pressed=true;else released=true;}
      return{acknowledged:true,source:op.source,action:op.action,down:op.down};
    },
    async step(){
      steps++;
      if(phase==='main')return base.step();
      if(stuck)return;
      if(phase==='loading'&&waited){phase='prompt';title={...title,screen:1,phase:2,stateTimer2:11,confirmationReady:true};return;}
      if(phase==='prompt'&&pressed){phase='transition';title={...title,phase:4,stateTimer2:1,confirmationReady:false,input:{current:1,previous:0}};return;}
      if(phase==='transition'){
        title.input={current:released?0:1,previous:title.input.current};title.stateTimer2++;
        if(++transition>=3){phase='main';}
      }
    },
  };
}
test('TH10 startup retains the real loading draw, physical wait and fresh prompt edge before menu navigation',async()=>{
  const result=await runOriginalMenuInputScene(startupIO(),{...TH10_MENU_CONTRACT,ingress:'keyboard'});
  assert.equal(result.checkStatus,'passed');
  const e=result.evidence.startup;
  assert.equal(e.physicalWaits.length,1);assert.equal(e.observations[0].value.title.screen,-2);
  assert.ok(e.observations.some(row=>row.label==='startup-confirm-consumer'));
  assert.equal(assertMenuStartupEvidence(e,TH10_MENU_CONTRACT.startup,TH10_MENU_CONTRACT.expected,
    {generation:1,pointer:0x1234,epoch:2},1),e);
  const bad=structuredClone(e);bad.observations.find(row=>row.label==='startup-confirm-consumer').value.title.input.previous=1;
  assert.throws(()=>assertMenuStartupEvidence(bad,TH10_MENU_CONTRACT.startup,TH10_MENU_CONTRACT.expected,{generation:1,pointer:0x1234,epoch:2},1),/fresh native edge/);
});
test('TH10 startup refuses wrong screen, epoch, fake physical wait and exhausted native tick budget',async()=>{
  for(const [patch,pattern]of [[{wrongScene:true},/undeclared native/],[{changedIdentity:true},/Runtime identity/],[{shortWait:true},/completed physical wait/]])
    await assert.rejects(runOriginalMenuInputScene(startupIO(patch),{...TH10_MENU_CONTRACT,ingress:'keyboard'}),pattern);
  const result=await runOriginalMenuInputScene(startupIO({stuck:true}),{...TH10_MENU_CONTRACT,ingress:'keyboard'});
  assert.equal(result.checkStatus,'incomplete');assert.match(result.reason,/tick budget/);
  assert.equal(result.evidence.startup.ticks,96);
});
test('TH10 reader preserves actual signed startup state while unsigned native input remains strict',()=>{
  const previous=globalThis.__th10Runtime;
  try{
    const buffer=new ArrayBuffer(128);new Uint32Array(buffer,16,8).set([0x54493130,1,8,1,0,2,4096,3]);
    let input=0;globalThis.__th10Runtime={core:{memory:{buffer},th10_runtime_identity_v1:()=>16,
      th10_menu_state_v1:index=>[-2,-2,0,0,-1,0,input,0,2][index]}};
    const contract={...TH10_MENU_CONTRACT,runtime:{global:'__th10Runtime',core:'core',module:'Module'}};
    const value=readOriginalMenuState({contract});assert.equal(value.nativeScene,-2);assert.equal(value.title.screen,-2);assert.equal(value.title.stateTimer2,-1);
    input=-1;assert.throws(()=>readOriginalMenuState({contract}),/currentInput/);
  }finally{globalThis.__th10Runtime=previous;}
});
