import {isUiLocale, localeHome, readUiLocalePreference, withUiLanguage} from '../../src/launcher/i18n.mts';

export const legacyRegionalLocaleKey = 'eagler-touhou-region-locale-v1';

interface InitialLocaleOptions {
  href: string;
  base: string;
  storage: Storage | null;
  document?: Pick<Document, 'cookie'>;
  reload?: boolean;
}

/** A plain entry uses the Chinese document unless the user saved a language. */
export function initialUiLocaleAddress({href, base, storage, document, reload = false}: InitialLocaleOptions) {
  const address = new URL(href), mount = new URL(base, address);
  if (address.origin !== mount.origin || !address.pathname.startsWith(mount.pathname)) return;
  const route = address.pathname.slice(mount.pathname.length).replace(/\/$/, '');
  if (isUiLocale(address.searchParams.get('lang')) || route === 'ui-review.html') return;

  const locale = readUiLocalePreference(document, storage);
  if (route === 'en.html' || route === 'ja.html') {
    // Old builds navigated to the regional language document even for a plain entry.
    // A fresh localized link and a saved manual language still take precedence.
    if (reload && !locale) {
      try {
        if (storage?.getItem(legacyRegionalLocaleKey) === route.slice(0, 2)) {
          const target = new URL(mount);
          target.search = address.search;
          target.hash = address.hash;
          return target;
        }
      } catch {}
    }
    return;
  }
  if (!locale || locale === 'zh-CN') return;
  if (!route || route === 'index.html') {
    return new URL(localeHome(locale) + address.search + address.hash, mount);
  }
  return new URL(withUiLanguage(address.href, locale));
}
