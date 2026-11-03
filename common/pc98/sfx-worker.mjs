import {workerEnvironment} from './sfx-worker-environment.js';
let emulator,mailbox=-1,active=true,timer,output=[];
const signature=new TextEncoder().encode('PC98LIVESFX0001!');
function findMailbox(){
  const heap=emulator.module.HEAPU8,view=new DataView(heap.buffer);
  for(let at=heap.indexOf(signature[0]);at>=0;at=heap.indexOf(signature[0],at+1)){
    if(signature.every((value,i)=>heap[at+i]===value)&&heap[at+19]===2&&view.getUint16(at+16,true)>0)return at;
  }
  return -1;
}
function step(){
  if(!active)return;
  emulator.step();
  for(const samples of output.splice(0))postMessage({type:'audio',samples},[samples.buffer]);
}
onmessage=async({data})=>{
  if(data.type==='start'){
    try{
      const canvas=new OffscreenCanvas(640,400);workerEnvironment(canvas,44100);
      const {DOSBoxPC98}=await import('./dosbox-adapter.js');
      emulator=await DOSBoxPC98.create({canvas,lockstep:true,rollbackEnabled:false,wasmBinary:data.wasm});
      const conf='[dosbox]\nmachine=pc98\nmemsize=16\n[cpu]\ncore=simple\ncputype=386\ncycles=16000\n[render]\nscaler=none\n[sblaster]\nsbtype=none\n[pc98]\npc-98 fm board=board86\n[mixer]\nrate=44100\nblocksize=512\nprebuffer=20\n[autoexec]\n@echo off\nmount a /home/web_user/game\na:\nPMD /M8 /V0 /E2 /K\nSFX.COM\n';
      emulator.ci=await globalThis.emulators.backend([
        {path:'game/PMD.COM',contents:new Uint8Array(data.driver)},
        {path:'game/SOUND.EFC',contents:new Uint8Array(data.bank)},
        {path:'game/SFX.COM',contents:new Uint8Array(data.fixture)},
        {dosboxConf:conf,jsdosConf:{version:'8'}},
      ],emulator.transport,{});
      emulator.ci.events().onSoundPush(samples=>output.push(samples.slice()));
      const messages=[];emulator.ci.events().onMessage((type,...parts)=>messages.push([type,...parts]));
      emulator.run();
      for(let tick=0;tick<10800&&mailbox<0;tick++){emulator.step();if(tick%15===0)mailbox=findMailbox();}
      if(mailbox<0){
        const m=emulator.module,h=m.HEAPU8,at=h.indexOf(signature[0]);
        throw Error('原始音效驱动未启动：'+JSON.stringify({messages:messages.slice(-8),program:m.UTF8ToString(m._emGetRunningProgram()),candidate:at,wakeAt:m.wakeAt,virtualNow:m.virtualNow,pendingWake:m.pendingWake,files:m.FS.readdir('/home/web_user/game')}));
      }
      output=[];postMessage({type:'ready'});timer=setInterval(step,1000/60);
    }catch(error){postMessage({type:'error',message:String(error.stack||error)});}
  }else if(data.type==='effect'){
    const heap=emulator.module.HEAPU8;heap[mailbox+18]=data.id;heap[mailbox+19]=1;
  }else if(data.type==='stop'){active=false;clearInterval(timer);emulator?.dispose();close();}
};
