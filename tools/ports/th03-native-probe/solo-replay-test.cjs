const {readdir,readFile}=require('node:fs/promises');
const {resolve,join}=require('node:path');
const assert=require('node:assert/strict');
const create=require(resolve(process.argv[2]));
async function copy(m,dir,dst){for(const e of await readdir(dir,{withFileTypes:true})){const p=dst+'/'+e.name;if(e.isDirectory()){m.FS.mkdir(p);await copy(m,join(dir,e.name),p);}else if(e.isFile())m.FS.writeFile(p,await readFile(join(dir,e.name)));}}
(async()=>{
 const modules=await Promise.all([create(),create()]);for(const m of modules){m.FS.mkdir('/content');m.FS.mkdir('/user');m.FS.mkdir('/user/REPLAY');await copy(m,process.argv[3],'/content');m.FS.mkdir('/content/PODDUI');await copy(m,process.argv[4],'/content/PODDUI');assert.equal(m._th03_classic_create(),1);}
 let gameplay=false,completed=false;
 for(let i=0;i<20000;i++){
  const fire=i>300&&i%100<8?32:0,active=i>1000&&i%180<50,begin=active&&i%180===0,dx=active?8:0,dy=active?-4:0,focus=i%240<80;
  for(const m of modules){m._th03_classic_assist(focus);m._th03_classic_touch(0,active,begin,false,dx,dy);assert.equal(m._th03_classic_step(fire,fire,0),0);}
  if(modules[0]._th03_classic_point_x()>=0)gameplay=true;
  if(i%120===0){assert.equal(modules[0]._th03_classic_scene(),modules[1]._th03_classic_scene());const a=modules[0]._th03_classic_pixels(),b=modules[1]._th03_classic_pixels();assert.deepEqual(modules[0].HEAPU8.slice(a,a+640*400*4),modules[1].HEAPU8.slice(b,b+640*400*4),'solo playback frame '+i);}
  if(modules[0]._th03_classic_replay_serial()){
   assert.equal(modules[1]._th03_classic_replay_serial(),1);const a=modules[0]._th03_classic_replay_data(),b=modules[1]._th03_classic_replay_data(),size=modules[0]._th03_classic_replay_size();assert(size>0);assert.equal(size,modules[1]._th03_classic_replay_size());assert.deepEqual(modules[0].HEAPU8.slice(a,a+size),modules[1].HEAPU8.slice(b,b+size));completed=true;break;
  }
 }
 assert(gameplay,'story reaches gameplay');assert(completed,'story finishes a native replay');console.log('Native solo recording and re-simulation matched through completion with touch and focus');
})().catch(e=>{console.error(e);process.exitCode=1;});
