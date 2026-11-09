import {useEffect, useRef, useState} from 'react';
import {Dialog, DialogContent, DialogTitle} from '../../ui/dialog';
import {Button} from '../../ui/button';
import {useLocale} from '../../i18n';
import './custom-background.css';

const event = 'eagler-background-change';
const settingsKey = 'eagler.background.settings';
const defaults = {shade: 45, panel: 88};
const text = {
  'zh-CN': {title:'背景图片', choose:'选择图片', change:'更换图片', remove:'移除图片', shade:'背景遮罩', panel:'面板不透明度', note:'图片只保存在此浏览器中。', error:'无法保存图片，请选择不超过 10 MB 的有效图片。'},
  en: {title:'Background image', choose:'Choose image', change:'Change image', remove:'Remove image', shade:'Background overlay', panel:'Panel opacity', note:'The image is stored only in this browser.', error:'Could not save the image. Choose a valid image up to 10 MB.'},
  ja: {title:'背景画像', choose:'画像を選択', change:'画像を変更', remove:'画像を削除', shade:'背景の覆い', panel:'パネルの不透明度', note:'画像はこのブラウザーにのみ保存されます。', error:'画像を保存できません。10 MB 以下の画像を選択してください。'},
};
function settings() {
  try {const value=JSON.parse(localStorage.getItem(settingsKey) || '{}'); return {shade: Math.max(25,Math.min(85,Number.isFinite(value.shade)?value.shade:defaults.shade)), panel: Math.max(70,Math.min(100,Number.isFinite(value.panel)?value.panel:defaults.panel))};} catch {return defaults;}
}
function db(): Promise<IDBDatabase> {
  return new Promise((resolve,reject)=>{const request=indexedDB.open('eagler-local-background',1); request.onupgradeneeded=()=>request.result.createObjectStore('images'); request.onsuccess=()=>resolve(request.result); request.onerror=()=>reject(request.error);});
}
async function imageStore(mode: IDBTransactionMode, value?: Blob | null): Promise<Blob | undefined> {
  const database=await db();
  try {return await new Promise((resolve,reject)=>{const tx=database.transaction('images',mode), store=tx.objectStore('images'); let result:Blob|undefined; const request=mode==='readonly'?store.get('background'):value ? store.put(value,'background') : store.delete('background'); request.onsuccess=()=>{if(mode==='readonly') result=request.result as Blob|undefined;}; tx.oncomplete=()=>resolve(result); tx.onerror=()=>reject(tx.error); tx.onabort=()=>reject(tx.error);});} finally {database.close();}
}
function announce() {window.dispatchEvent(new Event(event));}

export function BackgroundController() {
  useEffect(()=>{let url='',serial=0; const update=async()=>{const current=++serial, values=settings(), root=document.documentElement; root.style.setProperty('--custom-background-shade',`${values.shade}%`); root.style.setProperty('--custom-panel-opacity',`${values.panel}%`); try {const blob=await imageStore('readonly'); if(current!==serial)return; const next=blob?URL.createObjectURL(blob):''; root.style.setProperty('--custom-background-image', next?`url("${next}")`:'none'); root.toggleAttribute('data-custom-background',!!next); if(url)URL.revokeObjectURL(url); url=next;} catch {root.removeAttribute('data-custom-background');}}; void update(); window.addEventListener(event,update); window.addEventListener('storage',update); return ()=>{serial++;window.removeEventListener(event,update);window.removeEventListener('storage',update);if(url)URL.revokeObjectURL(url);};},[]);
  return null;
}
export function BackgroundSettings({open,onOpenChange}:{open:boolean;onOpenChange(open:boolean):void}) {
  const {locale}=useLocale(), labels=text[locale] ?? text.en, input=useRef<HTMLInputElement>(null);
  const [values,setValues]=useState(defaults),[hasImage,setHasImage]=useState(false),[error,setError]=useState(false);
  useEffect(()=>{if(open){setValues(settings()); void imageStore('readonly').then(blob=>setHasImage(!!blob)).catch(()=>setError(true));}},[open]);
  function setValue(key:'shade'|'panel',value:number){const next={...values,[key]:value}; setValues(next); try{localStorage.setItem(settingsKey,JSON.stringify(next));announce();}catch{setError(true);}}
  async function choose(file:File|undefined){if(!file)return; try{if(!file.type.startsWith('image/')||file.size>10*1024*1024)throw Error(); const bitmap=await createImageBitmap(file);bitmap.close();await imageStore('readwrite',file);setHasImage(true);setError(false);announce();}catch{setError(true);} finally {if(input.current)input.current.value='';}}
  async function remove(){try{await imageStore('readwrite',null);setHasImage(false);setError(false);announce();}catch{setError(true);}}
  return <Dialog open={open} onOpenChange={onOpenChange}><DialogContent className="custom-background-dialog" aria-describedby={undefined}><DialogTitle>{labels.title}</DialogTitle><p>{labels.note}</p><div className="custom-background-actions"><input ref={input} type="file" accept="image/*" hidden onChange={e=>void choose(e.currentTarget.files?.[0])}/><Button type="button" onClick={()=>input.current?.click()}>{hasImage?labels.change:labels.choose}</Button>{hasImage&&<Button type="button" variant="secondary" onClick={()=>void remove()}>{labels.remove}</Button>}</div><label>{labels.shade}<output>{values.shade}%</output><input type="range" min="25" max="85" value={values.shade} onChange={e=>setValue('shade',Number(e.currentTarget.value))}/></label><label>{labels.panel}<output>{values.panel}%</output><input type="range" min="70" max="100" value={values.panel} onChange={e=>setValue('panel',Number(e.currentTarget.value))}/></label>{error&&<p role="alert">{labels.error}</p>}</DialogContent></Dialog>;
}
export function backgroundLabel(locale:string){return (text[locale as keyof typeof text] ?? text.en).title;}
