import test from 'node:test';import assert from 'node:assert/strict';
import {Renderer,nativePixel,readCoreMeshes} from '../src/renderer.mjs';
import {asciiGlyphs} from '../src/ascii-presentation.mjs';
import {orderPresentation} from '../src/draw-scheduler.mjs';

function fixture(){
  const HEAPU8=new Uint8Array(65536),v=new DataView(HEAPU8.buffer),h=new Float32Array(HEAPU8.buffer,0,41);h[0]=100;h[4]=100;h[5]=2;h[6]=3;h[39]=1;
  const core={HEAPU8,_th12_hud:()=>0,_th12_hud_size:()=>41,_th12_draw:()=>0,_th12_draw_ptr:()=>512,_th12_draw_stride:()=>48};
  const poses=[{bank:2,sprite:0,layer:3,id:1,owner:5},{bank:0,sprite:16,layer:0,id:2,owner:0x10000001},
    {bank:8,sprite:7,layer:20,id:3,owner:100},{bank:11,sprite:261,layer:17,id:4,owner:101},
    {bank:7,sprite:102,layer:23,id:5,owner:102},{bank:7,sprite:264,layer:10,id:6,owner:0xfffd0003}];
  for(const [i,p]of poses.entries()){
    const at=1024+i*128,F=(j,n)=>v.setFloat32(at+j*4,n,true),I=(j,n)=>v.setInt32(at+j*4,n,true);
    F(0,p.sprite===102?528:224);F(1,p.sprite===102?288:128);F(6,1);F(7,1);F(10,1);F(11,1);F(28,1);
    I(14,0xffffffff);I(15,0xffffffff);I(16,p.bank);I(17,p.sprite);I(18,p.layer);I(20,1);I(21,1);I(24,3);I(26,p.id);I(27,p.owner);
  }
  Object.assign(core,{_th12_anm_draw:()=>poses.length,_th12_anm_draw_ptr:()=>1024,_th12_anm_draw_stride:()=>128,
    _th12_anm_schedule_count:()=>poses.length,_th12_anm_schedule_ptr:()=>4096,_th12_anm_schedule_stride:()=>16});
  for(const [i,p]of poses.entries())[p.id,[9,31,42,36,52,20][i],i,i===4?0:2].forEach((n,j)=>v.setInt32(4096+i*16+j*4,n,true));
  let ptr=8192;const json=(name,state)=>{const bytes=new TextEncoder().encode(JSON.stringify(state)),start=ptr;HEAPU8.set(bytes,start);ptr+=bytes.length+16;core[`_th12_${name}`]=()=>start;core[`_th12_${name}_size`]=()=>bytes.length;};
  json('spell',{flags:3,bonusDisplayed:2000000,titleHex:'82a0',records:[1,0,1,0]});
  json('ufo',{enemyId:7,age:114,duration:720,fill:.5,bucketA:12,bucketB:3,enemy:{id:7,x:40,y:100,z:0,health:120,maxHealth:400},displayFrames:60,multiplier:4,score:40000,rewardX:264,rewardY:160,rewardZ:0});
  // One direct Laser29 mesh, not an ANM-layer mesh.
  [0,16,0,0,0,4,1,29,0xb0000001,73,0,3,1,0].forEach((n,i)=>v.setInt32(16384+i*4,n,true));
  for(let i=0;i<3;i++){v.setFloat32(17408+i*28,224+i,true);v.setFloat32(17412+i*28,128,true);v.setFloat32(17420+i*28,1,true);v.setUint32(17424+i*28,0xffffffff,true);}
  Object.assign(core,{_th12_mesh_draw:()=>1,_th12_mesh_draw_ptr:()=>16384,_th12_mesh_draw_stride:()=>56,
    _th12_mesh_vertices_ptr:()=>17408,_th12_mesh_vertices_count:()=>3,_th12_mesh_vertex_stride:()=>28});
  return core;
}
test('renderer draws the missing texts and retains native layers without clipping the UFO right panel',()=>{
  const events=[],r=Object.create(Renderer.prototype),state={clipped:false,box:null};
  r.gl={SCISSOR_TEST:1,TRIANGLES:4,COLOR_BUFFER_BIT:16,enable:()=>state.clipped=true,disable:()=>state.clipped=false,scissor:(...box)=>state.box=box,uniform3fv(){},clearColor(){},clear(){}};
  r.begin=()=>{};r.flush=()=>{};r.primitive=()=>{};r.stageBackgrounds=new Map([[1,{geometry:()=>({camera:{fog:[0,0,0]},triangles:[]})}]]);r.stageJobs=new Map();r.textures=new Map();r.atlases={text:{sprites:{}}};r.lastFrame=-1;
  r.animations={beginFrame(){},endFrame(){},sampleAll(name,script){const sprite=script===0?1:script,layer=script===0?21:23;return [{name,sprite,layer,visible:true,alpha:1,sx:1,sy:1,r:1,g:1,b:1}];}};
  r.state=(a)=>events.push({label:`${a.name}:${a.sprite}`,clipped:state.clipped,box:state.box});
  r.dialogueLine=p=>events.push({label:'spell-title',text:p.text,clipped:state.clipped});
  r.nativeText=p=>events.push({label:`text:${p.text}`,font:p.font,clipped:state.clipped});
  r.mesh=()=>events.push({label:'laser',clipped:state.clipped});r.hudCounters=()=>events.push({label:'hud',clipped:state.clipped});
  r.render(fixture());
  const index=label=>events.findIndex(e=>e.label===label),get=label=>events[index(label)];
  assert.ok(index('stgenm01:0')<index('laser'));assert.ok(index('laser')<index('bullet:16'));
  assert.ok(index('bullet:16')<index('spell-title'));assert.equal(get('spell-title').text,'あ');
  assert.ok(index('spell-title')<index('text: 2000000'));assert.ok(index('text: 2000000')<index('front:1'));
  assert.ok(index('front:1')<index('front:102'));assert.ok(index('front:102')<index('text: 12'));
  for(const label of ['text: 2000000','text:00/01','text: 12','text:  3','text: 50%','text:*4.0','text:40,000','text:10:10'])assert.ok(index(label)>=0,label);
  assert.equal(get('front:102').clipped,false);assert.equal(get('text: 12').clipped,false);assert.equal(get('bullet:16').clipped,true);
  assert.deepEqual(get('bullet:16').box,[13,0,422,480]);
  assert.equal(events.at(-1).label,'hud');
});
test('glyph rendering uses original Failed and timer sprites, ARGB alpha and native rounding',()=>{
  const calls=[],r=Object.create(Renderer.prototype);r.atlases={ascii:{sprites:Object.fromEntries([227,228,238,241,262].map(id=>[id,{entry:0,x:0,y:0,w:id===262?32:8,h:10}]))}};r.region=(...args)=>calls.push(args);
  const p={font:2,text:'$01:%',x:10.5,y:20.5,colorARGB:0xc080ffc0};r.nativeText(p);
  const glyphs=asciiGlyphs(p);assert.deepEqual(glyphs.map(p=>p.id),[262,227,228,238,241]);assert.equal(calls.length,5);
  assert.equal(calls[0][11],192/255);assert.deepEqual(calls[0][12],[128/255,1,192/255]);assert.equal(calls[0][14].point,false);assert.equal(calls[0][14].pixel,true);
  assert.deepEqual([.5,1.5,2.5,-.5,-1.5,-2.5].map(nativePixel),[0,2,2,0,-2,-2]);
});
test('direct laser meshes preserve exported manager order and main/cap pairs across VM allocation IDs',()=>{
  const core=fixture(),view=new DataView(core.HEAPU8.buffer),first=core.HEAPU8.slice(16384,16440),ids=[90,91,12,13];
  // captureCustomGeometry exports laserManager.ordered(), with each main
  // immediately followed by its cap. IDs are allocation identity, not order.
  for(let i=0;i<ids.length;i++){core.HEAPU8.set(first,16384+i*56);view.setUint32(16384+i*56+36,ids[i],true);}
  core._th12_mesh_draw=()=>4;
  assert.deepEqual(orderPresentation(readCoreMeshes(core)).map(p=>p.id),ids);
});

test('native diffuse UFO trails use solid color and remain separate line strips',()=>{
  const core=fixture(),view=new DataView(core.HEAPU8.buffer),record=core.HEAPU8.slice(16384,16440);
  for(let i=0;i<2;i++){
    core.HEAPU8.set(record,16384+i*56);const at=16384+i*56;
    for(const [word,value]of [[0,-1],[1,-1],[3,13],[5,3],[6,0],[7,26],[9,80+i],[10,i*3]])view.setInt32(at+word*4,value,true);
    for(let j=0;j<3;j++){
      const vertex=17408+(i*3+j)*28;
      view.setFloat32(vertex,40*i+j,true);view.setFloat32(vertex+4,128+j,true);
      view.setFloat32(vertex+12,1,true);view.setUint32(vertex+16,0x40007fff,true);
    }
  }
  core._th12_mesh_draw=()=>2;core._th12_mesh_vertices_count=()=>6;
  const meshes=readCoreMeshes(core);assert.ok(meshes.every(p=>p.untextured));
  assert.deepEqual(orderPresentation(meshes).map(p=>p.drawPriority),[26,26]);
  const renderer=Object.create(Renderer.prototype),draws=[],textures=[];
  Object.assign(renderer,{gl:{TRIANGLES:4,LINE_STRIP:3},solid:{texture:'white'},textures:new Map(),batch:[],
    material(){},texture(t){textures.push(t);},primitive(mode){this.mode=mode;},
    flush(){if(this.batch.length){draws.push({mode:this.mode,vertices:this.batch.slice()});this.batch.length=0;}}});
  for(const mesh of meshes)renderer.mesh(mesh);
  assert.deepEqual(textures,[renderer.solid,renderer.solid]);
  assert.equal(draws.length,2,'native submits two independent trails without an endpoint connector');
  for(let i=0;i<2;i++){
    assert.equal(draws[i].mode,3);assert.equal(draws[i].vertices.length,30);
    assert.deepEqual(draws[i].vertices.slice(0,10),[40*i,128,.5,.5,0,127/255,1,64/255,1,0]);
  }
});
