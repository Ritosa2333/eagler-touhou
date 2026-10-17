// Mounted UI and native-report fixtures. No game-world restoration claim.
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
 await build({absWorkingDir:project,stdin:{resolveDir:project,loader:'ts',contents:`
 export * from './app/components/player/SpectatorPlayback.tsx';
 export * from './src/launcher/spectator-playback.mts';
 export * from './app/i18n.tsx';`},bundle:true,format:'esm',platform:'node',jsx:'automatic',outfile,logLevel:'silent',
 plugins:[{name:'shared-react',setup(ctx){ctx.onResolve({filter:/^[^./]/},args=>({path:pathToFileURL(require.resolve(args.path)).href,external:true}));}}]});
 views=await import(pathToFileURL(outfile).href);
});
afterEach(async()=>{for(const root of mounts.splice(0))await React.act(async()=>root.unmount());env.document.body.replaceChildren();assert.deepEqual(env.errors.splice(0),[]);});
after(async()=>{env?.close();if(work)await rm(work,{recursive:true,force:true});});
const report=(extra={})=>({version:1,run:'session_1',revision:1,min:600,position:600,received:1600,edge:1510,target:1200,
 paused:false,seeking:true,following:false,ended:false,nodes:[{id:1,kind:'stage',frame:600,stage:4,bossSlots:0},{id:2,kind:'boss',frame:1200,stage:4,bossSlots:3}],...extra});
const owner={epoch:3,game:'th06',launched:true,spectator:true};
async function fixture(locale='zh-CN'){
 const model=views.createSpectatorPlayback(),calls=[];
 const container=env.document.createElement('div');env.document.body.append(container);const root=createRoot(container);mounts.push(root);
 const session={spectatorPlayback:model,async controlSpectator(...args){calls.push(args);}};
 await React.act(async()=>root.render(React.createElement(views.LocaleProvider,{locale},React.createElement(views.SpectatorPlayback,{session}))));
 return {model,calls,container,session,async update(value,context=owner){await React.act(async()=>model.update(context,value));},async click(selector){await React.act(async()=>container.querySelector(selector).click());}};
}
for(const locale of ['zh-CN','en'])test(`${locale}: hidden until native recovery is ready, stage/Boss targets and local pause/live controls`,async()=>{
 const f=await fixture(locale);assert.equal(f.container.textContent,'');await f.update(report());
 assert.ok(f.container.querySelector('section'));await f.click('.spectator-playback-toggle');
 const nodes=[...f.container.querySelectorAll('.spectator-playback-nodes button')];assert.equal(nodes.length,2);
 assert.equal(nodes[1].textContent,locale==='en'?'Stage 4 Boss':'第 4 面 Boss');
 await React.act(async()=>nodes[1].click());assert.deepEqual(f.calls.pop(),['seek',1200,'session_1']);
 await f.click('.spectator-playback-actions button');assert.deepEqual(f.calls.pop(),['pause',undefined,'session_1']);
 assert.ok(f.container.textContent.includes(locale==='en'?'Seeking':'正在定位'),'commands do not optimistically change native state');
 await f.update(report({revision:2,paused:true}));await f.click('.spectator-playback-actions button');assert.equal(f.calls.pop()[0],'play');
 await f.click('.spectator-playback-actions button:last-child');assert.equal(f.calls.pop()[0],'live');
 await f.update(null,{...owner,epoch:4,game:'th09'});assert.equal(f.container.textContent,'');
});
test('dragging freezes received bounds, then validates the target against the latest native edge',async()=>{
 const f=await fixture();await f.update(report());await f.click('.spectator-playback-toggle');const range=f.container.querySelector('input');
 await React.act(async()=>range.dispatchEvent(new env.window.PointerEvent('pointerdown',{bubbles:true,pointerId:1})));
 await f.update(report({revision:2,received:1700,edge:1610}));assert.equal(range.max,'1510');
 await React.act(async()=>range.dispatchEvent(new env.window.PointerEvent('pointerup',{bubbles:true,pointerId:1})));
 assert.deepEqual(f.calls.pop(),['seek',1200,'session_1']);assert.equal(range.max,'1610');
});
