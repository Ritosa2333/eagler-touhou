'use strict';
const fs=require('node:fs'),path=require('node:path'),crypto=require('node:crypto'),assert=require('node:assert/strict'),{pathToFileURL}=require('node:url');
const r=JSON.parse(fs.readFileSync(process.argv[2],'utf8')),sha=b=>crypto.createHash('sha256').update(b).digest('hex');
let native,Factories=0,WorldStarts=0,ticks=0,rows=[],events=[],trace=[];
function save(name,v){const b=Buffer.from(JSON.stringify(v,null,2)+'\n');fs.writeFileSync(path.join(r.output,name),b,{flag:'wx'});}
function state(){return JSON.parse(native.UTF8ToString(native._th128_world_state()));}
function error(){return native.UTF8ToString(native._th128_world_error());}
function need(n,label){assert.equal(n,1,label+': '+error());}
function memory(b,f){const p=native._malloc(b.length);assert(p>0);try{native.HEAPU8.set(b,p);return f(p,b.length);}finally{native._free(p);}}
function raw(n){const row=r.assets.find(x=>x.name===n);assert(row,'asset pin '+n);const b=fs.readFileSync(row.path);assert.equal(b.length,row.bytes);assert.equal(sha(b),row.sha256);return b;}
function data(name,b,...prefix){need(memory(b,(p,n)=>native[name](...prefix,p,n)),name);}
function program(){
 const ins=(op,words=[],time=0,parameters=words.length)=>{const b=Buffer.alloc(16+4*words.length);b.writeInt32LE(time);b.writeUInt16LE(op,4);b.writeUInt16LE(b.length,6);b[10]=255;b[11]=parameters;words.forEach((x,i)=>b.writeUInt32LE(x>>>0,16+i*4));return b;};
 const title=Buffer.alloc(12);title.write('Mode check');let key=0x77,step=7;for(let i=0;i<title.length;i++){title[i]^=key;key=(key+step)&255;step=(step+16)&255;}
 const words=[70,2280,500000,12,...Array.from({length:3},(_,i)=>title.readUInt32LE(i*4))];
 const sub=Buffer.concat([Buffer.from('ECLH'),Buffer.from([16,0,0,0]),Buffer.alloc(8),ins(83,[1]),ins(411,[1200]),ins(422,words,0,4),ins(0,[],1000)]);
 const h=Buffer.alloc(36);h.write('SCPT');h.writeUInt16LE(1,4);h.writeUInt16LE(16,6);h.writeUInt32LE(36,8);h.writeUInt32LE(1,16);
 const includes=Buffer.concat([Buffer.from('ANIM'),Buffer.alloc(4),Buffer.from('ECLI'),Buffer.alloc(4)]),table=Buffer.alloc(12);table.writeUInt32LE(64);table.write('main',4);
 return Buffer.concat([h,includes,table,sub]);
}
async function drain(sound){
 await sound.consumeMenuSoundWindow(native,{consumeNativeRequest(id,pan,operation){events.push({frame:state().frame,phase:state().perfectFreeze.phase,id,pan,operation,syntheticOutput:true});return true;},flush(){}},
 {async commit(receipt){const payload=JSON.stringify(receipt);return{payload,digest:sha(Buffer.from(payload)),strictReadback:true,syntheticStorage:true};}},{present:false});
}
function configure(){
 native._th128_world_reset();const name=Buffer.from('mode-component.ecl'),ecl=fs.readFileSync(r.fixture);
 memory(name,(p,n)=>memory(ecl,(q,m)=>need(native._th128_world_load(p,n,q,m),'synthetic ECL load')));
 memory(name,(p,n)=>need(native._th128_world_start(p,n,0,34772),'actualWorldstart'));++WorldStarts;
 for(const [index,name]of ['enemy.anm','enemy_ll.anm','boss01.anm'].entries())data('_th128_world_load_enemy_anm',raw(name),index+1);
 data('_th128_world_enable_geometry526',raw('text.anm'));
 memory(raw('bullet-profile-8.bin'),(p,n)=>memory(raw('bullet.anm'),(q,m)=>need(native._th128_world_enable_bullets(p,n,q,m,0,400),'enablebullets')));
 data('_th128_world_enable_bullet_updates',raw('pl00a.sht'));need(native._th128_world_enable_graze(0,10000,0x1234),'graze');
 memory(raw('pl00.anm'),(p,n)=>need(native._th128_world_enable_player_context(p,n,1,0,120,0,0,0,0,0,10000),'playercontext'));
 need(native._th128_world_enable_player_motion(12800,51200),'player motion');
 memory(raw('pl00a.sht'),(p,n)=>memory(raw('pl00.anm'),(q,m)=>memory(Buffer.alloc(8000),f=>need(native._th128_world_enable_player_shots(p,n,q,m,f,2000,0,0,0,0,0,0),'player shots'))));
 need(native._th128_world_enable_shot_hits(20000,0,0,0,0),'shot hits');need(native._th128_world_enable_rapid_shot(-1),'rapid');need(native._th128_world_enable_player_death(-1),'death');
 data('_th128_world_enable_perfect_hud',raw('front.anm'));
 memory(Buffer.alloc(32),p=>need(native._th128_world_enable_player_respawn(1,0,p,8),'real respawn owner prerequisite'));
 data('_th128_world_load_score_font',raw('ascii.anm'));
 const banks=['bullet.anm','boss01.anm','text.anm','ascii.anm','card.anm','pl00.anm'],allocated=[];
 try{for(const n of banks){const b=raw(n),p=native._malloc(b.length);native.HEAPU8.set(b,p);allocated.push({p,n:b.length});}need(native._th128_world_enable_spells(...allocated.flatMap(x=>[x.p,x.n])),'real spell host');}finally{for(const x of allocated)native._free(x.p);}
 const masks=JSON.parse(raw('ice-masks.json').toString('utf8')),bytes=Buffer.concat(masks.profiles.map(x=>{const b=Buffer.from(x.hex,'hex');assert.equal(sha(b),x.sha256);return b;}));assert.equal(bytes.length,1228);
 memory(raw('ice.anm'),(p,n)=>memory(bytes,(q,m)=>need(native._th128_world_enable_z_fire(p,n,q,m),'Z/Perfect realhost')));
 const s=state();assert(s.perfectFreeze.configured&&!s.perfectFreeze.failed);assert.equal(s.perfectFreeze.systemMode3c,0);assert.equal(s.normalRecord.started,false);assert.equal(s.normalRecord.bound,false);
 trace.push({event:'configured',world:s.flow.worldGeneration,frame:s.frame,perfect:s.perfectFreeze,spell:s.spell});
}
(async()=>{
 fs.mkdirSync(r.output);assert(fs.readFileSync(r.fixture).equals(program()));
 const {default:factory}=await import(pathToFileURL(r.entry).href);native=await factory({wasmBinary:fs.readFileSync(r.wasm),print:s=>events.push({stdout:s}),printErr:s=>events.push({stderr:s})});++Factories;
 const exports=JSON.parse(fs.readFileSync(r.coreRequest,'utf8')).exports;for(const name of exports)assert.equal(typeof native[name],'function');assert.equal(exports.length,205);
 const sound=await import(pathToFileURL(r.sound).href);configure();
 const wantedSnapshots=new Set([0,1,119,120,179,180]);
 for(let pass=0;pass<181;++pass){
  need(native._th128_world_input(2,0,0),'hold X through bounded phase request audit');++ticks;assert(ticks<=r.maxTicks);
  need(native._th128_world_tick(),'actual native tick');
  const s=state();assert.equal(s.perfectFreeze.failed,false);assert.equal(error(),'');assert.deepEqual(s.flow.gameRandom,{seed:34772,calls:0});
  assert.equal(s.normalRecord.started,false);assert.equal(s.normalRecord.bound,false);assert.equal(s.normalRecord.frames,0);assert.equal(s.debug.runTainted,false);assert.equal(s.debug.infiniteRespawn,false);
  trace.push({frame:s.frame,phase:s.perfectFreeze.phase,mode:s.perfectFreeze.systemMode3c,active:s.perfectFreeze.active,charge:s.perfectFreeze.charge,random:s.flow.gameRandom});
  if(wantedSnapshots.has(s.perfectFreeze.phase))save('actual-phase-'+s.perfectFreeze.phase+'.json',s);
  await drain(sound);
 }
 const last=state();assert.equal(last.frame,181);assert.equal(last.perfectFreeze.phase,180);assert.equal(last.perfectFreeze.systemMode3c,0);assert.equal(last.perfectFreeze.charge,0);assert.equal(last.perfectFreeze.active,true);
 const actual=events.filter(x=>[46,51,47,69,71].includes(x.id)).map(({phase,id,pan,operation})=>({phase,id,pan,operation}));
 const expected=[{phase:0,id:46,pan:100,operation:0},{phase:0,id:51,pan:100,operation:0},{phase:120,id:47,pan:100,operation:0},{phase:120,id:69,pan:0,operation:0},{phase:120,id:51,pan:0,operation:1},{phase:180,id:71,pan:0,operation:0}];
 for(const want of expected){const matches=actual.filter(x=>JSON.stringify(x)===JSON.stringify(want));rows.push({label:'request-'+want.phase+'-'+want.id+'-'+want.operation,passed:matches.length===1,want,matches:matches.length});}
 rows.push({label:'ordered-target-request-trace-no-duplicates',passed:JSON.stringify(actual)===JSON.stringify(expected),want:expected,actual});
 const failed=rows.filter(x=>!x.passed).length,detail={passed:failed===0,Factories,WorldStarts,ticks,checks:rows.length,failed,rows,expected,actual,trace,events,syntheticECL:true,actualNativeWorldAndAssets:true,syntheticAudioOutput:true,syntheticReceiptStore:true,normalRecord:false,callableExports:205,originalEXEExecuted:false,browser:0,nativeCompiles:0};
 save('actual-result.json',detail);console.log(JSON.stringify({passed:detail.passed,Factories,WorldStarts,ticks,checks:rows.length,failed,rows:rows.map(({label,passed})=>({label,passed})),actual}));
 assert.equal(failed,0,'normal Perfect phase request trace must include original start46/start51/end71');
})().catch(e=>{save('first-failure.json',{message:e.message,stack:e.stack,Factories,WorldStarts,ticks,rows,trace,nativeError:native?error():null,state:native?state():null,events,syntheticECL:true,actualPhaseSoundRequestAudit:true,syntheticAudioOutput:true,syntheticReceiptStore:true});console.error(e.stack);process.exitCode=1;}).finally(()=>native?._th128_world_reset());
