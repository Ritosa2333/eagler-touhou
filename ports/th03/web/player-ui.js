import {createDirectDrag} from './direct-drag.js';
import './bgm-version.js';
// Presentation and local device state only. The caller samples once per tick.
import {packTouch} from './touch-input.js';
import {mountTouchLayout} from './touch-layout.js';
export function mountPlayer(host,{onGesture=()=>{},onChange=()=>{},onFullscreenExit=null,solo=false,pure=false,network=false}={}){
  host.classList.add('th03-player');
  const storagePrefix=pure?'th03.solo.touch':'th03.touch';
  // Keep styles outside the subtree replaced below, and resolve from this module
  // so local play and the netplay room use the same styles and geometry.
  for(const file of ['player-ui.css','touch-overlay.css']){
    const href=new URL(file+'?v=20261008-touch4',import.meta.url).href;
    if(![...document.querySelectorAll('link[rel="stylesheet"]')].some(link=>new URL(link.href).pathname===new URL(href).pathname))
      document.head.append(Object.assign(document.createElement('link'),{rel:'stylesheet',href}));
  }
  host.innerHTML='<div class="player-toolbar"><button type="button" data-full>全屏</button><button type="button" data-window>返回网页</button><button type="button" data-touch>触屏操作</button><button type="button" data-sound>启用声音</button><button type="button" data-layout>自定义布局</button><span class="player-label"></span></div><div class="player-stage"></div><div class="player-movement"><label class="touch-option"><input type="checkbox" data-unlimited> 触摸不限速移动</label><label>灵敏度 <input data-sensitivity type="range" min="100" max="300" step="10" value="150"><output>150%</output></label><label class="touch-option"><input type="checkbox" role="switch" data-always-point> 触摸模式一直显示判定点</label><label class="touch-option"><input type="checkbox" data-double-tap> 双击同一位置使用攻击</label><small>单指拖动移动 · 第二指按住低速</small><div class="player-menu-directions"><button type="button" data-pulse="1">菜单 ↑</button><button type="button" data-pulse="2">菜单 ↓</button></div></div><div class="player-actions"><button type="button" data-pulse="16">攻击 / 返回</button><button type="button" data-held="64">低速</button><button type="button" data-held="32">蓄力</button><button type="button" data-pulse="128">暂停 / 继续</button><button type="button" data-charge>蓄力</button></div><div class="player-note" role="status">等待开始；在画面上拖动控制当前玩家。</div><div class="touch-layout-editor" hidden><strong>按键与触控</strong><button type="button" data-layout-close>完成</button></div>';
  if(solo){
    host.classList.add('solo-player');
  }
  if(pure)host.classList.add('pure-player');
  const stage=host.querySelector('.player-stage');
  host.querySelector('[data-held="32"]').remove();
  const diagnosticButton=document.createElement('button');diagnosticButton.type='button';diagnosticButton.textContent='启动／运行详情';
  const diagnosticPanel=document.createElement('pre');diagnosticPanel.className='player-diagnostics';diagnosticPanel.hidden=true;
  host.querySelector('.player-toolbar').append(diagnosticButton);host.append(diagnosticPanel);
  diagnosticButton.onclick=()=>{diagnosticPanel.hidden=!diagnosticPanel.hidden;diagnosticButton.setAttribute('aria-expanded',String(!diagnosticPanel.hidden));};
  const surface=document.createElement('div');surface.className='touch-surface';surface.setAttribute('aria-label','拖动移动，第二指低速');stage.append(surface);
  const unlimited=host.querySelector('[data-unlimited]'),sensitivity=host.querySelector('[data-sensitivity]'),alwaysPoint=host.querySelector('[data-always-point]'),doubleTap=host.querySelector('[data-double-tap]');
  let touchDrag;
  try{unlimited.checked=localStorage.getItem(`${storagePrefix}.mode`)==='unlimited';sensitivity.value=String(Math.max(100,Math.min(300,Number(localStorage.getItem(`${storagePrefix}.sensitivity`))||150)));doubleTap.checked=localStorage.getItem(`${storagePrefix}.double-tap`)==='on';}catch{}
  try{alwaysPoint.checked=localStorage.getItem(`${storagePrefix}.always-point`)==='on';}catch{}
  const saveMovement=()=>{reset();host.querySelector('output').textContent=`${sensitivity.value}%`;try{localStorage.setItem(`${storagePrefix}.sensitivity`,sensitivity.value);}catch{}};
  sensitivity.oninput=saveMovement;host.querySelector('output').textContent=`${sensitivity.value}%`;
  unlimited.onchange=()=>{try{localStorage.setItem(`${storagePrefix}.mode`,unlimited.checked?'unlimited':'limited');}catch{};reset();notify();};
  doubleTap.onchange=()=>{touchDrag?.clearTap();tapDown=lastTap=null;try{localStorage.setItem(`${storagePrefix}.double-tap`,doubleTap.checked?'on':'off');}catch{};};
  alwaysPoint.onchange=()=>{try{localStorage.setItem(`${storagePrefix}.always-point`,alwaysPoint.checked?'on':'off');}catch{};notify();};
  const full=host.querySelector('[data-full]'),touch=host.querySelector('[data-touch]'),autoButton=host.querySelector('[data-charge]');
  let touchLayout,touchGeometry=null;
  // Movement needs dimensions only. Cache them until layout actually changes;
  // reading layout for every pointermove can force synchronous style/layout.
  function geometry(){
    if(!touchGeometry){const r=stage.getBoundingClientRect();touchGeometry={scale:Math.min(r.width/640,r.height/400),width:host.clientWidth,height:host.clientHeight};}
    return touchGeometry;
  }
  const geometryObserver=new ResizeObserver(()=>{touchGeometry=null;});
  geometryObserver.observe(stage);geometryObserver.observe(host);
  const held=new Map();let contextKey='',pulses=0,lastPulse=0,active=false,gameplay=false,immersive=false,requestId=0,lastTap=null,tapDown=null;
  let unlimitedAllowed=true;
  host.querySelector('.player-actions').insertAdjacentHTML('beforeend','<button type="button" data-rapid data-held="512" aria-label="按住连发"></button>');
  let touchEnabled=matchMedia('(pointer:coarse)').matches;
  try{const saved=localStorage.getItem(`${storagePrefix}.enabled`);if(saved!==null)touchEnabled=saved==='true';}catch{}
  const nativeFull=()=>document.fullscreenElement||document.webkitFullscreenElement;
  let hadNativeFullscreen=false,lastEscapeKeyAt=-Infinity;
  window.addEventListener('keydown',event=>{
    if(event.code==='Escape'&&nativeFull()===host)lastEscapeKeyAt=performance.now();
  },true);
  function viewChanged(){
    host.querySelector('.touch-full-open')?.setAttribute('aria-label',immersive?'退出全屏':'进入全屏');
    host.dispatchEvent(new CustomEvent('player-viewchange',{detail:{immersive}}));
  }
  function notify(){onChange();}
  function renderTouch(){host.classList.toggle('has-touch',touchEnabled);touch.setAttribute('aria-pressed',String(touchEnabled));}
  function reset(){held.clear();touchDrag?.reset();tapDown=lastTap=null;pulses=0;lastPulse=0;host.querySelectorAll('.pressed').forEach(n=>n.classList.remove('pressed'));notify();}
  touchLayout=mountTouchLayout(host,{reset,solo,storagePrefix});
  if(pure){
    const movement=host.querySelector('.player-movement');
    movement.insertAdjacentHTML('afterbegin','<label class="touch-option"><input type="checkbox" data-focus-enabled> 启用低速模式</label><label class="touch-option"><input type="checkbox" data-focus-points> 低速时显示判定点</label>');
    for(const name of ['focus-enabled','focus-points']){
      const control=host.querySelector(`[data-${name}]`);
      try{control.checked=localStorage.getItem(`${storagePrefix}.${name}`)==='on';}catch{}
      control.onchange=()=>{try{localStorage.setItem(`${storagePrefix}.${name}`,control.checked?'on':'off');}catch{};reset();renderAssists();};
    }
    const assists=document.createElement('section');assists.className='solo-assists';assists.setAttribute('aria-label','操作辅助');
    assists.innerHTML='<strong>操作辅助（可选）</strong>';
    for(const name of ['focus-enabled','focus-points','always-point','unlimited'])assists.append(host.querySelector(`[data-${name}]`).closest('label'));
    host.querySelector('.touch-workbench-scroll').prepend(assists);
    function renderAssists(){
      const enabled=host.querySelector('[data-focus-enabled]').checked;
      host.querySelector('[data-layout-control="focus"]').hidden=!enabled;
      surface.setAttribute('aria-label',enabled?'拖动移动，第二指低速':'拖动移动');
    }
    renderAssists();
    movement.querySelector('small').textContent='辅助可随时开关；低速键为空格，也可按触摸低速或用第二指。';

  }
  function note(text){host.querySelector('.player-note').textContent=text;}
  function fit(){touchGeometry=null;host.style.setProperty('--player-height',`${Math.round(window.visualViewport?.height||innerHeight)}px`);reset();}
  function gesture(){
    const focused=document.activeElement;if(focused?.closest?.('.player-toolbar,[data-layout-control]'))focused.blur();
    try{Promise.resolve(onGesture()).catch(()=>{});}catch{}
  }
  async function enter({native=true}={}){
    const token=++requestId;immersive=true;host.classList.add('immersive');document.documentElement.classList.add('th03-immersive');viewChanged();fit();gesture();
    if(!native){full.textContent='全屏布局';return;}
    try{
      if(nativeFull()!==host){
        if(host.requestFullscreen)await host.requestFullscreen({navigationUI:'hide'});
        else if(host.webkitRequestFullscreen)await host.webkitRequestFullscreen();
        else throw Error('unsupported');
      }
      if(token!==requestId){if(nativeFull()===host)await document.exitFullscreen?.();return;}
      full.textContent='已全屏';
      // Supported browsers deliver a short Esc to the game; a held Esc remains
      // the browser's escape hatch. Unsupported/denied lock uses the exit fallback.
      try{await navigator.keyboard?.lock(['Escape']);}catch{}
      if(token!==requestId)navigator.keyboard?.unlock();
    }catch{if(token===requestId){full.textContent='点击进入全屏';note('已铺满网页；浏览器未允许系统全屏，可点击“进入全屏”重试。');}}
  }
  async function exit(){
    requestId++;immersive=false;navigator.keyboard?.unlock();host.classList.remove('immersive');document.documentElement.classList.remove('th03-immersive');viewChanged();reset();full.textContent='全屏';
    try{if(nativeFull()===host){if(document.exitFullscreen)await document.exitFullscreen();else await document.webkitExitFullscreen?.();}}catch{}
  }
  full.onclick=()=>immersive?exit():enter();host.querySelector('[data-window]').onclick=()=>exit();
  touch.onclick=()=>{touch.blur();touchEnabled=!touchEnabled;reset();renderTouch();try{localStorage.setItem(`${storagePrefix}.enabled`,String(touchEnabled));}catch{}};
  host.querySelector('[data-sound]').onclick=gesture;
  autoButton.dataset.held='32';
  for(const button of host.querySelectorAll('[data-held],[data-pulse]')){
    button.addEventListener('pointerdown',event=>{
      if(!active||event.button!==0)return;event.preventDefault();gesture();button.setPointerCapture(event.pointerId);
      held.set(event.pointerId,{button,bits:Number(button.dataset.held)||0});
      pulses|=Number(button.dataset.pulse)||(Number(button.dataset.held)&544);button.classList.add('pressed');notify();
    });
    const release=event=>{if(!held.has(event.pointerId))return;held.delete(event.pointerId);if(![...held.values()].some(v=>v.button===button)){button.classList.remove('pressed');}notify();};
    button.addEventListener('pointerup',release);button.addEventListener('lostpointercapture',release);
    button.addEventListener('pointercancel',()=>reset());
    button.addEventListener('click',event=>event.preventDefault());
  }
  touchDrag=createDirectDrag(surface,{
    allowed:()=>active&&touchEnabled&&!document.hidden&&!touchLayout.isEditing(),
    gameplay:()=>gameplay,menu:()=>contextKey.endsWith(':menu'),geometry,
    sensitivity:()=>Number(sensitivity.value),doubleTap:()=>doubleTap.checked,
    gesture,change:notify,pulse:bit=>{pulses|=bit;notify();},
  });
  host.addEventListener('contextmenu',event=>{if(event.target.closest('.touch-surface,[data-layout-control]'))event.preventDefault();});
  // Prevent virtual controls from taking focus away from an active pointer.
  host.addEventListener('pointerdown',event=>{if(event.target.closest('[data-layout-control]'))event.preventDefault();});
  window.addEventListener('blur',reset);window.addEventListener('pagehide',reset);
  document.addEventListener('visibilitychange',()=>{if(document.hidden)reset();});
  window.addEventListener('resize',fit);window.visualViewport?.addEventListener('resize',fit);
  for(const name of ['fullscreenchange','webkitfullscreenchange'])document.addEventListener(name,()=>{
    reset();
    if(nativeFull()===host){hadNativeFullscreen=true;return;}
    if(!hadNativeFullscreen)return;
    hadNativeFullscreen=false;navigator.keyboard?.unlock();
    if(immersive){
      full.textContent='全屏布局';fit();
      // Do not send a second pause if the browser delivered Esc before exiting.
      if(performance.now()-lastEscapeKeyAt>500){
        if(onFullscreenExit)onFullscreenExit();
        else if(pure&&gameplay&&active)pulses|=128;
      }
    }
  });
  renderTouch();fit();
  return {stage,enter,exit,reset,note,
    setUnlimitedAllowed(allowed){unlimitedAllowed=!!allowed;unlimited.disabled=!allowed;if(!allowed)unlimited.checked=false;unlimited.closest('label').title=allowed?'':'房主已禁止触摸不限速移动';reset();},
    isEditing:()=>touchLayout.isEditing(),
    alwaysPointControl:alwaysPoint.closest('label'),
    alwaysPoint:()=>touchEnabled&&alwaysPoint.checked,
    focusEnabled:()=>!!host.querySelector('[data-focus-enabled]')?.checked,
    focusPoints:()=>!!host.querySelector('[data-focus-points]')?.checked,
    diagnostics(text,{show=false}={}){diagnosticPanel.textContent=text;if(show){diagnosticPanel.hidden=false;diagnosticButton.setAttribute('aria-expanded','true');}},
    setLabel(text){host.querySelector('.player-label').textContent=text;},
    setActive(value){active=value;if(!value)reset();},
    setGameplay(value){host.classList.toggle('touch-menu',!value);if(gameplay!==value){gameplay=value;reset();}},
    setContext({key,play}){
      host.classList.toggle('touch-menu',key.endsWith(':menu'));
      if(key!==contextKey){contextKey=key;reset();}
      // Temporary injury/entry blocks output, never forgets a held finger.
      if(gameplay!==play){touchDrag.clearMotion();pulses=lastPulse=0;tapDown=lastTap=null;}
      gameplay=play;
    },
    sample(base=0){
      if(touchLayout.isEditing())return 0;
      if(!active||document.hidden||!touchEnabled)return base;
      let value=base|(pulses&~lastPulse);
      for(const v of held.values())value|=v.bits;
      if(touchDrag.focused&&gameplay)value|=64;
      if(gameplay&&touchDrag.dragging)value&=~15;
      if(!gameplay)value|=touchDrag.menuBits;
      return value;
    },
    pack(buttons){
      const motion=touchDrag.sample();
      return packTouch(buttons,{x:motion.x,y:motion.y,begin:touchDrag.beginning,active:active&&touchEnabled&&gameplay&&!touchLayout.isEditing()&&motion.active,unlimited:unlimitedAllowed&&unlimited.checked,alwaysPoint:touchEnabled&&alwaysPoint.checked});
    },
    consume(){touchDrag.consume();const emitted=pulses&~lastPulse;pulses&=~emitted;lastPulse=emitted;}
  };
}
