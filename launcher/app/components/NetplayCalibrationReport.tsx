import {useLocale} from '../i18n';
import {Textarea} from '../ui/input';
import {Button as UiButton} from '../ui/controls';
import {useLayoutEffect, useRef, useState, useSyncExternalStore} from 'react';
import {createPortal} from 'react-dom';
import type {NetplayCalibration} from '../models/netplay-calibration';

/** Original diagnostics trigger and native report dialog. Opening captures the
 * current real network display; only a user's Copy click calls the clipboard. */
export function NetplayCalibrationReport({model, currentNetwork, clipboard}: {
  model: NetplayCalibration; currentNetwork(): string;
  clipboard?: Pick<Clipboard, 'writeText'>;
}) {
  const {t} = useLocale();
  const snapshot = useSyncExternalStore(model.subscribe, model.getSnapshot, model.getSnapshot);
  const dialog = useRef<HTMLDialogElement>(null), text = useRef<HTMLTextAreaElement>(null);
  const lifetime = useRef(0);
  const [copyState, setCopyState] = useState<'idle' | 'copied' | 'selected'>('idle');
  const visible = snapshot.reportText !== null;
  useLayoutEffect(() => {
    const node = dialog.current; if (!node) return;
    if (visible && !node.open) {lifetime.current++; setCopyState('idle'); node.showModal();}
    else if (!visible && node.open) {lifetime.current++; node.close();}
  }, [visible]);
  const heading = t('calibration.reportTitle');
  async function copyReport() {
    const value = snapshot.reportText, owner = lifetime.current; if (value === null) return;
    try {
      const writer = clipboard ?? navigator.clipboard;
      await writer.writeText(value);
      if (lifetime.current === owner && model.getSnapshot().reportText === value) setCopyState('copied');
    } catch {
      if (lifetime.current !== owner || model.getSnapshot().reportText !== value) return;
      text.current?.focus(); text.current?.select(); setCopyState('selected');
    }
  }
  return <>
    <UiButton id="netplayCalibrationReport" size="sm" className="calibration-report-trigger" type="button" hidden={!snapshot.report} onClick={() => model.openReport(currentNetwork())}>
      {t('calibration.reportTrigger')}
    </UiButton>
    {typeof document !== 'undefined' && createPortal(<dialog id="netplayCalibrationDialog" className="calibration-dialog" data-launcher-document="" ref={dialog}
      onClose={() => {if (!dialog.current?.open) model.closeReport();}} onCancel={event => {event.preventDefault(); model.closeReport();}}
      aria-labelledby="netplayCalibrationTitle">
      <header><h2 id="netplayCalibrationTitle">{heading}</h2><UiButton variant="icon" type="button" aria-label={t('calibration.closeReport')} onClick={model.closeReport}>×</UiButton></header>
      <p>{t('calibration.reportHint')}</p>
      <Textarea id="netplayCalibrationText" ref={text} readOnly aria-label={heading} value={snapshot.reportText ?? ''}/>
      <footer><UiButton variant="primary" size="lg" id="netplayCalibrationCopy" type="button" onClick={() => {void copyReport();}}>
        {copyState === 'copied' ? t('calibration.copied') : copyState === 'selected' ? t('calibration.selected') : t('calibration.copy')}
      </UiButton>
      <UiButton size="lg" type="button" onClick={model.closeReport}>{t('action.close')}</UiButton></footer>
    </dialog>, document.body)}
  </>;
}
