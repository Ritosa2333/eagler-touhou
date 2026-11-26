import {Button as UiButton} from '../../ui/controls';
import {useEffectEvent, useLayoutEffect, useRef, useState, useSyncExternalStore} from 'react';
import {useLocale} from '../../i18n';
import type {GameDataImportModel} from '../../models/game-data-import';
import {closeMainSelectMenus} from '../launcher/MainSelect';
import {TransientWindow} from '../TransientWindow';

export interface GameDataImportWindowsProps {
  model: GameDataImportModel;
  /** Read the live Runtime state for main's programmatic-popup fallback policy. */
  getRuntimeReady(): boolean;
  manualOpen?: boolean;
  onManualClose?(): void;
}

/** Main public/index.html838–864 + app.mts3328–3367,9643–9672.
 * Manual import owns a Router entry. Automatic prompts follow the active
 * transfer and reuse the same import model and fullscreen overlay host.
 */
export function GameDataImportWindows({model, getRuntimeReady, manualOpen, onManualClose}: GameDataImportWindowsProps) {
  const {t} = useLocale(), state = useSyncExternalStore(model.subscribe, model.getSnapshot, model.getSnapshot);
  const [password, setPassword] = useState('');
  const manualAttempt = useRef<number | undefined>(undefined);
  const finishManual = useEffectEvent(() => onManualClose?.());
  useLayoutEffect(() => {
    if (manualOpen) {
      model.openManual();
      manualAttempt.current = model.getSnapshot().attempt?.id;
      const frame = requestAnimationFrame(() => document.getElementById('gameDataImportClose')?.focus({preventScroll:true}));
      return () => cancelAnimationFrame(frame);
    }
    if (manualAttempt.current === undefined) return;
    if (manualAttempt.current === model.getSnapshot().attempt?.id) model.dismissImport();
    manualAttempt.current = undefined;
    document.getElementById('gamePackageImport')?.focus({preventScroll:true});
  }, [manualOpen, model]);
  // biome-ignore lint/correctness/useExhaustiveDependencies: Check committed model state because opening manual import synchronously updates the snapshot.
  useLayoutEffect(() => {
    if (!manualOpen) return;
    const current = model.getSnapshot();
    if (!current.importOpen || current.attempt?.id !== manualAttempt.current) finishManual();
  }, [manualOpen, model, state.importOpen, state.attempt?.id]);
  const present = useEffectEvent(() => {
    // Main app.mts3361/9656 closes menus on presentation, including repeated
    // opens. A hidden remount must not replay an earlier presentation, and
    // busy/progress/metadata changes are not presentation events.
    if (state.importOpen || state.linkOpen) closeMainSelectMenus();
  });
  useLayoutEffect(() => {setPassword(''); if (state.presentationRevision) present();}, [state.presentationRevision]);
  const fallback = state.fallback;
  return <>
    <TransientWindow id="gameDataImportWindow" titleId="gameDataImportTitle" title={t('package.importTitle')}
      closeId="gameDataImportClose" closeLabel={t('package.closeImport')} open={state.importOpen} busy={state.busy} onClose={model.dismissImport}>
      <p id="gameDataImportReason">{state.reason || t('package.unavailable')}</p>
      <div className="game-data-import-busy" id="gameDataImportBusy" role="status" aria-live="polite" hidden={!state.busy}><i className="eagler-preload-spinner" aria-hidden="true"/><span id="gameDataImportBusyText">{state.busyText || t('package.installing')}</span></div>
      {state.passwordRequired && <div className="game-data-archive-password">
        <input type="password" aria-label={t('package.originalPassword')} autoComplete="off" value={password}
          disabled={state.busy} onChange={event => setPassword(event.target.value)}
          onKeyDown={event => {if (event.key === 'Enter' && password) void model.chooseFile(password);}}/>
      </div>}
      <div className="game-data-import-actions">
        <UiButton variant="primary" className="transfer-import" id="transferImport" type="button"
          disabled={state.busy || (state.passwordRequired && !password)} onClick={() => {void model.chooseFile(password);}}>{t('action.import')}</UiButton>
        {!state.passwordRequired && <UiButton className="transfer-download" id="transferDownload" type="button" disabled={state.busy || !fallback}
          title={t(fallback ? 'package.fallbackLinkTitle' : 'package.fallbackLinkUnavailableTitle')} onClick={model.openLink}>{t('action.openLink')}</UiButton>}
      </div>
      <input id="gameDataImportInput" type="file" accept={state.fileAccept} multiple hidden onChange={event => {
        const files = Array.from(event.currentTarget.files ?? []);
        event.currentTarget.value = '';
        void model.importSelection(files);
      }}/>
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
