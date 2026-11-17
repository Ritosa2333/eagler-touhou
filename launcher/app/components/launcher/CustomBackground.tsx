import {useEffect, useRef, useState, type PointerEvent} from 'react';
import {Dialog, DialogContent, DialogTitle} from '../../ui/dialog';
import {Button} from '../../ui/button';
import {ChoiceGroup, ChoiceItem} from '../../ui/choice-group';
import {Sun, Moon} from 'lucide-react';
import {useLocale} from '../../i18n';
import './custom-background.css';

const settingsEvent = 'eagler-background-settings-change';
const imageEvent = 'eagler-background-image-change';
const settingsKey = 'eagler.background.settings';
const imageRevisionKey = 'eagler.background.image-revision';
const backgroundModes = ['plain','image','dots','grid','seigaiha'] as const;
type BackgroundMode = typeof backgroundModes[number];
const backgroundSurfaces = ['navigation', 'category', 'card', 'panel'] as const;
export type BackgroundSurface = typeof backgroundSurfaces[number];
const defaults = {mode:'image' as BackgroundMode, shade: 45, panel: 88, navigation: 88, category: 88, card: 88, fit: 'cover' as 'cover'|'contain'};
const text = {
  'zh-CN': {title:'主题设置', base:'主题基底', light:'亮色', dark:'暗色', background:'背景', presets:'内置背景', plain:'空白', image:'自定义图片', dots:'星点', grid:'方格', seigaiha:'青海波', choose:'选择图片', change:'更换图片', remove:'移除图片', shade:'背景遮罩', panel:'内容面板不透明度', navigation:'顶部栏不透明度', category:'分类条不透明度', card:'作品卡片不透明度', fit:'图片铺放', cover:'铺满', contain:'完整显示', note:'图片只保存在此浏览器中。', error:'无法保存图片，请选择不超过 10 MB 的有效图片。'},
  en: {title:'Theme settings', base:'Base theme', light:'Light', dark:'Dark', background:'Background', presets:'Built-in backgrounds', plain:'Plain', image:'Custom image', dots:'Dots', grid:'Grid', seigaiha:'Seigaiha', choose:'Choose image', change:'Change image', remove:'Remove image', shade:'Background overlay', panel:'Content panel opacity', navigation:'Navigation opacity', category:'Category opacity', card:'Game card opacity', fit:'Image fit', cover:'Cover', contain:'Contain', note:'The image is stored only in this browser.', error:'Could not save the image. Choose a valid image up to 10 MB.'},
  ja: {title:'テーマ設定', base:'基本テーマ', light:'ライト', dark:'ダーク', background:'背景', presets:'背景のプリセット', plain:'なし', image:'カスタム画像', dots:'星点', grid:'格子', seigaiha:'青海波', choose:'画像を選択', change:'画像を変更', remove:'画像を削除', shade:'背景の覆い', panel:'コンテンツの不透明度', navigation:'ナビの不透明度', category:'カテゴリーの不透明度', card:'作品カードの不透明度', fit:'画像の表示', cover:'画面を埋める', contain:'画像全体', note:'画像はこのブラウザーにのみ保存されます。', error:'画像を保存できません。10 MB 以下の画像を選択してください。'},
};
function settings(): typeof defaults {
  try {
    const value=JSON.parse(localStorage.getItem(settingsKey) || '{}');
    const number=(key:'shade'|'panel'|'navigation'|'category'|'card') => Math.max(0,Math.min(100,Number.isFinite(value[key])?value[key]:defaults[key]));
    return {mode:backgroundModes.includes(value.mode)?value.mode:value.mode==='waves'?'plain':'image',shade:number('shade'),panel:number('panel'),navigation:number('navigation'),category:number('category'),card:number('card'),fit:value.fit==='contain'?'contain':'cover'};
  } catch {return defaults;}
}
function db(): Promise<IDBDatabase> {
  return new Promise((resolve,reject)=>{const request=indexedDB.open('eagler-local-background',1); request.onupgradeneeded=()=>request.result.createObjectStore('images'); request.onsuccess=()=>resolve(request.result); request.onerror=()=>reject(request.error);});
}
async function imageStore(mode: IDBTransactionMode, value?: Blob | null): Promise<Blob | undefined> {
  const database=await db();
  try {return await new Promise((resolve,reject)=>{const tx=database.transaction('images',mode), store=tx.objectStore('images'); let result:Blob|undefined; const request=mode==='readonly'?store.get('background'):value ? store.put(value,'background') : store.delete('background'); request.onsuccess=()=>{if(mode==='readonly') result=request.result as Blob|undefined;}; tx.oncomplete=()=>resolve(result); tx.onerror=()=>reject(tx.error); tx.onabort=()=>reject(tx.error);});} finally {database.close();}
}
function announceImage() {
  window.dispatchEvent(new Event(imageEvent));
  try {localStorage.setItem(imageRevisionKey, crypto.randomUUID());} catch {}
}

export function BackgroundController() {
  useEffect(()=>{
    let url='',serial=0;
    const applyMode=()=>{const mode=settings().mode; root.dataset.backgroundMode=mode; root.toggleAttribute('data-custom-background',mode==='image'?!!url:mode!=='plain');};
    const root=document.documentElement;
    const updateSettings=()=>{
      const values=settings();
      root.style.setProperty('--custom-background-shade',`${values.shade}%`);
      for(const key of backgroundSurfaces) root.style.setProperty(`--custom-${key}-opacity`,`${values[key]}%`);
      root.style.setProperty('--custom-background-fit',values.fit);
      applyMode();
    };
    const updateImage=async()=>{
      const current=++serial;
      try {
        const blob=await imageStore('readonly');
        if(current!==serial)return;
        const next=blob?URL.createObjectURL(blob):'';
        root.style.setProperty('--custom-background-image',next?`url("${next}")`:'none');
        if(url)URL.revokeObjectURL(url);
        url=next;
        applyMode();
      } catch {if(current===serial)applyMode();}
    };
    const storage=(change:StorageEvent)=>{
      if(change.key===settingsKey||change.key===null)updateSettings();
      if(change.key===imageRevisionKey||change.key===null)void updateImage();
    };
    updateSettings(); void updateImage();
    window.addEventListener(settingsEvent,updateSettings);
    window.addEventListener(imageEvent,updateImage);
    window.addEventListener('storage',storage);
    return ()=>{serial++;window.removeEventListener(settingsEvent,updateSettings);window.removeEventListener(imageEvent,updateImage);window.removeEventListener('storage',storage);if(url)URL.revokeObjectURL(url);};
  },[]);
  return null;
}
export function BackgroundSettings({open,onOpenChange,dark,onThemeChange,surfaces=backgroundSurfaces}: {
  open:boolean;onOpenChange(open:boolean):void;dark:boolean;onThemeChange(value:'light'|'dark'):void;
  surfaces?:readonly BackgroundSurface[];
}) {
  const {locale}=useLocale(), labels=text[locale] ?? text.en, input=useRef<HTMLInputElement>(null);
  const dialog=useRef<HTMLDivElement>(null);
  const drag=useRef<{pointerId:number; x:number; y:number; left:number; top:number; offsetX:number; offsetY:number} | null>(null);
  const [offset,setOffset]=useState({x:0,y:0});
  const [values,setValues]=useState(defaults),[hasImage,setHasImage]=useState(false),[error,setError]=useState(false);
  useEffect(()=>{if(open){setOffset({x:0,y:0}); setValues(settings()); void imageStore('readonly').then(blob=>setHasImage(!!blob)).catch(()=>setError(true));}},[open]);
  function startDrag(event:PointerEvent<HTMLDivElement>){
    if(event.button!==0 || !dialog.current)return;
    const rect=dialog.current.getBoundingClientRect();
    drag.current={pointerId:event.pointerId,x:event.clientX,y:event.clientY,left:rect.left,top:rect.top,offsetX:offset.x,offsetY:offset.y};
    event.currentTarget.setPointerCapture(event.pointerId);
  }
  function moveDrag(event:PointerEvent<HTMLDivElement>){
    const start=drag.current, panel=dialog.current;
    if(!start || start.pointerId!==event.pointerId || !panel)return;
    const rect=panel.getBoundingClientRect();
    const x=Math.max(0,Math.min(window.innerWidth-rect.width,start.left+event.clientX-start.x));
    const y=Math.max(0,Math.min(window.innerHeight-rect.height,start.top+event.clientY-start.y));
    setOffset({x:start.offsetX+x-start.left,y:start.offsetY+y-start.top});
  }
  function endDrag(event:PointerEvent<HTMLDivElement>){if(drag.current?.pointerId===event.pointerId)drag.current=null;}
  function setValue(key:keyof typeof defaults,value:number|'cover'|'contain'|BackgroundMode){const next={...values,[key]:value}; setValues(next); try{localStorage.setItem(settingsKey,JSON.stringify(next));window.dispatchEvent(new Event(settingsEvent));}catch{setError(true);}}
  async function choose(file:File|undefined){if(!file)return; try{if(!file.type.startsWith('image/')||file.size>10*1024*1024)throw Error(); const bitmap=await createImageBitmap(file);bitmap.close();await imageStore('readwrite',file);setHasImage(true);setError(false);setValue('mode','image');announceImage();}catch{setError(true);} finally {if(input.current)input.current.value='';}}
  async function remove(){try{await imageStore('readwrite',null);setHasImage(false);setError(false);announceImage();}catch{setError(true);}}
  const mode = values.mode === 'image' && !hasImage ? 'plain' : values.mode;
  const opacitySettings = ['shade', ...surfaces] as const;
  const theme = dark ? 'dark' : 'light';
  return <Dialog open={open} onOpenChange={onOpenChange}>
    <DialogContent ref={dialog} className="custom-background-dialog" overlayClassName="custom-background-overlay" aria-describedby={undefined} style={{translate:`${offset.x}px ${offset.y}px`}}>
      <div className="custom-background-drag-handle" onPointerDown={startDrag} onPointerMove={moveDrag} onPointerUp={endDrag} onPointerCancel={endDrag} onLostPointerCapture={endDrag}>
        <DialogTitle>{labels.title}</DialogTitle>
      </div>
      <div className="custom-theme-base">
        <span id="custom-theme-label">{labels.base}</span>
        <ChoiceGroup type="single" aria-labelledby="custom-theme-label" className="custom-theme-switch" data-theme={theme} value={theme}
          onValueChange={value=>{if(value==='dark'||value==='light')onThemeChange(value);}}>
          <ChoiceItem value="light" data-theme-value="light"><Sun aria-hidden="true"/>{labels.light}</ChoiceItem>
          <ChoiceItem value="dark" data-theme-value="dark"><Moon aria-hidden="true"/>{labels.dark}</ChoiceItem>
        </ChoiceGroup>
      </div>
      <h3>{labels.background}</h3>
      <fieldset className="background-presets" aria-label={labels.presets}>
        {backgroundModes.filter(preset=>preset!=='image'||hasImage).map(preset=><button type="button" key={preset} className="background-preset"
          data-preset={preset} aria-pressed={mode===preset} onClick={()=>setValue('mode',preset)}>
          <span className="background-preset-preview" aria-hidden="true"/><span>{labels[preset]}</span>
        </button>)}
      </fieldset>
      <p>{labels.note}</p>
      <div className="custom-background-actions">
        <input ref={input} type="file" accept="image/*" hidden onChange={e=>void choose(e.currentTarget.files?.[0])}/>
        <Button type="button" onClick={()=>input.current?.click()}>{hasImage?labels.change:labels.choose}</Button>
        {hasImage&&<Button type="button" variant="secondary" onClick={()=>void remove()}>{labels.remove}</Button>}
      </div>
      {mode==='image'&&<label>{labels.fit}
        <select name="background-fit" value={values.fit} onChange={e=>setValue('fit',e.currentTarget.value as 'cover'|'contain')}>
          <option value="cover">{labels.cover}</option><option value="contain">{labels.contain}</option>
        </select>
      </label>}
      {mode!=='plain'&&opacitySettings.map(key=><label key={key}>{labels[key]}<output>{values[key]}%</output>
        <input name={key} type="range" min="0" max="100" value={values[key]} onChange={e=>setValue(key,Number(e.currentTarget.value))}/>
      </label>)}
      {error&&<p role="alert">{labels.error}</p>}
    </DialogContent>
  </Dialog>;
}
