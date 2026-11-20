import {isUiLocale, localeHome, UI_LOCALE_STORAGE_KEY, withUiLanguage, type UiLocale} from '../../src/launcher/i18n.mts';

const regionLocaleKey = 'eagler-touhou-region-locale-v1';
const chineseRegions = new Set(['CN', 'HK', 'MO', 'TW']);

export function localeForCountry(country: string): UiLocale {
  return chineseRegions.has(country) ? 'zh-CN' : country === 'JP' ? 'ja' : 'en';
}

interface InitialLocaleOptions {
  href: string;
  base: string;
  storage: Storage | null;
  fetchCountry(url: URL, init: RequestInit): Promise<Response>;
  online: boolean;
  timeoutMs?: number;
}

/** Cold entry only. Explicit URLs and user choices never need the network. */
export async function initialUiLocaleAddress({href, base, storage, fetchCountry, online, timeoutMs = 550}: InitialLocaleOptions) {
  const address = new URL(href), mount = new URL(base, address);
  if (address.origin !== mount.origin || !address.pathname.startsWith(mount.pathname)) return;
  const route = address.pathname.slice(mount.pathname.length).replace(/\/$/, '');
  if (isUiLocale(address.searchParams.get('lang')) || /^(?:en|ja|ui-review)\.html$/.test(route)) return;

  const saved = (key: string) => {
    try {const value = storage?.getItem(key); if (isUiLocale(value)) return value;} catch {}
  };
  let locale = saved(UI_LOCALE_STORAGE_KEY) ?? saved(regionLocaleKey);
  if (!locale && online) {
    const pending = new AbortController();
    const timer = setTimeout(() => pending.abort(), timeoutMs);
    try {
      const response = await fetchCountry(new URL('_eagler/ui-country', mount), {
        cache: 'no-store', credentials: 'omit', mode: 'same-origin', redirect: 'error', signal: pending.signal,
      });
      const country = response.ok ? (await response.text()).trim().toUpperCase() : '';
      if (/^[A-Z]{2}$/.test(country) && country !== 'ZZ') {
        locale = localeForCountry(country);
        try {storage?.setItem(regionLocaleKey, locale);} catch {}
      }
    } catch {
      // The static site also works when this optional Nginx endpoint is unavailable.
    } finally {
      clearTimeout(timer);
    }
  }
  locale = saved(UI_LOCALE_STORAGE_KEY) ?? locale ?? 'en';
  if (!route || route === 'index.html') {
    if (locale !== 'zh-CN') return new URL(localeHome(locale) + address.search + address.hash, mount);
    return;
  }
  if (locale !== 'zh-CN') return new URL(withUiLanguage(address.href, locale));
}
