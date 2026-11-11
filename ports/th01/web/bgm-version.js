// Local presentation preference only; never included in game or room settings.
const storageKey='th01.bgmVersion';
const game='th01';
export const bgmVersions=[['old','原版 FM（自动）'],
  ...(['th04','th05'].includes(game)?[['fm26','FM 26 音源'],['fm86','FM 86 音源']]:[]),
  ...(game==='th02'?[['midi','MIDI（GM 音源）']]:[]),['new','幺乐团专辑版']];
const normalize=next=>bgmVersions.some(([id])=>id===next)?next:'old';
let value='old';
try{value=normalize(localStorage.getItem(storageKey));}catch{}
const listeners=new Set(),controls=new Set();
let needsRestart=false;
export const bgmVersion=()=>value;
export function resolveBgmTrack(id,tracks,album={},midi={}){
  if(value==='new')return album[id]??tracks[id];
  if(value==='midi')return midi[id]??tracks[id];
  if(value==='fm26'||value==='fm86'){
    const base=id.replace(/-m(?:26|86)$/,'');
    return tracks[`${base}-${value==='fm26'?'m26':'m86'}`]??tracks[id];
  }
  return tracks[id];
}
export async function loadMidiTracks(signal){
  if(game!=='th02')return {};
  const response=await fetch(new URL('./bgm/midi/manifest.json',import.meta.url),{signal});
  if(!response.ok)throw Error('MIDI 音乐清单读取失败');
  return (await response.json()).tracks;
}
export function onBgmVersion(callback){listeners.add(callback);return()=>listeners.delete(callback);}
function updateControls(){
  const mode=document.getElementById('audio-mode');
  if(value!=='old'&&mode&&!mode.disabled&&mode.value!=='independent'){
    needsRestart=!!document.querySelector('#restart:not(:disabled)');
    mode.value='independent';mode.dispatchEvent(new Event('change'));
  }
  for(const select of controls){
    select.value=value;
    select.title=mode&&mode.value!=='independent'?'切换音乐音源需要独立 BGM；请重新启动游戏':'仅影响本机，自动保存';
    if(select.nextElementSibling)select.nextElementSibling.textContent=value!=='old'&&mode&&(needsRestart||mode.value!=='independent')?' 需使用独立 BGM 并重新启动':'';
  }
}
export function setBgmVersion(next){
  next=normalize(next);
  if(next===value){updateControls();return;}
  value=next;try{localStorage.setItem(storageKey,value);}catch{}
  updateControls();for(const callback of listeners)callback(value);
}
window.addEventListener('storage',event=>{
  if(event.key===storageKey||event.key===null){
    let next='old';try{next=normalize(localStorage.getItem(storageKey));}catch{}
    if(next!==value){value=next;updateControls();for(const callback of listeners)callback(value);}
  }
});
function mount(anchor){
  if(!anchor.matches('.player-toolbar')&&anchor.closest('.player-toolbar'))return;
  if('bgmVersionMounted' in anchor.dataset)return;
  anchor.dataset.bgmVersionMounted='';
  const label=document.createElement('label');label.className='player-volume bgm-version';label.textContent='本机音乐 ';
  const select=document.createElement('select');select.setAttribute('aria-label','本机音乐版本');
  for(const [id,text]of bgmVersions)select.add(new Option(text,id));
  select.style.cssText='width:auto;min-width:110px;font:inherit';select.value=value;
  select.onchange=()=>setBgmVersion(select.value);label.append(select,document.createElement('small'));controls.add(select);
  if(anchor.matches('.player-toolbar'))anchor.append(label);
  else (anchor.closest('label')||anchor).after(label);
  updateControls();
}
const targets='.player-toolbar,#music,#music-volume,#bgm-volume,#audio-mode';
function scan(root){
  const explicit=[...(root.matches?.('[data-bgm-version-control]')?[root]:[]),...(root.querySelectorAll?.('[data-bgm-version-control]')||[])];
  for(const select of explicit)if(!controls.has(select)){
    select.replaceChildren(...bgmVersions.map(([id,text])=>new Option(text,id)));
    select.value=value;select.onchange=()=>setBgmVersion(select.value);controls.add(select);updateControls();
  }
  // Pages with a permanent control need no generated toolbar copy.
  if(document.querySelector('[data-bgm-version-control]'))return;
  if(root.matches?.(targets))mount(root);for(const anchor of root.querySelectorAll?.(targets)||[])mount(anchor);
}
function start(){
  scan(document);
  document.getElementById('restart')?.addEventListener('click',()=>{needsRestart=false;updateControls();});
  new MutationObserver(records=>{for(const record of records)for(const node of record.addedNodes)if(node.nodeType===1&&!node.matches('.bgm-version'))scan(node);})
    .observe(document.body,{childList:true,subtree:true});
}
if(document.readyState==='loading')document.addEventListener('DOMContentLoaded',start,{once:true});else start();
