import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,string,report} from './helpers.mjs';

test('TH17 new-run state preserves native score, power, life selection and record bookkeeping',async()=>{
 const c=await core(),m=await oracle(),game=m.allocate(0x200),records=m.allocate(0x2d200),vm=m.allocate(0x600),player=m.allocate(0x19100),hud=m.allocate(0x300),loading=m.allocate(0x40);
 let f=0,events=[],step=100,maximum=400,cases=0,checks=0;
 // Execute the original factory through its run-initialization block, then
 // enter its normal epilogue. World construction is verified independently.
 const jump=Buffer.from([0xe9,0,0,0,0]);jump.writeInt32LE(0x430c9e-0x4305a0-5,1);m.write(0x4305a0,jump);
 m.u32(0x4b76b0,game);m.u32(0x4b77dc,records);m.u32(0x4b5c9c,vm);m.u32(0x4b77d0,player);m.u32(0x509a20,loading);m.i32(loading+0x20,-1);
 m.replace(0x466de0,'scene animation presentation outside run-state block',()=>0);
 m.replace(0x446d90,'independently verified player initialization boundary',()=>{events.push(2);m.i32(0x4b5a38,step);m.i32(0x4b5a34,maximum);return player;});
 m.replace(0x449630,'independently verified initial options boundary',()=>{events.push(3,m.i32(0x4b5a30));return 0;});
 m.replace(0x42fd50,'previous HUD stock boundary',()=>{events.push(1,m.i32(m.reg('ESP')+4),m.i32(m.reg('ESP')+8));return 0;},2);
 m.onImport=((previous)=>e=>{if(e.name==='Sleep')m.ret(0,1);else previous(e);})(m.onImport);
 const dv=()=>new DataView(c.memory.buffer),field=n=>c.game_start_field(f,n),write=(cp,np,value)=>{dv().setInt32(cp,value,true);m.i32(np,value);},same=(cp,np,n,label)=>{assert.deepEqual(Buffer.from(memory(c,cp,n)),Buffer.from(m.bytes(np,n)),label);checks++;};
 try{
  for(let stage=1;stage<=7;stage++)for(let loadout=0;loadout<9;loadout++)for(const mode of [0,0x10,0x20,0x30])for(const fresh of [0,1])for(const practice of [0,1]){
   f=c.game_start_create();m.view(game,0x200).fill(0);m.view(records,0x2d200).fill(0);m.i32(0x4b61e0,fresh);m.u32(0x4b76ac,cases%3?hud:0);dv().setUint8(field(22),cases%3?1:0);
   const char=Math.floor(loadout/3),animal=loadout%3,difficulty=loadout%5,spell=cases%101,flags=mode|4|(cases%3===0?8:0),slot=cases%4===0?1:-1,selectedLives=cases%11;
   for(const [cp,np,value]of [[field(0),0x4b59c0,cases%2?100:0x7fffffff],[field(0)+4,0x4b59c4,3],[field(0)+8,0x4b5a04,2],[field(0)+12,0x4b59c8,flags],[field(1),0x4b59dc,stage],[field(2),0x4b59f4,char],[field(3),0x4b59f8,animal],[field(4),0x4b5a00,difficulty],[field(5),0x4b5a14,spell],[field(7),0x4b59cc,selectedLives],[field(8),0x4b23e4,slot],[field(9),0x4b5a28,13579],[field(10),0x4b59d0,99],[field(14),0x4b5a0c,1001]])write(cp,np,value);
   dv().setUint8(field(6),practice);m.i32(game+0xa8,practice);
   const p=field(11),s=field(12),r=field(13),ps=[[0,0x4b5a40],[4,0x4b5a44],[8,0x4b5a18],[24,0x4b5a30],[28,0x4b5a38],[32,0x4b5a4c],[36,0x4b5a50],[40,0x4b5a54]],ss=[[0,0x4b59fc],[4,0x4b5a24],[8,0x4b5a2c],[12,0x4b5a20],[16,0x4b5a48],[24,0x4b5a34]];
   for(const [off,np]of ps)write(p+off,np,off===36?2:17+off);for(const [off,np]of ss)write(s+off,np,off===0?123456789:200+off);
   const state=Buffer.alloc(104);for(let i=0;i<26;i++)state.writeInt32LE(i===16||i===21?cases%2:70+i,i*4);memory(c,r,104).set(state);m.write(0x4b5a60,state);
   step=[100,100,100,50][cases%4];maximum=[200,400,500][cases%3];dv().setInt32(field(20),step,true);dv().setInt32(field(21),maximum,true);
   for(let d=0;d<5;d++){
    write(field(15)+(loadout*5+d)*8,records+0x18+loadout*0x4820+d*0x140,6000+loadout*100+d);
    write(field(15)+(loadout*5+d)*8+4,m.allocate(4),cases%2?255:2);m.view(records+0x1d+loadout*0x4820+d*0x140,1)[0]=cases%2?255:2;
    for(let st=0;st<8;st++)write(field(16)+((loadout*5+d)*8+st)*4,records+0x46a0+loadout*0x4820+d*64+st*8,7000+d*100+st);
   }
   write(field(17)+(loadout*101+spell)*4,records+0x970+loadout*0x4820+spell*0x9c,87654321);
   const count=[0,9999998,9999999,10000000][cases%4];write(field(18)+loadout*4,records+0x4664+loadout*0x4820,count);
   events=[];assert.equal(m.call(0x430080),0);assert.equal(c.game_start_begin(f,fresh),1,string(c,c.game_start_error(f)));const label=`${stage}/${loadout}/${mode}/${fresh}/${practice}`;
   assert.deepEqual(Array.from(new Int32Array(c.memory.buffer,c.game_start_events(f),c.game_start_event_count(f))),events,label+' boundaries');
   for(const [off,np]of [[0,0x4b59c0],[4,0x4b59c4],[8,0x4b5a04],[12,0x4b59c8]])same(field(0)+off,np,4,label+' scoring '+off);
   for(const [cp,np]of [[field(4),0x4b5a00],[field(9),0x4b5a28],[field(10),0x4b59d0],[field(14),0x4b5a0c],[field(18)+loadout*4,records+0x4664+loadout*0x4820],...ps.map(([off,np])=>[p+off,np]),...ss.map(([off,np])=>[s+off,np])])same(cp,np,4,label+' state');
   same(r,0x4b5a60,104,label+' retained roaring inventory and reset clocks');
   for(let d=0;d<5;d++)for(let st=0;st<8;st++)same(field(19)+(loadout*5+d)*8+st,records+0x46a5+loadout*0x4820+d*64+(st+1)*8,1,label+' available stage');
   c.game_start_delete(f);f=0;cases++;
  }
  report('game-start',{passed:true,cases,checks,originalFunction:'0x430080 (0x430146–0x4305a0 run initialization block)',scope:'Native new and retained-run initialization, all nine loadouts, seven stage selections, all four mode classes, Extra difficulty correction, practice lives, continue preservation, high-score record selection, score/point/power/life/bomb resets, retained Roaring inventory, timers and capped starts. Player construction/options are independently verified boundaries; this does not assert complete scene-factory or replay correctness.'});
 }finally{if(f)c.game_start_delete(f);m.close();}
});
