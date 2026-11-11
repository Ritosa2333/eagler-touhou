const {readdir,readFile}=require('node:fs/promises');const {resolve,join}=require('node:path');const assert=require('node:assert/strict');
const create=require(resolve(process.argv[2])),root=resolve(process.argv[3]),ui=resolve(process.argv[4]);
async function copy(m,dir,dst){for(const e of await readdir(dir,{withFileTypes:true})){const p=dst+'/'+e.name;if(e.isDirectory()){m.FS.mkdir(p);await copy(m,join(dir,e.name),p);}else if(e.isFile())m.FS.writeFile(p,await readFile(join(dir,e.name)));}}
(async()=>{const m=await create();m.FS.mkdir('/content');m.FS.mkdir('/user');m.FS.mkdir('/user/REPLAY');await copy(m,root,'/content');m.FS.mkdir('/content/PODDUI');await copy(m,ui,'/content/PODDUI');assert.equal(m._th03_classic_create(),1);m._th03_classic_start_versus();
 for(let frame=0;frame<1200;frame++){const bits=frame>500&&frame%60<5?32:0;assert.equal(m._th03_classic_step_pair(bits,bits,bits,0),0);}
 assert.equal(m._th03_classic_scene(),9);assert.equal(m._th03_classic_capture(0),1,'capture active gameplay');const start=m._th03_semantic_state_hash(),startSamples=m._th03_semantic_replay_samples();assert.ok(startSamples>0,'native recording active');
 const hash=()=>{const p=m._th03_classic_pixels();let h=2166136261;for(const x of m.HEAPU8.subarray(p,p+640*400*4))h=Math.imul(h^x,16777619);return h>>>0;};
 const run=()=>{const out=[];for(let i=0;i<25;i++){const a=i%4?4:32,b=i%6?8:0;assert.equal(m._th03_classic_step_pair(a,b,a,0),0);out.push([m._th03_semantic_state_hash(),hash()]);}return out;};
 const expected=run();assert.equal(m._th03_classic_restore(0),1,'restore active gameplay');assert.equal(m._th03_semantic_state_hash(),start);assert.equal(m._th03_semantic_replay_samples(),startSamples,'discard speculative replay samples');assert.deepEqual(run(),expected,'restored simulation and surfaces');
 console.log('Semantic process snapshot restored and replayed matching state and surface');
})().catch(e=>{console.error(e);process.exitCode=1;});
