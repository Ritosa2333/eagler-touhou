import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';
const bits=f=>{const b=Buffer.alloc(4);b.writeFloatLE(f);return b.readUInt32LE();};
function ins(op,args=[],{refs=0,time=0,count=args.length}={}){const b=Buffer.alloc(16+args.length*4);b.writeInt32LE(time);b.writeUInt16LE(op,4);b.writeUInt16LE(b.length,6);b.writeUInt16LE(refs,8);b[10]=255;b[11]=count;args.forEach((v,i)=>b.writeUInt32LE(v>>>0,16+i*4));return b;}
function call(name,params=[],{op=11,refs=0,id=null,time=0}={}){const text=Buffer.from(name+'\0'),length=(text.length+3)&~3,b=Buffer.alloc(20+length+(id===null?0:4)+params.length*8);ins(op,[],{refs,time,count:params.length+(id===null?1:2)}).copy(b);b.writeUInt16LE(b.length,6);b.writeUInt32LE(length,16);text.copy(b,20);let p=20+length;if(id!==null){b.writeInt32LE(id,p);p+=4;}for(const [from,to,value] of params){b[p]=from.charCodeAt();b[p+1]=to.charCodeAt();b.writeUInt32LE(value>>>0,p+4);p+=8;}return b;}
function resource(subs){const names=Object.keys(subs).sort(),strings=Buffer.concat(names.map(n=>Buffer.from(n+'\0'))),start=(36+names.length*4+strings.length+3)&~3,head=Buffer.alloc(start);head.write('SCPT');head.writeUInt16LE(1,4);head.writeUInt16LE(names.length,16);strings.copy(head,36+names.length*4);const chunks=[head];let off=start;for(let i=0;i<names.length;++i){head.writeUInt32LE(off,36+i*4);const sub=Buffer.alloc(16);sub.write('ECLH');sub.writeUInt32LE(16,4);const instructions=Buffer.concat(subs[names[i]]);chunks.push(sub,instructions);off+=sub.length+instructions.length;}return Buffer.concat(chunks);}
test('TH11 ECL calls, recursive frames and thread order match original',async()=>{
 const c=await core(),m=await oracle(),program=m.allocate(0x1100),pv=m.allocate(16),cpp=c.ecl_create(),g=c.ecl_globals_create(),co=c.ecl_owner_create(cpp),no=m.allocate(0x1040),nv=m.allocate(32),ci=c.allocate(1024),ni=m.allocate(1024),ct=c.allocate(0x1024),nt=m.allocate(0x1024);let callChecks=0,frameChecks=0,threadChecks=0;
 const nativeEvents=[],cppEvents=[];let released=0;
 const put=(p,o,v)=>new DataView(c.memory.buffer).setUint32(p+o,v>>>0,true),get=(p,o)=>new DataView(c.memory.buffer).getUint32(p+o,true);
 m.u32(program,pv);m.u32(pv+4,m.registerImport({dll:'fixture',name:'header',argc:0,handler:()=>0}));
 m.replace(0x45fce4,'new',()=>m.allocate(m.u32(m.reg('ESP')+4)));m.replace(0x45fd49,'delete',()=>{++released;return 0;});
 m.u32(nv,m.registerImport({dll:'fixture',name:'command',argc:0,handler:()=>{const p=m.u32(no+4);nativeEvents.push({id:m.i32(p+0x1010),time:m.f32(p),op:m.u32(m.u32(p+4)+4)&65535});return 0;}}));
 const code=resource({
  Main:[ins(40,[16]),call('WorkerA',[['i','f',123456789]],{op:16,id:11}),call('WorkerB',[],{op:16,id:22}),ins(20,[11,3]),ins(18,[11]),ins(19,[11]),ins(400),ins(17,[22],{time:1}),ins(21,[],{time:4}),ins(1,[],{time:6})],
  Nested:[ins(40,[12]),ins(42,[16777217]),call('Sub',[['i','f',0xffffffff]],{refs:2}),ins(400),ins(10)],
  Sub:[ins(40,[4]),ins(400),ins(10)],
  WorkerA:[ins(40,[8]),ins(401),ins(401,[],{time:2}),ins(10,[],{time:5})],
  WorkerB:[ins(40,[0]),ins(402),ins(402,[],{time:1}),ins(10,[],{time:3})]
 });
 const nf=m.allocate(code.length),data=c.allocate(code.length);m.write(nf,code);memory(c,data,code.length).set(code);assert.equal(m.call(0x45d900,{ecx:program,args:[nf]}),0);assert.equal(c.ecl_attach(cpp,data,code.length),0);c.release(data);const cf=c.ecl_file(cpp,0);
 const mappings=[[no,co,0x1040],[ni,ci,1024],[nt,ct,0x1024],[nf,cf,code.length]];
 const norm=(v,native)=>{if(!v)return 0;for(let j=0;j<mappings.length;++j){const [n,p,size]=mappings[j],base=native?n:p;if(v>=base&&v<base+size)return 0x10000000+j*0x100000+v-base;}return v;};
 const compare=(n,p,label)=>{for(let off=0;off<0x1024;off+=4)assert.equal(norm(get(p,off),false),norm(m.u32(n+off),true),`${label} +${off.toString(16)}`);};
 const initialize=()=>{memory(c,co+8,0x1024).fill(0);m.write(no,Buffer.alloc(0x1040));m.u32(no,nv);m.u32(no+0x102c,program);c.ecl_owner_initialize(co);m.reg('EAX',no);m.call(0x40fed0);put(co+8,0x101c,255);m.u32(no+8+0x101c,255);};
 const find=(name)=>{const b=Buffer.from(name+'\0');m.write(ni,b);memory(c,ci,b.length).set(b);m.reg('EAX',program);return [m.call(0x45db10,{args:[ni]}),c.ecl_find(cpp,ci)];};
 try{
  for(const separate of [false,true])for(const oldTop of [0,16,64,1024])for(const skipped of [0,1])for(const referenced of [false,true])for(const from of ['i','f','g'])for(const to of ['i','f']){
   initialize();const source=co+8,ns=no+8,target=separate?ct:source,nTarget=separate?nt:ns;memory(c,ct,0x1024).fill(0);m.write(nt,Buffer.alloc(0x1024));
   const state=Buffer.alloc(0x1008);for(let j=0;j<4096;++j)state[j]=(j*19+41)&255;state.writeUInt32LE(oldTop,4096);state.writeUInt32LE(128,4100);state.writeUInt32LE(from==='i'?16777217:bits(19.75),128);
   memory(c,source+8,state.length).set(state);m.write(ns+8,state);memory(c,target+8,state.length).set(state);m.write(nTarget+8,state);put(target,0x1014,co);m.u32(nTarget+0x1014,no);
   put(source,0,bits(7.25));m.f32(ns,7.25);const raw=referenced?(from==='i'?0:bits(0)):(from==='i'?16777217:bits(-27.875)),b=call('Sub',[[from,to,raw]],{id:skipped?22:null,refs:referenced?1<<(skipped+1):0});
   memory(c,ci,b.length).set(b);m.write(ni,b);put(source,4,ci);m.u32(ns+4,ni);m.reg('EAX',nTarget);m.reg('EDI',ns);m.resetThreadFPU();assert.equal(c.ecl_call(target,source,skipped,g),m.call(0x45b610,{args:[skipped]})|0);compare(nTarget,target,'call target');compare(ns,source,'call source');assert.equal(norm(get(co,4),false),norm(m.u32(no+4),true));++callChecks;
  }
  // Nested call pops an argument from its own stack and restores the caller.
  initialize();let [nStart,cStart]=find('Nested');m.u32(no+12,nStart);put(co+8,4,cStart);m.reg('EDI',no);const nr=m.call(0x45d420,{args:[bits(1)]})|0,cr=c.ecl_owner_update(co,g,1);assert.equal(cr,nr);compare(no+8,co+8,'nested completion');++frameChecks;
  // Independent threads are prepended, and start on the next owner update.
  initialize();[nStart,cStart]=find('Main');m.u32(no+12,nStart);put(co+8,4,cStart);nativeEvents.length=0;c.ecl_fixture_event_count(g,1);
  const addMappings=()=>{let n=m.u32(no+0x1034),p=get(co,0x1034);while(n||p){assert.ok(n&&p,'thread counts');const nc=m.u32(n),pc=get(p,0);if(!mappings.some(x=>x[0]===n))mappings.push([n,p,12],[nc,pc,0x1024]);compare(nc,pc,'thread');assert.equal(norm(m.u32(n+8),true),norm(get(p,8),false));n=m.u32(n+4);p=get(p,4);++threadChecks;}};
  for(let frame=0;frame<8;++frame){m.reg('EDI',no);m.resetThreadFPU();const n=m.call(0x45d420,{args:[bits(1)]})|0,p=c.ecl_owner_update(co,g,1);assert.equal(p,n,'owner return');addMappings();compare(no+8,co+8,'root frame '+frame);assert.equal(norm(get(co,4),false),norm(m.u32(no+4),true));++frameChecks;if(n===-1)break;}
  const eventBytes=Buffer.from(memory(c,c.ecl_fixture_events(g),c.ecl_fixture_event_count(g,0)*12));for(let off=0;off<eventBytes.length;off+=12)cppEvents.push({id:eventBytes.readInt32LE(off),time:eventBytes.readFloatLE(off+4),op:eventBytes.readUInt32LE(off+8)});
  assert.deepEqual(nativeEvents,[{id:-1,time:0,op:400},{id:11,time:0,op:401},{id:11,time:2,op:401}]);assert.deepEqual(cppEvents,nativeEvents,'C++ command callback order');assert.equal(released,4);
  // Missing callees terminate the source and leave the attempted target active.
  initialize();const missing=call('Absent');memory(c,ci,missing.length).set(missing);m.write(ni,missing);put(co+8,4,ci);m.u32(no+12,ni);m.reg('EAX',no+8);m.reg('EDI',no+8);assert.equal(m.call(0x45b610,{args:[0]})|0,-1);assert.equal(c.ecl_call(co+8,co+8,0,g),-1);compare(no+8,co+8,'missing callee');++callChecks;
  report('ecl-control',{passed:true,callChecks,frameChecks,threadChecks,nativeEvents,scope:'argument calls/conversion, self-stack argument, frame return, thread insertion/cancel/state/flag/scheduling, failed lookup'});
 }finally{c.ecl_owner_delete(co,g);c.ecl_delete(cpp);c.ecl_globals_delete(g);c.release(ci);c.release(ct);m.close();}
});
