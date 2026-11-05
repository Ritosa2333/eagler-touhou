import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync,readdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {oracle,core,memory,report,root,d3dxScalar} from './helpers.mjs';
import {installAnm,normalizedAnm} from './anm-oracle.mjs';

test('Complete match wiring retains original scene, backgrounds, CPU, HUD and round transitions',async()=>{
 const m=await oracle(),c=await core(),f=c.world_fixture(),name=c.allocate(128),data=c.allocate(16000000),scratch=c.allocate(88),files=new Map();
 const source=n=>{if(!files.has(n))files.set(n,readFileSync(resolve(root,'reference/assets',n)));return files.get(n);};
 for(const n of readdirSync(resolve(root,'reference/assets')).filter(n=>/\.(anm|sht|ecl|msg|std|bmp|png)$/.test(n))){const b=source(n);memory(c,name,128).fill(0);memory(c,name,128).set(new TextEncoder().encode(n));memory(c,data,b.length).set(b);c.world_file(f,name,data,b.length);}
 const arg=i=>m.u32(m.reg('ESP')+4+i*4),alloc=n=>{const p=m.allocate(n);m.view(p,n).fill(0);return p;};
 const fields=(fn,count,width=3)=>Array.from({length:count()},(_,i)=>Array.from({length:width},(_,j)=>new DataView(c.memory.buffer).getUint32(fn()+i*width*4+j*4,true)));
 const motion=fields(c.motion_fields,c.motion_field_count),control=fields(c.shot_control_fields,c.shot_control_field_count),sceneFields=fields(c.match_scene_fields,c.match_scene_field_count),dialogueFields=fields(c.dialogue_fields,c.dialogue_field_count).filter(x=>x[0]!==0x14),bgFields=fields(c.background_fields,c.background_field_count).filter(x=>![0x2c,0xce0].includes(x[0])),progressFields=fields(c.match_rules_fields,c.match_rules_field_count,2),selectionFields=fields(c.stage_selection_fields,c.stage_selection_field_count);
 const sceneOffsets=[...Array.from({length:7},(_,i)=>12+i*676),0x123f4,0x12698,0x106b4,...Array.from({length:7},(_,i)=>0x10968+i*676),0x11eac,0x12150];
 const err=()=>{const p=c.world_error(f),bytes=memory(c,p,256);return new TextDecoder().decode(bytes.subarray(0,bytes.indexOf(0)));};
 let anms=[],checks=0,frames=0,roundEnds=0;const scenarios=process.env.TH09_WORLD_CASE?[process.env.TH09_WORLD_CASE.split(',').map(Number)]:[[0,1,2,1],[4,0,0,1],[8,0,1,1],[12,13,2,1],[14,15,2,0]];
 d3dxScalar(m);
 m.replace(0x47b24e,'world-new',()=>alloc(arg(0)));m.replace(0x47b249,'world-free',()=>0);
 m.replace(0x401660,'world-animation',()=>{assert.ok(anms[arg(0)],'ANM slot '+arg(0));return anms[arg(0)].file;},1);
 m.replace(0x43c7a0,'world-animation-load',()=>{assert.ok(anms[arg(0)],'ANM load '+arg(0));return anms[arg(0)].file;},2);
 m.replace(0x42c970,'world-read',()=>{const bytes=source(m.string(m.reg('ECX'),128)),p=alloc(bytes.length);m.write(p,bytes);if(m.reg('EDX'))m.u32(m.reg('EDX'),bytes.length);return p;},1);
 const imports=m.onImport;m.onImport=e=>{if(e.name==='timeGetTime'){m.ret(1000,0);return;}imports(e);};
 for(const [a,n] of [[0x41a796,0],[0x41a8a2,0],[0x415910,0],[0x4217e0,0],[0x4307c0,0],[0x41cde0,0],[0x4316b0,0],[0x4304a0,0],[0x4343e0,0],[0x42b130,1],[0x42b160,1],[0x42fc20,1],[0x42fd90,1],[0x43be50,0],[0x43bdc0,0],[0x4346a0,4],[0x43e380,2],[0x43e2f0,2],[0x42fe20,1],[0x43bf30,1],[0x40e3d0,1]])m.replace(a,'world-platform',()=>0,n);
 m.replace(0x431930,'world-music',()=>{for(let n=0;n<19;++n)if(m.i32(0x4a21d0+n*16)===(arg(0)|0)){m.i32(0x4dc690,m.i32(0x4a21d8+n*16));break;}return 0;},1);m.replace(0x42fd30,'world-stop-music',()=>{return 0;});
 function compareAnimation(cpp,native,slot,label){const a=anms[slot]??{file:0,source:0,sprites:0},p=[0,1,2].map(i=>c.world_resource(f,slot,i));assert.deepEqual(normalizedAnm(memory(c,cpp,676),...p),normalizedAnm(m.bytes(native,676),a.file,a.source,a.sprites),label);}
 const heap=m.heap;
 try{
  for(const [left,right,mode,controller] of scenarios){
   m.heap=heap;anms=[];m.view(0x4a7d88,0x420).fill(0);m.view(0x4ace18,3*0x8e).fill(0);m.view(0x4acfc8,64).fill(0);m.view(0x4b3178,0x2d0).fill(0);m.view(0x4b3488,0xcc).fill(0);m.u32(0x4b36d4,0);m.f32(0x4b36b8,1);m.u32(0x4ac884,0);m.u32(0x4b3690,0);m.i32(0x4dc690,-1);
   const scores=[alloc(160),alloc(160)],config=alloc(204);m.u32(0x4b42d0,config);m.u32(0x4a7dac,scores[0]);m.u32(0x4a7de4,scores[1]);m.f32(scores[0],2);m.u32(0x4a7db0,left);m.u32(0x4a7de8,right);m.u32(0x4a7df0,controller);m.u32(0x4a7ea8,mode);m.u32(0x4a7eac,1);m.u32(0x4a7ec8,left);m.u32(0x4ace0c,0x7531);m.u32(0x4ace10,0);
   // The original route-selection routine has its own exhaustive oracle. Here
   // use that selection, including its RNG state, to prepare GPU resources
   // before executing the complete original world constructor.
   const selection=c.stage_selection_fixture(),sp=c.stage_selection_part(selection,0);for(const [a,o,n] of selectionFields)memory(c,sp+o,n).set(m.bytes(a,n));memory(c,c.stage_selection_part(selection,2),4).set(m.bytes(scores[0],4));memory(c,c.stage_selection_part(selection,1),8).set(m.bytes(0x4ace0c,8));assert.equal(c.stage_selection_step(selection),1);
   for(const [a,o,n] of selectionFields)m.write(a,memory(c,sp+o,n));m.write(0x4ace0c,memory(c,c.stage_selection_part(selection,1),8));const route=alloc(16);m.write(route,memory(c,c.stage_selection_part(selection,5),16));m.u32(0x4a7e80,route);c.stage_selection_delete(selection);
   const chars=[m.u32(0x4a7db0),m.u32(0x4a7de8)],bg=m.u32(0x4a7e84),bgName=m.string(m.u32(0x4a11b8+bg*8),64),enemyName=[0,3,7].includes(bg)?'enemy1.anm':[12,15].includes(bg)?'enemy13.anm':'enemy.anm';
   for(const [slot,n] of [[0,'text.anm'],[1,'ascii.anm'],[3,'capture.anm'],[4,bgName],[5,`pl${String(chars[0]).padStart(2,'0')}.anm`],[6,`pl${String(chars[1]).padStart(2,'0')}.anm`],[8,'etama.anm'],[9,enemyName],[10,'front.anm']])anms[slot]=installAnm(m,source(n),slot);
   const manager=alloc(0x12900);m.u32(0x4dc550,manager);m.u32(0x4b36cc,anms[0].file);m.u32(0x4d66e0,anms[1].file);m.call(0x41a9b8,{ecx:0x4a7d90});
   for(let s=0;s<2;++s){m.u32(0x4a8108+s*4,10);m.u32(0x4b3178+s*0xf0+0xcc,16+s*320);m.u32(0x4b3178+s*0xf0+0xd0,16);}
   if(mode!==2)m.i32(0x4a7e8c,-1);
   m.call(0x41af2d,{limit:50000000});assert.equal(m.u32(0x4a7eb4),0,'original world constructed');m.u32(0x4ac884,0);m.u32(0x4a811c,0);
   assert.equal(c.world_initialize(f,left,right,0x7531,mode,controller),1,err());
   const scene=m.u32(0x4a7e38),players=[m.u32(0x4a7d94),m.u32(0x4a7dcc)],enemies=[m.u32(0x4a7da0),m.u32(0x4a7dd8)],bullets=[m.u32(0x4a7d98),m.u32(0x4a7dd0)],backgrounds=[m.u32(0x4a7d90),m.u32(0x4a7dc8)],huds=[m.u32(0x4a7da8),m.u32(0x4a7de0)],controllers=[m.u32(0x4a7da4),m.u32(0x4a7ddc)],effects=[m.u32(0x4a7d9c),m.u32(0x4a7dd4),m.u32(0x4a7e0c)];
   const compare=label=>{
    assert.deepEqual(memory(c,c.world_part(f,0,6),8),m.bytes(0x4ace0c,8),label+' RNG');
    for(const [a,o] of progressFields)assert.equal(new DataView(c.memory.buffer).getUint32(c.world_part(f,0,19)+o,true),m.u32(a),label+' rules '+a.toString(16));
    for(const [o,a,n] of sceneFields)assert.deepEqual(memory(c,c.world_part(f,0,17)+a,n),m.bytes(scene+o,n),label+' scene '+o.toString(16));
    for(const [o,a,n] of dialogueFields)assert.deepEqual(memory(c,c.world_part(f,0,18)+a,n),m.bytes(scene+0xe944+o,n),label+' dialogue '+o.toString(16));
    for(let n=0;n<19;++n)compareAnimation(c.world_part(f,0,24)+n*676,scene+sceneOffsets[n],n===7||n===8?1:10,label+' scene ANM'+n);
    for(let n=0;n<11;++n){const q=scene+0xe958+n*676,file=m.u32(q+0x204);compareAnimation(c.world_part(f,0,25)+n*676,q,file?m.u32(file):0,label+' dialogue ANM'+n);}
    for(let s=0;s<2;++s){const p=players[s];for(const [kind,fields] of [[0,motion],[1,control]])for(const [o,a,n] of fields)assert.deepEqual(memory(c,c.world_part(f,s,kind)+a,n),m.bytes(p+o,n),label+' player'+s+'/'+o.toString(16));
     assert.deepEqual(memory(c,c.world_part(f,s,5),60),m.bytes(p+0x30414,60),label+' combo');assert.deepEqual(memory(c,c.world_part(f,0,20)+s*20,20),m.bytes(scores[s],20),label+' score');compareAnimation(c.world_part(f,s,2),p+0xc0,5+s,label+' body');
     for(let n=0;n<65;++n)compareAnimation(c.world_hud(f,s,n),n<63?huds[s]+n*676:huds[s]+0xa684+(n-63)*676,n<63?10:5+s,label+' HUD'+s+'/'+n);
     for(const [o,a,n] of bgFields)assert.deepEqual(memory(c,c.world_part(f,s,22)+a,n),m.bytes(backgrounds[s]+o,n),label+' background'+s+'/'+o.toString(16));
     for(const [o,a,n] of fields(c.background_fields,c.background_field_count).filter(x=>[0x2c,0xce0].includes(x[0])))for(let i=0;i<n/676;++i){const q=backgrounds[s]+o+i*676,file=m.u32(q+0x204);compareAnimation(c.world_part(f,s,22)+a+i*676,q,file?m.u32(file):4,label+' background ANM'+s+'/'+i);}
    }++checks;
   };
   compare('initial');
   for(let frame=0;frame<1800;++frame){
    let leftKeys=(frame%19<2?1:0)|(frame%120<60?0x40:0x80)|(frame%80<30?4:0);const rightKeys=0,menu=0x100|(frame%7===0?1:0);
    if(frame===650){const [,off]=control.find(x=>x[0]===0x30388);new DataView(c.memory.buffer).setFloat32(c.world_part(f,0,1)+off,400,true);m.f32(players[0]+0x30388,400);leftKeys|=2;}
    if(frame===900||frame===1400){const winner=mode!==2&&frame===1400?0:1;c.world_end_round(f,winner);m.call(0x415d70,{ecx:scene,args:[winner]});++roundEnds;}
    for(let s=0;s<2;++s){memory(c,scratch,88).set(memory(c,c.world_part(f,s,3),88));c.game_input(scratch,s?rightKeys:leftKeys,0);m.write(0x4ace18+s*0x8e,memory(c,scratch,88));}
    // Menu input has no CPU owner. Advance its independent state as well.
    memory(c,scratch,88).set(m.bytes(0x4acf34,88));c.game_input(scratch,menu,0);m.write(0x4acf34,memory(c,scratch,88));
    assert.equal(c.world_step(f,leftKeys,rightKeys,menu),1,err());m.call(0x41aa5f,{ecx:0x4a7d90});
    for(let node=m.u32(0x4acfc8+20),budget=0;node;){assert.ok(++budget<1000,'scheduler list');const next=m.u32(node+20),priority=m.bytes(node,2)[0];if(priority===3){const live=m.call(m.u32(node+4),{ecx:m.u32(node+28)});if(!live)m.call(0x42c8c0,{ecx:0x4acfc8,args:[node]});}node=next;}
    for(let s=0;s<2;++s)m.call(0x402350,{ecx:backgrounds[s]});for(let s=0;s<2;++s)m.call(0x410730,{ecx:enemies[s]});for(let s=0;s<2;++s)m.call(0x4146f0,{ecx:bullets[s]});m.call(0x415340,{ecx:m.u32(0x4a7e3c)});for(let s=0;s<2;++s)m.call(0x41e900,{ecx:players[s]});for(let s=0;s<2;++s)m.call(0x4041f0,{ecx:controllers[s]});for(let s=0;s<3;++s)m.call(0x40cdd0,{ecx:effects[s]});m.call(0x417630,{ecx:scene});for(let s=0;s<2;++s)m.call(0x418a90,{ecx:huds[s]});
    compare(`mode ${mode} frame ${frame}`);++frames;if(c.world_value(f,3))break;
   }
  }
  report('world',{checks,frames,scenarios,roundEnds,scope:'Original world constructors and match update chain, real resources, CPU, dialogue, background/HUD animation, forced Boss attacks, round endings and retries. Route selection uses the separately verified selector before native resource preparation. Final texture/glyph rasterization and audio output are platform boundaries.'});
 }finally{c.world_delete(f);c.release(name);c.release(data);c.release(scratch);m.close();}
});
