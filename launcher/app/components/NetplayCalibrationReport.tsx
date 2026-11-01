import { useLayoutEffect, useRef, useState, useSyncExternalStore } from 'react';
import { createPortal } from 'react-dom';
import type { UiLocale, UiMessageKey } from '../../src/launcher/i18n.mts';
import { translate, useLocale } from '../i18n';
import type { NetplayCalibration } from '../models/netplay-calibration';
import { Button } from './ui/button';
import { Textarea } from './ui/textarea';

/** Original diagnostics trigger and native report dialog. Opening captures the
 * current real network display; only a user's Copy click calls the clipboard. */
export function NetplayCalibrationReport({
  model,
  english,
  currentNetwork,
  clipboard,
}: {
  model: NetplayCalibration;
  english: boolean;
  currentNetwork(): string;
  clipboard?: Pick<Clipboard, 'writeText'>;
}) {
  const { locale } = useLocale();
  const effectiveLocale = locale === 'ja' ? 'ja' : english ? 'en' : 'zh-CN';
  const snapshot = useSyncExternalStore(model.subscribe, model.getSnapshot, model.getSnapshot);
  const dialog = useRef<HTMLDialogElement>(null),
    text = useRef<HTMLTextAreaElement>(null);
  const lifetime = useRef(0);
  const [openedLocale, setOpenedLocale] = useState<UiLocale>(effectiveLocale);
  const [copyState, setCopyState] = useState<'idle' | 'copied' | 'selected'>('idle');
  const visible = snapshot.reportText !== null;
  useLayoutEffect(() => {
    const node = dialog.current;
    if (!node) return;
    if (visible && !node.open) {
      lifetime.current++;
      setOpenedLocale(effectiveLocale);
      setCopyState('idle');
      node.showModal();
    } else if (!visible && node.open) {
      lifetime.current++;
      node.close();
    }
  }, [visible, snapshot.reportText]);
  const reportLocale = visible ? openedLocale : effectiveLocale;
  const tr = (key: UiMessageKey) => translate(reportLocale, key);
  const heading = tr('calibration.reportTitle');
  async function copyReport() {
    const value = snapshot.reportText,
      owner = lifetime.current;
    if (value === null) return;
    try {
      const writer = clipboard ?? navigator.clipboard;
      await writer.writeText(value);
      if (lifetime.current === owner && model.getSnapshot().reportText === value) setCopyState('copied');
    } catch {
      if (lifetime.current !== owner || model.getSnapshot().reportText !== value) return;
      text.current?.focus();
      text.current?.select();
      setCopyState('selected');
    }
  }
  return (
    <>
      <Button
        variant="legacy"
        size="legacy"
        id="netplayCalibrationReport"
        type="button"
        hidden={!snapshot.report}
        onClick={() => model.openReport(currentNetwork())}
        style={{
          pointerEvents: 'auto',
          minHeight: 32,
          padding: '5px 9px',
          border: '1px solid #777',
          borderRadius: 8,
          background: '#242421',
          color: '#fff',
          font: 'inherit',
          cursor: 'pointer',
        }}
      >
        {translate(effectiveLocale, 'calibration.reportTrigger')}
      </Button>
      {typeof document !== 'undefined' &&
        createPortal(
          <dialog
            id="netplayCalibrationDialog"
            data-launcher-document=""
            ref={dialog}
            onClose={() => {
              if (!dialog.current?.open) model.closeReport();
            }}
            onCancel={(event) => {
              event.preventDefault();
              model.closeReport();
            }}
            style={{
              boxSizing: 'border-box',
              width: 560,
              maxWidth: 'calc(100vw - 24px)',
              maxHeight: 'calc(100dvh - 24px)',
              padding: 18,
              border: '1px solid #777',
              borderRadius: 14,
              background: '#242421',
              color: '#fff',
            }}
          >
            <h2>{heading}</h2>
            <p>{tr('calibration.reportHint')}</p>
            <Textarea
              id="netplayCalibrationText"
              ref={text}
              readOnly
              aria-label={heading}
              value={snapshot.reportText ?? ''}
              style={{
                boxSizing: 'border-box',
                width: '100%',
                height: '40dvh',
                minHeight: 140,
                background: '#161614',
                color: '#eee',
                padding: 10,
                font: '12px/1.5 monospace',
              }}
            />
            <Button
              variant="legacy"
              size="legacy"
              id="netplayCalibrationCopy"
              type="button"
              onClick={() => {
                void copyReport();
              }}
              style={{ minHeight: 44, margin: '10px 12px 0 0', padding: '8px 14px' }}
            >
              {copyState === 'copied'
                ? tr('calibration.copied')
                : copyState === 'selected'
                  ? tr('calibration.selected')
                  : tr('calibration.copy')}
            </Button>
            <Button
              variant="legacy"
              size="legacy"
              type="button"
              onClick={model.closeReport}
              style={{ minHeight: 44, padding: '8px 14px' }}
            >
              {tr('action.close')}
            </Button>
          </dialog>,
          document.body,
        )}
    </>
  );
}
