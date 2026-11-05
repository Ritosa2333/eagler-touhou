import {createAdonisCalibration} from './adonis-calibration.mjs';

const integer=(n,min,max)=>Number.isInteger(n)&&n>=min&&n<=max;
// Keep completed native files available for a user-initiated mobile download.
export function showReplaySavePrompt({core,baseline,onReturn}){
  const names=core.FS.readdir('/save/replay').filter(name=>/^[a-zA-Z0-9_(). -]+\.rep$/.test(name)&&!baseline.has(name)).sort();
  if(!names.length)throw Error('Completed TH075 Replay was not saved');
  const files=names.map(name=>({name,bytes:core.FS.readFile('/save/replay/'+name).slice()}));
  if(files.some(file=>!file.bytes.length))throw Error('Empty TH075 Replay');
  const dialog=document.createElement('dialog');dialog.id='th075ReplaySavePrompt';
  const title=document.createElement('h2');title.textContent='保存录像 / Save Replay';
  const description=document.createElement('p');description.textContent='录像已保存在本机，可下载 .rep 文件。 / Replay saved locally. Download the .rep file below.';
  dialog.append(title,description);const urls=[];
  for(const {name,bytes} of files){
    const link=document.createElement('a');link.download=name;link.textContent='下载 / Download '+name;link.className='th075-replay-download';
    link.href=URL.createObjectURL(new Blob([bytes],{type:'application/octet-stream'}));urls.push(link.href);dialog.append(link);
  }
  const leave=document.createElement('button');leave.type='button';leave.id='th075ReplayReturn';leave.textContent='返回房间 / Return to room';leave.addEventListener('click',onReturn);dialog.append(leave);
  dialog.addEventListener('cancel',event=>event.preventDefault());document.body.append(dialog);dialog.showModal();
  return {dispose(){dialog.close();dialog.remove();for(const url of urls)URL.revokeObjectURL(url);}};
}
export function validateMultiplayerOptions(options){
  const o=options,url=new URL(o.netplayUrl,location.href),spectator=o.netplaySpectator===true;
  if(!['ws:','wss:'].includes(url.protocol)||!/^th075mp-\d{4}$/.test(url.searchParams.get('room')||'')||
     !/^\d+$/.test(url.searchParams.get('run')||'')||o.netplayMode!=='lan'||o.netplayPlayerCount!==2||
     (!spectator&&!integer(o.netplayPlayer,0,1))||!integer(o.netplaySeed,0,65535)||!integer(o.netplayDifficulty,0,3)||
     ![0,1].includes(o.netplayAdonisMode)||(o.netplayAdonisMode===1?(o.netplayPredictionLimit??0)!==0:!integer(o.netplayPredictionLimit,1,8)||o.netplayInputDelayAuto||(o.netplayInputDelay??0)!==0)||!integer(o.netplayInputDelay??0,0,9)||
     typeof o.netplayInputDelayAuto!=='boolean')throw Error('Invalid TH075 multiplayer configuration');
  if(spectator?!/^[A-Za-z0-9_-]{8,64}$/.test(o.netplaySpectatorId||'')||url.searchParams.get('spectator')!==o.netplaySpectatorId||url.searchParams.has('player'):
     !url.searchParams.has('player')||Number(url.searchParams.get('player'))!==o.netplayPlayer||url.searchParams.has('spectator'))throw Error('Invalid TH075 seat admission');
  if(!Array.isArray(o.netplayLoadouts)||o.netplayLoadouts.length!==2||o.netplayLoadouts.some(v=>!integer(v?.character,0,10)||!integer(v?.shot,0,8)))
    throw Error('Invalid TH075 characters or spell cards');
  return {url,spectator};
}

export class Multiplayer {
  constructor({core,options,build,emit,onError,onFinished,isBusy}){
    Object.assign(this,{core,options,build,emit,onError,onFinished,isBusy});
    this.reportError=e=>{if(this.failed)return;this.failed=true;clearInterval(this.timer);this.calibration?.stop();globalThis.__eaglerNetplayFailed=true;globalThis.__eaglerNetplayError=e?.message||String(e);onError(e);};
    this.spectator=options.netplaySpectator===true;this.closed=false;this.finished=false;
  }
  async connect(){
    const {core,options:o}=this,{url,spectator}=validateMultiplayerOptions(o);
    core.eaglerOptions=o;
    const [a,b]=o.netplayLoadouts;
    const prepared=await core.ccall('th075_mp_prepare','number',Array(6).fill('number'),[o.netplaySeed,a.character,b.character,a.shot,b.shot,o.netplayDifficulty],{async:true});
    if(!prepared)throw Error(core.UTF8ToString(core._th075_error())||'TH075 multiplayer preparation failed');
    if(typeof core.th075ContentIdentity!=='string'||!core.th075ContentIdentity)throw Error('Missing TH075 content identity');
    const identity=JSON.stringify({content:core.th075ContentIdentity,build:this.build.version,seed:o.netplaySeed,difficulty:o.netplayDifficulty,loadouts:o.netplayLoadouts,background:0,timingMode:o.netplayAdonisMode,predictionLimit:o.netplayPredictionLimit??0});
    const hash=await crypto.subtle.digest('SHA-256',new TextEncoder().encode(identity)),abi=new DataView(hash).getUint32(0,true)||1;
    const run=(BigInt(url.searchParams.get('run'))<<16n|BigInt(o.netplaySeed))+1n;
    if(run>0xffffffffffffffffn)throw Error('TH075 run identity overflow');
    const args=[url.href,spectator?o.netplaySpectatorId:'',spectator?2:o.netplayPlayer,Number(run&0xffffffffn),Number(run>>32n),abi,
      o.netplaySeed,a.character,b.character,a.shot,b.shot,o.netplayDifficulty,o.netplayInputDelay??0,+o.netplayInputDelayAuto];
    if(!core.ccall('th075_mp_begin','number',['string','string',...Array(12).fill('number')],args))throw Error('TH075 peer transport initialization failed');
    globalThis.__eaglerNetplayLanActive=false;globalThis.__eaglerNetplaySpectator=spectator;
    globalThis.__eaglerNetplayLanRollback=globalThis.__eaglerNetplayLanResimulated=0;
    globalThis.__eaglerNetplayLanFrame=globalThis.__eaglerNetplayLanConfirmed=0;
    globalThis.__eaglerNetplayAdonisMode=o.netplayAdonisMode;globalThis.__eaglerNetplayFailed=false;globalThis.__eaglerNetplayError='';
    const self=this;
    if(!spectator){
      // Use the shared calibration presentation; native peers own all choices.
      const adapter={get memory(){return {buffer:core.HEAPU8.buffer};},
        multiplayer_calibration_status:()=>core._th075_mp_calibration(),
        multiplayer_network_poll:()=>self.isBusy()?1:core._th075_mp_poll(performance.now()*1000)};
      this.calibration=createAdonisCalibration({core:adapter,getApp:()=>this.closed?null:1,getOptions:()=>o,
        emit:this.emit,game:'th075',build:this.build.version,onError:this.reportError,pause:()=>{},target:globalThis});
      this.calibration.start();
    }
    this.markLocalPlayer();this.timer=setInterval(()=>this.pump(),16);
  }
  state(){const at=this.core._th075_mp_status();return Array.from(this.core.HEAPU32.subarray(at/4,at/4+64));}
  pump(){
    if(this.closed||this.failed||this.isBusy())return;
    try{if(!this.core._th075_mp_poll(performance.now()*1000))throw Error(this.core.UTF8ToString(this.core._th075_error()));this.frame();}
    catch(e){this.reportError(e);}
  }
  frame(){
    if(this.closed)return;
    const s=this.state();
    if(this.spectator&&!this.timingReported){const at=this.core._th075_mp_timing();if(at){const p=new DataView(this.core.HEAPU8.buffer,at,40),timing={phase:'ready',automatic:!!p.getUint32(32,true),adonisMode:p.getUint8(5),inputDelay:p.getUint8(6),predictionReserve:0,fullDelay:p.getUint32(8,true),rttP95Us:p.getUint32(24,true),lost:p.getUint32(28,true),samples:p.getUint32(36,true),route:'spectator'};this.timingReported=true;globalThis.__eaglerNetplayTiming=timing;this.emit('runtime-info',{netplayTiming:timing});}}
    globalThis.__eaglerNetplayLanRollback=s[48];globalThis.__eaglerNetplayLanResimulated=s[49];
    globalThis.__eaglerNetplayLanFrame=s[2];globalThis.__eaglerNetplayLanConfirmed=s[3];
    globalThis.__eaglerNetplayLanActive=s[1]===3;
    globalThis.__eaglerNetplayInputDelayFrames=s[6];
    if(s[1]===5&&!this.finished){this.finished=true;globalThis.__eaglerNetplayLanActive=false;this.onFinished();}
  }
  markLocalPlayer(){
    if(this.spectator||!this.options.multiplayerLocalPlayerVisibility)return;
    const marker=this.marker=document.createElement('span');marker.textContent=`P${this.options.netplayPlayer+1} · 本机 / Local`;
    marker.style.cssText='position:fixed;z-index:2;pointer-events:none;font:12px system-ui;color:white;background:#0009;padding:2px 5px;border-radius:4px';
    const place=()=>{const r=document.querySelector('canvas').getBoundingClientRect();marker.style.left=(r.left+r.width*(this.options.netplayPlayer ? .84 : .03))+'px';marker.style.top=(r.top+r.height*.18)+'px';};
    document.body.append(marker);this.placeMarker=place;window.addEventListener('resize',place);place();
  }
  close(){
    if(this.closed)return;this.closed=true;clearInterval(this.timer);this.calibration?.stop();
    this.core._th075_mp_end();this.marker?.remove();if(this.placeMarker)window.removeEventListener('resize',this.placeMarker);
    globalThis.__eaglerNetplayLanActive=false;globalThis.__eaglerNetplaySpectator=false;
  }
}
