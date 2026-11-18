import{functionSource}from'../web-input/play-harness.mjs';
// Real TH128 schema/menu and current page caller; DOM, native module and start are declared seams.
import test from 'node:test';import assert from 'node:assert/strict';import fs from 'node:fs';
import {Element,flush,deferred} from '../practice-menu/surface.mjs';
const productUrl=new URL('../../site/runtime/thprac128.mjs',import.meta.url),menuUrl=new URL('../../site/runtime/thprac128-menu.mjs',import.meta.url);
const api=fs.existsSync(productUrl)?await import(productUrl):{},uiApi=fs.existsSync(menuUrl)?await import(menuUrl):{};
const method=name=>{assert.equal(typeof api[name],'function','Actual TH128 thprac '+name+' is missing');return api[name];};
const create=(p={},d=1)=>method('createThprac128Session')(p,d);
const counts=[7,9,7,7,9,7,8,7,7,8,6,8,9,9,8,10];
const expectedParams={mode:1,stage:0,section:0,phase:0,dlg:false,score:0,motivation:100000,perfect_freeze:30000,ice_power:10000,ice_area:3000};
test('TH128 session preserves exact defaults, owns the data, and floors display score without 32bit overflow',()=>{
 const input={score:9999999999-9,motivation:43210};const s=create(input);input.motivation=1;
 assert.equal(s.schema,'eagler-touhou/thprac-session/1');assert.equal(s.game,'th128');assert.deepEqual(s.params,{...expectedParams,score:9999999990,motivation:43210});assert.equal(s.difficulty,1);assert(Object.isFrozen(s)&&Object.isFrozen(s.params));assert.equal(create({score:1234567899}).params.score,1234567890);
});
test('strict data-only parameters reject coercion, accessors, unknown fields and every actual bound',()=>{
 method('createThprac128Session');
 for(const p of[{stage:'0'},{score:NaN},{score:1.2},{mode:true},{stage:16},{score:9999999991},{motivation:100001},{perfect_freeze:30001},{ice_power:10001},{ice_area:3001},{ice_area:-1},{dlg:1},{life:8}])assert.throws(()=>create(p));
 let invoked=0;const bad={};Object.defineProperty(bad,'score',{enumerable:true,get(){invoked++;return 1;}});assert.throws(()=>create(bad));assert.equal(invoked,0);
});
test('all16 actual stage and126 chapter identifiers retain upstream stage0-based encoding and reject foreign stage',()=>{
 let chapters=0;for(let stage=0;stage<16;stage++)for(let chapter=1;chapter<=counts[stage];chapter++){const code=10000+(stage+1)*100+chapter;assert.equal(create({stage,section:code},stage===15?4:1).params.section,code);chapters++;}
 assert.equal(chapters,126);assert.throws(()=>create({stage:0,section:10202}));assert.throws(()=>create({stage:15,section:11611},4));assert.throws(()=>create({stage:15},1));assert.throws(()=>create({stage:0},4));
});
test('fixed TH128 Boss groups and EX final phases select actual numeric IDs and locale labels',()=>{
 const choices=method('thprac128SectionChoices');assert.deepEqual(choices(0,2,'en').map(x=>x.value),[1,2,3]);assert.deepEqual(choices(0,3,'zh').map(x=>x.value),[4,5,6,7]);assert.deepEqual(choices(0,5,'ja').map(x=>x.value),[5,7]);assert.equal(choices(0,3,'ja')[0].label,'ボス 通常1');
 const catalog=JSON.parse(fs.readFileSync(new URL('../../docs/thprac128-fixed-section-catalog.json',import.meta.url))).sections;
 assert.equal(api.THPRAC128_SECTIONS.length,156);for(const row of catalog){const s=create({stage:row.stage-1,section:row.id},row.stage===16?4:1);assert.equal(s.params.section,row.id);}
 for(let phase=0;phase<4;phase++)assert.equal(create({stage:15,section:156,phase},4).params.phase,phase);assert.throws(()=>create({section:5,phase:1}));assert.throws(()=>create({stage:15,section:156,phase:4},4));assert.throws(()=>create({stage:0,section:1,dlg:true}));
});
test('original mode resets to the original firststage and never creates a practice native payload',()=>{
 const s=create({mode:0,stage:4,section:10502,score:9000});assert.deepEqual(s.params,{...expectedParams,mode:0});assert.equal(method('thprac128Payload')(s),null);
});
function nativeStub(s){const p=s.params,calls=[];let accepted=1;const initial={score:p.score,motivation:p.motivation,perfect_freeze:p.perfect_freeze,ice_power:p.ice_power,ice_area:p.ice_area};
 const receipt={schema:'th128-thprac-native-state/1',active:true,stage:p.stage,chapter:p.section>=10000?p.section%100:p.section?0:1,section:p.section>0&&p.section<10000?p.section:0,phase:p.phase,dlg:p.dlg,difficulty:s.difficulty,generation:3,frame:0,nonNormal:true,initial,actual:{...initial},contexts:15};
 const module={HEAPU8:new Uint8Array(512),_malloc:n=>{calls.push(['allocate',n]);return 64;},_free:x=>calls.push(['free',x]),UTF8ToString:x=>x,_th128_world_error:()=> 'native rejected',_th128_world_prepare_thprac128:(ptr,n)=>{calls.push(['prepare',ptr,n,[...new Int32Array(module.HEAPU8.buffer,ptr,n/4)]]);return accepted;},_th128_world_thprac128_state:()=>JSON.stringify(receipt)};
 return{module,receipt,calls,reject(){accepted=0;}};
}
test('real Web native adapter sends the bounded11word ABI and frees memory on acceptance or rejection',()=>{
 const s=create({stage:15,section:156,phase:2,score:9999999990,motivation:54321,perfect_freeze:12345,ice_power:6789,ice_area:800},4),h=nativeStub(s);
 method('prepareThprac128Native')(h.module,s);assert.deepEqual(h.calls,[['allocate',44],['prepare',64,44,[1,15,156,2,0,4,999999999,54321,12345,6789,800]],['free',64]]);
 h.reject();assert.throws(()=>method('prepareThprac128Native')(h.module,s),/native rejected/);assert.deepEqual(h.calls.at(-1),['free',64]);
});
test('native receipt must bind actual stage/chapter/fields/configuration/frame/generation and non-normal taint',()=>{
 const s=create({section:10102}),h=nativeStub(s);assert.equal(method('readThprac128Native')(h.module,s).generation,3);
 for(const [key,value] of[['stage',1],['chapter',1],['frame',1],['generation',0],['nonNormal',false],['contexts',7]]){const prior=h.receipt[key];h.receipt[key]=value;assert.throws(()=>method('readThprac128Native')(h.module,s));h.receipt[key]=prior;}
 h.receipt.actual.ice_area=0;assert.throws(()=>method('readThprac128Native')(h.module,s));
});
function surface(){const names=['enabled','locale','mode','startPoint','path','stage','difficulty','warp','section','phase','dlg','score','motivation','perfect_freeze','ice_power','ice_area','start','restart','cancel','status','replace','replaceRow'];const root=new Element('section'),opener=new Element('button'),nodes={};for(const name of names)nodes[name]=new Element(['locale','mode','startPoint','path','stage','difficulty','warp','section','phase'].includes(name)?'select':'input');root.querySelector=s=>nodes[s.match(/data-thprac="([^"]+)"/)?.[1]]??null;root.querySelectorAll=()=>[];return{root,opener,nodes};}
function harness(overrides={}){assert.equal(typeof uiApi.mountThprac128Menu,'function','Actual complete TH128 thprac menu is missing');const ui=surface(),availability={ready:true,busy:false,inRun:false,practiceSession:null},calls=[];
 const controller=uiApi.mountThprac128Menu({...ui,getAvailability:()=>availability,onOpen:async()=>{},onFocusGame(){},launch:async(s,owner)=>{calls.push({s,owner});return{session:s,stageId:s.params.stage+1,difficulty:s.difficulty,nativeTainted:s.params.mode===1,normalModeClear:false};},...overrides});return{...ui,availability,calls,controller};}
async function open(h){h.nodes.mode.value='1';h.nodes.mode.emit('change');h.opener.emit('click');await flush();}
test('unchanged native route ticks leave the existing select options in the DOM',async()=>{
 const h=harness();await h.controller.open({route:true});
 h.nodes.mode.value='1';h.nodes.mode.emit('change');h.nodes.path.value='1';h.nodes.path.emit('change');
 h.nodes.stage.value='2';h.nodes.stage.emit('change');h.nodes.warp.value='5';h.nodes.warp.emit('change');
 const current=h.nodes.section.children[0];assert(current);
 for(let i=0;i<120;++i)h.controller.syncRoute(0,1);
 assert.equal(h.nodes.section.children[0],current,'native title ticks must not replace form options');
 assert.equal(h.nodes.path.value,'1');assert.equal(h.nodes.stage.value,'2');
 h.controller.syncRoute(3,1);assert.equal(h.nodes.startPoint.value,'3');assert.equal(h.nodes.difficulty.value,'4');
 assert.notEqual(h.nodes.section.children[0],current);assert(h.nodes.section.children.some(el=>el.value==='154'));
 h.controller.syncRoute(0,2);assert.equal(h.nodes.startPoint.value,'0');assert.equal(h.nodes.difficulty.value,'2');
 h.controller.dispose();
});
test('complete menu opens explicitly and maps A2 third stage with upstream resource defaults',async()=>{
 const h=harness();assert.equal(h.root.hidden,true);h.controller.refresh();assert.equal(h.root.hidden,true);await open(h);assert.equal(h.root.hidden,false);assert.equal(h.nodes.motivation.value,'100000');assert.equal(h.nodes.perfect_freeze.value,'30000');assert.equal(h.nodes.ice_power.value,'10000');assert.equal(h.nodes.ice_area.value,'3000');
 h.nodes.startPoint.value='0';h.nodes.startPoint.emit('change');h.nodes.path.value='1';h.nodes.path.emit('change');h.nodes.stage.value='2';h.nodes.stage.emit('change');h.nodes.warp.value='1';h.nodes.warp.emit('change');h.nodes.section.value='9';h.nodes.start.emit('click');await flush();assert.equal(h.calls[0].s.params.stage,4);assert.equal(h.calls[0].s.params.section,10509);h.controller.dispose();
});
test('menu EX final phase, multilingual labels, dialogue eligibility and source group values are real form behavior',async()=>{
 const h=harness();await open(h);h.nodes.warp.value='3';h.nodes.warp.emit('change');assert.deepEqual(h.nodes.section.children.map(n=>n.value),['4','5','6','7']);assert.equal(h.nodes.dlg.disabled,false);h.nodes.locale.value='ja';h.nodes.locale.emit('change');assert.equal(h.nodes.section.children[0].textContent,'ボス 通常1');
 h.nodes.startPoint.value='3';h.nodes.startPoint.emit('change');h.nodes.warp.value='5';h.nodes.warp.emit('change');h.nodes.section.value='156';h.nodes.section.emit('change');assert.equal(h.nodes.phase.disabled,false);h.nodes.phase.value='3';h.nodes.start.emit('click');await flush();assert.equal(h.calls[0].s.params.phase,3);assert.equal(h.calls[0].s.params.stage,15);assert.equal(h.calls[0].s.difficulty,4);h.controller.dispose();
});
test('launcher off, unavailable runtime, busy state and active-run replacement actually gate launch',async()=>{
 const h=harness();await open(h);h.nodes.enabled.checked=false;h.nodes.enabled.emit('change');h.nodes.start.emit('click');await flush();assert.equal(h.calls.length,0);h.nodes.enabled.checked=true;h.nodes.enabled.emit('change');h.availability.inRun=true;h.controller.refresh();h.nodes.start.emit('click');await flush();assert.equal(h.calls.length,0);h.nodes.replace.checked=true;h.nodes.start.emit('click');await flush();assert.equal(h.calls.length,1);h.controller.dispose();
});
test('menu parses strict integer form values and refuses an unsupported native receipt',async()=>{
 const h=harness({launch:async()=>({stageId:1,difficulty:1,nativeTainted:false,normalModeClear:true})});await open(h);h.nodes.score.value='1e4';h.nodes.start.emit('click');await flush();assert.equal(h.root.hidden,false);assert.match(h.nodes.status.textContent,/integer|整数|分数|score/i);h.nodes.score.value='1000';h.nodes.start.emit('click');await flush();assert.equal(h.root.hidden,false);assert.match(h.nodes.status.textContent,/失败|taint|身份|practice|receipt/i);h.controller.dispose();
});
test('cancel/dispose aborts only the pending owner and cannot adopt a late successful launch',async()=>{
 const held=deferred(),calls=[],h=harness({launch:async(s,owner)=>{calls.push({s,owner});return held.promise;}});await open(h);h.nodes.start.emit('click');h.nodes.start.emit('click');assert.equal(calls.length,1);h.nodes.cancel.emit('click');assert.equal(calls[0].owner.signal.aborted,true);assert.equal(calls[0].owner.isCurrent(),false);held.resolve({session:calls[0].s,stageId:1,difficulty:1,nativeTainted:true,normalModeClear:false});await flush();assert.equal(h.root.hidden,true);h.controller.dispose();h.opener.emit('click');assert.equal(h.root.hidden,true);
});
test('restart uses actual owned params instead of edited form, and Escape isolates gameplay input',async()=>{
 const h=harness();h.availability.inRun=true;h.availability.practiceSession=create({stage:15,section:156,phase:2},4);await open(h);h.nodes.replace.checked=true;h.nodes.restart.emit('click');await flush();assert.equal(h.calls[0].s.params.section,156);assert.equal(h.calls[0].s.params.phase,2);await open(h);assert.equal(h.root.emit('keydown',{code:'KeyZ'}).propagationStopped,true);h.root.emit('keydown',{code:'Escape'});assert.equal(h.root.hidden,true);h.controller.dispose();
});
const source=fs.readFileSync(new URL('../../site/play.mjs',import.meta.url),'utf8').replace(/\r\n/g,'\n');
function productionCaller(){const a=source.indexOf('function thpracStageSelection('),b=source.indexOf('function installPracticeMenu(',a);assert(a>=0&&b>a,'actual current page TH128 thprac caller is missing');return source.slice(a,b);}
function callerHarness({held=null,badReceipt=false}={}){const calls=[],menu={launching:false,launchId:0},state={stageCatalog:{},handoff:false,pendingCarryToken:0,worldMode:false,active:false,paused:false},controller=new AbortController();const env={state,menu,DOMException,createThprac128Session:method('createThprac128Session'),thprac128RuntimeSupported:()=>true,readThprac128Native:(m,s)=>{if(badReceipt)throw Error('actual native practice receipt rejected');return{generation:1};},find:()=>({disabled:false}),prepareStage:(id,options)=>({stage:{id},...options}),lastCheckWorldError:'',clock:{reset(){}},resetHighFrameratePresentation(){},syncDebugStageJumpControls(){},
 startLaunch:async selection=>{calls.push(selection);menu.launching=true;menu.launchSelection=selection;++menu.launchId;if(held)await held.promise;menu.launching=false;state.stageSelection=selection;state.worldMode=true;state.active=true;},cancelLaunch(){calls.push('cancel');menu.launching=false;++menu.launchId;}};
 const run=new Function(...Object.keys(env),'let practiceLaunchOwner=null;'+functionSource('inheritThpracTools')+productionCaller()+';return startThprac128Session;')(...Object.values(env));return{run,calls,state,menu,controller,owner:{signal:controller.signal,isCurrent:()=>!controller.signal.aborted}};}
test('actual current page consumer passes complete selected Boss/phase/resources into existing fresh start',async()=>{
 const h=callerHarness(),s=create({stage:15,section:156,phase:3,motivation:54321},4),r=await h.run(s,h.owner),selection=h.calls[0];assert.equal(selection.stage.id,16);assert.equal(selection.thprac.params.section,156);assert.equal(selection.thprac.params.phase,3);assert.equal(selection.thprac.params.motivation,54321);assert.equal(selection.carryToken,undefined);assert.equal(selection.debugJump,undefined);assert.equal(r.nativeTainted,true);assert.equal(r.normalModeClear,false);
});
test('actual caller cancels matching pending start, rejects repeat, and ignores a late successor',async()=>{
 const held=deferred(),h=callerHarness({held}),s=create(),pending=h.run(s,h.owner);await assert.rejects(h.run(s,h.owner),/pending|正在|busy|already/i);h.controller.abort();assert.equal(h.calls.filter(x=>x==='cancel').length,1);held.resolve();await assert.rejects(pending,e=>e.name==='AbortError');
 const held2=deferred(),f=callerHarness({held:held2}),p=f.run(s,f.owner);f.menu.launchSelection={foreign:true};++f.menu.launchId;f.controller.abort();assert.equal(f.calls.includes('cancel'),false);held2.resolve();await assert.rejects(p,e=>e.name==='AbortError');
});
test('native readback rejection stops only the owned selection and releases launch ownership for retry',async()=>{
 const h=callerHarness({badReceipt:true}),s=create();await assert.rejects(h.run(s,h.owner),/receipt/);assert.equal(h.state.active,false);assert.equal(h.state.paused,true);await assert.rejects(h.run(s,h.owner),/receipt/);assert.equal(h.calls.length,2);
});
