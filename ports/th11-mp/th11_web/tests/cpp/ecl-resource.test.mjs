import test from 'node:test';import assert from 'node:assert/strict';import{readFileSync,readdirSync}from'node:fs';import{resolve}from'node:path';
import{core,oracle,memory,report,root}from'./helpers.mjs';
test('TH11 ECL files, merged lookup and dependency callbacks match the original loader',async()=>{
 const c=await core(),m=await oracle(),program=m.allocate(0x1100),vtable=m.allocate(16),manager=m.allocate(0x400),cpp=c.ecl_create(),cppName=c.allocate(1024),nativeName=m.allocate(1024);let checks=0,files=0,definitions=0,instructions=0;const events=[],sources=[];
 const text=p=>{const b=memory(c,p,1024);return new TextDecoder().decode(b.subarray(0,b.indexOf(0)));};
 m.u32(program,vtable);m.u32(vtable+4,0x410d50);m.u32(0x4a8d7c,manager);
 m.u32(vtable+8,m.registerImport({dll:'ecl-fixture',name:'include',argc:1,handler:()=>{events.push(['include',m.string(m.u32(m.reg('ESP')+4))]);return 0;}}));
 m.replace(0x454360,'load-animation',()=>{events.push(['animation',m.reg('ECX'),m.string(m.reg('EBX'))]);return manager;});
 try{for(const name of readdirSync(resolve(root,'reference/assets')).filter(n=>n.endsWith('.ecl')).sort()){
  const bytes=readFileSync(resolve(root,'reference/assets',name)),p=m.allocate(bytes.length),data=c.allocate(bytes.length);m.write(p,bytes);memory(c,data,bytes.length).set(bytes);events.length=0;
  assert.equal(m.call(0x45d900,{ecx:program,args:[p]}),files,name);assert.equal(c.ecl_attach(cpp,data,bytes.length),files,name);c.release(data);sources.push({p,size:bytes.length,cpp:c.ecl_file(cpp,files)});
  const expected=[];for(let i=0;i<c.ecl_include_count(cpp,files,1);++i)expected.push(['animation',8+i,text(c.ecl_include_name(cpp,files,i,1))]);for(let i=0;i<c.ecl_include_count(cpp,files,0);++i)expected.push(['include',text(c.ecl_include_name(cpp,files,i,0))]);assert.deepEqual(events,expected,name+' includes');
  const count=m.u32(program+8),table=m.u32(program+0x8c);assert.equal(c.ecl_count(cpp,1),count);assert.equal(c.ecl_count(cpp,0),files+1);
  for(let i=0;i<count;++i){const key=m.string(m.u32(table+i*8)),header=m.u32(table+i*8+4),source=sources.find(s=>header>=s.p&&header<s.p+s.size);assert.equal(text(c.ecl_name(cpp,i)),key);assert.equal(c.ecl_header(cpp,i)-source.cpp,header-source.p);
   const n=c.ecl_size(cpp,i);assert.deepEqual(Buffer.from(memory(c,c.ecl_header(cpp,i),n)),Buffer.from(m.bytes(header,n)),name+' bytecode '+key);
   const nb=Buffer.from(key+'\0');m.write(nativeName,nb);memory(c,cppName,nb.length).set(nb);m.reg('EAX',program);const found=m.call(0x45db10,{args:[nativeName]}),which=sources.find(s=>found>=s.p&&found<s.p+s.size);assert.equal(c.ecl_find(cpp,cppName)-which.cpp,found-which.p,name+' lookup '+key);++checks;
  }
  const missing=Buffer.from('!not-a-subroutine\0');m.write(nativeName,missing);memory(c,cppName,missing.length).set(missing);m.reg('EAX',program);assert.equal(m.call(0x45db10,{args:[nativeName]}),0);assert.equal(c.ecl_find(cpp,cppName),0);
  const n=bytes.readUInt16LE(16),tableOff=36+bytes.readUInt16LE(6),offsets=Array.from({length:n},(_,i)=>bytes.readUInt32LE(tableOff+i*4));definitions+=n;for(const off of offsets){const end=Math.min(bytes.length,...offsets.filter(p=>p>off));for(let p=off+16;p<end;p+=bytes.readUInt16LE(p+6))++instructions;}
  ++files;
 }
 // Malformed uploads must not mutate a working program.
 const valid=readFileSync(resolve(root,'reference/assets/default.ecl')),before=c.ecl_count(cpp,1);for(const kind of ['truncated','magic','offset','length','unterminated']){const bytes=Buffer.from(valid);let length=bytes.length;if(kind==='truncated')length=30;if(kind==='magic')bytes[0]=0;if(kind==='offset')bytes.writeUInt32LE(0xffffffff,64);if(kind==='length')bytes.writeUInt16LE(0,160+16+6);if(kind==='unterminated')bytes.fill(65,88,160);const p=c.allocate(bytes.length);memory(c,p,bytes.length).set(bytes);assert.equal(c.ecl_attach(cpp,p,length),-1,kind);assert.equal(c.ecl_count(cpp,1),before);c.release(p);}
 report('ecl-resources',{passed:true,files,definitions,instructions,lookupChecks:checks,dependencyCallbacks:true,scope:'loading, ordered merge, bytecode integrity, original binary lookup; instruction execution is not covered'});
 }finally{c.ecl_delete(cpp);c.release(cppName);m.close();}
});
