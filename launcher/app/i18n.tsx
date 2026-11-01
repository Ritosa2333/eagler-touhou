import { createContext, type ReactNode, useContext, useMemo } from 'react';
import {
  isUiMessageKey,
  translateUi,
  type UiLocale,
  type UiMessageKey,
  type UiMessageParams,
} from '../src/launcher/i18n.mts';

export type Translate = (key: UiMessageKey, params?: UiMessageParams) => string;

/** Paraglide compiled messages serve React and the legacy compatibility port. */
export const translate: (locale: UiLocale, key: UiMessageKey, params?: UiMessageParams) => string = translateUi;

const LocaleContext = createContext<{ locale: UiLocale; t: Translate }>({
  locale: 'zh-CN',
  t: (key, params) => translate('zh-CN', key, params),
});

export function LocaleProvider({ locale, children }: { locale: UiLocale; children: ReactNode }) {
  const value = useMemo(
    () => ({ locale, t: (key: UiMessageKey, params?: UiMessageParams) => translate(locale, key, params) }),
    [locale],
  );
  return <LocaleContext.Provider value={value}>{children}</LocaleContext.Provider>;
}

export function useLocale() {
  return useContext(LocaleContext);
}

/** Original game titles remain artwork-led; Japanese UI uses native titles. */
export function localizedGameTitle(locale: UiLocale, gameId: string, fallback: string) {
  const key = `game.name.${gameId}`;
  return locale === 'ja' && isUiMessageKey(key) ? translateUi(locale, key) : fallback;
}
