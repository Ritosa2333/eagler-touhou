import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {runInNewContext} from 'node:vm';
import {createPresentationFrameRateMeter,fpsTextDescriptors} from '../src/fps-presentation.mjs';
import {AnimationSampler} from '../src/animation.mjs';
import {Renderer} from '../src/renderer.mjs';
const endpoint=(frame,epoch=1)=>({stage:1,epoch,frame,nativeMotion:{}});

test('60 redraws per second with 20 new worlds report 20 FPS, then a network stall reports zero',()=>{
  const meter=createPresentationFrameRateMeter();meter.reset(0);
  for(let i=1;i<=180;i++)meter.frame(i*1000/60,null,endpoint(Math.floor(i/3)));
  assert(Math.abs(meter.value-20)<1);
  for(let i=181;i<=360;i++)meter.frame(i*1000/60,null,endpoint(60,i));
  assert.equal(meter.value,0,'repeated corrected snapshots and new renderer epochs do not invent advancing game frames');
  assert.equal(fpsTextDescriptors({currentFps:meter.value})[0].text,'0.0fps');
});

test('interpolated high refresh counts distinct subframes; redrawing without interpolation counts the 60 Hz world',()=>{
  for(const hz of [120,144,240])for(const interpolate of [false,true]){
    const meter=createPresentationFrameRateMeter();meter.reset(0);
    for(let i=1;i<=hz*3+2;i++){
      const position=i*60/hz,frame=Math.floor(position),alpha=position-frame;
      meter.frame(i*1000/hz,endpoint(frame-1),endpoint(frame),interpolate?alpha:1);
    }
    assert(Math.abs(meter.value-(interpolate?hz:60))<1.1,`${hz} Hz interpolate=${interpolate}: ${meter.value}`);
  }
});

test('catch-up ticks do not multiply visible frames; paused menu frames retain their refresh rate',()=>{
  const meter=createPresentationFrameRateMeter();meter.reset(0);
  for(let i=1;i<=32;i++)meter.frame(i*100,null,endpoint(i*6));
  assert.equal(meter.value,10);
  for(let i=1;i<=180;i++)meter.frame(3200+i*1000/60,null,endpoint(192),1,false);
  assert(Math.abs(meter.value-60)<1);
});

test('the actual game drawing boundary passes advancing-world FPS instead of callback FPS',()=>{
  const source=readFileSync(new URL('../site/game.mjs',import.meta.url),'utf8'),start=source.indexOf('function drawGamePresentation('),end=source.indexOf('\nfunction updatePracticeUI',start);
  const gameMeter=createPresentationFrameRateMeter();gameMeter.reset(0);
  let now=0;const seen=[],context={gameMeter,currentPresentation:endpoint(10),previousPresentation:null,accumulator:0,logicalStep:1000/60,
    model:{state:{scene:'playing'}},performance:{now:()=>now},interpolateGamePresentation:()=>false,
    captureGamePresentation(){},renderer:{renderPresentation:(p,c,alpha,live)=>seen.push(live.currentFps)}};
  runInNewContext(source.slice(start,end)+';globalThis.draw=drawGamePresentation;',context);
  for(let i=1;i<=180;i++){now=i*1000/60;context.draw();}
  assert.equal(seen.at(-1),0);
  context.model.state.scene='pause';
  for(let i=181;i<=360;i++){now=i*1000/60;context.draw();}
  assert(Math.abs(seen.at(-1)-60)<1);
});

test('waiting for input holds the exact endpoint without rebuilding the native presentation',()=>{
  const source=readFileSync(new URL('../site/game.mjs',import.meta.url),'utf8'),start=source.indexOf('function captureGamePresentation('),end=source.indexOf('\nfunction drawGamePresentation',start);
  let captures=0;const original=endpoint(10),context={session:{mode:'multiplayer',stage:1,clock:10},ready:true,currentPresentation:original,previousPresentation:endpoint(9),
    renderer:{capturePresentation:()=>{captures++;return endpoint(context.session.clock);}},core:{_th12_score_stored:()=>0},warmNextStage(){},gamePresentationOptions:()=>({})};
  runInNewContext(source.slice(start,end)+';globalThis.capture=captureGamePresentation;',context);
  for(let i=0;i<60;i++)context.capture();
  assert.equal(captures,0);assert.equal(context.previousPresentation,original);assert.equal(context.currentPresentation,original);
  context.session.clock=11;context.capture();assert.equal(captures,1);assert.equal(context.currentPresentation.frame,11);
  context.currentPresentation=null;context.capture();assert.equal(captures,2,'a rollback reset always rebuilds corrected poses');
});

test('rollback keeps immutable age sampling caches and rebuilds only a track that seeks backward',()=>{
  const atlases={front:{scripts:{0:{commands:[]}}}},sampler=new AnimationSampler(atlases),renderer={atlases,animations:sampler,presentationEpoch:1,lastFrame:600};
  sampler.sample('front',0,600);const track=[...sampler.tracks.values()][0];
  Renderer.prototype.resetPresentation.call(renderer,{retainSamples:true});
  assert.equal(renderer.animations,sampler);assert.equal(renderer.presentationEpoch,2);assert.equal(renderer.lastFrame,-1);
  sampler.sample('front',0,601);assert.equal([...sampler.tracks.values()][0],track);assert.equal(track.tickCount,602,'the corrected HUD needs one tick, not 602 replayed ticks');
  assert.deepEqual(sampler.sample('front',0,10),new AnimationSampler(atlases).sample('front',0,10));
  assert.notEqual([...sampler.tracks.values()][0],track,'an actual age rewind reconstructs its own deterministic sample');
  Renderer.prototype.resetPresentation.call(renderer);assert.notEqual(renderer.animations,sampler,'stage/loadout changes replace the atlas sampler');
});
