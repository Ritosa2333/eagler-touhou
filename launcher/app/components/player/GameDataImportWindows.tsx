import {Button as UiButton} from '../../ui/controls';
import {useEffect, useEffectEvent, useLayoutEffect, useRef, useState, useSyncExternalStore} from 'react';
import {useLocale} from '../../i18n';
import type {GameDataImportModel} from '../../models/game-data-import';
import {closeMainSelectMenus} from '../launcher/MainSelect';
import {TransientWindow} from '../TransientWindow';

export interface GameDataImportWindowsProps {
  model: GameDataImportModel;
  /** Read the live Runtime state for main's programmatic-popup fallback policy. */
  getRuntimeReady(): boolean;
}

function OriginalFiles({model, files, busy}: {model: GameDataImportModel; files: readonly string[]; busy: boolean}) {
  const {t} = useLocale();
  const archiveInput = useRef<HTMLInputElement>(null), folderInput = useRef<HTMLInputElement>(null), musicInput = useRef<HTMLInputElement>(null);
  const fontInput = useRef<HTMLInputElement>(null);
  const [archives, setArchives] = useState<File[]>([]), [fonts, setFonts] = useState<File[]>([]), [music, setMusic] = useState<File | null>(null);
  const [download, setDownload] = useState<{url: string; filename: string} | null>(null);
  useEffect(() => () => {if (download) URL.revokeObjectURL(download.url);}, [download]);
  const inputs = [...archives, ...fonts, ...(music ? [music] : [])];
  async function exportPackage() {
    setDownload(null);
    const result = await model.exportOriginal(inputs);
    if (result) setDownload({url: URL.createObjectURL(result.blob), filename: result.filename});
  }
  function selectArchives(list: FileList | null) {
    if (!list?.length) return;
    setArchives(Array.from(list));
    setDownload(null);
  }
  return <details className="game-data-original-files">
    <summary>{t('package.originalTitle')}</summary>
    <p>{t('package.originalPrivacy')}</p>
    <p>{t('package.originalRequired', {files: files.join('、')})}</p>
    <p>{t('package.originalExtras')}</p>
    <div className="game-data-import-actions">
      <UiButton type="button" disabled={busy} onClick={() => archiveInput.current?.click()}>{t('package.originalPick')}</UiButton>
      <UiButton type="button" disabled={busy} onClick={() => folderInput.current?.click()}>{t('package.originalFolder')}</UiButton>
      {files.length === 1 && <UiButton type="button" disabled={busy} onClick={() => musicInput.current?.click()}>{t('package.originalMusic')}</UiButton>}
      <UiButton type="button" disabled={busy} onClick={() => fontInput.current?.click()}>{t('package.originalFonts')}</UiButton>
    </div>
    <input ref={archiveInput} type="file" multiple hidden accept=".dat,.zip" onChange={event => {
      selectArchives(event.currentTarget.files); event.currentTarget.value = '';
    }}/>
    <input ref={folderInput} type="file" multiple hidden {...{webkitdirectory: ''}} onChange={event => {
      selectArchives(event.currentTarget.files); setMusic(null); setFonts([]); event.currentTarget.value = '';
    }}/>
    <input ref={musicInput} type="file" hidden accept=".dat" onChange={event => {
      setMusic(event.currentTarget.files?.[0] ?? null); setDownload(null); event.currentTarget.value = '';
    }}/>
    <input ref={fontInput} type="file" hidden multiple accept=".ttc,.otf" onChange={event => {
      setFonts(Array.from(event.currentTarget.files ?? [])); setDownload(null); event.currentTarget.value = '';
    }}/>
    {inputs.length > 0 && <p>{t('package.originalSelected', {count: inputs.length})} {inputs.slice(0, 3).map(file => file.name).join('、')}{inputs.length > 3 ? '…' : ''}</p>}
    <div className="game-data-import-actions">
      <UiButton variant="primary" type="button" disabled={busy || !archives.length} onClick={() => {void model.importOriginal(inputs);}}>{t('package.originalImport')}</UiButton>
      <UiButton type="button" disabled={busy || !archives.length} onClick={() => {void exportPackage();}}>{t('package.originalExport')}</UiButton>
    </div>
    {download && <p><a href={download.url} download={download.filename}>{t('package.originalSave')}</a></p>}
  </details>;
}
/** Main public/index.html838–864 + app.mts3328–3367,9643–9672.
 * Host places these transient sections in the correct fullscreen overlay host;
 * no second dialog/history, file session, continuation or startup owner.
 */
export function GameDataImportWindows({model, getRuntimeReady}: GameDataImportWindowsProps) {
  const {t} = useLocale(), state = useSyncExternalStore(model.subscribe, model.getSnapshot, model.getSnapshot);
  const present = useEffectEvent(() => {
    // Main app.mts3361/9656 closes menus on presentation, including repeated
    // opens. A hidden remount must not replay an earlier presentation, and
    // busy/progress/metadata changes are not presentation events.
    if (state.importOpen || state.linkOpen) closeMainSelectMenus();
  });
  useLayoutEffect(() => {if (state.presentationRevision) present();}, [state.presentationRevision]);
  const fallback = state.fallback;
  const originalFiles = model.originalFileNames();
  return <>
    <TransientWindow id="gameDataImportWindow" titleId="gameDataImportTitle" title={t('package.importTitle')}
      closeId="gameDataImportClose" closeLabel={t('package.closeImport')} open={state.importOpen} busy={state.busy} onClose={model.dismissImport}>
      <p id="gameDataImportReason">{state.reason || t('package.unavailable')}</p>
      <div className="game-data-import-busy" id="gameDataImportBusy" role="status" aria-live="polite" hidden={!state.busy}><i className="eagler-preload-spinner" aria-hidden="true"/><span id="gameDataImportBusyText">{state.busyText || t('package.installing')}</span></div>
      <div className="game-data-import-actions"><UiButton variant="primary" className="transfer-import" id="transferImport" type="button" disabled={state.busy} onClick={() => {void model.chooseFile();}}>{t('action.import')}</UiButton><UiButton className="transfer-download" id="transferDownload" type="button" disabled={state.busy || !fallback} title={t(fallback ? 'package.fallbackLinkTitle' : 'package.fallbackLinkUnavailableTitle')} onClick={model.openLink}>{t('action.openLink')}</UiButton></div>
      <input id="gameDataImportInput" type="file" accept=".zip,.dat,application/zip" hidden onChange={event => {const file = event.currentTarget.files?.[0] ?? null; event.currentTarget.value = ''; void model.importFile(file);}}/>
      {state.importOpen && originalFiles.length > 0 && <OriginalFiles key={state.attempt?.id} model={model} files={originalFiles} busy={state.busy}/>}
    </TransientWindow>
    <TransientWindow variant="reference" id="gameDataLinkWindow" titleId="gameDataLinkTitle" title={t('package.downloadTitle')}
      closeId="gameDataLinkClose" closeLabel={t('package.closeLink')} open={state.linkOpen} onClose={model.closeLink}>
      <div className="game-data-link-row"><span>{t('package.link')}</span><a id="gameDataFallbackUrl" href={fallback?.url} target="_blank" rel="noopener noreferrer" aria-label={t('package.openLinkAria')} onClick={event => {
        if (!fallback?.url || getRuntimeReady()) return;
        event.stopPropagation();
        const opened = window.open(fallback.url, '_blank');
        if (opened) {opened.opener = null; event.preventDefault();}
      }}>{fallback?.url || t('package.fallbackLinkUnavailable')}</a></div>
      <div className="game-data-link-row"><span>{t('package.codeHint')}</span><strong id="gameDataFallbackHint">{fallback?.hint || t('common.none')}</strong></div>
    </TransientWindow>
  </>;
}
