import test from 'node:test';import assert from 'node:assert/strict';import {readFileSync} from 'node:fs';import {runInNewContext} from 'node:vm';
import {ImGuiRenderer} from '../src/practice/imgui-renderer.mjs';import {PracticeOverlay} from '../src/practice/overlay.mjs';
import {Renderer} from '../src/renderer.mjs';import {AnimationSampler} from '../src/animation.mjs';import {createRendererFixture} from './helpers/renderer-workload.mjs';
const retail=JSON.parse(readFileSync(new URL('../site/assets/atlases.json',import.meta.url),'utf8'));
test('input-only reset retires interpolation endpoints but keeps deterministic HUD samples',()=>{
 const {renderer}=createRendererFixture(Renderer);renderer.atlases=retail;renderer.animations=new AnimationSampler(retail);
 const first=renderer.animations.sampleAll('front',0,60000),saved=structuredClone(first),sampler=renderer.animations,track=[...sampler.tracks.values()][0],clock=track.tickCount,epoch=renderer.presentationEpoch;
 renderer.resetPresentation({retainSamples:true});assert.equal(renderer.animations,sampler);assert.equal(renderer.presentationEpoch,epoch+1);assert.equal(renderer.lastFrame,-1);
 assert.deepEqual(sampler.sampleAll('front',0,60000),first);assert.equal(track.tickCount,clock);assert.deepEqual(first,saved);
 assert.deepEqual(sampler.sampleAll('front',0,3),new AnimationSampler(retail).sampleAll('front',0,3),'backward sampling still rebuilds the requested track');
 renderer.resetPresentation();assert.notEqual(renderer.animations,sampler,'actual scene changes retire cached state');
});
test('keyboard clearing keeps the same sampled world while releasing every input owner',()=>{
 const source=readFileSync(new URL('../site/game.mjs',import.meta.url),'utf8'),start=source.indexOf('function clearKeys('),end=source.indexOf('\nfunction resetGamePresentation',start),events=[];
 const context={keys:new Set(['KeyZ']),hostKeys:new Set(['ShiftLeft']),assets:{managed:true},managedInput:{clear(){events.push('managed');}},touch:{cancel(){events.push('touch');}},practiceUI:{cancel(){events.push('practice');}},model:{clearKeys(){events.push('menu');}},session:{previous:255},performance:{now:()=>123},meter:{reset(){}},gameMeter:{reset(){}},resetGamePresentation:options=>events.push(options)};
 runInNewContext(source.slice(start,end)+';clearKeys()',context);assert.equal(context.keys.size,0);assert.equal(context.hostKeys.size,0);assert.equal(context.session.previous,0);assert.deepEqual(JSON.parse(JSON.stringify(events.at(-1))),{retainSamples:true});assert.deepEqual(events.slice(0,-1),['managed','touch','practice','menu']);
});
test('next-stage texture warmup allows a display frame between each upload',async t=>{
 const frames=[],tasks=[],closed=[],uploads=[];
 const old={raf:globalThis.requestAnimationFrame,timer:globalThis.setTimeout,bitmap:globalThis.createImageBitmap};
 globalThis.requestAnimationFrame=cb=>frames.push(cb);globalThis.setTimeout=cb=>tasks.push(cb);
 globalThis.createImageBitmap=async blob=>({width:16,height:16,close(){closed.push(blob.id);},id:blob.id});
 t.after(()=>{for(const[key,value]of Object.entries({requestAnimationFrame:old.raf,setTimeout:old.timer,createImageBitmap:old.bitmap})){if(value===undefined)delete globalThis[key];else globalThis[key]=value;}});
 const{renderer}=createRendererFixture(Renderer);renderer.textures.clear();renderer.atlases={next:{entries:Array.from({length:4},(_,i)=>({image:'next-'+i+'.png'}))}};
 renderer.gl.pixelStorei=()=>{};renderer.fetch=async path=>({ok:true,blob:async()=>({id:path})});renderer.gl.texImage2D=(...args)=>uploads.push(args.at(-1).id);
 let done=false;const pending=renderer.loadAtlases(['next'],{background:true}).then(()=>{done=true;});
 const settle=async()=>{for(let i=0;i<20;i++)await Promise.resolve();};await settle();assert.equal(uploads.length,0);
 for(let i=0;i<4;i++){
  assert.equal(frames.length,1);frames.shift()();await settle();assert.equal(uploads.length,i,'frame must complete before upload task');assert.equal(tasks.length,1);tasks.shift()();await settle();assert.equal(uploads.length,i+1);
 }
 await pending;assert.equal(done,true);assert.equal(renderer.textures.size,4);assert.equal(closed.length,4);
 frames.length=tasks.length=uploads.length=0;renderer.textures.clear();await renderer.loadAtlases(['next']);assert.equal(uploads.length,4);assert.equal(frames.length,0,'foreground loading retains its direct path');
});

test('the first display callback after a viewport transition starts without catch-up debt',()=>{
 const source=readFileSync(new URL('../site/game.mjs',import.meta.url),'utf8'),start=source.indexOf('function frame(now){'),end=source.indexOf('\nfunction eventKey(',start),draws=[];
 const context={terminated:false,backgrounded:false,fatalError:false,viewportClockReset:true,last:0,accumulator:1000,presentationCadence:{fastFrames:4,observe(){}},presentationLimiter:{next:5,allows(){return true;}},runtimeOptions:{limitPresentationTo60:true},meter:{value:60,frame:now=>draws.push(now)},logicalStep:1000/60,ready:false,busy:false,launched:false,loadingScreen:{advance:elapsed=>draws.push(elapsed)},requestAnimationFrame(){},fail:e=>{throw e;}};
 runInNewContext(source.slice(start,end)+';frame(2000);globalThis.result={last,accumulator,viewportClockReset};',context);
 assert.deepEqual(JSON.parse(JSON.stringify(context.result)),{last:2000,accumulator:0,viewportClockReset:false});assert.deepEqual(draws,[2000,0]);assert.equal(context.presentationCadence.fastFrames,0);assert.equal(context.presentationLimiter.next,null);
 draws.length=0;runInNewContext('frame(2017)',context);assert.deepEqual(draws,[2017,17],'ordinary wall time resumes immediately and FPS observes actual timestamps');
});

test('empty THPrac frames clear old content once and stop submitting WebGL work',()=>{
 let clears=0,viewports=0;const heap=new Uint8Array(64),renderer=Object.create(ImGuiRenderer.prototype);
 renderer.core={HEAPU8:heap,_th12_practice_ui_draw:()=>0};renderer.gl={viewport(){viewports++;},disable(){},clearColor(){},clear(){clears++;}};
 for(let i=0;i<120;i++)assert.equal(renderer.render(),false);assert.equal(clears,0);assert.equal(viewports,0);
 renderer.hasContents=true;assert.equal(renderer.render(),false);assert.equal(clears,1);assert.equal(viewports,1);
 for(let i=0;i<120;i++)renderer.render();assert.equal(clears,1,'an empty canvas stays untouched after retiring its old widgets');
});
test('THPrac overlay is absent from compositing until native widgets become visible',()=>{
 const overlay=Object.create(PracticeOverlay.prototype);let visible=false,layouts=0;overlay.initialized=overlay.enabled=true;overlay.root={hidden:false};overlay.renderer={render:()=>visible};overlay.layout=()=>layouts++;
 for(let i=0;i<120;i++)overlay.render();assert.equal(overlay.root.hidden,true);assert.equal(layouts,0);
 visible=true;overlay.render();assert.equal(overlay.root.hidden,false);assert.equal(layouts,1);
 visible=false;overlay.render();assert.equal(overlay.root.hidden,true);assert.equal(layouts,1);
 visible=true;overlay.render();assert.equal(overlay.root.hidden,false);assert.equal(layouts,2,'returning native UI restores its current layout');
});
