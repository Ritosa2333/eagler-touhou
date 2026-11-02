import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import vm from 'node:vm';
import {TitleScenes} from '../src/title-scenes.mjs';
import {PracticeOverlay} from '../src/practice/overlay.mjs';
import {messageTextPoses} from '../src/renderer.mjs';

test('replay pause third row restarts the selected recording and original selected stage',()=>{
  const model=new TitleScenes();for(let i=0;i<200;i++)model.tick({held:0,pressed:0});model.drainActions();
  model.run={mode:'replay',replayId:'th12_02.rpy',stage:3,character:1,shot:0,difficulty:2};
  model.openPause({...model.run,stage:5});for(let i=0;i<12;i++)model.tick({held:0,pressed:0});
  assert.equal(model.state.choices[2].id,'replay-again');model.choose(2);
  for(let i=0;i<30;i++)model.tick({held:0,pressed:0});model.choose(0);
  for(let i=0;i<30;i++)model.tick({held:0,pressed:0});
  const commands=model.drainActions();assert.deepEqual(commands.filter(a=>a.type==='playReplay'),[{type:'playReplay',id:'th12_02.rpy',stage:3}]);
  assert.equal(commands.some(a=>a.type==='returnTitle'),false);
});

test('hidden Replay workspace overlay retains THPrac playback core without initializing interactive ImGui',async()=>{
  const states=[],overlay=Object.create(PracticeOverlay.prototype);
  Object.assign(overlay,{enabled:true,locale:'zh-CN',root:{hidden:false},bridge:{setEnabled:value=>states.push(value)},cancel(){}});
  overlay.configure({enabled:false,coreEnabled:true});await overlay.initialize(new Map());
  assert.deepEqual(states,[true]);assert.equal(overlay.enabled,false);assert.equal(overlay.root.hidden,true);
  overlay.configure({enabled:false});assert.deepEqual(states,[true,false]);
});

test('retired logical MSG rectangles never resurrect through legacy text fallback',()=>{
  const message={active:true,textRecords:[{textHex:'82a0',style:0,color:0xffffff}]};
  const atlases={text:{sprites:[{w:354,h:17}]}};
  assert.equal(messageTextPoses(message,[],atlases).length,1);
  assert.deepEqual(messageTextPoses(message,[],atlases,{logicalText:true}),[]);
});

function playbackFrame(codes,{tools=true,replay=true}={}){
  const source=fs.readFileSync(new URL('../site/game.mjs',import.meta.url),'utf8'),start=source.indexOf('function frame(now){'),end=source.indexOf('\nfunction eventKey(',start);
  let ticks=0;const keys=new Set(codes),TICK=1000/60;
  const context={TICK,terminated:false,backgrounded:false,fatalError:false,runtimeOptions:{thpracEnabled:tools,replayViewer:false},lastPresentation:0,last:0,meter:{frame(){},value:60},loadingScreen:{advance(){},renderOverlay(){},waiting:false},ready:true,busy:false,session:{replay:replay?{}:null},model:{state:{scene:'playing'},actions:[]},profile:{settings:{keyMap:{shoot:'KeyZ',focus:'ShiftLeft',skip:'ControlLeft'}}},previousPadUI:0,keyDown:key=>keys.has(key),practiceBridge:{interval:({fast,slow})=>fast?1/180:slow?1/15:1/60},logicalStep:TICK,accumulator:0,tick(){ticks++;},present(){},launched:false,requestAnimationFrame(){},fail(error){throw error;}};
  vm.runInNewContext(source.slice(start,end)+'\nframe(1000/60+.000001);',context);return {ticks,step:context.logicalStep};
}
for(const tools of [false,true])test(`live replay controls advance every recorded tick (THPrac ${tools})`,()=>{
  assert.equal(playbackFrame([],{tools}).ticks,1);
  assert.equal(playbackFrame(['KeyZ'],{tools}).ticks,4);
  assert.equal(playbackFrame(['KeyZ','ControlLeft'],{tools}).ticks,4,'speed controls do not multiply each other');
  assert.equal(playbackFrame(['ShiftLeft'],{tools}).step,1000/15);
  assert.equal(playbackFrame(['ControlLeft'],{tools}).ticks,tools?3:4);
  assert.equal(playbackFrame(['KeyZ'],{tools,replay:false}).ticks,1);
});
