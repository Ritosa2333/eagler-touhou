import test from'node:test';import assert from'node:assert/strict';
import{fakeWindow,envelope,flush}from'./environment.mjs';import{mountLauncherSession}from'../../site/runtime/launcher-session.mjs';
async function setup(){const e=fakeWindow(),calls=[],session=mountLauncherSession({window:e.win,hooks:{configure(){},launch(){},keyboard(){},keyboardClear(){},exit(){},directTouch:v=>calls.push(['direct',v]),touchControls:v=>calls.push(['controls',v]),touchCancel:()=>calls.push(['cancel'])}});session.markReady();e.send(envelope('configure','c',{music:'ogg',options:{touchEnabled:true}}));await flush();e.send(envelope('launch','l'));await flush();return{...e,calls,session,reply:r=>e.messages.map(x=>x.data).find(x=>x.request===r)};}
test('fixed upstream direct-touch and touch-controls shapes reach live runtime hooks once with normal origin/epoch fences',async()=>{
 const e=await setup();e.send(envelope('direct-touch','d',{type:'down',id:-1000000,x:.5,y:.5}));await flush();assert.equal(e.reply('d').ok,true);assert.deepEqual(e.calls[0],['direct',{type:'down',id:-1000000,x:.5,y:.5}]);
 e.send(envelope('direct-touch','d',{type:'down',id:-1000000,x:.5,y:.5}));await flush();assert.equal(e.calls.length,1);
 const controls={fireEnabled:true,focusEnabled:false,bombSerial:1,escapeSerial:0,joystickX:16384,joystickY:-32767,touchSensitivity:150,thpracMobilePanelsEnabled:true,thpracMobilePanelsVisible:false};e.send(envelope('touch-controls','t',controls));await flush();assert.equal(e.reply('t').ok,true);assert.deepEqual(e.calls.at(-1),['controls',controls]);
 e.send(envelope('touch-cancel','x'));await flush();assert.deepEqual(e.calls.at(-1),['cancel']);e.send(envelope('direct-touch','wrong',{type:'up',id:1,x:0,y:0}),{origin:'https://wrong.example'});await flush();assert.equal(e.reply('wrong'),undefined);await e.session.dispose();
});
test('invalid touch words/serials and nonrunning commands reject without reaching native input hook',async()=>{
 const e=await setup();for(const[id,value]of[['m',{type:'move',id:1,x:NaN,y:.5}],['s',{type:'down',id:1.5,x:.5,y:.5}],['b',{type:'bogus',id:1,x:0,y:0}]]){e.send(envelope('direct-touch',id,value));await flush();assert.equal(e.reply(id).errno,'invalid-field');}
 e.send(envelope('touch-controls','bad',{fireEnabled:true,focusEnabled:false,bombSerial:-1,escapeSerial:0,joystickX:99999,joystickY:0}));await flush();assert.equal(e.reply('bad').errno,'invalid-field');assert.equal(e.calls.length,0);await e.session.dispose();
});
