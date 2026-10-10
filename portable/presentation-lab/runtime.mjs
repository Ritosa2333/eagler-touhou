import createModule from '/runtime/th15-application.mjs';
import {scanCodes} from '/runtime/keyboard.mjs';
const epoch=Number(new URL(location.href).searchParams.get('runtimeEpoch'));
const canvas=document.querySelector('canvas');
const send=(event,extra={})=>parent.postMessage({event,epoch,...extra},location.origin);
try{
 const timing=[];
 const core=await createModule({canvas,onGameFrame:(ok,ms,ticks)=>{timing.push({timestampMs:performance.now(),cpuMs:ms,ticks,ok,simulationTick:core._th15_frame()});if(timing.length>512)timing.shift();}});
 const held=new Set();
 core.resetBrowserKeyboard=()=>{held.clear();for(const scan of Object.values(scanCodes))core._th15_key(scan,0);};
 const key=(event,down)=>{const scan=scanCodes[event.code];if(!scan||event.code==='F8'||event.altKey&&event.code==='Enter')return;event.preventDefault();if(down){if(event.repeat&&!held.has(event.code))return;held.add(event.code);}else held.delete(event.code);core._th15_key(scan,+down);};
 window.addEventListener('keydown',e=>key(e,true));window.addEventListener('keyup',e=>key(e,false));window.addEventListener('blur',()=>core._th15_keys_clear());
 window.__th15Runtime={core,canvas,timing,scanCodes,async launch(){
  const {buffer}=await parent.__eaglerPrepareManagedRuntimeDataV1({game:'th15',epoch});
  for(const dir of ['/fonts','/save','/music'])core.FS.mkdir(dir);
  for(const name of [...Array.from({length:13},(_,n)=>'font'+n+'.bin'),'cp932.bin','blend4444.bin']){const response=await fetch('/fonts/'+name);if(!response.ok)throw Error('Missing measured font '+name);core.FS.writeFile('/fonts/'+name,new Uint8Array(await response.arrayBuffer()));}
  core.FS.writeFile('/th15.dat',new Uint8Array(buffer));core._th15_music_enabled(0);core._th15_render_scale(2);
  if(!core._th15_initialize())throw Error('TH15 initialize failed');core._th15_loop_start();canvas.focus();
 },stop(){core._th15_loop_stop();core._th15_audio_close();}};
 send('ready');
}catch(error){send('error',{message:String(error.stack||error)});}
