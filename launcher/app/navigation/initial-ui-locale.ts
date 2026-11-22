import {isUiLocale, localeHome, readUiLocalePreference, withUiLanguage, type UiLocale} from '../../src/launcher/i18n.mts';

const regionLocaleKey = 'eagler-touhou-region-locale-v1';
const chineseRegions = new Set(['CN', 'HK', 'MO', 'TW']);

export function localeForCountry(country: string): UiLocale {
  return chineseRegions.has(country) ? 'zh-CN' : country === 'JP' ? 'ja' : 'en';
}

interface InitialLocaleOptions {
  href: string;
  base: string;
  storage: Storage | null;
  document?: Pick<Document, 'cookie'>;
  fetchCountry(url: URL, init: RequestInit): Promise<Response>;
  online: boolean;
  timeoutMs?: number;
}

/** Cold entry only. Explicit URLs and user choices never need the network. */
export async function initialUiLocaleAddress({href, base, storage, document, fetchCountry, online, timeoutMs = 550}: InitialLocaleOptions) {
  const address = new URL(href), mount = new URL(base, address);
  if (address.origin !== mount.origin || !address.pathname.startsWith(mount.pathname)) return;
  const route = address.pathname.slice(mount.pathname.length).replace(/\/$/, '');
  if (isUiLocale(address.searchParams.get('lang')) || /^(?:en|ja|ui-review)\.html$/.test(route)) return;

  let locale = readUiLocalePreference(document, storage);
  if (!locale) {
    try {const cached = storage?.getItem(regionLocaleKey); if (isUiLocale(cached)) locale = cached;} catch {}
  }
  if (!locale) {
    locale = online ? await firstVisitLocale(mount, fetchCountry, timeoutMs) : 'en';
    try {storage?.setItem(regionLocaleKey, locale);} catch {}
    locale = readUiLocalePreference(document, storage) ?? locale;
  }
  if (!route || route === 'index.html') {
    if (locale !== 'zh-CN') return new URL(localeHome(locale) + address.search + address.hash, mount);
    return;
  }
  if (locale !== 'zh-CN') return new URL(withUiLanguage(address.href, locale));
}

async function firstVisitLocale(mount: URL, fetchCountry: InitialLocaleOptions['fetchCountry'], timeoutMs: number): Promise<UiLocale> {
  const pending = new AbortController();
  const timer = setTimeout(() => pending.abort(), timeoutMs);
  try {
    const response = await fetchCountry(new URL('_eagler/ui-country', mount), {
      cache: 'no-store', credentials: 'omit', mode: 'same-origin', redirect: 'error', signal: pending.signal,
    });
    const country = response.ok ? (await response.text()).trim().toUpperCase() : '';
    return localeForCountry(country);
  } catch {
    return 'en';
  } finally {
    clearTimeout(timer);
  }
}
