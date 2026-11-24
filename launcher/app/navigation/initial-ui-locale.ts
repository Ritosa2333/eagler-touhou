import {isUiLocale, localeHome, readUiLocalePreference, withUiLanguage} from '../../src/launcher/i18n.mts';

interface InitialLocaleOptions {
  href: string;
  base: string;
  storage: Storage | null;
  document?: Pick<Document, 'cookie'>;
}

/** A plain entry uses the Chinese document unless the user saved a language. */
export function initialUiLocaleAddress({href, base, storage, document}: InitialLocaleOptions) {
  const address = new URL(href), mount = new URL(base, address);
  if (address.origin !== mount.origin || !address.pathname.startsWith(mount.pathname)) return;
  const route = address.pathname.slice(mount.pathname.length).replace(/\/$/, '');
  if (isUiLocale(address.searchParams.get('lang')) || /^(?:en|ja|ui-review)\.html$/.test(route)) return;

  const locale = readUiLocalePreference(document, storage);
  if (!locale || locale === 'zh-CN') return;
  if (!route || route === 'index.html') {
    return new URL(localeHome(locale) + address.search + address.hash, mount);
  }
  return new URL(withUiLanguage(address.href, locale));
}
