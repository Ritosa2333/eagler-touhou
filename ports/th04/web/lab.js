import {startFrameLoop} from './frame-limit.js';
import {mountPlayer} from './player-ui.js';
import {writeNativeTouch,touchPlayer} from './native-touch.js';
import {sha256} from './netplay/sha256.js';
import {NP21} from './dosbox-adapter.js';
import {WorkerNP21,supportsSoloWorker} from './solo-worker-client.js';
import {SoloMusic} from './solo-music.js';
import {mountSoloFault} from './solo-fault.js';
import {mountFocusSettings} from './focus-settings.js';
import {gameVersion,rememberLanguage} from './game-version.js';
import {encodeConfig,installConfig} from './launch-config.js';
import {mountControlSettings,keyboardBits,padBits,isBound,isFormTarget,controlsSummary,syntheticInputEvents} from './control-settings.js';
const $=id=>document.getElementById(id);
rememberLanguage($('language'),'local');
const signature=new TextEncoder().encode('TH04COOPLABv001!');
let emulator, mailbox=-1, candidates=[], scanCursor=0, lastDiagnostics=-Infinity, lastPadLabel=null, lastPadList=-Infinity, lastTick=-1, lastTickAt=0;
let keyboard=new Set(), testInput=null, testing=false, padMenu=0, pageFocused=true,oldP1=0;
let launchSettings=null, launchStarted=0, stageAnnounced=false;
let nativePatch,localMusic;
const screen=$('screen'),playerHost=document.createElement('div');screen.before(playerHost);
const player=mountPlayer(playerHost,{onGesture:async()=>{localMusic?.resume();const context=emulator?.module.SDL2?.audioContext;if(context)await context.resume();}});
player.stage.append(screen);player.setLabel('本地触屏控制 P1');
mountSoloFault({host:playerHost,getEmulator:()=>emulator,readState:snapshot,readContext:()=>({mode:'local-coop',settings:launchSettings,status:$('status').textContent})});
const focusSettings=mountFocusSettings($('control-settings'),{local:true,alwaysPointControl:player.alwaysPointControl});
const syntheticKeys=syntheticInputEvents;
const nativeActions=[['ArrowUp',1],['ArrowDown',2],['ArrowLeft',4],['ArrowRight',8],['KeyX',16],['KeyZ',32],['ShiftLeft',64],['Escape',128],['Enter',256]];
const status=text=>{$('status').textContent=text;player.note(text);player.diagnostics(text);};
const sleep=ms=>new Promise(r=>setTimeout(r,ms));
function nativeKey(code,down){
  if(!emulator)return;
  const keyCodes={KeyZ:90,KeyX:88,ArrowUp:38,ArrowDown:40,ArrowLeft:37,ArrowRight:39,Escape:27,ShiftLeft:16,Enter:13};
  const key={KeyZ:'z',KeyX:'x',ShiftLeft:'Shift'}[code]||code;
  const event=new KeyboardEvent(down?'keydown':'keyup',{code,key,keyCode:keyCodes[code],which:keyCodes[code],bubbles:true});
  syntheticKeys.add(event);$('canvas').dispatchEvent(event);
}
async function tap(code){nativeKey(code,true);await sleep(110);nativeKey(code,false);$('canvas').focus();}
for(const button of document.querySelectorAll('[data-key]'))button.onclick=()=>tap(button.dataset.key);
function syncP1(){
  let bits=controlEditor.isEditing()?0:player.sample(keyboardBits(keyboard,'local1'));
  if((bits&3)===3)bits&=~3;if((bits&12)===12)bits&=~12;
  for(const [code,bit]of nativeActions)if((bits&bit)!==(oldP1&bit))nativeKey(code,!!(bits&bit));
  oldP1=bits;
}
function release(){player.reset();keyboard.clear();syncP1();padMenu=0;if(emulator?.isWorker)emulator.clearInput();else if(mailbox>=0)emulator.module.HEAPU8.fill(0,mailbox+22,mailbox+25);}
function showControls(){
  $('controls-summary').textContent=`P1：${controlsSummary('local1')}。 P2：${controlsSummary('local2')}。`;
}
const controlEditor=mountControlSettings($('control-settings'),{local:true,onEditing:()=>release(),onChange:()=>{release();showControls();}});
showControls();
window.addEventListener('keydown',e=>{
  if(syntheticKeys.has(e))return;
  // SDL must only see translated events, including while a form has focus.
  e.stopImmediatePropagation();
  if(controlEditor.isEditing()||isFormTarget(e.target)||!emulator||e.metaKey)return;
  const bound=isBound(e.code,'local1')||isBound(e.code,'local2');
  // Do not let old physical Z/X/arrows also reach SDL after being rebound.
  if(bound||e.target===$('canvas')){
    if(bound||nativeActions.some(([code])=>code===e.code)||e.code==='ShiftRight'||e.code==='Space')e.preventDefault();
    e.stopImmediatePropagation();
  }
  if(bound){keyboard.add(e.code);syncP1();}
},true);
window.addEventListener('keyup',e=>{
  if(syntheticKeys.has(e))return;
  e.stopImmediatePropagation();
  const held=keyboard.delete(e.code);
  if(held||e.target===$('canvas')){e.preventDefault();e.stopImmediatePropagation();syncP1();}
},true);
window.addEventListener('keypress',e=>{if(!syntheticKeys.has(e)){e.stopImmediatePropagation();if(e.target===$('canvas'))e.preventDefault();}},true);
document.addEventListener('focusin',e=>{if(isFormTarget(e.target))release();});
window.addEventListener('blur',()=>{pageFocused=false;release();});
window.addEventListener('focus',()=>{pageFocused=true;});
document.addEventListener('visibilitychange',()=>{if(document.hidden){release();localMusic?.suspend();}else if(emulator?.state==='running')localMusic?.resume();});
window.addEventListener('pagehide',event=>{if(event.persisted)localMusic?.suspend();else localMusic?.dispose();});
window.addEventListener('pageshow',event=>{if(event.persisted&&emulator?.state==='running')localMusic?.resume();});
$('canvas').addEventListener('keydown',e=>{if(['ArrowUp','ArrowDown','ArrowLeft','ArrowRight','Space'].includes(e.code))e.preventDefault();});
$('canvas').addEventListener('pointerdown',()=>{$('canvas').focus();});

function scan(){
  if(emulator.isWorker){mailbox=emulator.snapshot.state?.at??-1;return;}
  const heap=emulator.module.HEAPU8,v=new DataView(heap.buffer);
  // Known addresses are cheap to validate. Only search when they stop being
  // active, and never traverse the whole heap in a single animation frame.
  candidates=candidates.filter(at=>signature.every((v,i)=>heap[at+i]===v));
  if(!candidates.some(at=>heap[at+25]===1&&v.getUint32(at+16,true)>0&&v.getUint16(at+20,true)>0)){
    if(scanCursor>=heap.length)scanCursor=0;
    const end=Math.min(scanCursor+1024*1024,heap.length);
    const slice=heap.subarray(scanCursor,Math.min(end+signature.length-1,heap.length));
    for(let offset=slice.indexOf(signature[0]);offset>=0&&scanCursor+offset<end;offset=slice.indexOf(signature[0],offset+1)){
      const at=scanCursor+offset;
      if(at+675<heap.length&&signature.every((v,i)=>heap[at+i]===v)&&!candidates.includes(at))candidates.push(at);
    }
    scanCursor=end;
  }
  const active=candidates.filter(at=>v.getUint32(at+16,true)>0&&v.getUint16(at+20,true)>0&&heap[at+25]===1);
  if(active.length===1)mailbox=active[0];
  else if(!active.includes(mailbox))mailbox=-1;
  emulator.module.projectileArt?.bind(mailbox);
}
function snapshot(){
  if(emulator?.isWorker)return emulator.snapshot.state;
  if(!emulator||mailbox<0)return null;
  const heap=emulator.module.HEAPU8,v=new DataView(heap.buffer),m=mailbox;
  const players=[];
  if(heap[m+605]===1){
    const count=Math.min(heap[m+606],heap[m+607]),stride=heap[m+608];
    for(let id=0;id<count;id++){
      const at=m+609+id*stride;
      players.push({id:id+1,power:heap[at],level:heap[at+1],
        lives:Math.max(0,heap[at+4]-1),bombs:heap[at+5],out:!!heap[at+6]});
    }
  }
  return {ticks:v.getUint32(m+16,true),ds:v.getUint16(m+20,true),stage:v.getUint16(m+26,true),
    players,
    shots:v.getUint32(m+28,true),hits:v.getUint32(m+32,true),shotHits:v.getUint32(m+85,true),
    items:v.getUint32(m+597,true),targets:v.getUint32(m+601,true),
    p1:[v.getInt16(m+36,true)/16,v.getInt16(m+38,true)/16],
    p2:[v.getInt16(m+46,true)/16,v.getInt16(m+48,true)/16],
    stageFrame:v.getUint16(m+40,true),p1Miss:heap[m+42],p1Inv:heap[m+43],
    p2Inv:heap[m+66],p2Miss:heap[m+72],input:v.getUint16(m+22,true)};
}
function gamepadInput(now){
  let all=[];
  try{all=Array.from(navigator.getGamepads?.()||[]).filter(p=>p?.connected);}catch{}
  const select=$('pad-select');
  if(now-lastPadList>=250){
    lastPadList=now;
    const known=new Set([...select.options].map(o=>o.value));
    for(const pad of all)if(!known.has(String(pad.index))){const o=document.createElement('option');o.value=pad.index;o.textContent=`${pad.index+1} · ${pad.id}`;select.append(o);}
  }
  const pad=select.value==='auto'?all[0]:all.find(p=>String(p.index)===select.value);
  const padLabel=pad?`${pad.id}${pad.mapping==='standard'?'':'（非标准映射，请核对按钮）'}`:'未检测到手柄：连接后按一下手柄按钮。';
  if(padLabel!==lastPadLabel){$('pad').textContent=padLabel;lastPadLabel=padLabel;}
  if(!pageFocused||document.hidden||controlEditor.isEditing()){padMenu=0;return {bits:0,focus:0};}
  const value=padBits(pad,'local2',.25)|keyboardBits(keyboard,'local2');
  if(value&128&&!(padMenu&128))tap('Escape');
  if(value&256&&!(padMenu&256))tap('Enter');
  padMenu=value&(128|256);
  return {bits:value&63,focus:value&64?1:0};
}
function tick(now){
  try{
    const pad=gamepadInput(now);
    if(emulator){
      if(!emulator.isWorker)localMusic?.poll(emulator.module.HEAPU8,now);
      scan();
      const nativePaused=emulator.isWorker?!!emulator.snapshot.state?.nativePaused:mailbox>=0&&!!emulator.module.HEAPU8[mailbox-nativePatch.mailbox_cs_offset+nativePatch.native_pause_cs_offset+3];
      const play=emulator.state==='running'&&!controlEditor.isEditing()&&!nativePaused;
      const s=snapshot();
      if(mailbox>=0){const p=emulator.isWorker?s.touch:touchPlayer(emulator.module.HEAPU8,mailbox,nativePatch,0);player.setContext({key:`${s?.stage}:${play?'game':'menu'}`,play:play&&!p.miss&&!p.entry&&!p.out});}
      syncP1();
      const touch=[player.pack(oldP1)];
      if(nativePatch&&!emulator.isWorker)writeNativeTouch(emulator.module.HEAPU8,mailbox,nativePatch,touch,!play);
      player.consume(s?.ticks);
      if(s){
        if(!stageAnnounced){stageAnnounced=true;status(`已进入关卡 · P1 ${$('loadout-p1').selectedOptions[0].textContent} / P2 ${$('loadout-p2').selectedOptions[0].textContent}`);}
        if(s.ticks!==lastTick){lastTick=s.ticks;lastTickAt=now;}
        const live=now-lastTickAt<1200&&emulator.state==='running';
        let bits=pad.bits;
        let focus=pad.focus;
        if(testInput){bits=testInput.bits;focus=testInput.focus||0;}
        if(!pageFocused||document.hidden||!live){bits=0;focus=0;}
        if((bits&3)===3)bits&=~3;if((bits&12)===12)bits&=~12;
        const points=focusSettings()|(player.alwaysPoint()?8:0);
        if(emulator.isWorker)emulator.setInput({generation:s.stage,bits,focus,points,touch,blocked:!play});
        else {const heap=emulator.module.HEAPU8,v=new DataView(heap.buffer);
          v.setUint16(mailbox+22,bits,true);heap[mailbox+24]=focus;
          heap[mailbox-nativePatch.mailbox_cs_offset+nativePatch.focus_visible_mask_cs_offset]=points;}
        if($('diagnostics').closest('details').open&&now-lastDiagnostics>=250){
          lastDiagnostics=now;
          $('diagnostics').textContent=`游戏帧 ${s.ticks} · 关卡帧 ${s.stageFrame} · ${live?'运行中':'暂停 / 非关卡状态'}\nP1 (${s.p1.map(n=>n.toFixed(1)).join(', ')}) 受击动画 ${s.p1Miss}\nP2 (${s.p2.map(n=>n.toFixed(1)).join(', ')}) 无敌 ${s.p2Inv} 受击动画 ${s.p2Miss}\nP2 射击调用 ${s.shots} · 子弹命中 ${s.shotHits} · 碰撞受击 ${s.hits}\nP2 拾取 ${s.items} · 瞄准 / 吸引选择 P2 ${s.targets}\nP2 输入 0x${bits.toString(16)} · ${focus?'低速':'通常速度'}\n手柄输入与 P1 的 SDL 输入已隔离。`;
          $('diagnostics').textContent+='\n'+s.players.map(p=>`P${p.id} · P ${p.power}/128 · LIFE ${p.lives} · BOMB ${p.bombs}${p.out?' · 已退场':''}`).join('\n');
        }
        for(const [id,xy,miss,index] of [['p1',s.p1,s.p1Miss,0],['p2',s.p2,s.p2Miss,1]]){
          $(id).hidden=!live||miss>0||s.players[index]?.out;
          $(id).style.left=`${(xy[0]+32)/640*100}%`;
          $(id).style.top=`${(xy[1]+16-24)/400*100}%`;
        }
        $('selftest').disabled=!live||testing||s.players[1]?.out;$('hit-test').disabled=!live||testing||s.players[1]?.out;
      }
      if(!stageAnnounced&&launchStarted&&now-launchStarted>90000){status('加载时间较长，请查看游戏画面；可返回设置后重新开始。');launchStarted=0;}
    }
  }catch(e){$('diagnostics').textContent=`诊断错误：${e.message}`;}
}
startFrameLoop(tick);

$('start').onclick=async()=>{
  player.enter();
  $('start').disabled=true;
  $('setup').disabled=true;
  try{
    launchSettings={p1:Number($('loadout-p1').value),p2:Number($('loadout-p2').value),difficulty:Number($('difficulty').value),lives:Number($('lives').value),bombs:Number($('bombs').value)};
    const config=encodeConfig(launchSettings);
    const version=gameVersion($('language').value);
    status('正在校验并加载实验磁盘…');
    const [disk,meta,patch]=await Promise.all([
      fetch(version.coopDisk,{cache:'no-cache'}).then(async response=>{
        if(!response.ok)throw Error(`磁盘 HTTP ${response.status}`);
        return new Uint8Array(await new Response(response.body.pipeThrough(new DecompressionStream('gzip'))).arrayBuffer());
      }),
      ...[version.coopMeta,version.coopPatch].map(path=>fetch(path,{cache:'no-cache'}).then(response=>{
        if(!response.ok)throw Error(`配置 HTTP ${response.status}`);return response.json();
      }))]);
    let data=disk;
    const hash=await sha256(data);
    if(hash!==meta.sha256||data.length!==meta.size)throw Error('实验磁盘校验失败');
    nativePatch=patch;
    installConfig(data,patch,config);
    if($('audio-mode').value==='independent'){
      localMusic=new SoloMusic(status);const musicDisk=data.slice();
      try{await localMusic.installCoop(musicDisk,patch,version.language,meta.sha256);data=musicDisk;}
      catch(error){localMusic.dispose();localMusic=null;status('独立音乐加载失败，将使用原声：'+error.message);}
    }
    const runtimeConfig={canvas:$('canvas'),artPatch:patch,ExMemory:7,Latencys:100,SampleHz:44100,SNDboard:4,no_mouse:true,use_menu:false,fontfile:'font.bmp'};
    const background=supportsSoloWorker()&&new URLSearchParams(location.search).get('runtime')!=='main';
    emulator=background?await WorkerNP21.create(runtimeConfig,{game:'04coop',patch,music:localMusic?.reader.patch,onError:error=>{localMusic?.suspend();status('模拟线程错误：'+error.message);}}):await NP21.create(runtimeConfig);
    if(emulator.isWorker)emulator.onMusic=(ax,hash)=>localMusic?.player.command(ax,hash);
    await emulator.addDiskImage('th04-lab.hdi',data);await emulator.setHdd(0,'th04-lab.hdi');emulator.run();localMusic?.start();player.setActive(true);
    $('canvas').focus();$('pause').disabled=false;$('restart').disabled=false;$('capture').disabled=false;
    launchStarted=performance.now();
    status('正在启动游戏并载入双方机体，请稍候…');
  }catch(e){localMusic?.dispose();localMusic=null;player.setActive(false);player.exit();status(`启动失败：${e.message}`);console.error(e);$('restart').disabled=false;if(!emulator){$('start').disabled=false;$('setup').disabled=false;}}
};
$('pause').onclick=()=>{if(!emulator)return;if(emulator.state==='running'){release();emulator.pause();localMusic?.suspend();$('pause').textContent='继续模拟器';}else{emulator.run();localMusic?.resume();$('pause').textContent='暂停模拟器';$('canvas').focus();}};
$('restart').onclick=()=>location.reload();
$('fullscreen').onclick=()=>player.enter();
$('capture').onclick=()=>{
  const c=document.createElement('canvas');c.width=640;c.height=400;
  c.getContext('2d').drawImage($('canvas'),0,0);
  const a=document.createElement('a');a.download='th04-coop.png';a.href=c.toDataURL();a.click();
};

async function inputForFrames(bits,frames,focus=0){
  const start=snapshot();if(!start)throw Error('尚未进入关卡');
  testInput={bits,focus};const deadline=performance.now()+10000;
  while(snapshot().ticks-start.ticks<frames){if(performance.now()>deadline)throw Error('游戏帧停止推进，请先取消游戏暂停');await sleep(20);}
  testInput={bits:0};return snapshot();
}
$('selftest').onclick=async()=>{
  testing=true;$('tests').textContent='正在检查：只给 P2 发送移动和射击，请松开控制器…';
  try{
    const start=snapshot(),direction=start.p2[0]>192?4:8;
    const moved=await inputForFrames(direction,18);
    const fired=await inputForFrames(32,36);
    const movedEnough=Math.abs(moved.p2[0]-start.p2[0])>20;
    const p1Independent=Math.abs(moved.p1[0]-start.p1[0])<1&&Math.abs(moved.p1[1]-start.p1[1])<1;
    $('tests').textContent=`${movedEnough&&p1Independent&&fired.shots>start.shots?'通过':'未通过 / 受到游戏事件干扰'}：P2 位移 ${(moved.p2[0]-start.p2[0]).toFixed(1)} px；P1 独立 ${p1Independent?'是':'否'}；P2 射击调用 +${fired.shots-start.shots}。实际击杀需结合画面和碰撞检查。`;
  }catch(e){$('tests').textContent=`检查中止：${e.message}`;}finally{testInput=null;testing=false;release();}
};
$('hit-test').onclick=async()=>{
  testing=true;
  try{
    let s=snapshot();if(!s)throw Error('尚未进入关卡');
    if(s.p2Inv||s.p2Miss)throw Error('请等 P2 无敌 / 复活动画结束后再测试');
    // Recover the guest RAM base from the live code signature and relocated DS.
    // CS is the original main_01 segment; mailbox offset supplied by build.
    if(emulator.isWorker)await emulator.call('coopHit');
    else {
    const patch=await (await fetch(gameVersion($('language').value).coopPatch)).json();
    const csLinear=mailbox-patch.mailbox_cs_offset;
    // Actual load CS cannot be inferred from DS alone; locate P1 data via
    // the exact code-to-DGROUP delta from the rebased MZ layout instead.
    const guestData=csLinear+(patch.data_segment-patch.code_segment)*16;
    const heap=emulator.module.HEAPU8,v=new DataView(heap.buffer);
    if(v.getInt16(guestData+0x464e,true)/16!==s.p1[0])throw Error('内存布局校验失败，未注入测试弹');
    let slot=-1;for(let i=0;i<440;i++){const p=guestData+0x5a22+i*26;if(heap[p]===0){slot=p;break;}}
    if(slot<0)throw Error('没有空闲敌弹位置');
    heap.fill(0,slot,slot+26);heap[slot]=1;heap[slot+1]=2;
    v.setInt16(slot+2,s.p2[0]*16,true);v.setInt16(slot+4,s.p2[1]*16,true);
    v.setInt16(slot+6,s.p2[0]*16,true);v.setInt16(slot+8,s.p2[1]*16,true);
    heap[slot+0x12]=2;heap[slot+0x13]=2;
    }
    $('tests').textContent='已生成测试敌弹，正在观察 P2 碰撞与复活…';
    const after=await inputForFrames(0,100);
    $('tests').textContent=`${after.hits>s.hits&&after.p2Miss===0?'通过':'未通过 / 被关卡清弹干扰'}：真实敌弹触发 P2 受击 +${after.hits-s.hits}，复活状态 ${after.p2Miss}，剩余无敌 ${after.p2Inv}。`;
  }catch(e){$('tests').textContent=`检查中止：${e.message}`;}finally{testing=false;testInput=null;release();}
};

// Explicit QA URL only; ordinary play never injects state or test inputs.
if(new URLSearchParams(location.search).get('qa')==='rescue'){
  import('./rescue-qa.js').then(({install})=>install({
    getEmulator:()=>emulator,snapshot,inputForFrames,nativeKey,
    getMailbox:()=>mailbox,setTesting:value=>{testing=value;},
    releaseTest:()=>{testInput=null;release();}
  }));
}
