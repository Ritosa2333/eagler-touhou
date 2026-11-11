import {FireControl} from '/fire-control.js';
import {createDirectDrag} from '/direct-drag.js';
const canvas = document.querySelector('#game');
const context = canvas.getContext('2d', { alpha: false });
const status = document.querySelector('#status');
const image = context.createImageData(640, 400);
const keys = new Set();
const mapping = {ArrowUp:1,ArrowDown:2,ArrowLeft:4,ArrowRight:8,KeyX:16};
const fire = new FireControl();
const options = Object.fromEntries(['rapid','focus','point','touch','unlimited'].map(name => [name, document.getElementById(name)]));
for (const [name, element] of Object.entries(options)) {
  try { const value = localStorage.getItem('th03-assist-' + name); if (value !== null) element.checked = value === 'true'; } catch {}
  element.addEventListener('change', () => { try { localStorage.setItem('th03-assist-' + name, String(element.checked)); } catch {} keys.clear(); fire.reset(); canvas.focus(); });
}
let module, audio, audioTime = 0, last = 0, accumulator = 0, running = false, lastSave = 0, inputFrame = 0;
const touchKeys=new Set();let touchPulse=0;
const touchControls=document.getElementById('touch-controls');
function touchVisibility(){touchControls.hidden=!options.touch.checked;touchControls.style.display=options.touch.checked?'flex':'none';if(!options.touch.checked){touchKeys.clear();touchPulse=0;drag.reset();}}
const drag=createDirectDrag(canvas,{allowed:()=>running&&options.touch.checked,gameplay:()=>module?._th03_classic_scene()===9,geometry:()=>{const r=canvas.getBoundingClientRect();return {width:r.width,height:r.height,scale:r.width/640};},gesture:unlockAudio,pulse:bits=>{touchPulse|=bits;}});
for(const button of touchControls.querySelectorAll('button')){
 const action=button.dataset.touch;
 button.addEventListener('pointerdown',event=>{event.preventDefault();unlockAudio();button.setPointerCapture(event.pointerId);if(action==='bomb')touchPulse|=16;else if(action==='pause')touchPulse|=128;else touchKeys.add(action);});
 for(const name of ['pointerup','pointercancel','lostpointercapture'])button.addEventListener(name,()=>touchKeys.delete(action));
}
options.touch.addEventListener('change',touchVisibility);touchVisibility();
const interval = 1000 * 9328 / 526315;

function unlockAudio() {
  if (!audio) audio = new AudioContext({ sampleRate: 48000 });
  if (audio.state === 'suspended') audio.resume();
}
addEventListener('keydown', event => {
  if (event.target instanceof HTMLInputElement || event.target instanceof HTMLElement && event.target.closest('details')) return;
  if (event.code in mapping || ['KeyZ','Enter','Escape','Space','ShiftLeft','ShiftRight'].includes(event.code)) event.preventDefault();
  keys.add(event.code); unlockAudio();
});
addEventListener('keyup', event => keys.delete(event.code));
addEventListener('blur', () => { keys.clear(); touchKeys.clear(); touchPulse=0; drag.reset(); fire.reset(); });
canvas.addEventListener('pointerdown', () => { canvas.focus(); unlockAudio(); });
document.addEventListener('visibilitychange', () => { last = 0; accumulator = 0; if (document.hidden) { keys.clear(); touchKeys.clear(); touchPulse=0; drag.reset(); fire.reset(); } });

const saveNames = ['YUME.CFG','YUME.NEM','PODD.CFG','YUME.NEW.NEM','TH3KEY.CFG'];
function save() {
  for (const name of saveNames) {
    try { localStorage.setItem('th03-native-' + name, JSON.stringify(Array.from(module.FS.readFile('/user/' + name)))); } catch {}
  }
}
function playAudio() {
  const count = module._th03_classic_audio_frames();
  if (!audio || audio.state !== 'running' || !count) return;
  const ptr = module._th03_classic_audio() >>> 2;
  const pcm = module.HEAP32.subarray(ptr, ptr + count * 2);
  const buffer = audio.createBuffer(2, count, 48000);
  const left = buffer.getChannelData(0), right = buffer.getChannelData(1);
  for (let i=0; i<count; i++) { left[i] = pcm[i*2] / 2147483648; right[i] = pcm[i*2+1] / 2147483648; }
  if (audioTime < audio.currentTime || audioTime > audio.currentTime + 0.3) audioTime = audio.currentTime + 0.04;
  const source = audio.createBufferSource(); source.buffer = buffer; source.connect(audio.destination); source.start(audioTime);
  audioTime += count / 48000;
}
function frame(now) {
  if (!running) return;
  if (!last) last = now;
  accumulator += Math.min(now - last, interval * 3); last = now;
  try {
    while (accumulator >= interval) {
      let bits = touchPulse&16; for (const key of keys) bits |= mapping[key] || 0;
      const charge = keys.has('ShiftLeft') || keys.has('ShiftRight') || touchKeys.has('charge');
      if (fire.sample(keys.has('KeyZ') || touchKeys.has('fire'), charge, inputFrame++ * 1000 / 60, options.rapid.checked)) bits |= 32;
      const focused = options.focus.checked && (keys.has('Space') || touchKeys.has('focus') || drag.focused);
      module._th03_classic_assist(focused ? 1 : 0);
      const motion=drag.sample();module._th03_classic_touch(0,motion.active,drag.beginning,options.unlimited.checked,motion.x,motion.y);drag.consume();
      let system = bits;
      system |= drag.menuBits;
      if((touchPulse&16) && module._th03_classic_scene()!==9)system|=0x1000;
      if(touchPulse&256)system|=0x2000;
      if(touchPulse&128)system|=0x1000;
      touchPulse=0;
      if (keys.has('Enter')) system |= 0x2000;
      if (keys.has('Escape')) system |= 0x1000;
      const result = module._th03_classic_step(bits, system, charge ? 1 : 0);
      if (result !== 0) throw Error('游戏已结束或运行失败：' + result);
      playAudio(); accumulator -= interval;
    }
    const pointer = module._th03_classic_pixels();
    if (!pointer) throw Error('画面生成失败');
    image.data.set(module.HEAPU8.subarray(pointer, pointer + image.data.length)); context.putImageData(image, 0, 0);
    if (options.focus.checked && options.point.checked && (keys.has('Space')||touchKeys.has('focus')||drag.focused)) {
      const x = module._th03_classic_point_x(), y = module._th03_classic_point_y();
      if (x >= 0 && y >= 0) { context.fillStyle = '#000'; context.fillRect(x, y, 4, 4); context.fillStyle = '#fff'; context.fillRect(x + 1, y + 2, 2, 2); }
    }
    if (now - lastSave > 5000) { save(); lastSave = now; }
    requestAnimationFrame(frame);
  } catch (error) { running = false; save(); status.textContent = error.message; }
}
addEventListener('pagehide', save);

try {
  const [files, created] = await Promise.all([fetch('/content-list').then(r => { if (!r.ok) throw Error('资源清单不可用'); return r.json(); }), createTh03Classic()]);
  module = created; module.FS.mkdir('/content'); module.FS.mkdir('/user'); module.FS.mkdir('/user/REPLAY');
  await Promise.all(files.map(async path => {
    const response = await fetch('/content/' + path.split('/').map(encodeURIComponent).join('/'));
    if (!response.ok) throw Error('无法加载资源：' + path);
    return [path, new Uint8Array(await response.arrayBuffer())];
  })).then(entries => { for (const [path, data] of entries) {
    const parts = path.split('/'); let dir = '/content';
    for (const part of parts.slice(0,-1)) { dir += '/' + part; if (!module.FS.analyzePath(dir).exists) module.FS.mkdir(dir); }
    module.FS.writeFile('/content/' + path, data);
  }});
  for (const name of saveNames) {
    try { const data = localStorage.getItem('th03-native-' + name); if (data) module.FS.writeFile('/user/' + name, Uint8Array.from(JSON.parse(data))); } catch {}
  }
  const result = module._th03_classic_create();
  if (result !== 1) throw Error('游戏启动失败：' + result);
  status.textContent = ''; canvas.focus(); running = true; requestAnimationFrame(frame);
} catch (error) { status.textContent = error.message; console.error(error); }
