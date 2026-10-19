import test from'node:test';import assert from'node:assert/strict';import{readFileSync}from'node:fs';import{resolve}from'node:path';import{core,memory,root,report}from'./helpers.mjs';
test('TH11 completed session retains recording through ending and title results',async()=>{
 const c=await core(),raw=readFileSync(resolve(root,'../[th11] 东方地灵殿 (汉化版+日文版)/th11.dat')),data=c.allocate(raw.length),source=c.resources_create();memory(c,data,raw.length).set(raw);assert.equal(c.resources_open(source,data,raw.length),1);
 let frames=0;
 try{for(let selection=0;selection<6;++selection)for(const continues of[0,1]){
  const s=c.game_session_create(),out=c.allocate(1<<20),name=c.allocate(9);memory(c,name,9).set(Buffer.from('ENDING  \0'));
  const tick=(held=0)=>{assert.equal(c.game_session_update(s,held,0,0,0,0,0),1);++frames;};
  try{assert.equal(c.game_session_begin(s,source,6,Math.floor(selection/3),selection%3,1,0),1);for(let i=0;i<120;++i)tick();c.game_session_end_probe(s,continues);tick();assert.equal(c.game_session_value(s,0),5);
   let n=0;while(c.game_session_value(s,0)===5&&n++<24000)tick(n%17===0?1:0);assert.equal(c.game_session_value(s,0),0,'ending returns title');
   const title=c.game_session_title_menu(s);assert.equal(c.title_value(title,0),14);for(let i=0;i<12;++i)tick();
   for(let i=0;i<9;++i){tick(1);tick();}for(let i=0;i<8;++i)tick();assert.equal(c.title_value(title,0),15,'results lead to replay save');
   const length=c.game_session_save_replay(s,name,out,1<<20);assert.ok(length>0,'recording survives battle teardown');const r=c.replay_create();try{assert.equal(c.replay_open(r,out,length),1);const header=memory(c,c.replay_data(r),0x70);assert.equal(new DataView(header.buffer,header.byteOffset,header.byteLength).getInt32(104,true),8);assert.equal(new DataView(header.buffer,header.byteOffset,header.byteLength).getInt32(108,true),continues);}finally{c.replay_delete(r);}
  }finally{c.release(out);c.release(name);c.game_session_delete(s);}
 }report('ending-session',{passed:true,cases:12,frames,scope:'Forced stage-6 completion boundary, all character ending selections with/without continue, staff roll, title result name entry, recording preservation and original-format decoding. Not full campaign playthrough.'});
 }finally{c.resources_delete(source);c.release(data);}
});
