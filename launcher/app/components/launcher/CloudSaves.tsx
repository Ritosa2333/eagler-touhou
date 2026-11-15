import {useCallback, useEffect, useRef, useState, useSyncExternalStore} from 'react';
import type {CloudSaveSync} from '../../services/cloud-save-sync';
import {ChevronDown, CloudUpload, Download, Gamepad2, LoaderCircle, RefreshCw, Trash2} from 'lucide-react';
import {PRODUCT_GAMES, isProductId} from '../../../src/contracts/product-catalog.mts';
import {Badge} from '../../ui/badge';
import {Button} from '../../ui/button';
import {Toggle} from '../../ui/controls';
import {Field, FieldContent, FieldDescription, FieldLabel} from '../../ui/field';
import {Select, SelectContent, SelectItem, SelectTrigger, SelectValue} from '../../ui/select';
import {ConfirmationDialog, SaveComparisonCards} from '../ConfirmationDialog';
import {useSurfaceLayerState} from '../../navigation/surface-layer';
import {useUnsavedChanges} from '../../navigation/unsaved-changes';
import {useLocale, type UiMessageKey} from '../../i18n';
import {AccountRequestError} from '../../services/account-client';
import type {LocalSaveReader} from '../../services/local-save-storage';
import {compareSaveVersions, localSaveFingerprint, relateSaveVersions, saveComparisonCards, type SaveVersionComparison} from '../../services/cloud-save-comparison';
import {cloudSaveFileName, deleteCloudSave, downloadCloudSave, listCloudSaves, uploadCloudSave, type CloudSaveList, type CloudSaveSnapshot} from '../../services/cloud-saves';
import {bookGameArtwork} from './book-artwork';
import './cloud-saves.css';

function errorKey(error: unknown): UiMessageKey {
  const code = error instanceof AccountRequestError ? error.code : '';
  switch (code) {
    case 'SAVE_CONFLICT': return 'cloudSave.conflict';
    case 'SAVE_BUSY': return 'cloudSave.busy';
    case 'SAVE_QUOTA_EXCEEDED': return 'cloudSave.quota';
    case 'SAVE_TOO_LARGE':
    case 'SAVE_PARSE_LIMIT': return 'cloudSave.tooLarge';
    case 'INVALID_SAVE':
    case 'WRONG_SAVE_GAME':
    case 'INVALID_SAVE_FILE':
    case 'MAIN_SAVE_REQUIRED': return 'cloudSave.invalid';
    case 'UNSUPPORTED_SAVE_VERSION': return 'cloudSave.unsupportedVersion';
    default: return 'account.requestFailed';
  }
}

const noSubscribe = () => () => {};
const unavailable = () => null;

export function CloudSaves({readLocalSaves, sync}: {readLocalSaves?: LocalSaveReader; sync?: CloudSaveSync | null}) {
  const syncStatus = useSyncExternalStore(sync?.subscribe ?? noSubscribe, sync?.getSnapshot ?? unavailable, unavailable);
  const enabled = useSyncExternalStore(sync?.subscribe ?? noSubscribe, sync?.getEnabled ?? unavailable, unavailable);
  const {locale, t} = useLocale();
  const [data, setData] = useState<CloudSaveList>();
  const [comparison,setComparison]=useState<{product:string;value?:SaveVersionComparison;failed?:boolean}>();

  const [product, setProduct] = useState('');
  useEffect(()=>{
    if(!product||!data)return;
    const current=data.products.find(item=>item.id===product);if(!current||!isProductId(product))return;
    const selected=product,controller=new AbortController();setComparison({product:selected});
    if(!readLocalSaves){setComparison({product:selected,failed:true});return;}
    const latest = data.saves.find(save => save.product === selected && save.revision === current.revision) ?? null;
    void readLocalSaves(selected, current.files).then(async files => {
      const relation = sync ? await sync.versionRelation(selected, files, latest, current.revision) :
        relateSaveVersions(await localSaveFingerprint(files), latest, current.revision);
      if (controller.signal.aborted) return;
      const value = await compareSaveVersions(selected, files, latest, relation, controller.signal);
      if (!controller.signal.aborted) setComparison({product: selected, value});
    }).catch(() => {if (!controller.signal.aborted) setComparison({product: selected, failed: true});});
    return()=>controller.abort();
  },[product,data,readLocalSaves,sync]);
  const [draft, setDraft] = useState<{product: string; files: File[]}>({product: '', files: []});
  const [busy, setBusy] = useState(false);
  const [refreshing, setRefreshing] = useState(true);
  const [listError, setListError] = useState<UiMessageKey>();
  const [error, setError] = useState<UiMessageKey>();
  const [message, setMessage] = useState<UiMessageKey>();
  const [preferenceError, setPreferenceError] = useState(false);
  const [deleteOpen, setDeleteOpen] = useSurfaceLayerState<boolean>('delete-cloud-save', false);
  const task = useRef<AbortController | undefined>(undefined);
  const listTask = useRef<AbortController | undefined>(undefined);
  const input = useRef<HTMLInputElement>(null);
  const activeRow = useRef<HTMLButtonElement>(null);
  const selectedFromMenu = useRef(false);
  const mounted = useRef(true);

  const selection = data?.products.find(item => item.id === product);
  const files = draft.product === product ? draft.files : [];
  const dates = new Intl.DateTimeFormat(locale, {dateStyle: 'medium', timeStyle: 'short'});
  const sizes = new Intl.NumberFormat(locale, {maximumFractionDigits: 1});
  const latestFor = (id: string,revision:number) => data?.saves.find(save => save.product === id&&save.revision===revision);
  const size = (bytes: number) => sizes.format(bytes / 1024) + ' KB';

  function rowStatus(id: string): UiMessageKey | undefined {
    if ((syncStatus?.product === id && syncStatus.error === 'cloudSave.syncConflict') || (product === id && error === 'cloudSave.conflict'))
      return 'cloudSave.versionConflict';
    if (isProductId(id) && sync?.hasLocalPending(id)) return 'cloudSave.pendingHere';
    if (syncStatus?.product === id) return 'cloudSave.syncDeferred';
  }
  const rows = data?.products.filter(item => !item.multiplayer).map(item => ({
    item, latest: latestFor(item.id,item.revision), status: rowStatus(item.id),
  })) ?? [];
  const listed = rows.filter(row => row.latest || row.status || row.item.id === product ||
    (row.item.id === draft.product && draft.files.length));
  const other = rows.filter(row => !listed.includes(row));

  const refresh = useCallback(async () => {
    listTask.current?.abort();
    const controller = new AbortController();
    listTask.current = controller;
    setRefreshing(true);
    try {
      const next = await listCloudSaves(controller.signal);
      if (controller.signal.aborted) return;
      setData(next);
      setListError(undefined);
    } catch (failure) {
      if (!controller.signal.aborted) setListError(errorKey(failure));
    } finally {
      if (!controller.signal.aborted) setRefreshing(false);
    }
  }, []);
  useEffect(() => {
    mounted.current = true;
    const reload = () => void refresh();
    reload();
    window.addEventListener('cloud-save-change', reload);
    return () => {
      mounted.current = false;
      listTask.current?.abort();
      task.current?.abort();
      window.removeEventListener('cloud-save-change', reload);
    };
  }, [refresh]);
  useUnsavedChanges(draft.files.length > 0, () => setDraft({product: '', files: []}));

  async function perform(action: (signal: AbortSignal) => Promise<void>) {
    if (task.current) return;
    const controller = new AbortController();
    task.current = controller;
    setBusy(true);
    setError(undefined);
    setMessage(undefined);
    try {
      await action(controller.signal);
    } catch (failure) {
      if (controller.signal.aborted || !mounted.current) return;
      setError(errorKey(failure));
      if (failure instanceof AccountRequestError && failure.status === 409) void refresh();
    } finally {
      if (mounted.current && task.current === controller) {
        task.current = undefined;
        setBusy(false);
      }
    }
  }
  function select(id: string) {
    setProduct(id);
    setError(undefined);
    setMessage(undefined);
  }
  function choose(next: File[]) {
    setDraft(previous => previous.product === product ? {product: '', files: []} : previous);
    setMessage(undefined);
    setError(undefined);
    if (!data || !selection) return;
    if (next.length > data.limits.files || next.some(file => file.size > data.limits.fileBytes) ||
      next.reduce((sum, file) => sum + file.size, 0) > data.limits.totalBytes) {
      setError('cloudSave.tooLarge');
      return;
    }
    const names = next.map(file => cloudSaveFileName(selection.files, file.name));
    if (next.some(file => !file.size) || names.some(name => !name) || new Set(names).size !== next.length) {
      setError('cloudSave.invalid');
      return;
    }
    if (next.length) setDraft({product, files: next});
  }
  function download(save: CloudSaveSnapshot) {
    void perform(async signal => {
      const blob = await downloadCloudSave(save.product, save.revision, signal);
      if (signal.aborted) return;
      const url = URL.createObjectURL(blob);
      const link = document.createElement('a');
      link.href = url;
      link.download = save.product + '-save-r' + save.revision + '.zip';
      link.click();
      setTimeout(() => URL.revokeObjectURL(url), 60_000);
    });
  }
  function saveUploaded(saved: CloudSaveSnapshot) {
    setData(previous => previous ? {
      ...previous,
      products: previous.products.map(item => item.id === saved.product ? {...item, revision: saved.revision} : item),
      saves: [saved, ...previous.saves.filter(item => item.product !== saved.product)],
    } : previous);
    setDraft({product: '', files: []});
    setMessage('cloudSave.uploaded');
    window.dispatchEvent(new Event('cloud-save-change'));
  }
  function removeSave() {
    setDeleteOpen(false);
    if (!selection) return;
    void perform(async signal => {
      const next = await deleteCloudSave(selection.id, selection.revision, signal);
      if (!mounted.current || signal.aborted) return;
      setData(previous => previous ? {
        ...previous,
        products: previous.products.map(item => item.id === selection.id ? {...item, revision: next.revision} : item),
        saves: previous.saves.filter(item => item.product !== selection.id),
      } : previous);
      setMessage('cloudSave.deleted');
      window.dispatchEvent(new Event('cloud-save-change'));
    });
  }

  return <section aria-labelledby="game-settings-title">
    <header className="account-settings-heading"><h2 id="game-settings-title">{t('room.gameSettingsGroup')}</h2></header>
    <section className="settings-section settings-section-first cloud-save-section" aria-labelledby="cloud-save-title" aria-busy={refreshing || busy}>
      <div className="cloud-save-heading">
        <h3 id="cloud-save-title">{t('cloudSave.title')}</h3>
        <Button variant="ghost" size="icon-sm" disabled={refreshing || busy} aria-label={t('cloudSave.refresh')} onClick={() => void refresh()}><RefreshCw/></Button>
      </div>
      {!data && refreshing && <p className="cloud-save-loading" role="status" aria-label={t('status.loading')}><LoaderCircle className="animate-spin motion-reduce:animate-none"/></p>}
      {data && !listed.length && <p className="cloud-save-empty">{t('cloudSave.noSave')}</p>}
      {!!listed.length && <ul className="cloud-save-list">{listed.map(({item, latest, status}) => {
        const open = product === item.id;
        const review = isProductId(item.id) && sync?.hasLocalPending(item.id);
        const artwork = bookGameArtwork(item.game);
        const detailId = 'cloud-save-details-' + item.id;
        return <li className="cloud-save-entry" key={item.id}>
          <Button ref={open ? activeRow : undefined} variant="ghost" className="cloud-save-row"
            aria-expanded={open} aria-controls={open ? detailId : undefined}
            disabled={busy} onClick={() => select(open ? '' : item.id)}>
            <span className="cloud-save-cover">{artwork ? <img src={artwork} alt="" loading="lazy" decoding="async"/> : <Gamepad2 aria-hidden="true"/>}</span>
            <span className="cloud-save-identity"><strong>{PRODUCT_GAMES[item.game].title}</strong>
              {latest ? <time dateTime={latest.uploadedAt}>{dates.format(new Date(latest.uploadedAt))}</time> : <span>{t('cloudSave.noSave')}</span>}
            </span>
            {status && <Badge variant={status === 'cloudSave.versionConflict' ? 'destructive' : 'outline'}>{t(status)}</Badge>}
            <ChevronDown className="cloud-save-chevron" aria-hidden="true"/>
          </Button>
          {open && <div className="cloud-save-details" id={detailId}>
            {syncStatus?.product === item.id && <div className="cloud-save-sync-message" role="status">
              <p>{t(syncStatus.error)}</p>
              {enabled && syncStatus.error === 'cloudSave.syncPending' && <Button variant="outline" size="sm" disabled={busy}
                onClick={() => void perform(async () => {await sync?.flush();})}>{t('action.retry')}</Button>}
            </div>}
              {comparison?.product===item.id&&comparison.value?<SaveComparisonCards value={saveComparisonCards(comparison.value,locale,t)}/>:<p className="cloud-save-hint">{t(comparison?.failed?'cloudSave.localReadFailed':'cloudSave.comparing')}</p>}
            {latest && <div className="cloud-save-current">
              <div className="cloud-save-version"><strong>{t('cloudSave.latest')}</strong>
                <Button variant="outline" size="sm" disabled={busy} onClick={() => download(latest)}><Download/>{t('cloudSave.download')}</Button>
              </div>
              {latest.statistics.warnings.includes('playtime-file-missing') && <p className="cloud-save-hint">{t('cloudSave.th06TimeHint')}</p>}
              {latest.statistics.game === 'th05' && latest.statistics.warnings.includes('clear-unavailable') && <p className="cloud-save-hint">{t('cloudSave.th05ClearHint')}</p>}
              {latest.statistics.warnings.includes('backup-used') && <p className="cloud-save-hint">{t('cloudSave.backupUsed')}</p>}

            </div>}
            <div className="cloud-save-upload">
              <input ref={input} type="file" className="sr-only" multiple accept=".dat,.cfg,.ini,.txt" disabled={busy} tabIndex={-1}
                aria-label={t('cloudSave.choose')} onChange={event => {choose(Array.from(event.target.files ?? [])); event.target.value = '';}}/>
              <div className="cloud-save-actions">
                {sync && (latest || review) && <Button variant="outline" size="sm" disabled={busy}
                  onClick={() => void perform(async signal => {if (isProductId(item.id)) await sync?.resolve(item.id, signal);})}>
                  {t('cloudSave.review')}
                </Button>}
                {readLocalSaves && <Button variant="outline" size="sm" disabled={busy} onClick={() => void perform(async signal => {
                  if (!isProductId(item.id)) return;
                  const local = await readLocalSaves(item.id, item.files);
                  if (!mounted.current || signal.aborted) return;
                  if (!local.length || !item.requiredFiles.some(name => local.some(file => cloudSaveFileName(item.files, file.name) === name))) {
                    setError('cloudSave.noLocalSave');
                    return;
                  }
                  choose(local);
                })}>{t('cloudSave.chooseLocal')}</Button>}
                <Button variant="outline" size="sm" disabled={busy} onClick={() => input.current?.click()}>{t('cloudSave.choose')}</Button>
              </div>
              <p className="cloud-save-hint">{t('cloudSave.requiredFiles', {files: item.requiredFiles.join(' | ')})}</p>
              {!!files.length && <>
                <ul className="cloud-save-selected">{files.map(file => <li key={file.name}><span>{file.name}</span><small>{size(file.size)}</small></li>)}</ul>
                <div className="cloud-save-actions">
                  <Button size="sm" disabled={busy} onClick={() => void perform(async signal => {
                    const saved = await uploadCloudSave(item.id, item.revision, files, signal);
                    if (mounted.current && !signal.aborted) saveUploaded(saved);
                  })}>{busy ? <LoaderCircle className="animate-spin motion-reduce:animate-none"/> : <CloudUpload/>}{t('cloudSave.upload')}</Button>
                  <Button variant="ghost" size="sm" disabled={busy} onClick={() => choose([])}>{t('action.cancel')}</Button>
                </div>
              </>}
            </div>
            {error && <p className="cloud-save-error" role="alert">{t(error)}</p>}
            {message && <p className="cloud-save-hint" role="status">{t(message)}</p>}
            {latest && <div className="cloud-save-danger">
              <Button className="cloud-save-delete" variant="ghost" size="sm" disabled={busy} onClick={() => setDeleteOpen(true)}>
                <Trash2/>{t('cloudSave.delete')}
              </Button>
            </div>}
          </div>}
        </li>;
      })}</ul>}
      {!!other.length && <div className="cloud-save-add">
        <Select value="" onValueChange={id => {selectedFromMenu.current = true; select(id);}} disabled={busy}>
          <SelectTrigger aria-label={t('cloudSave.addSave')}><SelectValue placeholder={t('cloudSave.addSave')}/></SelectTrigger>
          <SelectContent onCloseAutoFocus={event => {
            if (!selectedFromMenu.current) return;
            selectedFromMenu.current = false;
            if (!activeRow.current) return;
            event.preventDefault();
            activeRow.current.focus();
          }}>{other.map(({item}) => <SelectItem key={item.id} value={item.id}>{PRODUCT_GAMES[item.game].title}</SelectItem>)}</SelectContent>
        </Select>
      </div>}
      {listError && <p className="cloud-save-error" role="alert">{t(listError)}</p>}
    </section>
    <section className="settings-section cloud-save-preference" aria-labelledby="cloud-save-auto-sync-label">
      <Field orientation="horizontal" className="cloud-save-switch-row">
        <FieldContent>
          <FieldLabel id="cloud-save-auto-sync-label" htmlFor="cloud-save-auto-sync">{t('cloudSave.autoSync')}</FieldLabel>
          <FieldDescription id="cloud-save-auto-sync-help">{t(enabled === null ? 'cloudSave.syncUnavailable' : enabled ? 'cloudSave.autoHint' : 'cloudSave.pausedHint')}</FieldDescription>
        </FieldContent>
        <Toggle id="cloud-save-auto-sync" aria-describedby="cloud-save-auto-sync-help" checked={enabled === true} disabled={enabled === null}
          onCheckedChange={value => {
            setPreferenceError(false);
            try { sync?.setEnabled(value); }
            catch { setPreferenceError(true); }
          }}/>
      </Field>
      {preferenceError && <p className="cloud-save-error" role="alert">{t('cloudSave.preferenceFailed')}</p>}
    </section>
    <ConfirmationDialog idPrefix="cloudSaveDelete" open={deleteOpen} title={t('cloudSave.delete')} message={t('cloudSave.deleteConfirm')} confirmText={t('cloudSave.delete')}
      cancelText={t('action.cancel')} tone="danger" onCancel={() => setDeleteOpen(false)} onConfirm={removeSave}/>
  </section>;
}
