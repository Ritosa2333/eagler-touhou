// Mounted adapters and the shared TH09-derived view. No native world restore claim.
import test,{before,after,afterEach} from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp,rm} from 'node:fs/promises';
import {createRequire} from 'node:module';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {build} from 'esbuild';
import {installMountedDom} from './mounted-dom-environment.mjs';
const project=resolve(import.meta.dirname,'../..'),require=createRequire(resolve(project,'package.json'));
let env,work,React,createRoot,views;const mounts=[];
before(async()=>{
 env=installMountedDom();React=await import('react');({createRoot}=await import('react-dom/client'));
 work=await mkdtemp(resolve(project,'.cache/spectator-ui-'));const outfile=resolve(work,'views.mjs');
 await build({absWorkingDir:project,stdin:{resolveDir:project,loader:'ts',contents:[
 "export * from './app/components/player/SpectatorPlayback.tsx';",
 "export * from './src/launcher/spectator-playback.mts';",
 "export * from './app/i18n.tsx';"].join('\n')},bundle:true,format:'esm',platform:'node',jsx:'automatic',outfile,logLevel:'silent',
 plugins:[{name:'shared-react',setup(ctx){ctx.onResolve({filter:/^[^./]/},args=>({path:pathToFileURL(require.resolve(args.path)).href,external:true}));}}]});
 views=await import(pathToFileURL(outfile).href);
});
afterEach(async()=>{
 for(const root of mounts.splice(0))await React.act(async()=>root.unmount());
 env.document.body.replaceChildren();
 assert.equal(env.document.querySelectorAll('[data-spectator-timeline-view-style]').length,0,'shared styles retire with the last view');
 assert.deepEqual(env.errors.splice(0),[]);
});
after(async()=>{env?.close();if(work)await rm(work,{recursive:true,force:true});});
const report=(extra={})=>({version:1,run:'session_1',revision:1,min:600,position:600,received:1600,edge:1510,target:1200,
 paused:false,seeking:true,following:false,ended:false,nodes:[{id:1,kind:'stage',frame:600,stage:4,bossSlots:0},{id:2,kind:'boss',frame:1200,stage:4,bossSlots:3}],...extra});
const owner={epoch:3,game:'th06',launched:true,spectator:true};
async function fixture(locale='zh-CN'){
 const model=views.createSpectatorPlayback(),calls=[];
 let runtime=owner;
 const container=env.document.createElement('div');env.document.body.append(container);const root=createRoot(container);mounts.push(root);
 const session={baseUrl:'https://example.test/',spectatorPlayback:model,getSnapshot:()=>({runtime}),async controlSpectator(...args){calls.push(args);}};
 await React.act(async()=>root.render(React.createElement(views.LocaleProvider,{locale},React.createElement(views.SpectatorPlayback,{session}))));
 return {model,calls,container,session,async update(value,context=owner){runtime=context;await React.act(async()=>model.update(context,value));},async click(selector){await React.act(async()=>container.querySelector(selector).click());}};
}
for(const locale of ['zh-CN','en'])test(locale+': cooperative data and commands use the shared TH09 view',async()=>{
 const f=await fixture(locale);assert.equal(f.container.textContent,'');await f.update(report());
 const panel=f.container.querySelector('section.spectator-timeline');assert.ok(panel);
 assert.equal(panel.dataset.collapsed,'true');
 assert.equal(panel.querySelector('[id$="Score"]').textContent,locale==='en'?'Stage 4 Boss':'第 4 面 Boss');
 assert.equal(panel.querySelector('[data-icon="toggle"]').getAttribute('src'),'https://example.test/assets/room-caret-right.svg');
 await f.click('[id$="Toggle"]');assert.equal(panel.dataset.collapsed,'false');
 const nodes=[...panel.querySelectorAll('[id$="RoundMarks"] button')];assert.equal(nodes.length,2);
 assert.equal(nodes[1].getAttribute('aria-label'),(locale==='en'?'Stage 4 Boss':'第 4 面 Boss')+' · 0:10');
 assert.equal(nodes[1].dataset.nodeKind,'boss');assert.equal(nodes[1].dataset.kind,'round');
 await React.act(async()=>nodes[1].click());assert.deepEqual(f.calls.pop(),['seek',1200,'session_1']);
 await f.click('[id$="Pause"]');assert.deepEqual(f.calls.pop(),['pause',undefined,'session_1']);
 assert.ok(panel.textContent.includes(locale==='en'?'Seeking':'正在定位'),'commands wait for authoritative native reports');
 await f.update(report({revision:2,paused:true}));await f.click('[id$="Pause"]');assert.equal(f.calls.pop()[0],'play');
 await f.click('[id$="Live"]');assert.equal(f.calls.pop()[0],'live');
 await f.update(null,{...owner,epoch:4,game:'th09'});assert.equal(f.container.textContent,'');
});
test('shared touch release expands once and suppresses the compatibility click',async()=>{
 const f=await fixture();await f.update(report());const panel=f.container.querySelector('.spectator-timeline');
 const bar=panel.querySelector('.spectator-bar'),toggle=panel.querySelector('[id$="Toggle"]');
 await React.act(async()=>bar.dispatchEvent(new env.window.PointerEvent('pointerup',{bubbles:true,pointerId:3,pointerType:'touch'})));
 assert.equal(panel.dataset.collapsed,'false');
 await React.act(async()=>toggle.dispatchEvent(new env.window.MouseEvent('click',{bubbles:true,detail:1})));
 assert.equal(panel.dataset.collapsed,'false');
});
test('encoded route keys remain seek identities without becoming visible stage numbers',async()=>{
 const f=await fixture('en');await f.update(report({position:1300,target:1300,nodes:[
  {id:1,kind:'stage',frame:600,stage:257,bossSlots:0},{id:2,kind:'boss',frame:1200,stage:257,bossSlots:2}]}));
 const panel=f.container.querySelector('.spectator-timeline');assert.equal(panel.querySelector('[id$="Score"]').textContent,'Boss');
 await f.click('[id$="Toggle"]');const stage=panel.querySelector('[data-node-kind="stage"]');
 assert.equal(stage.getAttribute('aria-label'),'Stage · 0:00');assert.ok(!panel.textContent.includes('257'));
 await React.act(async()=>stage.click());assert.deepEqual(f.calls.pop(),['seek',600,'session_1']);
});
test('shared dragging freezes bounds and displayed time, then clamps against the latest native edge',async()=>{
 const f=await fixture();await f.update(report());await f.click('[id$="Toggle"]');const range=f.container.querySelector('input');
 await React.act(async()=>{
  range.dispatchEvent(new env.window.PointerEvent('pointerdown',{bubbles:true,pointerId:1}));
  range.value='1300';range.dispatchEvent(new env.window.Event('input',{bubbles:true}));
 });
 await f.update(report({revision:2,received:1700,edge:1610,target:1250}));
 assert.equal(range.min,'600');assert.equal(range.max,'1510');assert.equal(range.value,'1300');assert.equal(range.getAttribute('aria-valuetext'),'0:11');
 await React.act(async()=>range.dispatchEvent(new env.window.Event('change',{bubbles:true})));
 assert.deepEqual(f.calls.pop(),['seek',1300,'session_1']);assert.equal(range.max,'1610');
 assert.equal(range.value,'1250','the command does not change native position optimistically');
});
test('keyboard range change dispatches once, pointer cancel dispatches nothing',async()=>{
 const f=await fixture();await f.update(report());const range=f.container.querySelector('input');
 await React.act(async()=>{
  range.value='1260';range.dispatchEvent(new env.window.Event('input',{bubbles:true}));
  range.dispatchEvent(new env.window.Event('change',{bubbles:true}));
  range.dispatchEvent(new env.window.KeyboardEvent('keyup',{bubbles:true,key:'ArrowRight'}));
 });
 assert.deepEqual(f.calls.splice(0),[['seek',1260,'session_1']]);
 await React.act(async()=>{
  range.dispatchEvent(new env.window.PointerEvent('pointerdown',{bubbles:true,pointerId:1}));
  range.value='1400';range.dispatchEvent(new env.window.Event('input',{bubbles:true}));
  range.dispatchEvent(new env.window.PointerEvent('pointercancel',{bubbles:true,pointerId:1}));
  range.dispatchEvent(new env.window.Event('change',{bubbles:true}));
 });
 assert.deepEqual(f.calls,[]);
});
test('an epoch change cancels an old drag even when its title run string is reused',async()=>{
 const f=await fixture();await f.update(report());const range=f.container.querySelector('input');
 await React.act(async()=>{
  range.dispatchEvent(new env.window.PointerEvent('pointerdown',{bubbles:true,pointerId:1}));
  range.value='1400';range.dispatchEvent(new env.window.Event('input',{bubbles:true}));
 });
 await f.update(report({revision:2}),{...owner,epoch:4});
 await React.act(async()=>range.dispatchEvent(new env.window.Event('change',{bubbles:true})));
 assert.deepEqual(f.calls,[]);
});
test('an obsolete action error cannot leak into the next run',async()=>{
 const f=await fixture();await f.update(report());let rejectAction;
 f.session.controlSpectator=()=>new Promise((_,reject)=>{rejectAction=reject;});
 await f.click('[id$="Pause"]');
 await f.update(report({run:'session_2',revision:1}));
 await React.act(async()=>rejectAction(new Error('obsolete failure')));
 assert.equal(f.container.querySelector('[role="alert"]').hidden,true);
 assert.ok(!f.container.textContent.includes('obsolete failure'));
});
