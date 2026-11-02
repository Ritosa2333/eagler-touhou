import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {runInNewContext} from 'node:vm';
import {ManagedInput} from '../src/managed-input.mjs';

const source=readFileSync(new URL('../site/game.mjs',import.meta.url),'utf8');
test('managed keyboard release retires Shift pressed before a mapping bridge was available',()=>{
  const listeners=new Map(),keys=new Set(),hostKeys=new Set();
  const environment={location:{href:'https://game.test/?runtimeEpoch=1',origin:'https://game.test'}};
  const managedInput=new ManagedInput(environment);
  const context={window:{addEventListener:(type,callback)=>listeners.set(type,callback)},keys,hostKeys,managedInput,
    assets:{managed:true},ready:true,roomPreview:false,multiplayer:{active:true,readOnly:false},runtimeOptions:{},
    keyDown:code=>keys.has(code)||hostKeys.has(code),practiceUI:null,
    model:{state:{captureJoy:false}},profile:{settings:{keyMap:{focus:'ShiftLeft'}}},
    pendingKeys:0,pendingPressed:0,audioReady:async()=>{},notice(){},keyboardCode:e=>e.code};
  const begin=source.indexOf('function eventKey('),end=source.indexOf("for(const type of ['pointerdown'",begin);
  runInNewContext(source.slice(begin,end),context);
  const event=code=>({code,preventDefault(){}});
  listeners.get('keydown')(event('ShiftLeft'));
  assert(context.keyDown('ShiftLeft'));
  // Resource/room attachment installs the canonical mapping between DOWN/UP.
  environment.__eaglerKeyboardMapping={schema:'eagler-touhou/keyboard-input/1',protocol:'eagler-touhou/1',game:'th12',epoch:1,origin:'https://game.test',map:(_event,down)=>[{code:'ShiftLeft',down}]};
  listeners.get('keyup')(event('ShiftLeft'));
  assert(!context.keyDown('ShiftLeft'),'a released physical key cannot survive as a second local owner');
});

test('SDL keyboard uses one managed owner and processes modifier releases while Alt is held',()=>{
  const native=readFileSync(new URL('../site/game-sdl.mjs',import.meta.url),'utf8'),listeners=new Map(),keys=new Map(),calls=[];
  const environment={location:{href:'https://game.test/?runtimeEpoch=1',origin:'https://game.test'}};
  const input=new ManagedInput(environment);
  const context={keys,input,assets:{managed:true},vk:code=>code==='ShiftLeft'?16:0,
    core:{_th12_host_key:(...args)=>calls.push(args)},window:{addEventListener:(type,fn)=>listeners.set(type,fn)}};
  const keyStart=native.indexOf('function key('),keyEnd=native.indexOf('\nfunction clearKeyboard',keyStart);
  const inputStart=native.indexOf("for(const type of ['keydown','keyup'])"),inputEnd=native.indexOf("window.addEventListener('blur'",inputStart);
  context.keyboardCode=e=>e.code;
  runInNewContext(native.slice(keyStart,keyEnd)+'\n'+native.slice(inputStart,inputEnd),context);
  const event={code:'ShiftLeft',preventDefault(){}};
  listeners.get('keydown')(event);
  environment.__eaglerKeyboardMapping={schema:'eagler-touhou/keyboard-input/1',protocol:'eagler-touhou/1',game:'th12',epoch:1,origin:'https://game.test',map:(_event,down)=>[{code:'ShiftLeft',down}]};
  listeners.get('keyup')({...event,altKey:true});
  assert.equal(keys.size,0);assert.deepEqual(calls.at(-1),[16,0]);
});
