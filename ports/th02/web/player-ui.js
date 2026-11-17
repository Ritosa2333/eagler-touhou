import {createDirectDrag} from './direct-drag.js';
import './bgm-version.js';
// Presentation and local device state only. The caller samples once per tick.
import {packTouch} from './touch-input.js';
import {mountTouchLayout} from './touch-layout.js';
export function mountPlayer(host,{onGesture=()=>{},onChange=()=>{},solo=false,pulseMilliseconds=100}={}){
  host.classList.add('th02-player');
  // Keep styles outside the subtree replaced below, and resolve from this module
  // so local play and the netplay room use the same styles and geometry.
  for(const file of ['player-ui.css','touch-overlay.css']){
    const href=new URL(file+'?v=20261008-escape1',import.meta.url).href;
    if(![...document.querySelectorAll('link[rel="stylesheet"]')].some(link=>link.href===href))
      document.head.append(Object.assign(document.createElement('link'),{rel:'stylesheet',href}));
  }
  host.innerHTML='<div class="player-toolbar"><button type="button" data-full>全屏</button><button type="button" data-window>返回网页</button><button type="button" data-touch>触屏操作</button><button type="button" data-sound>启用声音</button><button type="button" data-layout>自定义布局</button><span class="player-label"></span></div><div class="player-stage"></div><div class="player-movement"><label class="touch-option"><input type="checkbox" data-unlimited> 触摸不限速移动</label><label>灵敏度 <input data-sensitivity type="range" min="100" max="300" step="10" value="150"><output>150%</output></label><label class="touch-option"><input type="checkbox" role="switch" data-always-point> 触摸模式一直显示判定点</label><label class="touch-option"><input type="checkbox" data-double-tap> 双击同一位置使用 Bomb</label><small>单指拖动移动 · 第二指按住低速</small><div class="player-menu-directions"><button type="button" data-pulse="1">菜单 ↑</button><button type="button" data-pulse="2">菜单 ↓</button></div></div><div class="player-actions"><button type="button" data-pulse="16">Bomb / 返回</button><button type="button" data-held="64">低速</button><button type="button" data-rescue>停火救援</button><button type="button" data-pulse="128">暂停 / 继续</button><button type="button" data-pulse="256">确认</button><button type="button" data-auto>开火：关</button></div><div class="player-note" role="status">等待开始；在画面上拖动控制当前玩家。</div><div class="touch-layout-editor" hidden><strong>按键与触控</strong><button type="button" data-layout-close>完成</button></div>';
  {
    host.classList.add('solo-player');
    host.querySelector('.player-menu-directions').insertAdjacentHTML('beforeend','<button type="button" data-pulse="4">菜单 ←</button><button type="button" data-pulse="8">菜单 →</button>');
  }
  const stage=host.querySelector('.player-stage');
  const diagnosticButton=document.createElement('button');diagnosticButton.type='button';diagnosticButton.textContent='启动／运行详情';
  const diagnosticPanel=document.createElement('pre');diagnosticPanel.className='player-diagnostics';diagnosticPanel.hidden=true;
  host.querySelector('.player-toolbar').append(diagnosticButton);host.append(diagnosticPanel);
  diagnosticButton.onclick=()=>{diagnosticPanel.hidden=!diagnosticPanel.hidden;diagnosticButton.setAttribute('aria-expanded',String(!diagnosticPanel.hidden));};
  const surface=document.createElement('div');surface.className='touch-surface';surface.setAttribute('aria-label','拖动移动，第二指低速');stage.append(surface);
  const unlimited=host.querySelector('[data-unlimited]'),sensitivity=host.querySelector('[data-sensitivity]'),alwaysPoint=host.querySelector('[data-always-point]'),doubleTap=host.querySelector('[data-double-tap]');
  let touchDrag;
  try{unlimited.checked=localStorage.getItem('th02.touch.mode')==='unlimited';sensitivity.value=String(Math.max(100,Math.min(300,Number(localStorage.getItem('th02.touch.sensitivity'))||150)));doubleTap.checked=localStorage.getItem('th02.touch.double-tap')==='on';}catch{}
  try{alwaysPoint.checked=localStorage.getItem('th02.touch.always-point')==='on';}catch{}
  const saveMovement=()=>{reset();host.querySelector('output').textContent=`${sensitivity.value}%`;try{localStorage.setItem('th02.touch.sensitivity',sensitivity.value);}catch{}};
  sensitivity.oninput=saveMovement;host.querySelector('output').textContent=`${sensitivity.value}%`;
  unlimited.onchange=()=>{try{localStorage.setItem('th02.touch.mode',unlimited.checked?'unlimited':'limited');}catch{};reset();notify();};
  doubleTap.onchange=()=>{touchDrag?.clearTap();tapDown=lastTap=null;try{localStorage.setItem('th02.touch.double-tap',doubleTap.checked?'on':'off');}catch{};};
  alwaysPoint.onchange=()=>{try{localStorage.setItem('th02.touch.always-point',alwaysPoint.checked?'on':'off');}catch{};notify();};
  const full=host.querySelector('[data-full]'),touch=host.querySelector('[data-touch]'),autoButton=host.querySelector('[data-auto]');
  let touchLayout,touchGeometry=null;
  // Movement needs dimensions only. Cache them until layout actually changes;
  // reading layout for every pointermove can force synchronous style/layout.
  function geometry(){
    if(!touchGeometry){const r=stage.getBoundingClientRect();touchGeometry={scale:Math.min(r.width/640,r.height/400),width:host.clientWidth,height:host.clientHeight};}
    return touchGeometry;
  }
  const geometryObserver=new ResizeObserver(()=>{touchGeometry=null;});
  geometryObserver.observe(stage);geometryObserver.observe(host);
  const held=new Map();let contextKey='',pulses=0,lastPulse=0,auto=false,active=false,gameplay=false,immersive=false,requestId=0,lastTap=null,tapDown=null;
  // Two native frames for each press/release survive emulator yields mid-frame.
  let rescueQueue=0,rescueGap=0,rescuePulseTicks=0,rescueQuietTicks=0,sampledRescueTap=false,rescueFrame=null;
  const pulseUntil=new Map();
  function pulse(bits){pulses|=bits;for(let bit=1;bit<=256;bit<<=1)if(bits&bit)pulseUntil.set(bit,performance.now()+pulseMilliseconds);}
  function sustainedPulses(){let bits=0;for(const [bit,until]of pulseUntil)if(performance.now()<until)bits|=bit;else pulseUntil.delete(bit);return bits;}
  let touchEnabled=matchMedia('(pointer:coarse)').matches;
  try{const saved=localStorage.getItem('th02.touch.enabled');if(saved!==null)touchEnabled=saved==='true';}catch{}
  const nativeFull=()=>document.fullscreenElement||document.webkitFullscreenElement;
  let retainPageFullscreen=false;
  const unlockEscape=()=>{try{navigator.keyboard?.unlock?.();}catch{}};
  async function exitNative(){
    if(nativeFull()!==host)return;
    if(document.exitFullscreen)await document.exitFullscreen();
    else await document.webkitExitFullscreen?.();
  }
  function notify(){onChange();}
  function renderTouch(){host.classList.toggle('has-touch',touchEnabled);touch.setAttribute('aria-pressed',String(touchEnabled));notify();}
  function renderAuto(){autoButton.querySelector('strong').textContent='开火';autoButton.querySelector('small').textContent=auto?'已开启 · 点按关闭':'点按开启';autoButton.setAttribute('aria-pressed',String(auto));}
  function reset({keepFire=false}={}){held.clear();pulseUntil.clear();touchDrag?.reset();tapDown=lastTap=null;pulses=0;lastPulse=0;rescueQueue=rescueGap=rescuePulseTicks=rescueQuietTicks=0;sampledRescueTap=false;if(!keepFire)auto=false;host.querySelectorAll('.pressed').forEach(n=>n.classList.remove('pressed'));renderAuto();notify();}
  touchLayout=mountTouchLayout(host,{reset,solo});
  const rescueButton=host.querySelector('[data-rescue]');
  if(rescueButton)rescueButton.title='每次点按计一次；连按 5 次，间隔不超过 24 个游戏帧。';
  if(rescueButton)rescueButton.setAttribute('aria-label','停火救援，点按 5 次；每两次最多间隔 24 个游戏帧');
  function note(text){host.querySelector('.player-note').textContent=text;}
  function fullscreenLabel(){
    host.querySelector('.touch-full-open').setAttribute('aria-label',immersive?'退出全屏':'进入全屏');
    host.querySelector('.touch-full-open').title=immersive?'退出全屏（游戏继续；Esc 暂停）':'进入全屏';
  }
  async function pageFullscreen(token){
    if(token!==requestId)return;
    // Keep the immersive layout when releasing a native fullscreen that could
    // not lock Escape. fullscreenchange must not tear down this fallback.
    retainPageFullscreen=true;unlockEscape();
    try{await exitNative();}catch{}
    if(token!==requestId)return;
    full.textContent='网页全屏 · F11';
    note('Esc 暂停／继续。已铺满网页，F11 可切换浏览器全屏；右上角按钮退出布局。');
  }
  function fit(){touchGeometry=null;host.style.setProperty('--player-height',`${Math.round(window.visualViewport?.height||innerHeight)}px`);reset();}
  function gesture(){
    const focused=document.activeElement;if(focused?.closest?.('.player-toolbar,[data-layout-control]'))focused.blur();
    try{Promise.resolve(onGesture()).catch(()=>{});}catch{}
  }
  async function enter({native=true}={}){
    const token=++requestId;immersive=true;retainPageFullscreen=false;host.classList.add('immersive');document.documentElement.classList.add('th02-immersive');fullscreenLabel();fit();gesture();
    if(!native){retainPageFullscreen=true;full.textContent='全屏布局';return;}
    const canLock=typeof navigator.keyboard?.lock==='function';
    const preserveEscape=!touchEnabled||matchMedia('(any-pointer:fine)').matches;
    // Insecure LAN pages and browsers without Keyboard Lock must not capture
    // Escape for the Fullscreen API. F11 remains the browser's own shortcut.
    if(preserveEscape&&!canLock){await pageFullscreen(token);return;}
    try{
      if(nativeFull()!==host){
        if(host.requestFullscreen)await host.requestFullscreen({navigationUI:'hide'});
        else if(host.webkitRequestFullscreen)await host.webkitRequestFullscreen();
        else throw Error('unsupported');
      }
      if(token!==requestId){if(nativeFull()===host)await document.exitFullscreen?.();return;}
      if(canLock){
        try{await navigator.keyboard.lock(['Escape']);}
        catch{if(preserveEscape){await pageFullscreen(token);return;}}
        if(token!==requestId){unlockEscape();return;}
      }
      full.textContent='已全屏';
    }catch{await pageFullscreen(token);}
  }
  async function exit(){
    requestId++;immersive=false;retainPageFullscreen=false;unlockEscape();host.classList.remove('immersive');document.documentElement.classList.remove('th02-immersive');fullscreenLabel();reset();full.textContent='全屏';
    try{await exitNative();}catch{}
  }
  full.onclick=()=>enter();host.querySelector('[data-window]').onclick=()=>exit();
  touch.onclick=()=>{touch.blur();touchEnabled=!touchEnabled;reset();renderTouch();try{localStorage.setItem('th02.touch.enabled',String(touchEnabled));}catch{};if(immersive&&!touchEnabled)enter();};
  host.querySelector('[data-sound]').onclick=gesture;
  autoButton.onclick=()=>{if(!active||!gameplay)return;gesture();auto=!auto;renderAuto();notify();};
  for(const button of host.querySelectorAll('[data-held],[data-pulse],[data-rescue]')){
    button.addEventListener('pointerdown',event=>{
      if(!active||event.button!==0)return;event.preventDefault();gesture();button.setPointerCapture(event.pointerId);
      const rescue=button.hasAttribute('data-rescue');
      if(rescue){if(rescueQuietTicks===0)rescueGap=2;rescueQueue=Math.min(5,rescueQueue+1);rescueQuietTicks=25;}
      held.set(event.pointerId,{button,bits:rescue?0:Number(button.dataset.held)||0,rescue});
      if(!rescue)pulse(Number(button.dataset.pulse)||(Number(button.dataset.held)&32));button.classList.add('pressed');notify();
    });
    const release=event=>{if(!held.has(event.pointerId))return;held.delete(event.pointerId);if(![...held.values()].some(v=>v.button===button))button.classList.remove('pressed');notify();};
    button.addEventListener('pointerup',release);button.addEventListener('lostpointercapture',release);
    button.addEventListener('pointercancel',()=>reset());
    button.addEventListener('click',event=>{
      event.preventDefault();
      // Keyboard/assistive activation has no pointerdown to emit the pulse.
      if(event.detail===0&&active&&button.hasAttribute('data-rescue')){
        gesture();if(rescueQuietTicks===0)rescueGap=2;rescueQueue=Math.min(5,rescueQueue+1);rescueQuietTicks=25;notify();
      }else if(event.detail===0&&active&&button.dataset.pulse==='128'){
        gesture();pulse(128);notify();
      }
    });
  }
  touchDrag=createDirectDrag(surface,{
    allowed:()=>active&&touchEnabled&&!document.hidden&&!touchLayout.isEditing(),
    gameplay:()=>gameplay,menu:()=>contextKey.endsWith(':menu'),geometry,
    sensitivity:()=>Number(sensitivity.value),doubleTap:()=>doubleTap.checked,
    gesture,change:notify,pulse:bit=>{pulse(bit);notify();},
  });
  host.addEventListener('contextmenu',event=>{if(event.target.closest('.touch-surface,[data-layout-control]'))event.preventDefault();});
  // Prevent virtual controls from taking focus away from an active pointer.
  host.addEventListener('pointerdown',event=>{if(event.target.closest('[data-layout-control]'))event.preventDefault();});
  window.addEventListener('blur',reset);window.addEventListener('pagehide',reset);
  window.addEventListener('pagehide',unlockEscape);
  document.addEventListener('visibilitychange',()=>{if(document.hidden)reset();});
  window.addEventListener('resize',fit);window.visualViewport?.addEventListener('resize',fit);
  for(const name of ['fullscreenchange','webkitfullscreenchange'])document.addEventListener(name,()=>{reset();if(!nativeFull()){unlockEscape();if(immersive&&!retainPageFullscreen)exit();}});
  renderTouch();renderAuto();fit();
  return {stage,enter,exit,reset,note,
    enabled:()=>touchEnabled,
    moving:()=>active&&touchEnabled&&gameplay&&!touchLayout.isEditing()&&touchDrag.dragging,
    alwaysPointControl:alwaysPoint.closest('label'),
    alwaysPoint:()=>touchEnabled&&alwaysPoint.checked,
    diagnostics(text,{show=false}={}){diagnosticPanel.textContent=text;if(show){diagnosticPanel.hidden=false;diagnosticButton.setAttribute('aria-expanded','true');}},
    setLabel(text){host.querySelector('.player-label').textContent=text;},
    setActive(value){active=value;if(!value)reset();},
    setGameplay(value){host.classList.toggle('touch-menu',!value);if(gameplay!==value){gameplay=value;reset();}},
    setContext({key,play}){
      host.classList.toggle('touch-menu',key.endsWith(':menu'));
      if(key!==contextKey){contextKey=key;reset({keepFire:true});}
      // Temporary injury/entry blocks output, never forgets a held finger.
      if(gameplay!==play){touchDrag.clearMotion();pulses=lastPulse=0;tapDown=lastTap=null;rescueQueue=rescueGap=rescuePulseTicks=rescueQuietTicks=0;sampledRescueTap=false;}
      gameplay=play;
    },
    sample(base=0){
      if(touchLayout.isEditing())return 0;
      if(!active||document.hidden)return base;
      // The on-screen pause button also belongs to keyboard/mouse players.
      if(!touchEnabled){let value=base|(((pulses&~lastPulse)|sustainedPulses())&128);if(gameplay&&rescueQuietTicks>0){value&=~64;if(rescueQueue&&rescueGap===0){value|=64;sampledRescueTap=true;}}if(gameplay&&(rescueQuietTicks>0||[...held.values()].some(v=>v.rescue)))value&=~(16|32);return value;}
      let value=base|(pulses&~lastPulse)|sustainedPulses(),rescue=false;
      for(const v of held.values()){value|=v.bits;rescue||=v.rescue;}
      if(auto&&gameplay)value|=32;
      if(touchDrag.focused&&gameplay)value|=64;
      if(gameplay&&touchDrag.dragging)value&=~15;
      if(!gameplay)value|=touchDrag.menuBits;
      // Rescue deliberately suppresses firing/Bomb across all local devices.
      if(gameplay&&rescueQuietTicks>0){value&=~64;if(rescueQueue&&rescueGap===0){value|=64;sampledRescueTap=true;}}
      if(gameplay&&(rescueQuietTicks>0||rescue))value&=~(16|32);
      return value;
    },
    pack(buttons,{includePointPreference=true}={}){
      const motion=touchDrag.sample();
      return packTouch(buttons,{x:motion.x,y:motion.y,begin:touchDrag.beginning,active:active&&touchEnabled&&gameplay&&!touchLayout.isEditing()&&motion.active,unlimited:unlimited.checked,alwaysPoint:includePointPreference&&touchEnabled&&alwaysPoint.checked});
    },
    consume(frame){touchDrag.consume();const emitted=pulses&~lastPulse;pulses&=~emitted;lastPulse=emitted;if(frame===undefined||frame!==rescueFrame){rescueFrame=frame;if(sampledRescueTap){if(++rescuePulseTicks===2){rescueQueue--;rescuePulseTicks=0;rescueGap=2;}}else if(rescueGap)rescueGap--;if(rescueQuietTicks)rescueQuietTicks--;sampledRescueTap=false;}}
  };
}
