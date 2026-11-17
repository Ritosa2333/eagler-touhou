const {readdir, readFile} = require('node:fs/promises');
const {resolve, join} = require('node:path');
const assert = require('node:assert/strict');
const create = require(resolve(process.argv[2]));
const root = resolve(process.argv[3]), ui = resolve(process.argv[4]);
async function copy(m,dir,dst) {
  for(const entry of await readdir(dir,{withFileTypes:true})) {
    const path=dst+'/'+entry.name;
    if(entry.isDirectory()){m.FS.mkdir(path);await copy(m,join(dir,entry.name),path);}
    else if(entry.isFile())m.FS.writeFile(path,await readFile(join(dir,entry.name)));
  }
}
(async()=>{
 const modules=await Promise.all([create(),create()]);
 for(const m of modules){
  m.FS.mkdir('/content');m.FS.mkdir('/user');m.FS.mkdir('/user/REPLAY');
  await copy(m,root,'/content');m.FS.mkdir('/content/PODDUI');await copy(m,ui,'/content/PODDUI');
  assert.equal(m._th03_classic_create(),1);
  m._th03_classic_start_versus();
 }
 let seen=0;
 for(let frame=0;frame<1200;frame++){
  // Exercise the same stateful menu inputs and then both player lanes.
  const input=frame>500&&frame%60<5?32:0, focus=frame>600&&frame%100<50;
  for(const m of modules){m._th03_classic_assist_pair(focus,focus);assert.equal(m._th03_classic_step_pair(input,input,input,0),0);}
  if(frame%60===0){
   const images=modules.map(m=>{const p=m._th03_classic_pixels();assert.ok(p);return m.HEAPU8.slice(p,p+640*400*4);});
   assert.deepEqual(images[0],images[1],`frame ${frame}`);
  }
  if(modules[0]._th03_classic_point_x()>=0 && modules[0]._th03_classic_point_x2()>=0)seen++;
 }
 assert.ok(seen>0,'match reached');console.log('Native peers: 1200 matching lifecycle frames');
 const before=modules.map(m=>m._th03_classic_point_x());
 for(let i=0;i<12;i++)for(const m of modules){m._th03_classic_touch(0,1,i===0,0,64,0);m._th03_classic_touch(1,1,i===0,1,-64,0);assert.equal(m._th03_classic_step_pair(0,0,0,0),0);}
 for(const [i,m] of modules.entries()){assert.ok(m._th03_classic_point_x()>before[i],'touch drag moves the player');m._th03_classic_touch(0,0,0,0,0,0);m._th03_classic_touch(1,0,0,0,0,0);}
 assert.equal(modules[0]._th03_classic_point_x(),modules[1]._th03_classic_point_x());
 console.log('Native touch movement is deterministic');
 assert.ok(modules.every(m=>m._th03_classic_surrender_allowed()),'active round accepts surrender');
 for(const m of modules)m._th03_classic_surrender(1,0);
 for(let frame=0;frame<500;frame++)for(const m of modules)assert.equal(m._th03_classic_step_pair(0,0,0,0),0);
 assert.ok(modules.every(m=>m._th03_classic_surrender_allowed()),'round surrender advances to another playable round');
 for(const m of modules)m._th03_classic_surrender(1,1);
 let ended=false;
 for(let frame=0;frame<1000;frame++){
  for(const m of modules)assert.equal(m._th03_classic_step_pair(0,0,0,0),0);
  if(modules.every(m=>m._th03_classic_scene()!==9)){ended=true;break;}
 }
 assert.ok(ended,'match surrender reaches the native result transition');
 console.log('Native round and match surrender settle normally');
})().catch(e=>{console.error(e);process.exitCode=1;});
