import {X} from 'lucide-react';
import {Button as UiButton} from '../../ui/button';
import {useLayoutEffect, useRef, useSyncExternalStore} from 'react';
import {useSurfaceMotion} from '../../ui/use-surface-motion';
import {useLocale} from '../../i18n';
import type {FeedbackModel} from '../../models/feedback';
import {closeMainSelectMenus} from '../launcher/MainSelect';
import {localizeKnownError} from '../../../src/launcher/localized-error-text.mts';
/** Original main index732–737. Root places this in the active fullscreen host. */
export function Feedback({model}: {model: FeedbackModel}) {
  const state = useSyncExternalStore(model.subscribe, model.getSnapshot, model.getSnapshot), {t} = useLocale();
  const toast = useRef<HTMLDivElement>(null);
  useSurfaceMotion(toast, state.toast ? state.toastOpen ? 'enter' : 'exit' : null, {kind: 'dialog'});
  // biome-ignore lint/correctness/useExhaustiveDependencies: A replacement toast closes menus even when toastOpen stays true.
  useLayoutEffect(() => {
    if (state.toastOpen) closeMainSelectMenus();
  }, [state.toastOpen, state.toastRevision]);
  return <><span id="status" className="visually-hidden" role="status">{localizeKnownError(state.status, t)}</span>
    <div ref={toast} className={`toast${state.toastOpen?' show':''}`} id="toast" role="status" aria-live="polite" aria-hidden={!state.toastOpen} inert={!state.toastOpen}><span className="toast-copy" id="toastText">{localizeKnownError(state.toast, t)}</span>
      <UiButton variant="ghost" size="icon-sm" className="toast-close max-[780px]:size-11 pointer-coarse:size-11" id="toastClose" type="button" aria-label={t('dialog.closeNotice')} title={t('dialog.closeNotice')} onClick={model.dismiss}><X aria-hidden="true"/></UiButton>
    </div></>;
}
