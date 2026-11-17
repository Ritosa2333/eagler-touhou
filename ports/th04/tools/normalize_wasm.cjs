// Expand Binaryen 132's optional compact import groups to the standard format.
// Preserve import order and type indices; no code or execution behavior changes.
const fs=require('node:fs'),path=require('node:path');
function normalize(input){
 const leb=n=>{const a=[];do{const b=n&127;n>>>=7;a.push(b|(n?128:0));}while(n);return Buffer.from(a);};
 let p=8;
 const read=()=>{let n=0,k=0,c;do{c=input[p++];n|=(c&127)<<k;k+=7;}while(c&128);return n>>>0;};
 const string=()=>{const start=p,length=read();p+=length;return input.subarray(start,p);};
 const desc=kind=>{const start=p;if(kind===0)read();else if(kind===4){p++;read();}else if(kind===2){const flags=read();if(flags&~3)throw Error('Unaudited memory import flags');read();if(flags&1)read();}else throw Error('Unaudited import descriptor '+kind);return Buffer.concat([Buffer.from([kind]),input.subarray(start,p)]);};
 const parts=[input.subarray(0,8)];
 while(p<input.length){const id=input[p++],n=read(),start=p,end=p+n;let body=input.subarray(start,end);
  if(id===2){const count=read(),entries=[];for(let i=0;i<count;i++){
   const module=string(),name=string(),kind=input[p++];
   if(name.length===1&&name[0]===0&&kind===127){const group=read();for(let j=0;j<group;j++){const field=string(),d=desc(input[p++]);entries.push(Buffer.concat([module,field,d]));}}
   else if(name.length===1&&name[0]===0&&kind===126){const d=desc(input[p++]),group=read();for(let j=0;j<group;j++)entries.push(Buffer.concat([module,string(),d]));}
   else entries.push(Buffer.concat([module,name,desc(kind)]));
  }if(p!==end)throw Error('Import section size mismatch');body=Buffer.concat([leb(entries.length),...entries]);
  }
  parts.push(Buffer.from([id]),leb(body.length),body);p=end;
 }
 const result=Buffer.concat(parts);new WebAssembly.Module(result);return result;
}
if(require.main===module){const root=path.resolve(__dirname,'..');fs.writeFileSync(path.join(root,'build/dosbox-asyncify-compatible.wasm'),normalize(fs.readFileSync(path.join(root,'build/dosbox-asyncify-native.wasm'))));console.log('Expanded compact imports, browser WASM validation passed');}
module.exports={normalize};
