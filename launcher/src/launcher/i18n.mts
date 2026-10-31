import {m} from '../paraglide/messages.js';
import manifest from '../../messages/keys.json' with {type: 'json'};

export const UI_LOCALES = Object.freeze(['zh-CN', 'en', 'ja'] as const);
export const UI_LOCALE_STORAGE_KEY = 'eagler-touhou-ui-locale-v1';
export type UiLocale = typeof UI_LOCALES[number];
export type UiMessageKey = keyof typeof manifest;
export type UiMessageParams = Readonly<Record<string, unknown>>;
const entryKeys = new Set(Object.keys(manifest));
type CompiledMessage = (inputs: Record<string, string>, options: {locale: UiLocale}) => string;
const compiled = m as unknown as Record<string, CompiledMessage>;

/** Compatibility keys only. Paraglide owns all translation and interpolation. */
export function translateUi(locale: UiLocale, key: UiMessageKey, params: UiMessageParams = {}): string {
  const entry = manifest[key];
  if (!entry || !compiled[entry.id]) {
    globalThis.console?.warn?.(`Missing UI translation: ${String(key)}`);
    return String(key);
  }
  const inputs = Object.fromEntries(entry.params.map(name => [name, String(params[name] ?? `{${name}}`)]));
  return compiled[entry.id]!(inputs, {locale});
}
const catalogs: Partial<Record<UiLocale, Readonly<Record<UiMessageKey, string>>>> = {};
function catalog(locale: UiLocale) {
  return catalogs[locale] ??= Object.freeze(Object.fromEntries([...entryKeys].map(key => [key, translateUi(locale, key as UiMessageKey)]))) as Readonly<Record<UiMessageKey, string>>;
}
// Legacy inspection API is lazy; production translation calls compiled functions.
export const UI_MESSAGES = Object.freeze({get 'zh-CN'() {return catalog('zh-CN');}, get en() {return catalog('en');}, get ja() {return catalog('ja');}});
export function localeHome(locale: UiLocale) {return locale === 'ja' ? 'ja.html' : locale === 'en' ? 'en.html' : './';}

let currentLocale: UiLocale = "zh-CN";
let bound = false;

function safeStorage(): Storage | null { try { return globalThis.localStorage; } catch { return null; } }
export function isUiLocale(value: unknown): value is UiLocale {
  return value === "zh-CN" || value === "en" || value === "ja";
}
export function isUiMessageKey(value: unknown): value is UiMessageKey {
  return typeof value === "string" && entryKeys.has(value as UiMessageKey);
}
export function resolveUiLocale(value: unknown): UiLocale { return /^ja(?:-|$)/i.test(String(value || "")) ? "ja" : /^zh(?:-|$)/i.test(String(value || "")) ? "zh-CN" : "en"; }
export function detectUiLocale(): UiLocale {
  // A published document owns its language. Browser preferences must not turn
  // the same crawlable URL into a different language after hydration.
  if (typeof location !== 'undefined') {
    const explicit = new URLSearchParams(location.search).get('lang');
    if (isUiLocale(explicit)) return explicit;
    if (/\/ja\.html\/?$/.test(location.pathname)) return 'ja';
    if (/\/en\.html\/?$/.test(location.pathname)) return 'en';
  }
  // The shared lobby document has no per-locale path; restore its saved UI.
  if (typeof location !== 'undefined' && /\/(?:lobby|lobby\.html)\/?$/.test(location.pathname)) {
    try {const saved = safeStorage()?.getItem(UI_LOCALE_STORAGE_KEY); if (isUiLocale(saved)) return saved;} catch {}
  }
  const documentLocale = globalThis.document?.documentElement?.dataset.uiLocale;
  if (isUiLocale(documentLocale)) return documentLocale;
  try { const saved = safeStorage()?.getItem(UI_LOCALE_STORAGE_KEY); if (isUiLocale(saved)) return saved; } catch {}
  for (const value of globalThis.navigator?.languages || [globalThis.navigator?.language]) if (value) return resolveUiLocale(value);
  return "zh-CN";
}
export function getUiLocale(): UiLocale { return currentLocale; }
export function t(key: UiMessageKey, params: UiMessageParams = {}): string {
  return translateUi(currentLocale, key, params);
}
function translatedValue(key: string | undefined): string | null {
  if (!isUiMessageKey(key)) {
    if (key) globalThis.console?.warn?.(`Missing UI translation: ${key}`);
    return null;
  }
  return t(key);
}
export function applyStaticTranslations(root: ParentNode | null | undefined = globalThis.document): void {
  if (!root?.querySelectorAll) return;
  for (const element of root.querySelectorAll<HTMLElement>("[data-i18n]")) {
    const value = translatedValue(element.dataset.i18n);
    if (value != null) element.textContent = value;
  }
  const bindings = [["aria-label", "i18nAriaLabel"], ["title", "i18nTitle"], ["placeholder", "i18nPlaceholder"], ["alt", "i18nAlt"], ["content", "i18nContent"]] as const;
  for (const [attribute, property] of bindings) {
    const selector = `[data-${property.replace(/[A-Z]/g, value => `-${value.toLowerCase()}`)}]`;
    for (const element of root.querySelectorAll<HTMLElement>(selector)) {
      const value = translatedValue(element.dataset[property]);
      if (value != null) element.setAttribute(attribute, value);
    }
  }
}
export function setUiLocale(value: unknown, { persist = true, notify = true }: { persist?: boolean; notify?: boolean } = {}): UiLocale {
  currentLocale = resolveUiLocale(value);
  if (persist) { try { safeStorage()?.setItem(UI_LOCALE_STORAGE_KEY, currentLocale); } catch {} }
  if (typeof document !== "undefined") {
    document.documentElement.lang = currentLocale;
    document.documentElement.dataset.uiLocale = currentLocale;
    const select = document.querySelector<HTMLSelectElement>("#uiLanguageSelect");
    if (select) select.value = currentLocale;
    applyStaticTranslations(document);
    if (notify && typeof location !== "undefined") {
      const target = new URL(location.pathname.endsWith("/lobby.html") ? "lobby.html" : localeHome(currentLocale), location.href);
      target.search = location.search;
      target.hash = location.hash;
      // Keep the running game and history state when switching UI language.
      history.replaceState(history.state, "", target);
      const canonical = document.querySelector<HTMLLinkElement>('link[rel="canonical"]');
      if (canonical) canonical.href = new URL(localeHome(currentLocale), canonical.href).href;
    }
    const alternate = document.querySelector<HTMLAnchorElement>("#uiLanguageLink");
    if (alternate) {
      alternate.href = currentLocale === "en" ? "./" : "en.html";
      alternate.hreflang = currentLocale === "en" ? "zh-CN" : "en";
      alternate.textContent = currentLocale === "en" ? "简体中文" : "English";
    }
  }
  if (notify && typeof window !== "undefined") window.dispatchEvent(new CustomEvent("eagler-ui-locale-change", { detail: { locale: currentLocale } }));
  return currentLocale;
}
export function initUiLocale(): UiLocale {
  setUiLocale(detectUiLocale(), { persist: false, notify: false });
  const select = document.querySelector<HTMLSelectElement>("#uiLanguageSelect");
  if (select && !bound) {
    bound = true;
    select.addEventListener("change", () => setUiLocale(select.value));
  }
  return currentLocale;
}
export function validateUiCatalogs(): UiMessageKey[] {
  const keys = [...entryKeys] as UiMessageKey[];
  for (const key of keys) for (const locale of UI_LOCALES) {
    if (!translateUi(locale, key)) throw new Error(`Incomplete UI translation: ${locale}/${key}`);
  }
  return keys;
}
