import {Button as UiButton} from '../../ui/controls';
import {useLayoutEffect, useSyncExternalStore} from 'react';
import {useLocale} from '../../i18n';
import type {StartupErrorModel} from '../../models/startup-error';
import {closeMainSelectMenus} from '../launcher/MainSelect';
import {localizeKnownError} from '../../../src/launcher/localized-error-text.mts';
/** Exact main index738–745. Root supplies its fullscreen transient host. */
export function StartupError({model}: {model: StartupErrorModel}) {
  const {t} = useLocale(), state = useSyncExternalStore(model.subscribe, model.getSnapshot, model.getSnapshot);
  const reason = localizeKnownError(state.reason, t);
  useLayoutEffect(() => {if (state.open) closeMainSelectMenus();}, [state]);
  return <section className="startup-error" data-window-transition="" id="startupError" role="alertdialog" aria-modal="false" aria-labelledby="startupErrorTitle" aria-describedby="startupErrorText" hidden={!state.open} inert={!state.open}>
    <header><strong id="startupErrorTitle">{t('dialog.errorTitle')}</strong><UiButton variant="icon" className="startup-error-close" id="startupErrorClose" type="button" aria-label={t('dialog.closeError')} title={t('dialog.closeError')} onClick={model.close}>×</UiButton></header>
    {reason !== state.reason && <p className="startup-error-reason">{reason}</p>}
    <pre className="startup-error-detail"><code id="startupErrorText">{state.detail}</code></pre><footer><UiButton className="startup-error-copy" id="startupErrorCopy" type="button" onClick={() => {void model.copy();}}>{t('dialog.copyError')}</UiButton></footer>
  </section>;
}
