const fs=require('node:fs'),path=require('node:path');
const root=path.resolve(__dirname,'..');
const input=path.resolve(root,process.argv[2]||'build/core-postopt-candidate.wasm');
const output=path.resolve(root,process.argv[3]||'build/core-postopt-compatible.wasm');
let bytes=require('./normalize_wasm.cjs').normalize(fs.readFileSync(input));
if(!WebAssembly.Module.exports(new WebAssembly.Module(bytes)).some(e=>e.name.startsWith('__rollback_g'))){
  const leb=n=>{const a=[];do{const b=n&127;n>>>=7;a.push(b|(n?128:0));}while(n);return Buffer.from(a);};
  const read=(b,s)=>{let n=0,k=0;for(;;){const c=b[s.p++];n|=(c&127)<<k;if(c<128)return n>>>0;k+=7;}};
  const state={p:8},sections=[],mutable=[];
  while(state.p<bytes.length){const id=bytes[state.p++],size=read(bytes,state),body=bytes.subarray(state.p,state.p+size);state.p+=size;sections.push([id,body]);if(id===6){const s={p:0},count=read(body,s);for(let i=0;i<count;i++){const type=body[s.p++],mut=body[s.p++];if(type!==127||body[s.p++]!==65)throw Error('unaudited global initializer');if(mut)mutable.push(i);read(body,s);if(body[s.p++]!==11)throw Error('global end');}}}
  if(mutable.length!==4)throw Error('native globals changed');
  const parts=[bytes.subarray(0,8)];
  for(let [id,body]of sections){if(id===5)body=Buffer.concat([leb(1),leb(1),leb(1024),leb(4096)]);if(id===7){const s={p:0},count=read(body,s),entries=[leb(count+mutable.length),body.subarray(s.p)];for(const i of mutable){const name=Buffer.from('__rollback_g'+i);entries.push(leb(name.length),name,Buffer.from([3]),leb(i));}body=Buffer.concat(entries);}parts.push(Buffer.from([id]),leb(body.length),body);}
  bytes=Buffer.concat(parts);new WebAssembly.Module(bytes);
}
fs.writeFileSync(output,bytes);
console.log('Prepared validated core candidate',path.relative(root,output));
