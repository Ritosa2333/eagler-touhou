import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {fakeWindow,envelope,flush} from './environment.mjs';
const url=new URL('../../site/runtime/launcher-session.mjs',import.meta.url),product=fs.existsSync(url)?await import(url):{};
function setup(options={}){
 assert.equal(typeof product.mountLauncherSession,'function','Launcher session transport is missing');
 const env=fakeWindow(options),calls={configure:[],resources:[],launch:[],keys:[],clear:0,exit:0},hooks={
  async configure(value){calls.configure.push(value);},async resources(value){calls.resources.push(value);},
  async launch(value){calls.launch.push(value);},keyboard(value){calls.keys.push(value);},
  keyboardClear(){calls.clear++;},async exit(){calls.exit++;}
 };
 Object.assign(hooks,options.hooks);
 const session=product.mountLauncherSession({window:env.win,document:env.win.document,location:env.win.location,hooks});
 const reply=request=>env.messages.map(x=>x.data).find(x=>x.request===request);
 const events=name=>env.messages.map(x=>x.data).filter(x=>x.event===name);
 return{...env,calls,hooks,session,reply,events};
}
async function running(e){e.session.markReady();e.send(envelope('configure','config',{music:'ogg',language:'zh',resources:[]}));await flush();e.send(envelope('launch','launch',{launchSerial:7}));await flush();}
test('standalone pages and missing epochs mount no launcher listener',()=>{
 for(const options of [{search:''},{standalone:true}]){const e=setup(options);assert.equal(e.session,null);assert.equal(e.win.listeners.get('message')?.size??0,0);}
});
test('malformed unsafe or repeated runtime epochs cannot create a channel',()=>{
 for(const search of ['?runtimeEpoch=0','?runtimeEpoch=-1','?runtimeEpoch=01','?runtimeEpoch=1.2','?runtimeEpoch=9007199254740992','?runtimeEpoch=41&runtimeEpoch=42']){
  const e=setup({search});assert.equal(e.session,null);assert.equal(e.messages.length,0);
 }
});
test('a launcher frame without an HTTP referrer origin fails closed without wildcard replies',()=>{
 for(const referrer of ['', 'file:///C:/index.html','null']){const e=setup({referrer});assert.equal(e.session,null);assert.equal(e.messages.length,0);}
});
test('ready is explicit once and carries no unimplemented room or save root',()=>{
 const e=setup();assert.equal(e.messages.length,0);assert.equal(e.session.markReady(),true);assert.equal(e.session.markReady(),false);
 assert.equal(e.events('ready').length,1);assert.equal(e.messages[0].origin,'https://launcher.example');
 for(const field of ['roomReturn','roomPreview','saveRoot','playback'])assert.equal(Object.hasOwn(e.events('ready')[0],field),false);
});
test('foreign parent origin epoch game and malformed request never mutate a hook',async()=>{
 const e=setup();e.session.markReady();
 e.send(envelope('configure','bad1',{music:'ogg'}),{source:{}});
 e.send(envelope('configure','bad2',{music:'ogg'}),{origin:'https://other.example'});
 for(const extra of [{epoch:40},{game:'th12'},{protocol:'other'},{request:7}])e.send({...envelope('configure','bad',{music:'ogg'}),...extra});
 await flush();assert.equal(e.calls.configure.length,0);assert.equal(e.messages.length,1);
});
test('commands received before ready return a correlated invalid state instead of loading',async()=>{
 const e=setup();e.send(envelope('configure','early',{music:'ogg'}));await flush();
 assert.equal(e.reply('early').ok,false);assert.equal(e.reply('early').errno,'invalid-state');assert.equal(e.calls.configure.length,0);
});
test('configure detaches valid array resources and string request identity before ACK',async()=>{
 const e=setup();e.session.markReady();const resources=[{path:'/th128.dat',url:'blob:https://game.example/a'}],data=envelope('configure','configure-1',{music:'ogg',language:'zh',resources});
 e.send(data);resources[0].path='changed';await flush();
 assert.equal(e.calls.configure[0].resources[0].path,'/th128.dat');assert.equal(e.reply('configure-1').ok,true);assert.equal(e.reply('configure-1').request,'configure-1');
});
test('configure field validation rejects wrong music language resources and options shapes',async()=>{
 const e=setup();e.session.markReady();let n=0;
 for(const extra of [{music:'invalid'},{music:'ogg',language:''},{music:'ogg',resources:{}},{music:'ogg',runtimeResources:{}},{music:'ogg',sharedResources:{}},{music:'ogg',options:[]}]){
  const id='invalid-'+n++;e.send(envelope('configure',id,extra));await flush();assert.equal(e.reply(id).ok,false);
 }
 assert.equal(e.calls.configure.length,0);
});
test('configure waits for its real asynchronous hook and reports rejection',async()=>{
 let reject;const e=setup({hooks:{configure:()=>new Promise((_,no)=>reject=no)}});e.session.markReady();
 e.send(envelope('configure','slow',{music:'ogg'}));await flush();assert.equal(e.reply('slow'),undefined);
 reject(Error('configuration rejected'));await flush();assert.equal(e.reply('slow').ok,false);assert.match(e.reply('slow').error,/rejected/);
});
test('resources use the wire array shape and await the bridge commit',async()=>{
 let resolve;const e=setup({hooks:{resources:value=>{e.calls.resources.push(value);return new Promise(yes=>resolve=yes);}}});e.session.markReady();
 e.send(envelope('resources','files',{resources:[{path:'/th128.dat'}]}));await flush();assert.equal(e.reply('files'),undefined);
 resolve();await flush();assert.equal(e.reply('files').ok,true);assert.equal(e.calls.resources.length,1);
});
test('missing resource and file hooks are unavailable rather than hollow success',async()=>{
 const e=setup({hooks:{resources:undefined}});e.session.markReady();e.send(envelope('resources','files',{resources:[]}));
 for(const name of ['read','write','list','sync','remove'])e.send(envelope(name,name,{path:'/score.dat',bytes:[1]}));
 await flush();assert.equal(e.reply('files').errno,'unavailable');for(const name of ['read','write','list','sync','remove'])assert.equal(e.reply(name).ok,false);
});
test('launch configures first then awaits launch and first frame requires actual positive presentation',async()=>{
 const e=setup();e.session.markReady();e.send(envelope('launch','tooearly'));await flush();assert.equal(e.reply('tooearly').ok,false);
 e.send(envelope('configure','config',{music:'ogg'}));await flush();assert.equal(e.session.notePresented(true),false);
 e.send(envelope('launch','launch',{launchSerial:7}));await flush();assert.equal(e.calls.launch.length,1);assert.equal(e.reply('launch').ok,true);
 assert.equal(e.session.notePresented(false),false);assert.equal(e.events('first-frame').length,0);
 assert.equal(e.session.notePresented(true),true);assert.equal(e.session.notePresented(true),false);
 assert.equal(e.events('first-frame').length,1);assert.equal(e.events('first-frame')[0].launchSerial,7);
});
test('reusing one request ID cannot invoke a side effect twice',async()=>{
 const e=setup();e.session.markReady();e.send(envelope('configure','one',{music:'ogg'}));e.send(envelope('configure','one',{music:'ogg'}));await flush();
 assert.equal(e.calls.configure.length,1);assert.equal(e.messages.filter(x=>x.data.request==='one').length,1);
});
test('a failed launch never reports first-frame or successful launch and may retry with a new ID',async()=>{
 let attempt=0;const e=setup({hooks:{launch:async()=>{if(++attempt===1)throw Error('load failed');}}});e.session.markReady();e.send(envelope('configure','config',{music:'ogg'}));await flush();
 e.send(envelope('launch','fail'));await flush();assert.equal(e.reply('fail').ok,false);assert.equal(e.session.notePresented(true),false);
 e.send(envelope('launch','retry'));await flush();assert.equal(e.reply('retry').ok,true);
});
test('keyboard is not buffered before launch and only declared runtime keys reach the hook',async()=>{
 const e=setup();e.session.markReady();e.send(envelope('keyboard','early',{down:true,code:'KeyZ'}));await flush();assert.equal(e.calls.keys.length,0);
 await running(e);e.send(envelope('keyboard','key',{down:true,code:'KeyZ'}));e.send(envelope('keyboard','bad',{down:true,code:'F8'}));await flush();
 assert.deepEqual(e.calls.keys,[{down:true,code:'KeyZ',repeat:false,inputSource:'physical'}]);assert.equal(e.reply('bad').ok,false);
});
test('keyboard clear only calls the injected input lease and never claims absent network or playback modes',async()=>{
 const e=setup();await running(e);const before=e.calls.clear;e.send(envelope('keyboard-clear','clear'));await flush();assert.equal(e.calls.clear,before+1);
 for(const command of ['prepare-room','playback']){e.send(envelope(command,command));await flush();assert.equal(e.reply(command).errno,'unsupported-command');}
 e.send(envelope('direct-touch','unbound',{type:'down',id:1,x:.5,y:.5}));await flush();assert.equal(e.reply('unbound').errno,'unavailable');
});
test('exit aborts an in-flight load and suppresses its late response and first frame',async()=>{
 let resolve,signal;const e=setup({hooks:{launch:args=>{signal=args.signal;return new Promise(yes=>resolve=yes);}}});e.session.markReady();e.send(envelope('configure','cfg',{music:'ogg'}));await flush();
 e.send(envelope('launch','pending'));await flush();e.send(envelope('exit','exit'));await flush();assert.equal(signal.aborted,true);assert.equal(e.calls.exit,1);
 resolve();await flush();assert.equal(e.reply('pending'),undefined);assert.equal(e.reply('exit').ok,true);assert.equal(e.events('exit').length,1);assert.equal(e.session.notePresented(true),false);
});
test('pagehide removes listeners aborts work and emits no fabricated clean exit',async()=>{
 const e=setup();await running(e);e.win.dispatchEvent({type:'pagehide'});await flush();
 assert.equal(e.win.listeners.get('message').size,0);assert.equal(e.win.listeners.get('pagehide').size,0);assert.equal(e.events('exit').length,0);
 e.send(envelope('keyboard','late',{down:true,code:'KeyZ'}));await flush();assert.equal(e.calls.keys.length,0);assert.equal(e.calls.exit,1);
});

test('a completed rejected request is re-ACKed from cache without calling the hook again',async()=>{
 const e=setup();e.session.markReady();const data=envelope('configure','retry-id',{music:'invalid'});e.send(data);await flush();e.send(data);await flush();
 const replies=e.messages.filter(x=>x.data.request==='retry-id').map(x=>x.data);assert.equal(replies.length,2);assert.deepEqual(replies[0],replies[1]);assert.equal(e.calls.configure.length,0);
});
test('long keyboard sessions do not exhaust the control channel or prevent exit',async()=>{
 const e=setup();await running(e);
 for(let i=0;i<4200;i++){e.send(envelope('keyboard','key-'+i,{down:!!(i%2),code:'KeyZ'}));await flush();}
 e.send(envelope('keyboard-clear','long-clear'));await flush();e.send(envelope('exit','long-exit'));await flush();
 assert.equal(e.calls.keys.length,4200);assert.equal(e.reply('long-clear').ok,true);assert.equal(e.reply('long-exit').ok,true);assert.equal(e.calls.exit,1);
});
test('runtime supplied health and info cannot overwrite the pinned message identity',async()=>{
 const e=setup();await running(e);e.session.notePresented(true);
 const alien={protocol:'other',game:'th12',epoch:1,event:'fake',architecture:'wasm',fps:60};
 e.session.runtimeInfo(alien);e.session.health(alien,alien);
 for(const row of e.messages){assert.equal(row.data.protocol,'eagler-touhou/1');assert.equal(row.data.game,'th128');assert.equal(row.data.epoch,41);assert.notEqual(row.data.event,'fake');}
});
test('an absent exit hook never ACKs a clean runtime cleanup',async()=>{
 const e=setup({hooks:{exit:undefined}});await running(e);e.send(envelope('exit','noexit'));await flush();
 assert.equal(e.reply('noexit').ok,false);assert.equal(e.reply('noexit').errno,'unavailable');assert.equal(e.events('exit').length,0);
});
