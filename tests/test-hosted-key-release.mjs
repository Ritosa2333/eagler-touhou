import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {stripTypeScriptTypes} from 'node:module';
import {runInNewContext} from 'node:vm';

const source=readFileSync(new URL('../src/launcher/app.mts',import.meta.url),'utf8');
const start=source.indexOf('const hostedGameKeyCodes =');
const end=source.indexOf('function clearHostedKeyboard()',start);
assert.ok(start>=0&&end>start);
const handlers={},messages=[];
class Element {constructor(chrome=false){this.chrome=chrome;}closest(){return this.chrome?this:null;}}
runInNewContext(stripTypeScriptTypes(source.slice(start,end)),{
  Set,Element,window:{addEventListener:(kind,handler)=>handlers[kind]=handler},
  state:{launched:true,game:'th07'},player:{classList:{contains:()=>true}},frame:{contentWindow:{}},
  protocol:'test',touchRuntimeMessageContext:()=>({epoch:1}),
  deliverRuntimeInput:(_context,message)=>messages.push(message),
});
const event=(kind,chrome=false,altKey=false)=>({type:kind,target:new Element(chrome),code:'ShiftLeft',key:'Shift',
  keyCode:16,location:1,altKey,metaKey:false,preventDefault(){}});
handlers.keydown(event('keydown'));
handlers.keyup(event('keyup',true,true));
assert.deepEqual(messages.map(message=>message.down),[true,false],
  'a key started over the game must be released even after focus moves to Launcher controls');
handlers.keydown(event('keydown',true));
handlers.keyup(event('keyup',true));
assert.equal(messages.length,2,'Launcher-only keys must not enter the game');
console.log('PASS hosted key release across Launcher focus changes');
