import {Button as UiButton} from '../../ui/controls';
import {useLayoutEffect, useRef, useState, useSyncExternalStore} from 'react';
import {useSurfaceMotion} from '../../ui/use-surface-motion';
import {Progress, ProgressIndicator} from '../../ui/progress';
import {useLocale} from '../../i18n';
import type {TransferModel} from '../../models/transfer';

/** Original index.html822–837. The existing transfer owner arbitrates network,
 * package/runtime/language/music progress and owns cancellation/retry timers. */
export function TransferPanel({model}: {model: TransferModel}) {
  const {t} = useLocale(), state = useSyncExternalStore(model.subscribe, model.getSnapshot, model.getSnapshot);
  const notice = useRef<HTMLDivElement>(null);
  const [noticePresent, setNoticePresent] = useState(state.midiNoticeVisible);
  useLayoutEffect(() => {if (state.midiNoticeVisible) setNoticePresent(true);}, [state.midiNoticeVisible]);
  useSurfaceMotion(notice, noticePresent ? state.midiNoticeVisible ? 'enter' : 'exit' : null, {
    kind: 'dialog', onComplete() {if (!state.midiNoticeVisible) setNoticePresent(false);},
  });
  return <><div ref={notice} className={`music-notice${state.midiNoticeVisible ? ' show' : ''}`} data-present={noticePresent || undefined} hidden={!noticePresent} id="musicNotice" role="status" aria-live="polite">
    <strong>{t('player.oggNotReady')}</strong><span>{t('player.oggFallback')}</span>
  </div><aside className="transfer" id="transfer" aria-live="polite" hidden={state.hidden}
    data-network-owned={state.networkOwned ? '1' : '0'} data-network-active={state.networkActive ? '1' : undefined}>
    <strong id="transferTitle">{state.title}</strong>
    <div className="transfer-line"><span id="transferLabel">{state.label}</span><span id="transferAmount">{state.amount}</span></div>
    <Progress className="transfer-bar" aria-label={state.title} value={state.indeterminate ? null : Number.parseFloat(state.barWidth) || 0}>
      <ProgressIndicator asChild><i id="transferBar" style={state.indeterminate ? undefined : {width: state.barWidth}}/></ProgressIndicator>
    </Progress>
    <div className="transfer-meta"><span id="transferSpeed">{state.speedText}</span><span id="transferEta">{state.etaText}</span></div>
    <pre className="player-debug" id="playerDebug" hidden={!state.debugVisible}>{state.debugText}</pre>
    <div className="transfer-warning" id="transferWarning" hidden={!state.warningVisible}>{state.warningText}</div>
    <div className="transfer-actions"><UiButton variant="quiet" className="transfer-cancel" id="transferCancel" type="button" hidden={!state.cancelVisible} onClick={() => {void model.cancel();}}>{state.cancelLabel}</UiButton>
      <UiButton className="transfer-retry" id="transferRetry" type="button" hidden={!state.retryVisible} onClick={() => {void model.retry();}}>{t('action.retry')}</UiButton></div>
  </aside></>;
}
