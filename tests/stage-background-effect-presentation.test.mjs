import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import crypto from 'node:crypto';
import {parseReplay} from '../src/replay.mjs';
import {initializeCandidate,restoreStage,state} from '../tools/replay-verifier/capture-candidate.mjs';
import {Renderer,readCoreAnimationPoses,readCoreMeshes} from '../src/renderer.mjs';
import {readCoreDrawSchedule,orderPresentation} from '../src/draw-scheduler.mjs';
import {StageBackground} from '../src/stage-background.mjs';
import {AnimationSampler} from '../src/animation.mjs';

test('th12_14 stage05 STD17 registers sixteen real text81 strips with native layer28 scheduling and render-target UV',async()=>{
 const bytes=fs.readFileSync(new URL('../site/replays/th12_14.rpy',import.meta.url));
 assert.equal(crypto.createHash('sha256').update(bytes).digest('hex'),
  'c92747c90ceacc9907440dd03b00afd88ef206bc76abfe1e64b7bc1de24ab516');
 const replay=parseReplay(bytes),stage=replay.stages.find(s=>s.number===5),core=await initializeCandidate(5);
 restoreStage(core,replay,stage);core._th12_oracle_fixture(0);
 for(let tick=0;tick<=2564;tick++){
  const input=replay.input(5,tick);core._th12_tick(input.held,input.pressed);
  if(![2563,2564].includes(tick))continue;
  const snapshot=state(core),poses=readCoreAnimationPoses(core,5),meshes=readCoreMeshes(core,5),schedule=readCoreDrawSchedule(core);
  const stripes=orderPresentation([...poses,...meshes],schedule).filter(p=>p.mesh&&(p.owner>>>24)===0xcf);
  if(tick===2563){assert.equal(stripes.length,0);continue;}
  assert.equal(snapshot.background.backgroundMode,1);
  assert.equal(stripes.length,16,'original40ff10 creates columns-1 strip VMs');
  assert.deepEqual(stripes.map(p=>p.owner),Array.from({length:16},(_,i)=>0xcf000000+i),'native registration order');
  for(const stripe of stripes){
   assert.equal(stripe.primitive,5);assert.equal(stripe.a.name,'text');assert.equal(stripe.a.entry,2);
   assert.equal(stripe.a.sprite,75);assert.equal(stripe.a.layer,28);assert.equal(stripe.drawPriority,14);
   assert.equal(stripe.a.blend,0);assert.equal(stripe.clip,2);assert.equal(stripe.vertices.length,14);
   assert.ok(schedule.has(stripe.id),'strip keeps the native Primary node schedule');
   assert.ok(!poses.some(p=>p.id===stripe.id),'external mode12 does not duplicate as a sprite quad');
   assert.ok(stripe.vertices.every(v=>v.z===0&&v.rhw===1&&v.color===0xc0ffffff));
  }
  assert.deepEqual(stripes[0].vertices.slice(0,2).map(v=>[v.x,v.y,v.u,v.v]),
   [[32,16,Math.fround(.05),Math.fround(16/480)],[56,16,Math.fround(.0875),Math.fround(16/480)]]);
  const faults=new Uint32Array(core.HEAPU8.buffer,core._th12_unsupported(),1024);
  assert.equal(faults[982],0,'the genuine STD17 wave path is restored');
  // Execute the actual Renderer orchestration with a recorded WebGL boundary.
  // Shader compilation and pixels are outside this candidate integration test.
  const events=[],renderer=Object.create(Renderer.prototype),gl={};
  for(const [i,name] of ['SCISSOR_TEST','TEXTURE_2D','RGBA','RGB','TEXTURE_MIN_FILTER','TEXTURE_MAG_FILTER',
   'TEXTURE_WRAP_S','TEXTURE_WRAP_T','REPEAT','LINEAR','NEAREST','CLAMP_TO_EDGE','TRIANGLES','LINE_STRIP',
   'LINES','ARRAY_BUFFER','STREAM_DRAW','SRC_ALPHA','ONE_MINUS_SRC_ALPHA','ONE','ZERO','ONE_MINUS_SRC_COLOR',
   'ONE_MINUS_DST_COLOR','DST_COLOR','COLOR_BUFFER_BIT'].entries())gl[name]=i+1;
  for(const name of ['viewport','clearColor','clear','bindVertexArray','bindBuffer','enable','disable','scissor',
   'uniform3fv','bindTexture','texParameteri','blendFunc','bufferData','drawArrays','copyTexImage2D'])
   gl[name]=(...args)=>events.push([name,...args]);
  gl.createTexture=()=>({created:true});renderer.gl=gl;renderer.batch=[];renderer.textures=new Map();
  renderer.solid={texture:{},w:1,h:1};renderer.atlases=JSON.parse(fs.readFileSync(new URL('../site/assets/atlases.json',import.meta.url)));
  for(const [name,bank] of Object.entries(renderer.atlases))for(let entry=0;entry<bank.entries.length;entry++){
   const image=bank.entries[entry];if(image.image)renderer.textures.set(name+':'+entry,{texture:{},w:image.width,h:image.height,defaultWrap:gl.REPEAT});
  }
  renderer.animations=new AnimationSampler(renderer.atlases);
  renderer.background=new StageBackground(JSON.parse(fs.readFileSync(new URL('../site/assets/stage05-background.json',import.meta.url))),renderer.atlases);
  renderer.stageBackgrounds=new Map([[5,renderer.background]]);
  renderer.requestedStage=5;renderer.lastFrame=-1;renderer.dialogueTextures=new Map();
  renderer.render(core);
  const copies=events.filter(e=>e[0]==='copyTexImage2D');assert.equal(copies.length,1,'one background texture copy for sixteen strips');
  assert.deepEqual(copies[0].slice(1),[gl.TEXTURE_2D,0,gl.RGB,0,0,640,480,0]);
  assert.ok(renderer.textures.has('text:2'),'virtual source texture is available to actual mesh rendering');
  assert.ok(events.some(e=>e[0]==='blendFunc'&&e[1]===gl.ONE&&e[2]===gl.ZERO),'native Main81 source copy reaches WebGL');
  assert.ok(events.some(e=>e[0]==='drawArrays'),'wave and ordinary game batches reach recorded draw boundary');
 }
});
