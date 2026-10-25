import assert from 'node:assert/strict';
import test from 'node:test';
import {adaptationTitleModes,resolveTitleFlow} from './title-flow.mjs';
import {PRODUCT_GAMES} from '../../../common/contracts/product-catalog.mjs';

test('normal Runtime flows are explicit for every current game and never fall back from multiplayer',()=>{
  for(const game of Object.keys(adaptationTitleModes)){
    assert.equal(resolveTitleFlow({game,variant:'normal'}).game,game);
    const hasMultiplayer=Boolean(PRODUCT_GAMES[game].multiplayerRuntime&&PRODUCT_GAMES[game].multiplayer);
    assert.equal(adaptationTitleModes[game].multiplayer,hasMultiplayer?'room-owned':null);
    assert.throws(()=>resolveTitleFlow({game,variant:'multiplayer'}),new RegExp(hasMultiplayer?'room acceptance driver':'not a registered product Runtime variant'));
  }
  assert.throws(()=>resolveTitleFlow({game:'th08'}),/explicit variant/);
  assert.throws(()=>resolveTitleFlow({game:'th99',variant:'normal'}),/No adaptation flow/);
});

test('shared normal entry waits on native readiness and bounds confirmation input',async()=>{
  const flow=resolveTitleFlow({game:'th11',variant:'normal'}),pressed=[];
  let state={capabilities:{input:false,position:false,phase:false}};
  const result=await flow.enterGameplay({lane:'runtime',observe:async()=>state,press:async code=>{
    pressed.push(code);
    if(pressed.length===2)state={capabilities:{input:true,position:true,phase:true},input:{value:0},position:{x:2,y:3},
      phase:{name:'stage'},pause:{active:false},nativeObservation:{version:1},localSeat:0,frames:{simulation:1}};
  }});
  assert.equal(pressed.length,2);
  assert.deepEqual(pressed,['KeyZ','KeyZ']);
  assert.equal(flow.isGameplayReady(result,'runtime'),true);
  assert.equal(flow.needsControlWarmup,false);
});

test('th08 confirms only its three native menu gates and waits through gameplay loading',async()=>{
  const flow=resolveTitleFlow({game:'th08',variant:'normal'}),pressed=[];
  let state={status:{supervisorActive:1,active:1,loading:0,titleScreen:0,titlePhase:1,titleInputReady:true,titleStateTimer2:10,cursor:0}};
  const result=await flow.enterGameplay({observe:async()=>state,press:async code=>{
    pressed.push(code);assert.equal(code,'KeyZ');
    if(state.status.titleScreen===0)state.status={...state.status,titleScreen:4,titlePhase:0,titleInputReady:false};
    else if(state.status.titleScreen===4)state.status={...state.status,titleScreen:5,titlePhase:0,titleInputReady:false,titleStateTimer2:0};
    else {assert.equal(state.status.titleScreen,5);assert.equal(state.status.titleStateTimer2,8);state.status={...state.status,supervisorActive:2,loading:1};}
  },waitUntil:async(_label,predicate)=>{
    if(state.status.supervisorActive===2)state={...state,status:{...state.status,loading:0},capabilities:{input:true,position:true,phase:true},
      input:{value:0},position:{x:192,y:384},phase:{name:'stage'},pause:{active:false}};
    else if(state.status.titleScreen===4)state.status={...state.status,titlePhase:1,titleInputReady:true};
    else if(state.status.titleScreen===5)state.status={...state.status,titlePhase:1,titleStateTimer2:8};
    assert.equal(predicate(state),true);return structuredClone(state);
  }});
  assert.deepEqual(pressed,['KeyZ','KeyZ','KeyZ']);assert.equal(flow.isGameplayReady(result),true);
});

test('TH20 entry selects the ordinary route and accepts only the observed default stone path',async()=>{
  const flow=resolveTitleFlow({game:'th20',variant:'normal'}),calls=[];
  let state={status:{titleState:1,titlePhase:2,titleCursor:1}};
  const waitUntil=async(label,predicate)=>{
    calls.push(label);
    if(label.includes('difficulty'))state={status:{titleState:5,titlePhase:2}};
    else if(label.includes('character'))state={status:{titleState:6,titlePhase:2}};
    else if(label.includes('stone'))state={status:{titleState:7,titlePhase:2},ui:{stone:{state:3,category:0}}};
    else if(label.includes('loadout'))state={status:{titleState:7,titlePhase:3},capabilities:{input:true,position:true,phase:true},
      input:{value:0},position:{x:0,y:0},phase:{name:'stage'},pause:{active:false}};
    return predicate(state)?state:assert.fail(`unexpected state for ${label}`);
  };
  const result=await flow.enterGameplay({lane:'runtime',observe:async()=>state,waitUntil,press:async code=>{
    calls.push(code);
    if(code==='ArrowUp')state={status:{titleState:1,titlePhase:2,titleCursor:0}};
  }});
  assert.equal(result.status.titlePhase,3);
  assert.equal(calls.filter(item=>item==='KeyZ').length,4);
  assert.ok(calls.some(item=>item==='TH20 difficulty input ready'));
});

test('TH20 restart confirms yes only after native retry menu appears',async()=>{
  const flow=resolveTitleFlow({game:'th20',variant:'normal'}),calls=[];
  const confirmation=await flow.restartFromPause({press:async code=>calls.push(code),waitUntil:async(label,predicate)=>{
    calls.push(label);
    const state=label.includes('ready')?{ui:{pause:{retryConfirmInputReady:true,cursor:1}}}:
      {ui:{pause:{retryConfirmInputReady:true,cursor:0}}};
    assert.equal(predicate(state),true);return state;
  }});
  assert.equal(confirmation.ui.pause.cursor,1);
  assert.deepEqual(calls,['KeyR','TH20 retry confirmation input ready','ArrowUp','TH20 retry confirmation selects yes','KeyZ']);
});
