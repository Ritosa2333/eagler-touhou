// Development oracle snapshots only. Production does not include this module.
import {readFileSync,writeFileSync,mkdirSync,existsSync} from 'node:fs';
import {resolve} from 'node:path';
import {gzipSync,gunzipSync} from 'node:zlib';

export function nativeStageCheckpoint(machine,directory,identity){
 const path=directory&&resolve(directory,identity.replaySha256+'-'+identity.stage);
 const registers=['EAX','EBX','ECX','EDX','ESI','EDI','EBP','EFLAGS','FPCW','FPSW','FPTAG','MXCSR'];
 return {
  load(){
   if(!path||!existsSync(path+'.json'))return null;
   const state=JSON.parse(readFileSync(path+'.json','utf8'));
   if(JSON.stringify(state.identity)!==JSON.stringify(identity)||state.initialHeap!==machine.heap)return null;
   for(let i=0;i<state.ranges.length;i++){
    const [address,length]=state.ranges[i],bytes=gunzipSync(readFileSync(path+'.'+i+'.gz'));
    if(bytes.length!==length)throw Error('Invalid native checkpoint range');
    machine.write(address,bytes);
   }
   machine.heap=state.heap;
   for(const [name,value]of Object.entries(state.registers))machine.reg(name,value);
   return state.host;
  },
  save(host){
   if(!path)return;
   mkdirSync(directory,{recursive:true});
   const initialHeap=identity.initialHeap,ranges=[[0x10000,0x1ff0000],[0x07000000,0x1000000],[0x08000000,machine.heap-0x08000000]];
   for(let i=0;i<ranges.length;i++){
    const [address,length]=ranges[i];writeFileSync(path+'.'+i+'.gz',gzipSync(machine.view(address,length),{level:1}));
   }
   writeFileSync(path+'.json',JSON.stringify({identity,initialHeap,heap:machine.heap,ranges,registers:Object.fromEntries(registers.map(n=>[n,machine.reg(n)])),host}));
  }
 };
}
