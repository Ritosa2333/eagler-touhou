import {test} from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
const binary=await readFile(process.env.TH01_NINE_CANDIDATE || new URL('../web/game.wasm',import.meta.url));
const make=async()=>(await WebAssembly.instantiate(binary,{})).instance.exports;
const positions=e=>Array.from({length:9},(_,i)=>[e.th_get(20,i),e.th_get(21,i)]);
test('nine orbs form a grid and simulation survives rollback',async()=>{
 const a=await make(), b=await make();for(const e of [a,b]){e.th_init(2,9,123,0);e.th_set_netplay_rules(1);assert.equal(e.th_get(4,0),9);}
 assert.deepEqual(positions(a),[264,296,328].flatMap(y=>[600,568,536].map(x=>[x,y])));
 const heap=new Uint8Array(a.memory.buffer), saved=heap.slice();let expected=[];
 for(let i=0;i<3000;i++){let x=i===0?4:(i%120<60?1:2)|(i%13===0?4:0);a.th_step(x,0,0);b.th_step(x,0,0);assert.equal(a.th_net_checksum(),b.th_net_checksum());if(i<200)expected.push(a.th_net_checksum());}
 assert(a.th_get(2,0)>0);heap.set(saved);
 for(let i=0;i<200;i++){let x=i===0?4:(i%120<60?1:2)|(i%13===0?4:0);a.th_step(x,0,0);assert.equal(a.th_net_checksum(),expected[i]);}
});
test('nine-orb boss health extends the eight-orb scaling',async()=>{
 for(const [p,hp] of [[1,1200],[2,1576],[3,1920]]){const e=await make();e.th_init(p,9,123,0);e.th_stage_command(14,0,5,0,0,0);assert.equal(e.th_get(61,0),hp);}
});

test('the ninth orb independently flips cards and participates in checksums',async()=>{
 const e=await make();e.th_init(1,9,123,0);const data=new DataView(e.memory.buffer),base=4171008;
 for(let i=0;i<8;i++)data.setInt32(base+i*96+16,1,true);
 const before=e.th_net_checksum();data.setInt32(base+8*96,data.getInt32(base+8*96,true)-1,true);assert.notEqual(e.th_net_checksum(),before);
 e.th_step(4,0,0);let best=0;for(let i=0;i<3000;i++){e.th_step(i%30<15?1:2,0,0);best=Math.max(best,e.th_get(2,0));}
 assert(best>0,'the ninth orb must generate score with the other eight inactive');
});

test('native options include nine and wrap in both directions',async()=>{
 const e=await make();e.th_init(1,1,123,0);e.th_menu_open(1,1);for(let n=0;n<3;n++)e.th_menu_input(1);e.th_menu_input(4);e.th_menu_input(1);e.th_menu_input(1);
 for(const expected of [2,3,4,6,8,9,1]){e.th_menu_input(3);assert.equal(e.th_get(54,0),expected)}
 e.th_menu_input(2);assert.equal(e.th_get(54,0),9);e.th_menu_input(2);assert.equal(e.th_get(54,0),8);
});
test('grid rows remain separated during launch',async()=>{
 for(const count of [4,6,8,9]){const e=await make();e.th_init(1,count,123,0);e.th_step(4,0,0);for(let i=0;i<8;i++){const xy=Array.from({length:count},(_,j)=>[e.th_get(20,j),e.th_get(21,j)]);for(let a=0;a<count;a++)for(let b=a+1;b<count;b++)assert(Math.abs(xy[a][0]-xy[b][0])>=25||Math.abs(xy[a][1]-xy[b][1])>=25,`${count} balls frame ${i}`);e.th_step(0,0,0)}}
});
