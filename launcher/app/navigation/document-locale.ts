import {useSyncExternalStore} from 'react';
import {useLocation} from 'react-router';
import {isUiLocale, localeFromPath, type UiLocale} from '../../src/launcher/i18n.mts';

function subscribeLocale(listener: () => void) {
  window.addEventListener('eagler-ui-locale-change', listener);
  return () => window.removeEventListener('eagler-ui-locale-change', listener);
}

export function useDocumentUiLocale(): UiLocale {
  const {pathname, search} = useLocation();
  const explicit = new URLSearchParams(search).get('lang');
  const publishedLocale = isUiLocale(explicit) ? explicit : localeFromPath(pathname);
  return useSyncExternalStore(subscribeLocale, () => {
    if (isUiLocale(explicit) || /\/(?:en|ja)\.html\/?$/.test(pathname)) return publishedLocale;
    const value = document.documentElement.dataset.uiLocale;
    return isUiLocale(value) ? value : publishedLocale;
  }, () => publishedLocale);
}
