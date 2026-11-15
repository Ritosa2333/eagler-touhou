import {createContext, useContext, useMemo, type ReactNode} from 'react';
import {translateUi, type UiLocale, type UiMessageKey, type UiMessageParams} from '../src/launcher/i18n.mts';

export type Translate = (key: UiMessageKey, params?: UiMessageParams) => string;
export type {UiMessageKey, UiMessageParams};
export interface UiMessage {key: UiMessageKey; params?: UiMessageParams;}

/** React supplies locale explicitly; its provider owns reactivity and navigation. */
export function translate(locale: UiLocale, key: UiMessageKey, params: UiMessageParams = {}): string {
  return translateUi(locale, key, params);
}

const LocaleContext = createContext<{locale: UiLocale; t: Translate}>({
  locale: 'zh-CN', t: (key, params) => translate('zh-CN', key, params),
});

export function LocaleProvider({locale, children}: {locale: UiLocale; children: ReactNode}) {
  const value = useMemo(() => ({locale, t: (key: UiMessageKey, params?: UiMessageParams) => translate(locale, key, params)}), [locale]);
  return <LocaleContext.Provider value={value}>{children}</LocaleContext.Provider>;
}

export function useLocale() {return useContext(LocaleContext);}
