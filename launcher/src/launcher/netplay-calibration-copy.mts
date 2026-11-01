import { translateUi, type UiLocale, type UiMessageKey } from './i18n.mts';

/** UI copy uses Paraglide; boolean callers retain their original locale contract. */
export function calibrationConnectionCopy(progress: Readonly<Record<string, unknown>>, language: boolean | UiLocale) {
  const locale = typeof language === 'boolean' ? (language ? 'en' : 'zh-CN') : language;
  const t = (key: UiMessageKey, params: Record<string, unknown> = {}) => translateUi(locale, key, params);
  const phase = String(progress.phase);
  const titles: Record<string, UiMessageKey> = {
    ready: 'calibration.ready',
    unavailable: 'calibration.unavailable',
    suspended: 'calibration.suspended',
    retrying: 'calibration.retrying',
    waiting: 'calibration.waiting',
    stabilizing: 'calibration.stabilizing',
    negotiating: 'calibration.negotiating',
  };
  const summary =
    phase === 'unavailable'
      ? t(Number(progress.reason) === 8 ? 'calibration.delayHigh' : 'calibration.failed')
      : phase === 'suspended'
        ? t('calibration.keepOpen')
        : phase === 'retrying'
          ? t('calibration.attempt', { attempt: progress.attempt ?? 1, maximum: progress.maxAttempts ?? 4 })
          : phase === 'stabilizing'
            ? t('calibration.waitSecond')
            : phase === 'waiting'
              ? t('calibration.waitChannels')
              : phase === 'negotiating'
                ? t('calibration.waitConfirm')
                : t('calibration.probes', { probes: progress.probes ?? 0, replies: progress.replies ?? 0 });
  return {
    title: t(titles[phase] ?? 'calibration.measuring'),
    summary,
    delay: t('calibration.delay', { frames: progress.inputDelay }),
    rollback: t('calibration.rollback', {
      state: t(Number(progress.adonisMode) === 2 ? 'calibration.on' : 'calibration.off'),
    }),
    columns: [t('calibration.player'), t('calibration.latency'), t('calibration.maximum')],
    note: t('calibration.note'),
    dismiss: t('calibration.dismiss'),
    returnToRoom: t('netplay.return'),
  };
}
