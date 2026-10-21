import assert from 'node:assert/strict';
import test from 'node:test';
import {TITLE_FLOW_STEPS_SCHEMA,executeTitleFlowSteps,titleFlowKeyFields,validateTitleFlow} from './flow-steps.mjs';

const source='ports/th99/web/cpp/game/Title.cpp:120';
function makeFlow(overrides={}){
  return{
    schema:TITLE_FLOW_STEPS_SCHEMA,
    needsControlWarmup:false,
    budget:{timeoutMs:3000,maxSteps:4,maxPresses:3},
    predicates:{
      titleReady:{condition:{path:['status','menuReady'],op:'eq',value:true},source},
      ordinaryRoute:{condition:{path:['status','route'],op:'eq',value:'normal'},source},
    },
    enter:[
      {op:'wait',until:'titleReady',timeoutMs:1000,source},
      {op:'press-until',input:'ArrowUp',when:'titleReady',until:'ordinaryRoute',maxPresses:2,source},
      {op:'press',input:'KeyZ',when:'ordinaryRoute',source},
      {op:'wait',until:'gameplayReady',source},
    ],
    ...overrides,
  };
}

test('title flow validator accepts legacy strategies and bounded serializable step declarations',()=>{
  assert.equal(validateTitleFlow({strategy:'confirm',needsControlWarmup:true}).status,'passed');
  assert.equal(validateTitleFlow({strategy:'th20-menu',needsControlWarmup:false}).status,'passed');
  const flow=makeFlow();
  assert.deepEqual(validateTitleFlow(flow),{status:'passed',issues:[]});
  assert.deepEqual(flow.enter[1],{op:'press-until',input:'ArrowUp',when:'titleReady',until:'ordinaryRoute',maxPresses:2,source});
});

test('title flow validator rejects code, open-ended steps, unsafe paths and missing source anchors',()=>{
  const withFunction=makeFlow();
  withFunction.enter[1].callback=()=>{};
  assert.equal(validateTitleFlow(withFunction).status,'failed');

  const withUnsafePath=makeFlow();
  withUnsafePath.predicates.titleReady.condition.path=['__proto__','x'];
  assert.equal(validateTitleFlow(withUnsafePath).status,'failed');

  const withoutAnchor=makeFlow();
  delete withoutAnchor.enter[1].source;
  assert.equal(validateTitleFlow(withoutAnchor).issues.some(item=>item.path==='flow.enter[1].source'),true);

  const unbounded=makeFlow();
  unbounded.enter[1].maxPresses=99;
  assert.equal(validateTitleFlow(unbounded).status,'failed');

  const guessedEntry=makeFlow();
  guessedEntry.enter[3].until='ordinaryRoute';
  assert.equal(validateTitleFlow(guessedEntry).issues.some(item=>item.reason.includes('gameplayReady')),true);

  const missingPredicates=makeFlow({predicates:null});
  assert.equal(validateTitleFlow(missingPredicates).status,'failed');
});

test('executor follows observed predicates and uses only the supplied production IO callbacks',async()=>{
  const flow=makeFlow(),events=[];
  let state={status:{menuReady:true,route:'extra'},ready:false};
  const result=await executeTitleFlowSteps({flow,
    observe:async()=>structuredClone(state),
    press:async code=>{
      events.push(`press:${code}`);
      if(code==='ArrowUp'){state.status.route='normal';state.status.menuReady=false;}
      if(code==='KeyZ')state.ready=true;
    },
    waitUntil:async(label,predicate,timeoutMs)=>{
      events.push(`wait:${label}:${timeoutMs<=3000}`);
      assert.equal(predicate(state),true);
      return structuredClone(state);
    },
    isGameplayReady:value=>value.ready===true,
    onTitleReady:async value=>events.push(`title:${value.status.route}`),
    labelSuffix:'epoch 7',
  });
  assert.equal(result.ready,true);
  assert.deepEqual(events,[
    'wait:enter step 1 epoch 7:true',
    'title:extra',
    'press:ArrowUp',
    'press:KeyZ',
    'wait:enter step 4 epoch 7:true',
  ]);
});

test('missing observed state is incomplete and does not send speculative input',async()=>{
  const flow=makeFlow(),pressed=[];
  let waitCalls=0;
  await assert.rejects(executeTitleFlowSteps({flow,
    observe:async()=>({}),
    press:async code=>pressed.push(code),
    waitUntil:async(_label,predicate)=>{
      waitCalls++;
      return waitCalls===1?{status:{menuReady:true,route:'normal'}}:{};
    },
    isGameplayReady:value=>value.ready===true,
  }),error=>error.status==='incomplete'&&error.evidence.predicate==='titleReady');
  assert.deepEqual(pressed,[]);
});

test('bounded navigation stops at its declared limit when native state does not change',async()=>{
  const flow=makeFlow();
  flow.enter[1].maxPresses=2;
  let count=0;
  const state={status:{menuReady:true,route:'extra'},ready:false};
  await assert.rejects(executeTitleFlowSteps({flow,
    observe:async()=>structuredClone(state),
    press:async()=>count++,
    waitUntil:async(_label,predicate)=>{
      assert.equal(predicate(state),true);return structuredClone(state);
    },
    isGameplayReady:value=>value.ready===true,
  }),error=>error.status==='incomplete'&&error.evidence.maxPresses===2);
  assert.equal(count,2);
});

test('entry flow requires a real gameplay-ready observation before returning',async()=>{
  const flow=makeFlow(),state={status:{menuReady:true,route:'normal'},ready:false};
  let inputs=0;
  await assert.rejects(executeTitleFlowSteps({flow,
    observe:async()=>structuredClone(state),
    press:async()=>inputs++,
    waitUntil:async(label,predicate)=>{
      if(label.startsWith('enter step 1'))return structuredClone(state);
      throw new Error('gameplay was not observed');
    },
    isGameplayReady:value=>value.ready===true,
  }),error=>error.status==='incomplete'&&error.evidence.predicate==='gameplayReady');
  assert.equal(inputs,1);
});

test('a stalled production IO callback is bounded by the total flow deadline',async()=>{
  const flow=makeFlow({budget:{timeoutMs:40,maxSteps:4,maxPresses:3}}),inputs=[];
  flow.enter[0].timeoutMs=40;
  const state={status:{menuReady:true,route:'extra'},ready:false};
  await assert.rejects(executeTitleFlowSteps({flow,
    observe:async()=>structuredClone(state),
    press:async code=>{inputs.push(code);return new Promise(()=>{});},
    waitUntil:async(_label,predicate)=>{assert.equal(predicate(state),true);return structuredClone(state);},
    isGameplayReady:value=>value.ready===true,
  }),error=>error.status==='incomplete'&&error.evidence.pendingOperationMaySettle===true);
  assert.deepEqual(inputs,['ArrowUp']);
});

test('press-until accepts a reached target after leaving the source menu state',async()=>{
  const flow=makeFlow();
  let state={status:{menuReady:true,route:'extra'},ready:false};
  const result=await executeTitleFlowSteps({flow,
    observe:async()=>structuredClone(state),
    press:async code=>{
      if(code==='ArrowUp')state={status:{menuReady:false,route:'normal'},ready:false};
      if(code==='KeyZ')state.ready=true;
    },
    waitUntil:async(_label,predicate)=>{assert.equal(predicate(state),true);return structuredClone(state);},
    isGameplayReady:value=>value.ready===true,
  });
  assert.equal(result.ready,true);
});

test('undeclared restart is incomplete and sends no guessed R input',async()=>{
  const flow=makeFlow(),pressed=[];
  await assert.rejects(executeTitleFlowSteps({flow,phase:'restart',
    waitUntil:async()=>assert.fail('missing restart must stop before waiting'),
    press:async code=>pressed.push(code),
  }),error=>error.status==='incomplete'&&error.evidence.capability==='restart'&&
    error.evidence.gameplayInputSent===false);
  assert.deepEqual(pressed,[]);
});

test('title flow key mapping covers every accepted category and rejects unsupported codes',()=>{
  for(let codePoint=65;codePoint<=90;codePoint++){
    const letter=String.fromCharCode(codePoint),code=`Key${letter}`;
    assert.deepEqual(titleFlowKeyFields(code),{code,key:letter.toLowerCase(),keyCode:codePoint,location:0});
  }
  for(let codePoint=48;codePoint<=57;codePoint++){
    const digit=String.fromCharCode(codePoint),code=`Digit${digit}`;
    assert.deepEqual(titleFlowKeyFields(code),{code,key:digit,keyCode:codePoint,location:0});
  }
  for(const [code,keyCode] of [['ArrowUp',38],['ArrowDown',40],['ArrowLeft',37],['ArrowRight',39],
    ['Enter',13],['Escape',27],['Space',32]])
    assert.deepEqual(titleFlowKeyFields(code),{code,key:code==='Space'?' ':code,keyCode,location:0});
  assert.deepEqual(titleFlowKeyFields('ShiftLeft'),{code:'ShiftLeft',key:'Shift',keyCode:16,location:1});
  assert.deepEqual(titleFlowKeyFields('ControlLeft'),{code:'ControlLeft',key:'Control',keyCode:17,location:1});
  for(const code of ['ShiftRight','ControlRight','NumpadEnter','KeyAA','Keya','F1',''])
    assert.throws(()=>titleFlowKeyFields(code),/Unsupported title flow input code/);
});
