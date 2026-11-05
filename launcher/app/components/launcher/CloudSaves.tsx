import {useEffect, useRef, useState} from 'react';
import {CloudUpload, Download, LoaderCircle, RefreshCw, Trash2} from 'lucide-react';
import {PRODUCT_GAMES, isProductId} from '../../../src/contracts/product-catalog.mts';
import {Button} from '../../ui/button';
import {Select, SelectContent, SelectItem, SelectTrigger, SelectValue} from '../../ui/select';
import {ConfirmationDialog} from '../ConfirmationDialog';
import {useSurfaceLayerState} from '../../navigation/surface-layer';
import {useUnsavedChanges} from '../../navigation/unsaved-changes';
import {useLocale, type UiMessageKey} from '../../i18n';
import {AccountRequestError} from '../../services/account-client';
import type {LocalSaveReader} from '../../services/local-save-storage';
import {cloudSaveFileName, deleteCloudSave, downloadCloudSave, formatPlaytime, listCloudSaves, uploadCloudSave, type CloudSaveList, type CloudSaveSnapshot} from '../../services/cloud-saves';
import './cloud-saves.css';

const errorKey=(error:unknown):UiMessageKey=>{
  const code=error instanceof AccountRequestError?error.code:'';
  return code==='SAVE_CONFLICT'?'cloudSave.conflict':code==='SAVE_BUSY'?'cloudSave.busy':code==='SAVE_QUOTA_EXCEEDED'?'cloudSave.quota':code==='SAVE_TOO_LARGE'||code==='SAVE_PARSE_LIMIT'?'cloudSave.tooLarge':code==='INVALID_SAVE'||code==='WRONG_SAVE_GAME'||code==='INVALID_SAVE_FILE'||code==='MAIN_SAVE_REQUIRED'?'cloudSave.invalid':code==='UNSUPPORTED_SAVE_VERSION'?'cloudSave.unsupportedVersion':'account.requestFailed';
};
export function CloudSaves({readLocalSaves}:{readLocalSaves?:LocalSaveReader}) {
  const {locale,t}=useLocale();
  const [data,setData]=useState<CloudSaveList>(),[product,setProduct]=useState('th06'),[files,setFiles]=useState<File[]>([]);
  const [busy,setBusy]=useState(false),[error,setError]=useState<UiMessageKey>(),[message,setMessage]=useState<UiMessageKey>();
  const [deleteOpen,setDeleteOpen]=useSurfaceLayerState<boolean>('delete-cloud-save',false);
  const task=useRef<AbortController | undefined>(undefined),input=useRef<HTMLInputElement>(null);
  const mounted=useRef(true);
  const selection=data?.products.find(item=>item.id===product),versions=data?.saves.filter(item=>item.product===product).sort((a,b)=>b.revision-a.revision)??[],latest=versions[0];
  useEffect(()=>{mounted.current=true;const controller=new AbortController();void listCloudSaves(controller.signal).then(value=>{
    if(controller.signal.aborted)return;
    setData(value);setProduct(value.saves[0]?.product??(value.products.some(item=>item.id==='th06')?'th06':value.products[0]?.id??'th06'));
  }).catch(error=>{if(!controller.signal.aborted)setError(errorKey(error));});return()=>{mounted.current=false;controller.abort();task.current?.abort();};},[]);
  useUnsavedChanges(files.length>0,()=>setFiles([]));
  async function perform(action:(signal:AbortSignal)=>Promise<void>) {
    if(task.current)return;const controller=new AbortController();task.current=controller;setBusy(true);setError(undefined);setMessage(undefined);
    try{await action(controller.signal);}catch(error){if(!controller.signal.aborted&&mounted.current){setError(errorKey(error));if(error instanceof AccountRequestError&&error.code==='SAVE_CONFLICT')void listCloudSaves().then(value=>{if(mounted.current)setData(value);}).catch(()=>{});}}
    finally{if(mounted.current&&task.current===controller){setBusy(false);task.current=undefined;}}
  }
  function update(next:CloudSaveSnapshot) {
    setData(previous=>previous?{...previous,products:previous.products.map(item=>item.id===product?{...item,revision:next.revision}:item),saves:[next,...previous.saves.filter(item=>item.product!==product||item.revision!==next.revision)].filter(item=>item.product!==product||item.revision>=next.revision-1)}:previous);
    window.dispatchEvent(new Event('cloud-save-change'));
  }
  const size=(bytes:number)=>new Intl.NumberFormat(locale,{maximumFractionDigits:1}).format(bytes/1024)+' KB';
  function choose(next:File[]) {
    setFiles([]);setMessage(undefined);setError(undefined);
    if(!data||!selection)return;
    if(next.length>data.limits.files||next.some(file=>file.size>data.limits.fileBytes)||next.reduce((sum,file)=>sum+file.size,0)>data.limits.totalBytes){setError('cloudSave.tooLarge');return;}
    const names=next.map(file=>cloudSaveFileName(selection.files,file.name));
    if(next.some(file=>!file.size)||names.some(name=>!name)||new Set(names).size!==next.length){setError('cloudSave.invalid');return;}
    setFiles(next);
  }
  const download=(save:CloudSaveSnapshot)=>void perform(async signal=>{
    const blob=await downloadCloudSave(product,save.revision,signal),url=URL.createObjectURL(blob),link=document.createElement('a');
    link.href=url;link.download=`${product}-save-r${save.revision}.zip`;link.click();setTimeout(()=>URL.revokeObjectURL(url),60_000);
  });
  return <section aria-labelledby="game-settings-title">
    <header className="account-settings-heading"><h2 id="game-settings-title">{t('room.gameSettingsGroup')}</h2></header>
    <section className="settings-section settings-section-first cloud-save-section" aria-labelledby="cloud-save-title" aria-busy={busy||!data&&!error}>
      <div className="cloud-save-heading"><h3 id="cloud-save-title">{t('cloudSave.title')}</h3><Button variant="ghost" size="icon-sm" disabled={busy} aria-label={t('cloudSave.refresh')} onClick={()=>void perform(async signal=>setData(await listCloudSaves(signal)))}><RefreshCw/></Button></div>
      <p className="cloud-save-hint">{t('cloudSave.manualHint')}</p>
      {data&&<>
        <label className="cloud-save-game-label" htmlFor="cloud-save-game">{t('cloudSave.game')}</label>
        <Select value={product} onValueChange={value=>{setProduct(value);setFiles([]);setError(undefined);setMessage(undefined);}} disabled={busy}>
          <SelectTrigger id="cloud-save-game"><SelectValue/></SelectTrigger><SelectContent>{data.products.map(item=><SelectItem key={item.id} value={item.id}>{PRODUCT_GAMES[item.game].title}{item.multiplayer?' | '+t('nav.lobby'):''}</SelectItem>)}</SelectContent>
        </Select>
        {latest&&<div className="cloud-save-current"><div><strong>{t('cloudSave.latest')}</strong><time dateTime={latest.uploadedAt}>{new Intl.DateTimeFormat(locale,{dateStyle:'medium',timeStyle:'short'}).format(new Date(latest.uploadedAt))}</time></div>
          {latest.statistics.playtimeSeconds!==null&&<span>{t('profile.flightTime')} {formatPlaytime(latest.statistics.playtimeSeconds,locale)}</span>}
          <div className="cloud-save-file-summary">{latest.files.map(file=><span key={file.name}>{file.name} <small>{size(file.bytes)}</small></span>)}</div>
          {latest.statistics.warnings.includes('playtime-file-missing')&&<p className="cloud-save-hint">{t('cloudSave.th06TimeHint')}</p>}
          {latest.statistics.game==='th05'&&latest.statistics.warnings.includes('clear-unavailable')&&<p className="cloud-save-hint">{t('cloudSave.th05ClearHint')}</p>}
          {latest.statistics.warnings.includes('backup-used')&&<p className="cloud-save-hint">{t('cloudSave.backupUsed')}</p>}
          <div className="cloud-save-actions"><Button variant="outline" size="sm" disabled={busy} onClick={()=>download(latest)}><Download/>{t('cloudSave.download')}</Button><Button variant="ghost" size="icon-sm" disabled={busy} aria-label={t('cloudSave.delete')} onClick={()=>setDeleteOpen(true)}><Trash2/></Button></div>
        </div>}
        <div className="cloud-save-upload">
          <input ref={input} type="file" className="sr-only" multiple accept=".dat,.cfg,.ini,.txt" disabled={busy} aria-label={t('cloudSave.choose')} onChange={event=>{choose(Array.from(event.target.files??[]));event.target.value='';}}/>
          <div className="cloud-save-actions">{readLocalSaves&&<Button variant="outline" disabled={busy} onClick={()=>void perform(async()=>{
            if(!selection||!isProductId(product))return;
            const local=await readLocalSaves(product,selection.files);
            if(!mounted.current)return;
            if(!local.length||!selection.requiredFiles.some(name=>local.some(file=>cloudSaveFileName(selection.files,file.name)===name))){setError('cloudSave.noLocalSave');return;}
            choose(local);
          })}>{t('cloudSave.chooseLocal')}</Button>}<Button variant="outline" disabled={busy} onClick={()=>input.current?.click()}>{t('cloudSave.choose')}</Button>
            {files.length>0&&<Button disabled={busy} onClick={()=>void perform(async signal=>{const saved=await uploadCloudSave(product,selection!.revision,files,signal);if(!mounted.current)return;update(saved);setFiles([]);setMessage('cloudSave.uploaded');})}>{busy?<LoaderCircle className="animate-spin motion-reduce:animate-none"/>:<CloudUpload/>}{t('cloudSave.upload')}</Button>}
          </div>
          {files.length>0&&<ul className="cloud-save-selected">{files.map(file=><li key={file.name}><span>{file.name}</span><small>{size(file.size)}</small></li>)}</ul>}
          <p className="cloud-save-hint">{t('cloudSave.requiredFiles',{files:selection?.requiredFiles.join(' | ')??''})}</p>
        </div>
        {versions[1]&&<div className="cloud-save-previous"><span>{t('cloudSave.previous')}</span><time dateTime={versions[1].uploadedAt}>{new Intl.DateTimeFormat(locale,{dateStyle:'medium',timeStyle:'short'}).format(new Date(versions[1].uploadedAt))}</time><Button variant="ghost" size="icon-sm" disabled={busy} aria-label={t('cloudSave.downloadPrevious')} onClick={()=>download(versions[1]!)}><Download/></Button></div>}
      </>}
      {error&&<p className="cloud-save-error" role="alert">{t(error)}</p>}{message&&<p role="status">{t(message)}</p>}
      <ConfirmationDialog open={deleteOpen} title={t('cloudSave.delete')} message={t('cloudSave.deleteConfirm')} confirmText={t('cloudSave.delete')} cancelText={t('action.cancel')} tone="danger"
        onCancel={()=>setDeleteOpen(false)} onConfirm={()=>{setDeleteOpen(false);void perform(async signal=>{const next=await deleteCloudSave(product,selection!.revision,signal);if(!mounted.current)return;setData(previous=>previous?{...previous,products:previous.products.map(item=>item.id===product?{...item,revision:next.revision}:item),saves:previous.saves.filter(item=>item.product!==product)}:previous);window.dispatchEvent(new Event('cloud-save-change'));setMessage('cloudSave.deleted');});}}/>
    </section>
  </section>;
}
