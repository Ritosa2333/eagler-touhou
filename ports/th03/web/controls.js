import {FireControl} from './fire-control.js';
import {limitTouch,withoutPoint} from './touch-input.js';
export const actions = ['up', 'down', 'left', 'right', 'shot', 'charge', 'attack', 'focus'];
const labels = {up: '上', down: '下', left: '左', right: '右', shot: '连发（最高速）', charge: '蓄力', attack: 'Bomb', focus: '低速'};
const defaults = [
  {up: 'ArrowUp', down: 'ArrowDown', left: 'ArrowLeft', right: 'ArrowRight', shot: 'KeyZ', attack: 'KeyX', charge: 'ShiftLeft', focus: 'Space'},
  {up: 'KeyW', down: 'KeyS', left: 'KeyA', right: 'KeyD', shot: 'KeyJ', attack: 'KeyL', charge: 'KeyK', focus: 'ControlLeft'},
];
// TH03 keyboard-vs-keyboard inputs, from ReC98 th03/hardware/input_s.cpp.
const native = [
  {up: ['KeyT', 't', 84], down: ['KeyB', 'b', 66], left: ['KeyF', 'f', 70], right: ['KeyH', 'h', 72], shot: ['KeyZ', 'z', 90], attack: ['KeyX', 'x', 88]},
  {up: ['Numpad8', '8', 104], down: ['Numpad2', '2', 98], left: ['Numpad4', '4', 100], right: ['Numpad6', '6', 102], shot: ['ArrowLeft', 'ArrowLeft', 37], attack: ['ArrowRight', 'ArrowRight', 39]},
];
const storageKey = 'th03-local-controls-v2';
export function keyName(code) {
  return ({ArrowUp: '↑', ArrowDown: '↓', ArrowLeft: '←', ArrowRight: '→', Space: 'Space'})[code]
    || code.replace(/^Key/, '').replace(/^Digit/, '').replace(/^Numpad/, 'Num ');
}

export function mountControls({canvas, getEmulator, getNetwork, getTouch, getRoomFocus, getRoomUnlimited=()=>true, networkMode=false, dialog, onGesture, onOtherInput, onPause, onMenu, onEscape, onFocus, onTouch, isPaused, inGameplay}) {
  const activeSlots=networkMode?[0]:[0,1];
  const settingsKey=networkMode?'th03-network-controls-v1':storageKey;
  const padDefaults={up:12,down:13,left:14,right:15,shot:0,attack:1,charge:2,focus:5,pause:9};
  let padBindings=structuredClone(padDefaults);
  let bindings = structuredClone(defaults), padSlots = ['auto', networkMode?'none':'auto'];
  let pending = null, focused = true, previous = new Set(), lastPadInventory = '';
  const fire=[new FireControl(),new FireControl()]; let fireFrame=0;
  const tapped=new Set();
  const keys = new Set(), synthetic = new WeakSet(), pointers = new Map();
  const padPrevious = [new Set(), new Set()], padBlocked = [new Set(), new Set()];
  try {
    const saved = JSON.parse(localStorage.getItem(settingsKey));
    const used = new Set();
    // Upgrade the six-action layout without discarding custom movement keys.
    const savedActions = actions.filter(action => action !== 'focus');
    const hasFocus = saved?.bindings?.some(row => row.focus !== undefined);
    if (hasFocus) savedActions.push('focus');
    if (saved?.bindings?.length === 2 && activeSlots.every(slot => savedActions.every(action => {
      const code = saved.bindings[slot][action];
      if (typeof code !== 'string' || !/^(Key[A-Z]|Digit[0-9]|Numpad[0-9]|Arrow(Up|Down|Left|Right)|Space|Shift(Left|Right)|Control(Left|Right)|Alt(Left|Right))$/.test(code) || used.has(code)) return false;
      used.add(code); return true;
    }))) {
      bindings = saved.bindings;
      // Migrate the old default focus pair without resetting custom bindings.
      if (!saved.focusDefaultsVersion && bindings[0].focus === 'ControlLeft' && bindings[1].focus === 'Space') {
        bindings[0].focus = 'Space'; bindings[1].focus = 'ControlLeft';
      }
      if (!hasFocus) for (let slot = 0; slot < 2; slot++) {
        const code = [defaults[slot].focus, 'ShiftRight', 'ControlLeft', 'ControlRight', 'AltRight',
          ...'ABCDEFGHIJKLMNOPQRSTUVWXYZ'.split('').map(letter => `Key${letter}`)]
          .find(candidate => !used.has(candidate));
        bindings[slot].focus = code; used.add(code);
      }
    }
    if (saved?.pads?.length === 2 && saved.pads.every(p => /^(auto|none|[0-3])$/.test(p))) padSlots = saved.pads;
    if (networkMode) padSlots[1]='none';
    if(saved?.padBindings&&Object.keys(padDefaults).every(action=>Number.isInteger(saved.padBindings[action])&&saved.padBindings[action]>=0&&saved.padBindings[action]<32))padBindings=saved.padBindings;
    if (saved && !saved.focusDefaultsVersion) save();
  } catch {}

  const focusSettings = [{enabled: true, points: true}, {enabled: true, points: true}];
  try {
    const saved = JSON.parse(localStorage.getItem('th03-local-focus-v1'));
    for (let slot = 0; slot < 2; slot++) for (const key of ['enabled', 'points']) {
      if (typeof saved?.[slot]?.[key] === 'boolean') focusSettings[slot][key] = saved[slot][key];
    }
  } catch {}
  for (let slot = 0; slot < 2; slot++) for (const key of ['enabled', 'points']) {
    const checkbox = document.getElementById(`focus-${key}-${slot}`);
    checkbox.checked = focusSettings[slot][key];
    checkbox.addEventListener('change', () => {
      release(); focusSettings[slot][key] = checkbox.checked;
      try { localStorage.setItem('th03-local-focus-v1', JSON.stringify(focusSettings)); }
      catch { document.getElementById('binding-status').textContent = '浏览器未保存低速设置'; }
    });
  }

  function matches(slot, action, code) {
    const binding = bindings[slot][action];
    if (binding === code) return true;
    // A modifier binding also accepts its other side unless it has an
    // explicit owner in a legacy/custom layout.
    return /^(Shift|Control)(Left|Right)$/.test(binding)
      && code.replace(/(Left|Right)$/, '') === binding.replace(/(Left|Right)$/, '')
      && !activeSlots.some(slot => Object.values(bindings[slot]).includes(code));
  }

  function save() {
    try { localStorage.setItem(settingsKey, JSON.stringify({bindings, pads: padSlots, padBindings, focusDefaultsVersion: 1})); }
    catch { document.getElementById('binding-status').textContent = '浏览器未保存操作设置'; }
  }
  function send(target, touch = 0, cancelled = []) {
    const network = getNetwork?.();
    if(network?.isLockstep){
      const local=new Set([...target].filter(token=>token.startsWith('0:')).map(token=>token.slice(2)));
      if(target.has('confirm'))local.add('shot');
      // Display preferences never enter the shared simulation or input stream.
      network.setLocal(local,withoutPoint(touch),{focusEnabled:getRoomFocus(),points:false,cancelled});
      return;
    }
    const touches = [touch, 0];
    if (network) {
      const local = new Set([...target].filter(token => token.startsWith('0:')).map(token => token.slice(2)));
      network.send(local, touch);
      const localSeat = network.localSeat ?? 0, remoteSeat = localSeat ^ 1;
      target = new Set([...local].map(action => `${localSeat}:${action}`));
      for (const action of network.remote) target.add(`${remoteSeat}:${action}`);
      touches[localSeat] = touch; touches[remoteSeat] = network.takeTouch();
    }
    let mask = 0, points = 0;
    for (let slot = 0; slot < 2; slot++) {
      if (focusSettings[slot].enabled && target.has(`${slot}:focus`)) mask |= 1 << slot;
      if (focusSettings[slot].points) points |= 1 << slot;
    }
    onFocus(mask, points);
    onTouch?.(touches);
    const emulator = getEmulator();
    if (!emulator) return;
    for (const token of new Set([...previous, ...target])) {
      if (token.endsWith(':focus')) continue; // native mailbox, no DOS key
      if (previous.has(token) === target.has(token)) continue;
      const item = token === 'confirm' ? ['Enter', 'Enter', 13] : native[Number(token[0])][token.slice(2)];
      const [code, key, keyCode] = item;
      const event = new KeyboardEvent(target.has(token) ? 'keydown' : 'keyup', {
        code, key, keyCode, which: keyCode, location: code.startsWith('Numpad') ? 3 : 0, bubbles: true,
      });
      synthetic.add(event);
      if(emulator.config?.lockstep)emulator.module.netKey(event.type,event);
      else (emulator.config?.canvas || canvas).dispatchEvent(event);
    }
    previous = target;
  }
  function release() {
    getNetwork?.()?.clearLocal?.();
    getTouch?.()?.reset();
    keys.clear(); tapped.clear(); pointers.clear(); if(!getNetwork?.()?.isLockstep)fire.forEach(f=>f.reset()); send(new Set());
    const list = pads();
    for (let slot = 0; slot < 2; slot++) padBlocked[slot] = samplePad(gamepadFor(slot, list));
    document.querySelectorAll('.held').forEach(button => button.classList.remove('held'));
  }
  function render() {
    const table = document.createElement('table'); table.className = 'binding-table';
    table.innerHTML = networkMode?'<thead><tr><th>操作</th><th>本机键盘</th><th>本机手柄按键</th></tr></thead>':'<thead><tr><th>操作</th><th>1P</th><th>2P</th></tr></thead>';
    const body = document.createElement('tbody');
    for (const action of networkMode?[...actions,'pause']:actions) {
      const row = document.createElement('tr');
      const label = document.createElement('td'); label.textContent = labels[action]||'暂停'; row.append(label);
      for (const slot of activeSlots) {
        const cell = document.createElement('td'), button = document.createElement('button');
        if(action==='pause'){cell.textContent='Esc';row.append(cell);continue;}
        button.type = 'button'; button.textContent = keyName(bindings[slot][action]);
        button.setAttribute('aria-label', `${slot + 1}P ${labels[action]}键位`);
        button.addEventListener('click', () => {
          release(); pending = {slot, action, button};
          document.querySelectorAll('.listening').forEach(b => b.classList.remove('listening'));
          button.textContent = '按下新键'; button.classList.add('listening');
        });
        cell.append(button); row.append(cell);
      }
      if(networkMode){
        const cell=document.createElement('td'),select=document.createElement('select');
        select.setAttribute('aria-label',`手柄${labels[action]||'暂停'}键位`);
        const names={0:'A / ×',1:'B / ○',2:'X / □',3:'Y / △',4:'LB / L1',5:'RB / R1',6:'LT / L2',7:'RT / R2',8:'Back / Share',9:'Start / Options',10:'L3',11:'R3',12:'十字键上',13:'十字键下',14:'十字键左',15:'十字键右'};
        for(let i=0;i<32;i++)select.add(new Option(names[i]||`按键 ${i+1}`,String(i)));
        select.value=String(padBindings[action]);
        select.onchange=()=>{release();const next=Number(select.value),old=padBindings[action];for(const other of Object.keys(padBindings))if(other!==action&&padBindings[other]===next)padBindings[other]=old;padBindings[action]=next;save();render();};
        cell.append(select);row.append(cell);
      }
      body.append(row);
    }
    table.append(body); document.getElementById('bindings').replaceChildren(table);
    for (let slot = 0; slot < 2; slot++) {
      document.getElementById(`summary-${slot}`).textContent = actions.map(action => keyName(bindings[slot][action])).join(' · ');
    }
  }
  function sampleChangedInput(){
    // Preserve physical edges immediately; the accepted network frame still
    // owns consumption. Offline fire timing stays on its existing clock.
    const network=getNetwork?.();if(!network?.isLockstep)return;
    sampleLocal();network.onLocalInput?.();
  }
  for (const type of ['keydown', 'keyup']) window.addEventListener(type, event => {
    if (synthetic.has(event)) return;
    // Register before SDL: physical keys never leak into the native second seat.
    event.stopImmediatePropagation();
    if (type === 'keyup') { if(keys.delete(event.code))sampleChangedInput(); return; }
    if (pending) {
      event.preventDefault();
      if (event.code === 'Escape') { pending = null; render(); return; }
      if (!/^(Key[A-Z]|Digit[0-9]|Numpad[0-9]|Arrow(Up|Down|Left|Right)|Space|Shift(Left|Right)|Control(Left|Right)|Alt(Left|Right))$/.test(event.code)) return;
      if (activeSlots.some(slot => actions.some(action => bindings[slot][action] === event.code && (slot !== pending.slot || action !== pending.action)))) {
        document.getElementById('binding-status').textContent = '该按键已被另一项操作占用'; return;
      }
      bindings[pending.slot][pending.action] = event.code; pending = null;
      document.getElementById('binding-status').textContent = ''; save(); render(); return;
    }
    if (dialog.open || !getEmulator() || event.metaKey) return;
    if (event.code !== 'Escape') onOtherInput();
    if (isPaused()) {
      event.preventDefault();
      if (event.repeat) return;
      if (event.code === 'Tab') { onMenu(null, event.shiftKey ? 'up' : 'down'); return; }
      if (event.code === 'Enter' || event.code === 'Space') { onMenu(null, 'confirm'); return; }
      if (event.code === 'Escape') { onMenu(0, 'resume'); return; }
      if (event.code === 'Backquote') { onMenu(1, 'resume'); return; }
      for (const slot of activeSlots) for (const action of actions) {
        if (bindings[slot][action] === event.code) onMenu(slot, action === 'shot' ? 'confirm' : action === 'attack' ? 'resume' : action);
      }
      return;
    }
    if (event.target.closest?.('input, select, textarea, [contenteditable="true"]')) return;
    if (event.code === 'Escape') {
      event.preventDefault(); if (!event.repeat) onEscape(); return;
    }
    if (event.code === 'Backquote') {
      event.preventDefault(); if (!getNetwork?.() && !event.repeat && inGameplay()) onPause(1); return;
    }
    if (event.target.closest?.('button')) return;
    if (event.code === 'Enter' || bindings.some((row, slot) => actions.some(action => matches(slot, action, event.code)))) {
      event.preventDefault(); if (!event.repeat) {keys.add(event.code);tapped.add(event.code);sampleChangedInput();}
    }
  }, true);
  window.addEventListener('blur', () => { focused = false; release(); });
  window.addEventListener('focus', () => { focused = true; });
  window.addEventListener('pointerdown', () => { focused = true; }, true);
  document.addEventListener('visibilitychange', () => { if (document.hidden) release(); });
  dialog.addEventListener('close', () => { pending = null; release(); render(); canvas.focus(); });
  document.getElementById('defaults').addEventListener('click', () => {
    release(); pending = null; bindings = structuredClone(defaults); padSlots = ['auto', networkMode?'none':'auto']; padBindings=structuredClone(padDefaults);save(); render(); refreshPads(true);
  });
  for (let slot = 0; slot < 2; slot++) document.getElementById(`pad-${slot}`).addEventListener('change', event => {
    release(); padSlots[slot] = event.target.value; save();
  });

  function pads() { try { return Array.from(navigator.getGamepads?.() || []).filter(p => p?.connected); } catch { return []; } }
  function refreshPads(force = false) {
    const list = pads(), signature = JSON.stringify(list.map(p => [p.index, p.id]));
    if (!force && signature === lastPadInventory) return;
    lastPadInventory = signature;
    for (let slot = 0; slot < 2; slot++) {
      const select = document.getElementById(`pad-${slot}`);
      select.replaceChildren(new Option('自动分配', 'auto'), new Option('关闭', 'none'),
        ...list.map(p => new Option(`${p.index + 1}: ${p.id}`, String(p.index))));
      if (!Array.from(select.options).some(o => o.value === padSlots[slot])) select.add(new Option(`手柄 ${Number(padSlots[slot]) + 1}（未连接）`, padSlots[slot]));
      select.value = padSlots[slot];
    }
  }
  function gamepadFor(slot, list) {
    if (padSlots[slot] === 'none') return null;
    if (padSlots[slot] !== 'auto') return list.find(p => p.index === Number(padSlots[slot]));
    const available = list.filter(p => !activeSlots.some(slot => padSlots[slot] === String(p.index)));
    return available[padSlots[0] === 'auto' ? slot : 0];
  }
  function samplePad(pad) {
    const selected = new Set();
    if (!pad) return selected;
    const pressed = i => !!pad.buttons[i]?.pressed;
    if(networkMode){
      for(const [action,index] of Object.entries(padBindings))if(pressed(index))selected.add(action);
      if(pad.axes[1]<-.35)selected.add('up');if(pad.axes[1]>.35)selected.add('down');
      if(pad.axes[0]<-.35)selected.add('left');if(pad.axes[0]>.35)selected.add('right');
      return selected;
    }
    if (pressed(12) || pad.axes[1] < -.35) selected.add('up');
    if (pressed(13) || pad.axes[1] > .35) selected.add('down');
    if (pressed(14) || pad.axes[0] < -.35) selected.add('left');
    if (pressed(15) || pad.axes[0] > .35) selected.add('right');
    if (pressed(0)) selected.add('shot');
    if (pressed(1)) selected.add('attack');
    if (pressed(2)) selected.add('charge');
    if (pressed(5)) selected.add('focus');
    if (pressed(9)) selected.add('pause');
    return selected;
  }
  for (const area of document.querySelectorAll('.touch-controls')) {
    area.innerHTML = '<div class="dpad"><button data-action="up" aria-label="上">↑</button><button data-action="left" aria-label="左">←</button><button data-action="down" aria-label="下">↓</button><button data-action="right" aria-label="右">→</button></div><div class="actions"><button data-action="shot">连发</button><button data-action="charge">蓄力</button><button data-action="attack">攻击</button><button data-action="focus">低速</button></div>';
    for (const button of area.querySelectorAll('button')) {
      button.type = 'button';
      button.addEventListener('pointerdown', event => {
        event.preventDefault(); onGesture();
        if (getEmulator()?.state !== 'running' || dialog.open) return;
        canvas.focus(); button.setPointerCapture(event.pointerId); button.classList.add('held');
        pointers.set(event.pointerId, {slot: Number(area.dataset.slot), action: button.dataset.action, button});
      });
      const finish = event => {
        const held = pointers.get(event.pointerId); pointers.delete(event.pointerId);
        if (held && ![...pointers.values()].some(p => p.button === held.button)) held.button.classList.remove('held');
      };
      for (const type of ['pointerup', 'pointercancel', 'lostpointercapture']) button.addEventListener(type, finish);
    }
  }
  function sampleLocal() {
    const network = getNetwork?.();
    const target = new Set();
    const cancelled = [];
    const touch = getTouch?.();
    let packed = 0;
    if (focused && !document.hidden && !dialog.open && !touch?.isEditing() && getEmulator()) {
      const list = pads();
      for (let slot = 0; slot < 2; slot++) {
        if (network && slot === 1) continue;
        const raw = samplePad(gamepadFor(slot, list));
        const edge = new Set([...raw].filter(action => !padPrevious[slot].has(action) && !padBlocked[slot].has(action)));
        if (edge.size) onOtherInput();
        padPrevious[slot] = raw;
        for (const action of padBlocked[slot]) if (!raw.has(action)) padBlocked[slot].delete(action);
        if (edge.has('pause')) {
          if (isPaused()) onMenu(slot, 'resume'); else onPause(slot);
        } else if (isPaused()) {
          for (const action of edge) onMenu(slot, action === 'shot' ? 'confirm' : action === 'attack' ? 'resume' : action);
        }
        if (isPaused() || getEmulator()?.state !== 'running') continue;
        const selected = new Set(actions.filter(action => [...keys,...tapped].some(code => matches(slot, action, code))));
        for (const held of pointers.values()) if (held.slot === slot) selected.add(held.action);
        for (const action of raw) if (actions.includes(action) && !padBlocked[slot].has(action)) selected.add(action);
        for (const [a, b] of [['up', 'down'], ['left', 'right']]) if (selected.has(a) && selected.has(b)) { selected.delete(a); selected.delete(b); cancelled.push(a,b); }
        for (const action of selected) target.add(`${slot}:${action}`);
      }
      if (!isPaused() && getEmulator()?.state === 'running') {
        if (keys.has('Enter')) target.add('confirm');
        const bits = touch?.sample() || 0;
        for (const [action, bit] of [['up',1],['down',2],['left',4],['right',8],['attack',16],['charge',32],['shot',512],['focus',64]]) {
          if (bits & bit) target.add(`0:${action}`);
        }
        if (bits & 256) target.add(network ? '0:charge' : 'confirm');
        if (bits & 128) onEscape();
        packed = touch?.pack(0) || 0;
        touch?.consume();
      }
    }
    if (isPaused() || getEmulator()?.state !== 'running') target.clear();
    tapped.clear();
    for(let slot=0;slot<2;slot++){
      const rapid=target.delete(`${slot}:shot`),charge=target.delete(`${slot}:charge`);
      if(network?.isLockstep){
        if(rapid)target.add(`${slot}:rapid`);
        if(charge)target.add(`${slot}:shot`);
      }else if(fire[slot].sample(rapid,charge,performance.now()))target.add(`${slot}:shot`);
    }
    if(isPaused()||getEmulator()?.state!=='running'){target.clear();if(!network?.isLockstep)fire.forEach(f=>f.reset());}
    if(!focused||document.hidden||dialog.open||touch?.isEditing()||isPaused()||getEmulator()?.state!=='running')network?.clearLocal?.();
    send(target, packed, cancelled);
  }
  function poll(){refreshPads();sampleLocal();requestAnimationFrame(poll);}
  render(); refreshPads(true); requestAnimationFrame(poll);
  function applyFrame(inputs){
    const target=new Set(),touches=inputs.map(input=>limitTouch(input.touch,getRoomUnlimited()));let mask=0,points=0;
    inputs.forEach((input,slot)=>{
      for(const action of input.actions)if(action!=='rapid'&&action!=='shot')target.add(`${slot}:${action}`);
      if(fire[slot].sample(input.actions.includes('rapid'),input.actions.includes('shot'),fireFrame*1000/60))target.add(`${slot}:shot`);
      if(getRoomFocus()&&input.actions.includes('focus'))mask|=1<<slot;
    });
    fireFrame++;
    onFocus(mask,points);onTouch?.(touches);
    const emulator=getEmulator();if(!emulator)return;
    for(const token of new Set([...previous,...target])){
      if(token.endsWith(':focus')||previous.has(token)===target.has(token))continue;
      const [code,key,keyCode]=native[Number(token[0])][token.slice(2)];
      emulator.module.netKey(target.has(token)?'keydown':'keyup',{code,key,keyCode,which:keyCode,charCode:0,
        location:code.startsWith('Numpad')?3:0,timeStamp:emulator.module.netInfo().now,
        shiftKey:false,ctrlKey:false,altKey:false,metaKey:false,repeat:false,preventDefault(){}});
    }
    previous=target;
  }
  return {release,applyFrame,sampleLocal,pointEnabled:slot=>focusSettings[slot].points,
    capture:()=>({previous:[...previous],fireFrame,fire:fire.map(f=>({...f.state}))}),restore:saved=>{previous=new Set(saved?.previous||[]);fireFrame=saved?.fireFrame||0;fire.forEach((f,i)=>{if(saved?.fire?.[i])f.state={...saved.fire[i]};else f.reset();});}};
}
