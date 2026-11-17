import test from'node:test';import assert from'node:assert/strict';import fs from'node:fs';
import{surface,flush,deferred}from'./surface.mjs';
const url=new URL('../../site/runtime/practice-menu.mjs',import.meta.url),product=fs.existsSync(url)?await import(url):{};
const expected={'A1-3':[3,1],'A2-3':[5,1],'B1-3':[8,1],'B2-3':[10,1],'C1-3':[13,1],'C2-3':[15,1],EX:[16,4]};
function harness(overrides={}){
 assert.equal(typeof product.mountPracticeMenu,'function','Visible practice controller is missing');
 const ui=surface(),calls=[],availability={ready:true,busy:false,inRun:false,practiceRoute:null};
 const h={...ui,calls,availability,onOpenCalls:0,focusGameCalls:0};
 h.controller=product.mountPracticeMenu({...ui,getAvailability:()=>({...availability}),onOpen:async()=>{h.onOpenCalls++;},onFocusGame(){h.focusGameCalls++;},launch:async(route,owner)=>{calls.push({route,owner});return{route,stageId:expected[route][0],difficulty:expected[route][1],nativeTainted:true,normalModeClear:false};},...overrides});
 return h;
}
async function open(h){h.opener.emit('click');await flush();}
test('practice remains closed until an explicit open action and refresh never opens it',async()=>{
 const h=harness();assert.equal(h.root.hidden,true);h.controller.refresh();assert.equal(h.root.hidden,true);await open(h);assert.equal(h.root.hidden,false);assert.equal(h.onOpenCalls,1);assert.equal(h.nodes.select.focused,true);h.controller.dispose();
});
test('only six Normal third stages and the original EX start are offered',()=>{
 const h=harness();assert.deepEqual(h.nodes.select.children.map(x=>x.value),['A1-3','A2-3','B1-3','B2-3','C1-3','C2-3','EX']);h.controller.dispose();
});
test('readiness and busy ownership prevent a hidden or repeated start',async()=>{
 const h=harness();h.availability.ready=false;h.controller.refresh();assert.equal(h.opener.disabled,true);await open(h);assert.equal(h.root.hidden,true);h.availability.ready=true;await open(h);h.availability.busy=true;h.nodes.start.emit('click');await flush();assert.equal(h.calls.length,0);h.controller.dispose();
});
test('an active run requires explicit replacement confirmation',async()=>{
 const h=harness();h.availability.inRun=true;await open(h);assert.equal(h.nodes.replaceRow.hidden,false);h.nodes.start.emit('click');await flush();assert.equal(h.calls.length,0);h.nodes.replace.checked=true;h.nodes.start.emit('click');await flush();assert.equal(h.calls.length,1);assert.equal(h.root.hidden,true);h.controller.dispose();
});
for(const route of Object.keys(expected))test('explicit practice choice '+route+' reaches the launch owner and accepts only tainted native completion',async()=>{
 const h=harness();await open(h);h.nodes.select.value=route;h.nodes.start.emit('click');await flush();assert.equal(h.calls.length,1);assert.equal(h.calls[0].route,route);assert.equal(h.calls[0].owner.signal.aborted,false);assert.equal(h.root.hidden,true);assert.match(h.nodes.status.textContent,/练习/);h.controller.dispose();
});
test('repeated confirm clicks keep exactly one pending launch',async()=>{
 const held=deferred(),calls=[];const h=harness({launch:async(route,owner)=>{calls.push({route,owner});return held.promise;}});await open(h);h.nodes.start.emit('click');h.nodes.start.emit('click');assert.equal(calls.length,1);assert.equal(h.nodes.start.disabled,true);held.resolve({route:'A1-3',stageId:3,difficulty:1,nativeTainted:true,normalModeClear:false});await flush();h.controller.dispose();
});
test('cancel aborts only its own pending owner and ignores late successful completion',async()=>{
 const held=deferred(),calls=[];const h=harness({launch:async(route,owner)=>{calls.push({route,owner});return held.promise;}});await open(h);h.nodes.start.emit('click');h.nodes.cancel.emit('click');assert.equal(calls[0].owner.signal.aborted,true);assert.equal(calls[0].owner.isCurrent(),false);assert.equal(h.root.hidden,true);const text=h.nodes.status.textContent;held.resolve({route:'A1-3',stageId:3,difficulty:1,nativeTainted:true,normalModeClear:false});await flush();assert.equal(h.nodes.status.textContent,text);h.controller.dispose();
});
test('failed or untainted completion stays visible and never earns a restart owner',async()=>{
 const h=harness({launch:async()=>({route:'A1-3',stageId:3,difficulty:1,nativeTainted:false,normalModeClear:true})});await open(h);h.nodes.start.emit('click');await flush();assert.equal(h.root.hidden,false);assert.equal(h.nodes.restart.disabled,true);assert.match(h.nodes.status.textContent,/失败|taint|练习/i);h.controller.dispose();
});
test('restart uses the actual current practice route and refuses an ordinary run',async()=>{
 const h=harness();h.availability.inRun=true;h.availability.practiceRoute='C2-3';await open(h);h.nodes.replace.checked=true;h.nodes.select.value='A1-3';h.nodes.restart.emit('click');await flush();assert.equal(h.calls[0].route,'C2-3');h.availability.practiceRoute=null;await open(h);assert.equal(h.nodes.restart.disabled,true);h.nodes.restart.emit('click');await flush();assert.equal(h.calls.length,1);h.controller.dispose();
});
test('Escape closes practice and dialog key events never leak to gameplay',async()=>{
 const h=harness();await open(h);const down=h.root.emit('keydown',{code:'KeyZ'});assert.equal(down.propagationStopped,true);assert.equal(h.root.emit('keyup',{code:'KeyZ'}).propagationStopped,true);h.root.emit('keydown',{code:'Escape'});assert.equal(h.root.hidden,true);assert.equal(h.calls.length,0);h.controller.dispose();
});
test('dispose removes input listeners, aborts pending work and never reopens on stale clicks',async()=>{
 const held=deferred(),calls=[];const h=harness({launch:async(route,owner)=>{calls.push(owner);return held.promise;}});await open(h);h.nodes.start.emit('click');h.controller.dispose();assert.equal(calls[0].signal.aborted,true);h.opener.emit('click');assert.equal(h.root.hidden,true);held.reject(Error('late cancelled'));await flush();assert.equal(h.root.hidden,true);
});
