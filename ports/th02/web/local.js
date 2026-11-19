import {DOSBoxPC98} from './dosbox-adapter.js';
import {Fat12,sha256} from './solo-fat12.js';
import {mountFocusSettings} from './focus-settings.js';
import {mountPlayer} from './player-ui.js';
import {writeNativeTouch} from './native-touch.js';
import {configureLauncher,launchConfig} from './netplay/launch-config.js';
import {SoloMusic} from './solo-music.js';

const $=id=>document.getElementById(id),keyboard=new Set();
const signature=new TextEncoder().encode('TH02COOPv0002!!\0');
const bindings=[
  new Map([['ArrowUp',1],['ArrowDown',2],['ArrowLeft',4],['ArrowRight',8],['KeyZ',16],['KeyX',32],['ShiftLeft',256],['ShiftRight',256],['Escape',64],['KeyP',64],['Enter',128]]),
  new Map([['KeyW',1],['KeyS',2],['KeyA',4],['KeyD',8],['KeyJ',16],['KeyL',32],['KeyK',256]]),
  new Map([['Numpad8',1],['Numpad2',2],['Numpad4',4],['Numpad6',8],['Numpad0',16],['NumpadDecimal',32],['NumpadEnter',256]]),
];
const nativeKeys=[['ArrowUp',1],['ArrowDown',2],['ArrowLeft',4],['ArrowRight',8],['KeyZ',16],['KeyX',32],['Escape',64],['Enter',128]];
let emulator=null,metadata=null,mailbox=-1,candidates=[],cursor=0,lastNative=0,focused=true,launching=false,lastHud=0;
let debugInvincible=false;
let music=null;
let launchSettings=null;
let touch=null,touchSlot=0,touchPlay=false,touchMenu=true,touchActive=false;
const focusSettings=mountFocusSettings(()=>syncInput());
const status=text=>{$('status').textContent=text;};
const form=target=>target?.matches?.('input,select,textarea,button')||target?.isContentEditable;
for(const id of ['pad1','pad2'])$(id).add(new Option('第 3 个手柄','2'));
const shotChoices=[...document.querySelectorAll('[data-shot]')];
const shotSelect=button=>button.closest('.shot-picker').querySelector('select');
function showShotChoice(){for(const button of shotChoices)button.setAttribute('aria-pressed',String(button.dataset.shot===shotSelect(button).value));}
for(const id of ['p1-shot','p2-shot','p3-shot'])$(id).addEventListener('change',()=>{showShotChoice();showLaunchSettings();});
for(const button of shotChoices)button.onclick=()=>{const select=shotSelect(button);if(select.disabled)return;select.value=button.dataset.shot;showShotChoice();showLaunchSettings();};
const launchSelects=['language','player-count','stage','difficulty','lives','bombs','p1-shot','p2-shot','p3-shot'];
function readLaunchSettings(){return {language:$('language').value,players:Number($('player-count').value),
  ...Object.fromEntries(['stage','difficulty','lives','bombs'].map(key=>[key,Number($(key).value)])),
  ...Object.fromEntries([1,2,3].map(id=>['p'+id,Number($('p'+id+'-shot').value)]))};}
function showLaunchSettings(){
  const s=readLaunchSettings();$('difficulty').disabled=launching||!!emulator||s.stage===5;
  const f=focusSettings();
  $('launch-summary').textContent=`${s.stage===5?'Extra（固定 Extra 难度）':`第 ${s.stage+1} 关 · ${['Easy','Normal','Hard','Lunatic'][s.difficulty]}`} · 每人 ${s.lives} 残机 / ${s.bombs} Bomb · ${Array.from({length:s.players},(_,i)=>`P${i+1} ${['高速','平衡','低速'][s['p'+(i+1)]]} / 低速${f.seats[i].enabled?f.seats[i].percent+'%':'关闭'}`).join(' · ')}`;
}
for(const id of launchSelects)$(id).addEventListener('change',showLaunchSettings);
for(let seat=1;seat<=3;seat++){
  const assist=$('focus-enabled-'+seat).closest('fieldset');
  $('p'+seat+'-picker').append(assist);
}
$('launch-assists').append($('team-targeting').closest('label'));
for(const event of ['change','input'])$('launch').addEventListener(event,showLaunchSettings);
function showPlayerCount(){const count=Number($('player-count').value);document.body.dataset.players=String(count);
  document.querySelectorAll('.p3-seat').forEach(el=>el.hidden=count<3);if(count<3)$('pad3').value='none';
  if($('touch-seat')){ $('touch-seat').options[2].hidden=count<3;if(touchSlot>=count){touchSlot=0;$('touch-seat').value='0';} }
  $('start').textContent=`启动${count===3?'三':'双'}人游戏`;showLaunchSettings();release();}
$('player-count').addEventListener('change',showPlayerCount);showPlayerCount();

function bitsFor(player){let bits=0;for(const code of keyboard)bits|=bindings[player].get(code)||0;return bits;}
function gamepad(player){
  const choice=$('pad'+(player+1)).value;
  if(choice==='none'||!focused||document.hidden)return 0;
  const pad=navigator.getGamepads?.()[Number(choice)];if(!pad?.connected)return 0;
  const pressed=i=>!!pad.buttons[i]?.pressed;
  let bits=0;
  if(pad.axes[0]<-.25||pressed(14))bits|=4;if(pad.axes[0]>.25||pressed(15))bits|=8;
  if(pad.axes[1]<-.25||pressed(12))bits|=1;if(pad.axes[1]>.25||pressed(13))bits|=2;
  if(pressed(0))bits|=16;if(pressed(1))bits|=32;if(pressed(2)||pressed(4))bits|=256;
  if(pressed(9))bits|=64;if(pressed(8))bits|=128;
  return bits;
}
function normalized(bits){if((bits&3)===3)bits&=~3;if((bits&12)===12)bits&=~12;return bits;}
// Retain TH04's UI button protocol while translating to TH02's native bits.
const toTouch=bits=>(bits&15)|((bits&16)<<1)|((bits&32)>>1)|((bits&64)<<1)|((bits&128)<<1)|((bits&256)>>2);
const fromTouch=bits=>(bits&15)|((bits&16)<<1)|((bits&32)>>1)|((bits&64)<<2)|((bits&128)>>1)|((bits&256)>>1);
function syncInput(motion=false){
  if(touch)$('touchmode').setAttribute('aria-pressed',String(touch.enabled()));
  if(!emulator)return;
  const enabled=focused&&!document.hidden&&emulator.state==='running';
  const count=Number($('player-count').value),inputs=[0,1,2].map(i=>enabled&&i<count?normalized(bitsFor(i)|gamepad(i)):0);
  const slot=touchMenu?0:touchSlot;
  if(enabled&&touch){const sampled=fromTouch(touch.sample(toTouch(inputs[slot])));
    inputs[slot]=normalized(touchPlay||touchMenu?sampled:inputs[slot]|(sampled&64));}
  const [p1,p2,p3]=inputs;
  // P1 owns the original menu. Either controller can pause/confirm globally.
  const native=p1|((p2|p3)&192);
  for(const [code,bit]of nativeKeys)if((native&bit)!==(lastNative&bit))emulator.key(code,!!(native&bit));
  lastNative=native;
  if(mailbox>=0){
    const v=new DataView(emulator.module.HEAPU8.buffer),settings=focusSettings();
    metadata.players.inputOffsets.forEach((offset,i)=>v.setUint16(mailbox+offset,inputs[i],true));
    v.setUint8(mailbox+metadata.debug.invincibilityMailboxOffset,Number(debugInvincible));
    metadata.focus.percentOffsets.forEach((offset,i)=>v.setUint8(mailbox+offset,settings.seats[i].percent));
    const mask=(1<<count)-1;
    v.setUint8(mailbox+metadata.focus.enabledOffset,settings.enabledMask&mask);
    const points=settings.pointMask|(touch?.alwaysPoint()&&touchPlay?(1<<(touchSlot+3)):0);
    v.setUint8(mailbox+metadata.focus.pointOffset,points&(mask|(mask<<3)));
    v.setUint8(mailbox+metadata.targeting.enabledOffset,Number(settings.targeting));
    if(!enabled||!touchPlay||!touch?.moving())writeNativeTouch(emulator.module.HEAPU8,mailbox,metadata,touchSlot,0,true);
  }
  if(motion&&touch){
    const packed=touch.pack(toTouch(inputs[slot]));
    if(mailbox>=0)writeNativeTouch(emulator.module.HEAPU8,mailbox,metadata,touchSlot,packed,!enabled||!touchPlay);
    // Keep rescue presses/releases visible until a native frame has read them.
    const frame=mailbox>=0?new DataView(emulator.module.HEAPU8.buffer).getUint32(mailbox+16,true):undefined;
    touch.consume(frame);
  }
}
function release(){keyboard.clear();touch?.reset();syncInput();}
window.addEventListener('keydown',event=>{
  if(!emulator||form(event.target)||event.metaKey||event.ctrlKey||event.altKey)return;
  if(bindings.some(b=>b.has(event.code))){event.preventDefault();event.stopImmediatePropagation();keyboard.add(event.code);syncInput();}
},true);
window.addEventListener('keyup',event=>{if(keyboard.delete(event.code)){event.preventDefault();event.stopImmediatePropagation();syncInput();}},true);
window.addEventListener('blur',()=>{focused=false;release();});
window.addEventListener('focus',()=>{focused=true;});
document.addEventListener('visibilitychange',()=>{if(document.hidden)release();});
document.addEventListener('focusin',event=>{if(form(event.target))release();});
$('canvas').addEventListener('pointerdown',()=>{$('canvas').focus();emulator?.audio.context.resume().catch(()=>{});music?.resume();});

// Reuse TH04's complete touch surface/workbench, retaining the existing canvas.
const gameCanvas=$('canvas');
touch=mountPlayer($('screen'),{onGesture:()=>{gameCanvas.focus();music?.resume();return emulator?.audio.context.resume();},onChange:()=>syncInput()});
const musicVolume=document.createElement('label');musicVolume.className='player-volume';
musicVolume.innerHTML='音乐音量 <input type="range" min="0" max="100" value="70" aria-label="本机音乐音量">';
$('screen').querySelector('.player-toolbar').append(musicVolume);
musicVolume.querySelector('input').oninput=event=>music?.player.setVolume(Number(event.target.value)/100);
const presentation=document.createElement('div');presentation.className='screen';presentation.append(gameCanvas);touch.stage.prepend(presentation);
const seatLabel=document.createElement('label');seatLabel.className='touch-seat-picker';
const seatSelect=document.createElement('select');seatSelect.id='touch-seat';seatSelect.setAttribute('aria-label','触摸控制玩家');
for(let i=0;i<3;i++)seatSelect.add(new Option('P'+(i+1),String(i)));
seatLabel.append(seatSelect);$('screen').append(seatLabel);
seatSelect.onchange=()=>{touch.reset();touchSlot=Number(seatSelect.value);touchPlay=false;touch.setContext({key:'switch:menu',play:false});syncInput(true);};
showPlayerCount();
$('touchmode').onclick=()=>{$('screen').querySelector('[data-touch]').click();$('touchmode').setAttribute('aria-pressed',$('screen').querySelector('[data-touch]').getAttribute('aria-pressed'));};
$('touchmode').setAttribute('aria-pressed',$('screen').querySelector('[data-touch]').getAttribute('aria-pressed'));

function scan(){
  const h=emulator.module.HEAPU8,v=new DataView(h.buffer);
  candidates=candidates.filter(at=>signature.every((b,i)=>h[at+i]===b));
  const valid=()=>candidates.filter(at=>h[at+22]===1&&v.getUint16(at+20,true)>0);
  let active=valid();
  if(!active.length){
    // The native core exposes conventional guest RAM. Searching its 640 KiB
    // finds MAIN immediately instead of cycling through the whole WASM heap.
    const ramBase=emulator.module.nativeExports?.().thCoreState?.(0);
    const start=ramBase??0,limit=ramBase===undefined?h.length:Math.min(h.length,ramBase+0xa0000);
    if(cursor<start||cursor>=limit)cursor=start;
    const end=Math.min(limit,cursor+1024*1024),slice=h.subarray(cursor,Math.min(limit,end+signature.length-1));
    for(let i=slice.indexOf(signature[0]);i>=0&&cursor+i<end;i=slice.indexOf(signature[0],i+1)){
      const at=cursor+i;if(at+metadata.players.mailboxSize<=h.length&&signature.every((b,j)=>h[at+j]===b)&&!candidates.includes(at))candidates.push(at);
    }
    cursor=end;active=valid();
  }
  mailbox=active.length===1?active[0]:-1;
  emulator.module.playerArt?.bind(mailbox);
}
export function readState(){
  if(!emulator||mailbox<0)return null;
  const h=emulator.module.HEAPU8,v=new DataView(h.buffer),m=mailbox,out=h[m+23];
  const cb=m-metadata.entries.mailbox;
  return {ticks:v.getUint32(m+16,true),generation:v.getUint16(m+28,true),stage:h[m+30],score:v.getUint32(cb+metadata.entries.team_score,true),
    playerCount:h[m+metadata.players.countOffset],
    players:metadata.players.stateOffsets.slice(0,h[m+metadata.players.countOffset]).map((offset,index)=>({id:index+1,x:v.getInt16(m+offset,true),y:v.getInt16(m+offset+2,true),
      lives:h[m+offset+4],bombs:h[m+offset+5],power:h[m+offset+6],hit:h[m+offset+7],invincibility:h[m+offset+8],
      shot:h[m+offset+9],out:!!(out&(1<<index)),items:v.getUint32(m+metadata.players.itemOffsets[index],true),
      rescueTaps:h[m+metadata.players.progressOffsets[index]]>>5,rescueLatched:!!h[m+metadata.players.latchOffsets[index]],
      rescueTarget:h[m+metadata.players.recipientOffsets[index]]+1})),
    debugInvincible:!!h[m+89],focus:{percent:metadata.focus.percentOffsets.slice(0,h[m+110]).map(offset=>h[m+offset]),enabledMask:h[m+102],pointMask:h[m+103]},
    targets:[0,1,2].map(i=>v.getUint32(m-metadata.entries.mailbox+metadata.entries.target_counts+i*4,true)),
    attackTargets:[0,1,2].map(i=>v.getUint32(m-metadata.entries.mailbox+metadata.entries.attack_counts+i*4,true)),
    teamTargeting:!!h[m+104],
    rescueClearFrames:h[m+88],boss:{current:v.getUint16(m+90,true),max:v.getUint16(m+92,true),bar:h[m+94],visible:!!h[m+95]}};
}
// Authoritative native statistics for future LAN/public score presentation.
// Local HUD deliberately keeps the team score; reading this does not change it.
export function readPersonalStats(){
  if(!emulator||mailbox<0)return null;
  const h=emulator.module.HEAPU8,v=new DataView(h.buffer),cb=mailbox-metadata.entries.mailbox,b=metadata.balance;
  const count=h[mailbox+metadata.players.countOffset];
  return {version:b.version,units:b.scoreUnits,startingPlayerCount:count,
    scoreMultiplier:v.getUint16(cb+metadata.entries.score_factor,true)/10,
    bossMultiplier:v.getUint16(cb+metadata.entries.damage_denominator,true)/10,
    teamScore:v.getUint32(cb+metadata.entries[b.teamScoreEntry],true),
    players:Array.from({length:count},(_,index)=>{
      const at=cb+metadata.entries[b.statsEntry],next=v.getUint32(at+b.nextExtendOffset,true);
      return {id:index+1,score:v.getUint32(at+b.scoreOffset,true),extends:v.getUint16(at+b.extendsOffset,true),nextExtend:next||null};
    })};
}
function tick(now){
  try{
    if(emulator?.module.HEAPU8){scan();
      music?.poll(emulator.module.HEAPU8,now);
      const state=readState(),active=emulator.state==='running',h=emulator.module.HEAPU8;
      const nativePaused=mailbox>=0&&!!h[mailbox-metadata.entries.mailbox+metadata.entries.touch_paused];
      const playing=!!state&&!nativePaused;
      touchMenu=!playing;
      const selected=state?.players[touchSlot];
      touchPlay=playing&&!!selected&&!selected.out&&!selected.hit;
      if(active!==touchActive){touchActive=active;touch.setActive(active);}
      touch.setContext({key:`${state?.generation??0}:${touchSlot}:${playing?'game':'menu'}`,play:touchPlay});
      syncInput(true);
      if(now-lastHud>100){lastHud=now;const state=readState();
        if(state){for(const player of state.players)$('p'+player.id+'-state').textContent=player.out?`P${player.id} · 幽灵，等待救援`:`P${player.id} · ${['高速僚机','平衡僚机','低速僚机'][player.shot]}　备用残机 ${player.lives}　Bomb ${player.bombs}　P ${player.power}/80`;
          $('team-score').textContent='团队分数 '+state.score.toLocaleString('zh-CN');
          if(emulator.state==='running')status(`本机${state.playerCount===3?'三':'双'}人 · ${state.stage===5?'EX 关卡':`第 ${state.stage+1} 关`} · ${state.players.map(p=>'P'+p.id).join(' / ')} 独立控制${debugInvincible?' · 测试无敌已开启':''}`);
        }
      }
    }
  }catch(error){status('读取游戏状态失败：'+error.message);}
  requestAnimationFrame(tick);
}
requestAnimationFrame(tick);

$('start').onclick=async()=>{
  if(launching||emulator)return;launching=true;$('start').disabled=true;
  for(const id of launchSelects)$(id).disabled=true;
  for(const button of shotChoices)button.disabled=true;
  try{
    launchSettings=readLaunchSettings();launchConfig(launchSettings);
    status('加载 DOSBox-X 和 TH02 合作磁盘，准备直接进入关卡…');
    const [response,launcherResponse,versionResponse]=await Promise.all([fetch('local-patch.json',{cache:'no-store'}),fetch('netplay/th02-launch.com',{cache:'no-store'}),fetch('netplay/runtime.json',{cache:'no-store'})]);
    if(!response.ok||!launcherResponse.ok||!versionResponse.ok)throw Error('开局资源加载失败');
    metadata=await response.json();
    const launcher=new Uint8Array(await launcherResponse.arrayBuffer()),version=await versionResponse.json();
    if(await sha256(launcher)!==version.assets['netplay/th02-launch.com'])throw Error('网页启动器校验失败，请刷新页面');
    const entry=metadata.disks[launchSettings.language],diskResponse=await fetch(entry.file);
    if(!diskResponse.ok)throw Error('游戏磁盘加载失败');
    const stream=diskResponse.body.pipeThrough(new DecompressionStream('gzip'));
    const bytes=new Uint8Array(await new Response(stream).arrayBuffer());
    if(bytes.length!==entry.size||await sha256(bytes)!==entry.sha256)throw Error('游戏磁盘校验失败，请重新构建或刷新页面');
    const fat=new Fat12(bytes),main=fat.find('HUMAROKU/MAIN.EXE'),exe=fat.read(main);
    if(await sha256(exe)!==entry.mainSha256)throw Error('主程序校验失败');
    const configOffset=metadata.headerSize+metadata.codeSegment*16+metadata.entries.run_config;
    exe[configOffset]=launchSettings.p2;exe[configOffset+1]=launchSettings.p3;
    exe[configOffset+2]=launchSettings.players;fat.replace(main,exe);
    // Reuse the single-player resident music bridge; keep native sound effects.
    // This edits only the private disk buffer passed to this local emulator.
    music=new SoloMusic(message=>status(message));
    await music.install(bytes,'HUMAROKU');music.player.setVolume(Number(musicVolume.querySelector('input').value)/100);
    emulator=await DOSBoxPC98.create({canvas:$('canvas'),artPatch:metadata,
      launchFiles:[{path:'game/HUMAROKU/WEBSTART.COM',contents:configureLauncher(launcher,launchSettings)}],
      onError:error=>status('DOSBox-X 停止：'+error.message),onExit:()=>status('游戏已退出，可重新启动')});
    emulator.addDiskImage('th02-local.hdi',bytes);await emulator.setHdd(0,'th02-local.hdi');emulator.run();
    document.body.dataset.running='true';for(const id of ['pause','menu','confirm','restart','diagnostics'])$(id).disabled=false;
    $('canvas').focus();
    status('正在直接进入所选关卡，无需操作原版菜单…');
  }catch(error){status('启动失败：'+error.message);music?.dispose();music=null;emulator?.dispose();emulator=null;mailbox=-1;launchSettings=null;$('start').disabled=false;
    for(const id of launchSelects)$(id).disabled=false;for(const button of shotChoices)button.disabled=false;
  }
  finally{launching=false;showLaunchSettings();}
};
async function tap(code){if(!emulator)return;emulator.key(code,true);await new Promise(r=>setTimeout(r,100));emulator.key(code,false);$('canvas').focus();}
$('menu').onclick=()=>tap('Escape');$('confirm').onclick=()=>tap('Enter');
$('pause').onclick=()=>{
  if(!emulator)return;
  if(emulator.state==='running'){release();emulator.pause();music?.suspend();$('pause').textContent='继续';status('模拟器已暂停');}
  else if(emulator.state==='paused'){emulator.run();music?.resume();$('pause').textContent='暂停';$('canvas').focus();}
};
$('restart').onclick=()=>location.reload();
$('invincible').onclick=()=>{
  debugInvincible=!debugInvincible;
  $('invincible').setAttribute('aria-pressed',String(debugInvincible));
  $('invincible').textContent='测试无敌：'+(debugInvincible?'开':'关');
  syncInput();
  status(debugInvincible?'测试无敌已开启：保护所有存活玩家。':'测试无敌已关闭：恢复正常受伤规则。');
  if(emulator)$('canvas').focus();
};
$('fullscreen').onclick=()=>touch.enter();
for(const id of ['pad1','pad2','pad3'])$(id).onchange=()=>{
  if($(id).value!=='none'&&['pad1','pad2','pad3'].some(other=>other!==id&&$(other).value===$(id).value)){$(id).value='none';status('同一个手柄只能分配给一名玩家');}
  release();
};
$('diagnostics').onclick=()=>{
  if(!emulator)return;
  const report={schema:1,game:'th02',mode:'local',at:new Date().toISOString(),status:$('status').textContent,
    settings:launchSettings,state:readState(),runtime:emulator.module.mobileRuntime,logs:emulator.logs,patch:metadata,
    browser:navigator.userAgent,render:emulator.module.framePresenter?.metrics};
  const blob=new Blob([JSON.stringify(report,null,2)],{type:'application/json'}),url=URL.createObjectURL(blob),a=document.createElement('a');
  a.href=url;a.download='th02-local-diagnostics.json';a.click();setTimeout(()=>URL.revokeObjectURL(url),1000);
};
window.addEventListener('pagehide',()=>{release();music?.dispose();emulator?.dispose();});
