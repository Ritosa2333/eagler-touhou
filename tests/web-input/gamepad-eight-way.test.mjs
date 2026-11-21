import test from 'node:test';
import assert from 'node:assert/strict';
import {installLauncherGamepadInput,mappedGamepadCodes,mountStageGamepad,rawGamepadDirections} from '../../site/runtime/gamepad-input.mjs';
import {mountLocalBindings,readInputBindings} from '../../site/runtime/local-input-bindings.mjs';
import {createGamepadMappingModel} from '../../../../launcher/src/launcher/gamepad-mapping.mts';
import {createPlayerGamepad} from '../../../../launcher/app/services/player-gamepad.ts';

const pad=()=>({id:'Controller',index:0,connected:true,mapping:'standard',axes:[0,0,0,0],buttons:Array.from({length:17},()=>({pressed:false,value:0}))});
const directions=[[[0,-.65],['ArrowUp']],[[.36,-.68],['ArrowRight','ArrowUp']],[[.65,-.2],['ArrowRight']],
  [[.36,.68],['ArrowDown','ArrowRight']],[[0,.65],['ArrowDown']],[[-.34,.34],['ArrowDown','ArrowLeft']],
  [[-.65,.2],['ArrowLeft']],[[-.34,-.34],['ArrowLeft','ArrowUp']],[[.18,-.18],[]],[[0,0],[]]];
function keySource(){const sources=new Map();return {setKeySource:(id,keys)=>sources.set(id,keys),clearKeySource:id=>sources.delete(id),
  keys:()=>[...new Set([...sources.values()].flat())].sort()};}

test('default gamepad turns through all eight directions, ignores center drift, and releases on disconnect',()=>{
  const p=pad(),window={navigator:{getGamepads:()=>[p]}},input=keySource();
  const stage=mountStageGamepad({window,document:{hidden:false},input});
  for(const [axes,keys] of directions){p.axes=axes;stage.poll();assert.deepEqual(input.keys(),keys,`stick ${axes}`);}
  p.axes=[.4,-.4];assert.equal(rawGamepadDirections(p),16|128,'original title uses the same directional decision');
  p.axes=[0,0];p.buttons[13]={pressed:true,value:1};stage.poll();assert.deepEqual(input.keys(),['ArrowDown']);
  p.connected=false;stage.poll();assert.deepEqual(input.keys(),[]);
});

test('standalone custom buttons keep the same stick and release behavior',()=>{
  const p=pad(),window=Object.assign(new EventTarget(),{navigator:{getGamepads:()=>[p]}}),document=Object.assign(new EventTarget(),{hidden:false});
  const input=keySource(),poll=mountLocalBindings({window,document,input,bindings:readInputBindings({}, {KeyX:6}),getActive:()=>true});
  p.buttons[6]={pressed:true,value:1};p.axes=[.34,.34];poll();assert.deepEqual(input.keys(),['ArrowDown','ArrowRight','KeyX']);
  document.hidden=true;poll();assert.deepEqual(input.keys(),[]);
});

function managed(game){
  const p=pad(),other={...pad(),id:'Other controller',index:1},target={};
  const options={protocol:'eagler-touhou/1',game,epoch:1,origin:'http://localhost',isReady:()=>true,target};
  const detach=installLauncherGamepadInput(options);
  const mapping=createGamepadMappingModel({storage:null,getGamepads:()=>[other,p]});
  mapping.refresh();const key=mapping.getSnapshot().devices.find(d=>d.index===0).key;
  mapping.select(0,key);mapping.setEnabled(true);
  const context={...options,target,targetOrigin:options.origin,launched:true,spectator:false};
  let focused=true;
  const owner=createPlayerGamepad({model:mapping,window:new EventTarget(),playerOpen:()=>true,
    document:Object.assign(new EventTarget(),{visibilityState:'visible',hasFocus:()=>focused}),
    runtime:{getInputContext:()=>context,subscribe:()=>()=>{}}});
  const read=()=>mappedGamepadCodes(target).sort();read();
  return {p,other,mapping,key,read,blur:()=>{focused=false;},dispose(){owner.dispose();detach();}};
}

test('the Launcher sends eight-way actions for the selected standard TH128 stick',()=>{
  const f=managed('th128');
  try{
    f.other.axes=[1,0,0,0];
    for(const [axes,keys] of directions){
      const [x,y]=axes.map(axis=>Math.abs(axis)===.34?Math.sign(axis)*.38:axis);
      f.p.axes=[x,y,0,0];assert.deepEqual(f.read(),keys,`mapped stick ${x},${y}`);
    }
    f.p.buttons[14]={pressed:true,value:1};assert.deepEqual(f.read(),['ArrowLeft']);
    f.blur();assert.deepEqual(f.read(),[]);
  }finally{f.dispose();}
});

test('explicit directional bindings, threshold settings, and other games keep their configured mapping',()=>{
  const f=managed('th128'),legacy=managed('th12');
  try{
    legacy.p.axes=[.34,.34,0,0];assert.deepEqual(legacy.read(),[]);
    f.mapping.setThreshold(f.key,.8);f.read();f.p.axes=[.49,.49,0,0];assert.deepEqual(f.read(),[]);
    f.p.axes=[.7,.7,0,0];assert.deepEqual(f.read(),['ArrowDown','ArrowRight']);
    f.p.axes=[0,0,0,0];f.mapping.setThreshold(f.key,.5);f.mapping.bind(f.key,'right',[{kind:'button',index:7}]);f.read();
    f.p.axes=[.7,.32,0,0];assert.deepEqual(f.read(),[]);
    f.p.buttons[7]={pressed:true,value:1};assert.deepEqual(f.read(),['ArrowRight']);
  }finally{f.dispose();legacy.dispose();}
});
