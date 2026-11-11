import {NativePause} from '../native-pause.js';
import {battleProof} from '../replay/battle-proof.js';
import {NativeMusic,ConfirmedMusic} from '../native-music.js';
import {createFrameControls} from './frame-controls.js';
import {readNativeCoreDiagnostics} from '../network-performance.js';
// All mutable guest/bridge/control state stays on the simulation thread.
export class NetworkSimulation {
  constructor(emulator,{patch,music,focusEnabled,touchUnlimitedAllowed,fullHash=false},onAudio=()=>{}){
    this.emulator=emulator;this.pause=new NativePause(()=>emulator,patch);this.music=new NativeMusic(music);
    this.controls=createFrameControls(emulator,this.pause,{focusEnabled,touchUnlimitedAllowed});
    this.audio=new ConfirmedMusic({command:(...event)=>onAudio(event)});
    this.snapshots=new Map();this.hidden=new Set();this.pauseSeat=null;this.selection=0;this.state=null;this.fullHash=fullHash;this.renderEpoch=0;
  }
  now(){return this.emulator.module.virtualNow;}
  update(){return this.state=this.pause.update(this.now(),this.music.dosRange(this.emulator.module.HEAPU8));}
  allowed(){return this.state?.phase===1&&!this.pause.pending;}
  open(seat){
    if(this.emulator.state==='exited'||this.pause.pending||this.pauseSeat!==null)return;
    this.update();if(!this.state?.phase)return;
    this.emulator.pause();this.pauseSeat=seat;this.selection=0;
  }
  close(){this.pauseSeat=null;if(this.emulator.state!=='exited')this.emulator.run();}
  choose(choice){
    if(this.pauseSeat===null)return;
    if(choice==='resume'){this.close();return;}
    if(!this.pause.request(choice==='round'?1:2,this.pauseSeat,this.now())){this.update();this.selection=0;return;}
    this.close();
  }
  command(seat,command){
    if(command==='hidden'){this.hidden.add(seat);return;}
    if(command==='visible'){this.hidden.delete(seat);return;}
    if(command==='pause'||command==='escape'){
      if(this.pauseSeat===seat)this.close();else if(this.pauseSeat===null)this.open(seat);return;
    }
    if(this.pauseSeat!==seat)return;
    if(['round','match','resume'].includes(command)){this.choose(command);return;}
    if(command==='confirm'){this.choose(['resume','round','match'][this.selection]);return;}
    if(['up','down'].includes(command)){
      this.selection=this.allowed()?(this.selection+(command==='up'?2:1))%3:0;
    }
  }
  step(inputs,frame,replay=false){
    const e=this.emulator,m=e.module;m.netBeginFrame(frame,replay);
    try{
      this.update();
      for(let seat=0;seat<2;seat++)for(const command of inputs[seat].commands)this.command(seat,command);
      const paused=this.pauseSeat!==null||this.hidden.size>0;
      this.controls.applyFrame(paused?inputs.map(input=>({...input,actions:[],touch:0})):inputs);
      if(!paused){e.run();e.step();this.audio.stage(frame,this.music.read(m.HEAPU8));this.update();this.pause.result(this.now());}
      else e.pause();
    }finally{m.netEndFrame();}
  }
  capture(frame){
    if(this.snapshots.has(frame))return;
    this.emulator.module.netCapture(frame);
    this.snapshots.set(frame,{pauseSeat:this.pauseSeat,selection:this.selection,state:this.state&&{...this.state},hidden:[...this.hidden],pause:this.pause.capture(),music:this.music.capture(),controls:this.controls.capture()});
  }
  restore(frame){
    const saved=this.snapshots.get(frame);if(!saved)throw Error('同步快照已过期：'+frame);
    this.emulator.module.netRestore(frame);this.renderEpoch++;this.pauseSeat=saved.pauseSeat;this.selection=saved.selection;this.hidden=new Set(saved.hidden);
    this.pause.restore(saved.pause);this.music.restore(saved.music);this.controls.restore(saved.controls);this.audio.rewind(frame);
    if(this.pauseSeat===null)this.emulator.run();else this.emulator.pause();
    this.state=saved.state&&{...saved.state};for(const at of this.snapshots.keys())if(at>=frame)this.snapshots.delete(at);return frame;
  }
  confirm(prefix,retainFrom){this.audio.confirm(prefix);this.emulator.module.netConfirm(prefix,retainFrom);for(const at of this.snapshots.keys())if(at<retainFrom)this.snapshots.delete(at);}
  proof(){return battleProof(this.emulator,this.pause,this.controls,this.music,this.pauseSeat,this.selection,this.hidden);}
  hash(){
    const m=this.emulator.module,heap=m.HEAPU8,range=this.music.dosRange(heap);
    if(!this.fullHash&&!range)throw Error('DOS 常规内存校验范围尚未绑定');
    const start=this.fullHash?0:range.start,end=this.fullHash?heap.length:range.end;
    let h=2166136261;if(m.netHash)h=m.netHash(start,end);else for(let i=start;i<end;i++)h=Math.imul(h^heap[i],16777619);
    return (h>>>0).toString(16).padStart(8,'0')+':'+m.netInfo().tick+':'+this.pauseSeat+':'+[...this.hidden].sort().join(',');
  }
  describe({diagnostics=false,compact=false}={}){
    const e=this.emulator,m=e.module;
    return {state:e.state,tick:e.tick,virtualNow:m.virtualNow,heapBytes:m.HEAPU8.length,frameUploads:m.frameUploads||0,
      nativeState:this.state,selecting:this.pause.selecting,pending:this.pause.pending,markers:this.pause.markers(),pauseSeat:this.pauseSeat,selection:this.selection,hidden:[...this.hidden],
      renderEpoch:this.renderEpoch,
      ...(!compact?{snapshots:m.netSnapshotInfo(),
      stepMetrics:diagnostics?m.stepMetrics:{count:m.stepMetrics.count,totalMs:m.stepMetrics.totalMs,maxMs:m.stepMetrics.maxMs},
      ...(diagnostics?{programReads:m.programReads,nativeCore:readNativeCoreDiagnostics(m),pauseSearch:this.pause.search?'wasm-simd':'javascript',render:m.framePresenter?.info()}:{} )}:{} )};
  }
}
