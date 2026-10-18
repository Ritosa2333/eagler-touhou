import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync,readdirSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {oracle,core,memory,report,root,d3dxScalar} from './helpers.mjs';
import {installAnm,normalizedAnm} from './anm-oracle.mjs';

test('Supplied campaign replay matches the original world frame by frame',{skip:!process.env.TH09_REPLAY_FILE},async()=>{
 const m=await oracle(),routeOracle=await oracle(),c=await core(),f=c.session_create(),name=c.allocate(128),data=c.allocate(16000000),scratch=c.allocate(88),files=new Map();
 const source=n=>{if(!files.has(n))files.set(n,readFileSync(resolve(root,'reference/assets',n)));return files.get(n);};
 for(const n of readdirSync(resolve(root,'reference/assets')).filter(n=>/\.(anm|sht|ecl|msg|std|bmp|png)$/.test(n))){const b=source(n);memory(c,name,128).fill(0);memory(c,name,128).set(new TextEncoder().encode(n));memory(c,data,b.length).set(b);c.session_file(f,name,data,b.length);}
 const arg=i=>m.u32(m.reg('ESP')+4+i*4),alloc=n=>{const p=m.allocate(n);m.view(p,n).fill(0);return p;};
 if(process.env.TH09_TRACE==='1')c.hazard_trace_enable(1);
 const fields=(fn,count,width=3)=>Array.from({length:count()},(_,i)=>Array.from({length:width},(_,j)=>new DataView(c.memory.buffer).getUint32(fn()+i*width*4+j*4,true)));
 const motion=fields(c.motion_fields,c.motion_field_count),control=fields(c.shot_control_fields,c.shot_control_field_count),sceneFields=fields(c.match_scene_fields,c.match_scene_field_count),dialogueFields=fields(c.dialogue_fields,c.dialogue_field_count).filter(x=>x[0]!==0x14),bgFields=fields(c.background_fields,c.background_field_count).filter(x=>![0x2c,0xce0].includes(x[0])),progressFields=fields(c.match_rules_fields,c.match_rules_field_count,2),selectionFields=fields(c.stage_selection_fields,c.stage_selection_field_count);
 const sceneOffsets=[...Array.from({length:7},(_,i)=>12+i*676),0x123f4,0x12698,0x106b4,...Array.from({length:7},(_,i)=>0x10968+i*676),0x11eac,0x12150];
 const err=()=>{const p=c.session_error(f),bytes=memory(c,p,256);return new TextDecoder().decode(bytes.subarray(0,bytes.indexOf(0)));};
 let anms=[],checks=0,frames=0,playerDefeats=0,stageComplete=false;const external=process.env.TH09_REPLAY_FILE,stage=Number(process.env.TH09_REPLAY_STAGE??0);const scenarios=external?['external']:process.env.TH09_DEMO_CASE?[Number(process.env.TH09_DEMO_CASE)]:[0,1,2];
 d3dxScalar(m);
 m.replace(0x47b24e,'world-new',()=>alloc(arg(0)));m.replace(0x47b249,'world-free',()=>0);
 m.replace(0x401660,'world-animation',()=>{assert.ok(anms[arg(0)],'ANM slot '+arg(0));return anms[arg(0)].file;},1);
 m.replace(0x43c7a0,'world-animation-load',()=>{assert.ok(anms[arg(0)],'ANM load '+arg(0));return anms[arg(0)].file;},2);
 m.replace(0x42c970,'world-read',()=>{const bytes=source(m.string(m.reg('ECX'),128)),p=alloc(bytes.length);m.write(p,bytes);if(m.reg('EDX'))m.u32(m.reg('EDX'),bytes.length);return p;},1);
 const imports=m.onImport;m.onImport=e=>{if(e.name==='timeGetTime'){m.ret(1000,0);return;}imports(e);};
 for(const [a,n] of [[0x41a796,0],[0x41a8a2,0],[0x415910,0],[0x4217e0,0],[0x4307c0,0],[0x41cde0,0],[0x4316b0,0],[0x4304a0,0],[0x4343e0,0],[0x42b130,1],[0x42b160,1],[0x42fc20,1],[0x42fd90,1],[0x43be50,0],[0x43bdc0,0],[0x4346a0,4],[0x43e380,2],[0x43e2f0,2],[0x42fe20,1],[0x43bf30,1],[0x40e3d0,1]])m.replace(a,'world-platform',()=>0,n);
 m.replace(0x431930,'world-music',()=>{for(let n=0;n<19;++n)if(m.i32(0x4a21d0+n*16)===(arg(0)|0)){m.i32(0x4dc690,m.i32(0x4a21d8+n*16));break;}return 0;},1);m.replace(0x42fd30,'world-stop-music',()=>{return 0;});
 function compareAnimation(cpp,native,slot,label){if(process.env.TH09_GAMEPLAY_ONLY)return;const a=anms[slot]??{file:0,source:0,sprites:0},p=[0,1,2].map(i=>c.session_resource(f,slot,i));assert.deepEqual(normalizedAnm(memory(c,cpp,676),...p),normalizedAnm(m.bytes(native,676),a.file,a.source,a.sprites),label);}
 const heap=m.heap;
 try{
  for(const replayIndex of scenarios){
   const raw=external?readFileSync(external):source(`demorpy${replayIndex}.rpy`),rawPointer=alloc(raw.length);m.write(rawPointer,raw);const decoded=m.call(0x4205e0,{ecx:rawPointer,edx:raw.length,limit:200000000}),replayManager=alloc(0x164);m.u32(replayManager+8,decoded);const mode=m.bytes(decoded+0x1e4,1)[0],round=external?stage:9;
   anms=[];m.view(0x4a7d88,0x420).fill(0);m.view(0x4ace18,3*0x8e).fill(0);m.view(0x4acfc8,64).fill(0);m.view(0x4b3178,0x2d0).fill(0);m.view(0x4b3488,0xcc).fill(0);m.u32(0x4b36d4,0);m.f32(0x4b36b8,1);m.u32(0x4ac884,0);m.u32(0x4b3690,0);m.i32(0x4dc690,-1);
   const scores=[alloc(160),alloc(160)],config=alloc(204);m.u32(0x4b42d0,config);m.u32(0x4a7dac,scores[0]);m.u32(0x4a7de4,scores[1]);m.u32(0x4a7e78,config);m.u32(0x4a7ea8,mode);m.u32(0x4a7e8c,round);m.u32(0x4a7ec4,12);m.u32(0x4ace0c,0);m.u32(0x4ace10,0);m.call(0x420840,{ecx:replayManager});m.write(0x4b3488,m.bytes(config,204));
   // Derive the route with the original routine, independently of C++.
   // Constructor resource requests need these animation slots prepared first.
   // A separate oracle avoids executing previously cached x86 blocks after
   // changing a function to a platform hook in the constructor oracle.
   routeOracle.write(0x4a7d88,m.bytes(0x4a7d88,0x420));routeOracle.write(0x4a81cc,m.bytes(0x4a81cc,16));
   const routeScore=routeOracle.allocate(160),routeConfig=routeOracle.allocate(204);routeOracle.write(routeScore,m.bytes(scores[0],160));routeOracle.write(routeConfig,m.bytes(config,204));routeOracle.u32(0x4a7dac,routeScore);routeOracle.u32(0x4b42d0,routeConfig);routeOracle.write(0x4ace0c,m.bytes(0x4ace0c,8));routeOracle.replace(0x4158e0,'encounter',()=>0,1);
   routeOracle.call(0x415910,{ecx:0x4a7d90});
   for(const [address,,size] of selectionFields)m.write(address,routeOracle.bytes(address,size));m.write(0x4ace0c,routeOracle.bytes(0x4ace0c,8));m.u32(0x4a7e80,routeOracle.u32(0x4a7e80));
   const chars=[m.u32(0x4a7db0),m.u32(0x4a7de8)],bg=m.u32(0x4a7e84),bgName=m.string(m.u32(0x4a11b8+bg*8),64),enemyName=[0,3,7].includes(bg)?'enemy1.anm':[12,15].includes(bg)?'enemy13.anm':'enemy.anm';
   for(const [slot,n] of [[0,'text.anm'],[1,'ascii.anm'],[3,'capture.anm'],[4,bgName],[5,`pl${String(chars[0]).padStart(2,'0')}.anm`],[6,`pl${String(chars[1]).padStart(2,'0')}.anm`],[8,'etama.anm'],[9,enemyName],[10,'front.anm']])anms[slot]=installAnm(m,source(n),slot);
   const manager=alloc(0x12900);m.u32(0x4dc550,manager);m.u32(0x4b36cc,anms[0].file);m.u32(0x4d66e0,anms[1].file);m.call(0x41a9b8,{ecx:0x4a7d90});
   for(let s=0;s<2;++s){m.u32(0x4b3178+s*0xf0+0xcc,16+s*320);m.u32(0x4b3178+s*0xf0+0xd0,16);}
   if(mode!==2)m.i32(0x4a7e8c,round-1);
   if(process.env.TH09_ORACLE_CTOR_FPCW)m.reg('FPCW',Number(process.env.TH09_ORACLE_CTOR_FPCW));
   m.call(0x41af2d,{limit:50000000});assert.equal(m.u32(0x4a7eb4),0,'original world constructed');m.u32(0x4ac884,0);m.u32(0x4a811c,0);
   m.reg('FPCW',Number(process.env.TH09_ORACLE_FPCW??0x7f));
   if(process.env.TH09_ORACLE_ONLY){for(const head of [0x4acfc8,0x4acfe8]){const callbacks=[];for(let n=m.u32(head+20);n;n=m.u32(n+20))callbacks.push({priority:m.bytes(n,1)[0],callback:m.u32(n+4).toString(16),object:m.u32(n+28).toString(16)});console.log({head:head.toString(16),callbacks});}}
   memory(c,data,raw.length).set(raw);assert.equal(c.session_play(f,data,raw.length,round),1,err());
   const scene=m.u32(0x4a7e38),players=[m.u32(0x4a7d94),m.u32(0x4a7dcc)],enemies=[m.u32(0x4a7da0),m.u32(0x4a7dd8)],bullets=[m.u32(0x4a7d98),m.u32(0x4a7dd0)],backgrounds=[m.u32(0x4a7d90),m.u32(0x4a7dc8)],huds=[m.u32(0x4a7da8),m.u32(0x4a7de0)],controllers=[m.u32(0x4a7da4),m.u32(0x4a7ddc)],effects=[m.u32(0x4a7d9c),m.u32(0x4a7dd4),m.u32(0x4a7e0c)];
   const hex=b=>Buffer.from(b).toString('hex'),trace=process.env.TH09_TRACE==='1',traceRing=[];
   const cppU32=(p,o)=>p?new DataView(c.memory.buffer).getUint32(p+o,true):-1;
   const hazCount=side=>cppU32(c.session_part(f,side,8),6144);
   const oracleHazRing=[];
   const hazTrace=()=>{const base=c.hazard_trace_ptr(),view=new DataView(c.memory.buffer),next=view.getUint32(base+12,true),out=[];for(let k=Math.max(0,next-4);k<next;++k){const r=base+16+(k%16)*6152,count=view.getUint32(r+4,true),n=Math.min(count,128);out.push({k,side:view.getInt32(r,true),count,entries:hex(new Uint8Array(c.memory.buffer,r+8,n*48))});}return out;};
   const cppDecisions=()=>{const base=c.cpu_decisions_ptr(),view=new DataView(c.memory.buffer),out=[];for(let s=0;s<2;++s){const o=base+s*84;out.push({side:view.getInt32(o,true),frame:view.getUint32(o+4,true),px:view.getFloat32(o+8,true),py:view.getFloat32(o+12,true),tx:view.getFloat32(o+16,true),ty:view.getFloat32(o+20,true),region:view.getInt32(o+24,true),focus:view.getInt32(o+28,true),chosen:view.getInt32(o+32,true),tier:view.getInt32(o+36,true),flags:view.getInt32(o+40,true),hold:view.getInt32(o+44,true),keys:view.getInt32(o+48,true),path:view.getInt32(o+52,true),mirror:view.getInt32(o+56,true),branch:view.getInt32(o+60,true),probe:view.getInt32(o+64,true),desired:view.getInt32(o+68,true),sampled:view.getInt32(o+72,true),move:view.getInt32(o+76,true),hazardCount:view.getUint32(o+80,true)});}return out;};
   const traceHazards2=frame=>{if(!trace)return;const e={frame};for(const s of [0,1]){const p=players[s],count=m.u32(p+0x1b70),n=Math.min(count,128);e['oh'+s]=count;e['oe'+s]=hex(m.bytes(p+0x370,n*48));e['op'+s]=hex(m.bytes(p+0x1b88,12));e['ot'+s]=hex(m.bytes(p+0x30f64,8));const master=m.u32(m.u32(p+0xc)+0x10);e['om'+s]=master;e['opri'+s]=master?m.u32(master+0x2ac444):0;e['ofir'+s]=master?m.u32(master+0x2ac448):0;e['osp'+s]=master?m.u32(master+0x2ac3b8):-1;e['one'+s]=master?m.u32(master+0x2ac3ac):-1;const pr=master?m.u32(master+0x2ac444):0,fr=master?m.u32(master+0x2ac448):0;e['oprp'+s]=pr?hex(m.bytes(pr+0x2d74,12))+'/'+m.u32(pr+0x3380).toString(16):'-';e['ofrp'+s]=fr?hex(m.bytes(fr+0x2d74,12)):'-';}oracleHazRing.push(e);if(oracleHazRing.length>4)oracleHazRing.shift();};
   const dumpDivergence=(label,error)=>{
    const lines=['divergence '+label+' :: '+error.message];
    for(const s of [0,1]){const p=players[s],owner=m.u32(p+0x36c),oh=owner?m.u32(owner+0x1804):-1,ch=hazCount(s);
     lines.push('player'+s+' input cpp='+hex(memory(c,c.session_part(f,s,3),88)));
     lines.push('player'+s+' input org='+hex(m.bytes(0x4ace18+s*0x8e,88)));
     lines.push('player'+s+' cpu cpp='+hex(memory(c,c.session_part(f,s,4),120)));
     lines.push('player'+s+' cpu org='+hex(m.bytes(p+0x24,120)));
     for(const [kind,fields] of [[0,motion],[1,control]])for(const [o,a,n] of fields)lines.push('player'+s+' part'+kind+' off'+o.toString(16)+' cpp='+hex(memory(c,c.session_part(f,s,kind)+a,n))+' org='+hex(m.bytes(p+o,n)));
     lines.push('player'+s+' hazards cpp='+ch+' org='+oh+' owner='+(owner>>>0).toString(16));
     if(owner&&oh>0&&oh<33)lines.push('player'+s+' hazards org='+hex(m.bytes(owner+4,oh*48)));
     if(ch>0&&ch<33)lines.push('player'+s+' hazards cpp='+hex(new Uint8Array(c.memory.buffer,c.session_part(f,s,8),ch*48)));
    }
    for(const e of traceRing)lines.push(JSON.stringify(e));
    for(const e of oracleHazRing)lines.push(JSON.stringify(e));
    for(const r of hazTrace())lines.push('cppHaz '+JSON.stringify(r));
    for(const d of cppDecisions())lines.push('cppDecision '+JSON.stringify(d));
    writeFileSync(resolve(root,'artifacts/th09-divergence.log'),lines.join('\n')+'\n');
    console.error(lines.join('\n'));
   };
   const snapshot=frame=>{if(!trace)return;const e={frame};for(const s of [0,1]){const p=players[s];e['in'+s]=hex(memory(c,c.session_part(f,s,3),88));e['oi'+s]=hex(m.bytes(0x4ace18+s*0x8e,88));e['cpu'+s]=hex(memory(c,c.session_part(f,s,4),120));e['ocpu'+s]=hex(m.bytes(p+0x24,120));e['ch'+s]=hazCount(s);const owner=m.u32(p+0x36c);e['oh'+s]=owner?m.u32(owner+0x1804):-1;}traceRing.push(e);if(traceRing.length>6)traceRing.shift();};
   const compare=label=>{
    if(process.env.TH09_ORACLE_ONLY)return;
    try{
    assert.deepEqual(memory(c,c.session_part(f,0,6),8),m.bytes(0x4ace0c,8),label+' RNG');
    for(const [a,o] of progressFields)assert.equal(new DataView(c.memory.buffer).getUint32(c.session_part(f,0,19)+o,true),m.u32(a),label+' rules '+a.toString(16));
    for(const [o,a,n] of sceneFields)assert.deepEqual(memory(c,c.session_part(f,0,17)+a,n),m.bytes(scene+o,n),label+' scene '+o.toString(16));
    for(const [o,a,n] of dialogueFields)assert.deepEqual(memory(c,c.session_part(f,0,18)+a,n),m.bytes(scene+0xe944+o,n),label+' dialogue '+o.toString(16));
    for(let n=0;n<19;++n)compareAnimation(c.session_part(f,0,24)+n*676,scene+sceneOffsets[n],n===7||n===8?1:10,label+' scene ANM'+n);
    for(let n=0;n<11;++n){const q=scene+0xe958+n*676,file=m.u32(q+0x204);compareAnimation(c.session_part(f,0,25)+n*676,q,file?m.u32(file):0,label+' dialogue ANM'+n);}
    for(let s=0;s<2;++s){const p=players[s];
     assert.deepEqual(memory(c,c.session_part(f,s,3),88),m.bytes(0x4ace18+s*0x8e,88),label+' input'+s);
     for(const [kind,fields] of [[0,motion],[1,control]])for(const [o,a,n] of fields)assert.deepEqual(memory(c,c.session_part(f,s,kind)+a,n),m.bytes(p+o,n),label+' player'+s+'/'+o.toString(16));
     assert.deepEqual(memory(c,c.session_part(f,s,4),120),m.bytes(p+0x24,120),label+' cpu'+s);
     assert.deepEqual(memory(c,c.session_part(f,s,14),8),m.bytes(p+0x30f64,8),label+' item-target'+s);
     assert.deepEqual(memory(c,c.session_part(f,s,5),60),m.bytes(p+0x30414,60),label+' combo');assert.deepEqual(memory(c,c.session_part(f,0,20)+s*20,20),m.bytes(scores[s],20),label+' score');compareAnimation(c.session_part(f,s,2),p+0xc0,5+s,label+' body');
     for(let n=0;n<65;++n)compareAnimation(c.session_hud(f,s,n),n<63?huds[s]+n*676:huds[s]+0xa684+(n-63)*676,n<63?10:5+s,label+' HUD'+s+'/'+n);
     for(const [o,a,n] of bgFields)assert.deepEqual(memory(c,c.session_part(f,s,22)+a,n),m.bytes(backgrounds[s]+o,n),label+' background'+s+'/'+o.toString(16));
     for(const [o,a,n] of fields(c.background_fields,c.background_field_count).filter(x=>[0x2c,0xce0].includes(x[0])))for(let i=0;i<n/676;++i){const q=backgrounds[s]+o+i*676,file=m.u32(q+0x204);compareAnimation(c.session_part(f,s,22)+a+i*676,q,file?m.u32(file):4,label+' background ANM'+s+'/'+i);}
    }++checks;
    }catch(error){dumpDivergence(label,error);throw error;}
   };
   compare('initial');let nativeHealth=m.i32(players[0]+0xa8);
   for(let frame=0;frame<c.session_value(f,3)&&frame<Number(process.env.TH09_REPLAY_LIMIT??120000);++frame){
    m.call(0x4204b0,{ecx:replayManager});assert.equal(c.session_step(f,0,0,0),1,err());
    m.call(0x41aa5f,{ecx:0x4a7d90});
    for(let node=m.u32(0x4acfc8+20),budget=0;node;){assert.ok(++budget<1000,'scheduler list');const next=m.u32(node+20),priority=m.bytes(node,2)[0];if(priority===3){const live=m.call(m.u32(node+4),{ecx:m.u32(node+28)});if(!live)m.call(0x42c8c0,{ecx:0x4acfc8,args:[node]});}node=next;}
    for(let s=0;s<2;++s)m.call(0x402350,{ecx:backgrounds[s]});for(let s=0;s<2;++s)m.call(0x410730,{ecx:enemies[s]});for(let s=0;s<2;++s)m.call(0x4146f0,{ecx:bullets[s]});m.call(0x415340,{ecx:m.u32(0x4a7e3c)});
    traceHazards2(frame);
    for(let s=0;s<2;++s)m.call(0x41e900,{ecx:players[s]});for(let s=0;s<2;++s)m.call(0x4041f0,{ecx:controllers[s]});for(let s=0;s<3;++s)m.call(0x40cdd0,{ecx:effects[s]});m.call(0x417630,{ecx:scene});for(let s=0;s<2;++s)m.call(0x418a90,{ecx:huds[s]});
    if(process.env.TH09_ORACLE_ONLY&&nativeHealth!==m.i32(players[0]+0xa8)){nativeHealth=m.i32(players[0]+0xa8);console.log({frame,nativeHealth,seed:m.u32(0x4ace0c)&65535});}
    ++frames;if(m.i32(players[0]+0xa8)===0)++playerDefeats;
    if(frames%1000===0)console.log({stage,frames,nativeHealth:m.i32(players[0]+0xa8),opponentHealth:m.i32(players[1]+0xa8)});
    if(c.session_value(f,5)!==stage){assert.equal(m.i32(players[1]+0xa8),0,'original opponent defeated before the stage transition');stageComplete=true;break;}
    if(process.env.TH09_COMPARE_LASERS)for(let side=0;side<2;++side)for(let index=0;index<48;++index){
     const actual=memory(c,c.session_laser(f,side,index)+0x548,0x52),expected=m.bytes(bullets[side]+0x24d424+index*0x59c+0x548,0x52);
     if(!Buffer.from(actual).equals(Buffer.from(expected))){console.log({frame,side,index,laserCpp:hex(actual),laserOriginal:hex(expected)});assert.deepEqual(actual,expected,'laser state');}
    }
    snapshot(frame);compare(`replay stage ${stage} frame ${frame}`);if(c.session_value(f,0)!==1){stageComplete=c.session_value(f,2)===c.session_value(f,3);break;}
   }
  }
  report(external?'external-replay-stage-'+stage:'session-replays',{checks,frames,stage,playerDefeats,stageComplete,scenarios,animationComparison:!process.env.TH09_GAMEPLAY_ONLY,laserComparison:!!process.env.TH09_COMPARE_LASERS,scope:'Supplied original replay: original constructors and update chain versus C++ RNG, inputs, CPU decisions, player motion, collisions, score, dialogue, background and scene state. Animation bytes compared unless gameplay-only is selected. No forced battle outcome.'});
 }finally{c.session_delete(f);c.release(name);c.release(data);c.release(scratch);m.close();routeOracle.close();}
});
