const {readdir,readFile} = require('node:fs/promises');
const {resolve,join} = require('node:path');
const assert = require('node:assert/strict');
const create = require(resolve(process.argv[2]));
const root=resolve(process.argv[3]),ui=resolve(process.argv[4]);
async function copy(m,dir,dst){for(const e of await readdir(dir,{withFileTypes:true})){const path=dst+'/'+e.name;if(e.isDirectory()){m.FS.mkdir(path);await copy(m,join(dir,e.name),path);}else if(e.isFile())m.FS.writeFile(path,await readFile(join(dir,e.name)));}}
(async()=>{
 const m=await create();m.FS.mkdir('/content');m.FS.mkdir('/user');m.FS.mkdir('/user/REPLAY');await copy(m,root,'/content');m.FS.mkdir('/content/PODDUI');await copy(m,ui,'/content/PODDUI');
 assert.equal(m._th03_classic_create(),1);m._th03_classic_start_versus();
 const step=(a,b)=>assert.equal(m._th03_classic_step_pair(a,b,a,0),0);
 for(let i=0;i<900;i++)step(i>500&&i%60<5?32:0,i>500&&i%60<5?32:0);
 assert.ok(m._th03_classic_point_x()>=0 && m._th03_classic_point_x2()>=0,'versus match started');
 const snapshot=m.HEAPU8.slice();
 const play=()=>{const audio=[];for(let i=0;i<40;i++){step(i%3?4:32,i%2?8:0);const p=m._th03_classic_audio(),n=m._th03_classic_audio_frames();audio.push(m.HEAPU8.slice(p,p+n*8));}const p=m._th03_classic_pixels();return {pixels:m.HEAPU8.slice(p,p+640*400*4),audio};};
 const first=play();assert.equal(m.HEAPU8.length,snapshot.length,'fixed memory during probe');m.HEAPU8.set(snapshot);const second=play();
 assert.deepEqual(second,first,'restored gameplay must reproduce visual and audio output');
 console.log('WASM heap rollback probe reproduced 40 gameplay frames and audio');
})().catch(e=>{console.error(e);process.exitCode=1;});
