import test from 'node:test';
import assert from 'node:assert/strict';
import {assertKeyboardLayoutEvidence} from './keyboard-layout-evidence.mjs';
import {resolveAdaptationAdapter} from './registry.mjs';
const adapter=resolveAdaptationAdapter('th08','normal');
function fixture(){
  const bindings={up:'KeyI',down:'KeyK',left:'KeyU',right:'KeyO',shoot:'KeyF',bomb:'KeyG',focus:'KeyT',pause:'KeyP'};
  const aliases={KeyI:'ArrowUp',KeyK:'ArrowDown',KeyU:'ArrowLeft',KeyO:'ArrowRight',KeyF:'KeyZ',KeyG:'KeyX',KeyT:'ShiftLeft',KeyP:'Escape'};
  const saved={id:'saved-fixture',before:{layout:{selected:'saved-fixture',persisted:true},aliases}};saved.after=structuredClone(saved.before);
  const e={bindings,saved,trials:[]};
  for(const [index,mode] of ['wasd','custom','arrows'].entries()){
    const epoch=index+2,keys=mode==='wasd'?['KeyW','KeyS','KeyA','KeyD']:mode==='custom'?['KeyI','KeyK','KeyU','KeyO']:['ArrowUp','ArrowDown','ArrowLeft','ArrowRight'];
    const codes=mode==='wasd'?{ShiftLeft:'KeyL',KeyZ:'KeyJ',KeyX:'KeyK'}:mode==='custom'?{ShiftLeft:'KeyT',KeyZ:'KeyF',KeyX:'KeyG',Escape:'KeyP'}:{};
    const layout=mode==='custom'?saved.id:mode,events=[],keyboard={source:'physical-keyboard',disposed:true,pulseTicks:3,events:[]};let ticks=0;
    const directions=keys.map((physical,i)=>{
      const canonical=['ArrowUp','ArrowDown','ArrowLeft','ArrowRight'][i],axis=i<2?'y':'x',sign=i%2?1:-1;
      for(const down of [true,false])events.push({code:physical,down,epoch,source:'physical',transitions:[{code:canonical,down}]});
      const state=(value,position)=>({input:{value},position});const position={x:100,y:100};position[axis]+=sign*4;
      return{physical,canonical,axis,sign,before:state(0,{x:100,y:100}),held:state(adapter.movement.directionMasks[canonical],position),released:state(0,position),settled:state(0,{...position})};
    });
    for(const code of ['ShiftLeft','KeyZ','KeyX','Escape','Escape']){
      const physical=codes[code]??code;
      for(const down of [true,false]){
        const receipt={code:physical,down,epoch,source:'physical',transitions:[{code,down}]};events.push(receipt);
        keyboard.events.push({code,physical,down,afterTick:ticks,receipt:{acknowledged:true,physicalReceipt:receipt}});ticks+=3;
      }
    }
    e.trials.push({mode,transition:{pagehide:true,epoch},layout,codes,directions:{layout,selection:{selected:layout,base:'arrows'},directions},
      actions:{epoch,complete:true,inputSource:'physical-keyboard',keyboard,proof:'native-action-fixture'},events});
  }
  return JSON.parse(JSON.stringify(e));
}
const options={adapter,assertActions:actions=>assert.equal(actions.proof,'native-action-fixture')};
test('keyboard layouts require saved reconstruction, all directions/actions and exactly one mapped edge',()=>{
  let calls=0;assertKeyboardLayoutEvidence(fixture(),{...options,assertActions:actions=>{options.assertActions(actions);calls++;}});assert.equal(calls,3);
});
for(const [name,mutate] of Object.entries({
  'missing custom layout':e=>e.trials.splice(1,1),
  'failed persistence':e=>e.saved.after.layout.persisted=false,
  'wrong saved mapping':e=>e.saved.after.aliases.KeyI='ArrowDown',
  'missing default restoration':e=>e.trials[2].directions.selection.selected='wasd',
  'duplicate canonical press':e=>e.trials[1].events[0].transitions.push({...e.trials[1].events[0].transitions[0]}),
  'duplicate mapper delivery':e=>e.trials[0].events.push({...e.trials[0].events[0]}),
  'custom key not used':e=>e.trials[1].actions.keyboard.events[0].receipt.physicalReceipt.code='ShiftLeft',
  'wrong native direction':e=>e.trials[1].directions.directions[0].held.input.value=32,
  'residual movement':e=>e.trials[1].directions.directions[0].settled.position.x++,
  'missing native action proof':e=>delete e.trials[0].actions.proof,
  'another Runtime epoch':e=>e.trials[1].events[0].epoch=1,
  'unreleased key':e=>e.trials[1].actions.keyboard.events.pop(),
}))test(`keyboard layout proof rejects ${name}`,()=>{const e=fixture();mutate(e);assert.throws(()=>assertKeyboardLayoutEvidence(e,options));});
