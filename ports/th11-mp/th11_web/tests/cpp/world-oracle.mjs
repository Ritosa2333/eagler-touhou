// Development-only original world harness. Host file/GPU/audio boundaries are
// replaced; original manager construction, ECL and gameplay callbacks execute.
import {readFileSync} from 'node:fs';import {resolve} from 'node:path';
import {root} from './helpers.mjs';import {animationOracle} from './anm-oracle.mjs';import {graphicsOracle} from './graphics-oracle.mjs';
export function worldOracle(m,{stage,character,subtype,difficulty,demo=0,replayData=null,campaign=false}){
 const o=animationOracle(m);
 const files=new Map(),anms=new Map(),arg=n=>m.u32(m.reg('ESP')+4+n*4),alloc=n=>{const p=m.allocate(n);m.view(p,n).fill(0);return p;};
 const read=name=>{name=name.replaceAll('\\','/').split('/').pop();if(replayData&&name===`demo${demo}.rpy`)return replayData;if(!files.has(name))files.set(name,readFileSync(resolve(root,'reference/assets',name)));return files.get(name);};
 for(const [slot,name]of [[0,'text.anm'],[2,'ascii.anm'],[(stage&1)+3,`stage${String(stage).padStart(2,'0')}.anm`],[5,'front.anm'],[6,'bullet.anm'],[7,`pl${String(character).padStart(2,'0')}.anm`],[8,'enemy.anm'],[9,`stgenm${String(stage).padStart(2,'0')}.anm`],[27,`st${String(stage).padStart(2,'0')}logo.anm`]]){
  const loaded=o.load(name,anms.size>0);m.u32(loaded.original,slot);anms.set(name,loaded.original);m.u32(o.manager+0x4350c0+slot*4,loaded.original);
 }
 if(campaign)for(let n=1;n<=7;++n)if(n!==stage)for(const[slot,name]of[[(n&1)+3,`stage0${n}.anm`],[9,`stgenm0${n}.anm`],[27,`st0${n}logo.anm`]]){const loaded=o.load(name,true);m.u32(loaded.original,slot);anms.set(name,loaded.original);}
 graphicsOracle(m);o.reset();m.i32(o.manager,-1);
 // Long replays must honor frees. Other small fixtures deliberately use a
 // bump heap, which exhausts its development-only address space in a battle.
 const bump=m.allocate.bind(m),sizes=new Map(),freeLists=new Map();let reused=0;
 m.allocate=size=>{size=Math.max(16,(size+15)&~15);const list=freeLists.get(size),p=list?.length?list.pop():bump(size);if(sizes.has(p))throw Error('oracle allocation still live');sizes.set(p,size);if(list)++reused;return p;};
 const free=p=>{const size=sizes.get(p);if(size===undefined)return;sizes.delete(p);if(!freeLists.has(size))freeLists.set(size,[]);freeLists.get(size).push(p);};
 m.replace(0x45fc3b,'world free',()=>{free(arg(0));return 0;});m.replace(0x45fd49,'world delete',()=>{free(arg(0));return 0;});
 const resourceHeaders=new Map([...anms].map(([name,p])=>[name,Buffer.from(m.bytes(p,0x134))]));
 const prepared=(name,slot)=>{const p=anms.get(name);if(!p)throw Error('ANM not prepared: '+name);if(!m.u32(o.manager+0x4350c0+slot*4))m.write(p,resourceHeaders.get(name));m.u32(p,slot);m.u32(o.manager+0x4350c0+slot*4,p);return p;};
 m.replace(0x454360,'preloaded ANM resource',()=>prepared(m.string(m.reg('EBX'),128),m.reg('ECX')));
 m.replace(0x454190,'preloaded ANM resource async',()=>prepared(m.string(m.reg('ECX'),128),arg(1)),2);
 m.replace(0x458400,'archive source',()=>{const name=m.string(m.reg('EAX'),128),bytes=read(name),p=alloc(bytes.length);if(process.env.TH11_ORACLE_TRACE)console.log("read",name,bytes.length);m.write(p,bytes);if(arg(0))m.u32(arg(0),bytes.length);return p;},2);
 const format=(out,fmt,values)=>{let pointer=values;const string=p=>{const b=Buffer.from(m.bytes(p,2048)),n=b.indexOf(0);return b.subarray(0,n<0?b.length:n).toString('latin1');};
  const result=string(fmt).replace(/%([ 0+-]*)(\d*)(?:\.(\d+))?([sdiuxXfc%])/g,(_,flags,width,precision,type)=>{if(type==='%')return '%';let result;
   if(type==='f'){const n=Buffer.from(m.bytes(pointer,8)).readDoubleLE();pointer+=8;result=n.toFixed(precision===undefined?6:+precision);}
   else {const v=m.u32(pointer);pointer+=4;result=type==='s'?string(v):type==='c'?String.fromCharCode(v&255):type==='u'?String(v):type==='x'||type==='X'?v.toString(16):String(v|0);if(precision!==undefined&&type!=='s')result=result.padStart(+precision,'0');if(type==='X')result=result.toUpperCase();}
   return flags.includes('-')?result.padEnd(+width||0,' '):result.padStart(+width||0,flags.includes('0')?'0':' ');
  });m.write(out,Buffer.from(result+'\0','latin1'));return result.length;
 };
 m.replace(0x46025c,'host sprintf',()=>format(arg(0),arg(1),m.reg('ESP')+12));
 m.replace(0x45fdd2,'host vsprintf',()=>format(arg(0),arg(1),arg(2)));
 const scheduler=alloc(0x100),supervisor=alloc(0x18500),game=alloc(0x100),scores=alloc(0x30000),vm=alloc(0x434),clock=alloc(0x80),device=alloc(4),vtable=alloc(0x180);
 for(const [a,p]of [[0x4c3234,scheduler],[0x4a8d58,supervisor],[0x4a8e88,game],[0x4a8ebc,scores],[0x4c343c,vm],[0x4a8d80,clock],[0x4c3288,device]])m.u32(a,p);
 m.u32(device,vtable);m.u32(vtable+0xb0,m.registerImport({dll:'world-reference',name:'GPU matrix',argc:3,handler:()=>0}));
 m.reg('ESI',vm);m.call(0x401fd0);m.u32(supervisor+0x184ac,anms.get('ascii.anm'));m.u32(0x4c3808,anms.get('text.anm'));
 // Enable the original screen-copy surface. Its absence disables Bomb
 // deformation construction and incorrectly removes gameplay RNG calls.
 m.u32(0x4c342c,1);m.u32(0x4c3810,0x4000);m.call(0x42aae0,{ecx:0x4c3280});
 m.u32(0x4a8ec8,0x4a3828+stage*64);m.u32(0x4c37e4,1);m.u32(0x4a5758,0x20);m.u32(game+0x74,1);
 for(const [a,v]of [[0x4a5710,character],[0x4a5714,subtype],[0x4a5720,difficulty],[0x4a5728,stage]])m.i32(a,v);
 m.write(0x4a8d88,Buffer.from(`demo${demo}.rpy\0`));
 const baseImports=m.onImport;m.onImport=e=>{if(e.name==='Sleep'){m.ret(0,1);return;}if(e.name==='timeGetTime'||e.name==='GetTickCount'){m.ret(1000,0);return;}baseImports(e);};
 // Audio loading/control, external resource threads, and logging only.
 for(const [address,argc]of [[0x42a240,0],[0x42a0d0,2],[0x42a640,0],[0x42a740,0],[0x459430,0]])m.replace(address,'world host boundary',()=>0,argc);
 for(const [a,n]of [[0x44a1e0,0],[0x44a260,1],[0x44a300,0],[0x42a150,2],[0x42a270,1]])m.replace(a,'audio output boundary',()=>0,n);
 m.replace(0x454c60,'offline verified glyph raster/upload boundary',()=>0);
 m.replace(0x454d00,'offline verified spell glyph raster/upload boundary',()=>0);
 const drawClocks=process.env.TH11_WORLD_DRAW_CLOCKS!=='0';
 if(campaign||drawClocks)m.replace(0x403885,'end of native stage foreground clock',()=>1);
 if(drawClocks){
  m.replace(0x403384,'end of native stage background transition',()=>1);
  m.replace(0x403528,'end of native stage effect draw counter',()=>1);
 }
 const backgroundClock=p=>{if(!p||(m.u32(p+0x2ff0)&8))return;m.reg('EBX',p);m.call(0x403339);if(m.u32(p+0x2ff0)&1){m.reg('EBX',p);m.call(0x403511);}};
 const foregroundClock=p=>{if(!p||(m.u32(p+0x2ff0)&8))return;m.reg('EBP',p);m.reg('EBX',0);m.call(0x403835);};
 return {o,anms,scheduler,game,supervisor,heap_stats:()=>({live:sizes.size,reused,heap:m.heap}),construct(){return m.call(0x41f8f0,{limit:100000000});},tick(){o.update(true);m.reg('EBX',scheduler);const result=m.call(0x456cb0,{limit:100000000});o.update(false);return result;},next_stage(){
  // The desktop supervisor services MSG21's transition between frame calls.
  // Run the original teardown, stage increment and construction; only its
  // asynchronous host-thread allocation is replaced with zeroed game storage.
  if(m.u32(0x4c37d8)!==12)throw Error('native next-stage request absent');
  m.call(0x41fe10,{args:[m.u32(0x4a8e88)],limit:100000000});
  // MSG21 has already advanced 4a5728 / 4a8ec8 through native 41f450.
  m.u32(0x4c37e4,0);const incoming=alloc(0x78);m.u32(incoming+0x74,1);m.u32(0x4a8e88,incoming);
  const result=m.call(0x41f8f0,{limit:100000000});m.u32(0x4c37d8,0);return result;
 },draw_transition(){const old=m.u32(0x4a8d5c),active=m.u32(0x4a8d60);if(drawClocks){backgroundClock(active);if(old!==active)backgroundClock(old);foregroundClock(active);}if(old!==active)foregroundClock(old);},heap_state(state){if(state){m.heap=state.heap;reused=state.reused;sizes.clear();freeLists.clear();for(const pair of state.sizes)sizes.set(...pair);for(const pair of state.freeLists)freeLists.set(...pair);}return {heap:m.heap,reused,sizes:[...sizes],freeLists:[...freeLists]};}};
}
