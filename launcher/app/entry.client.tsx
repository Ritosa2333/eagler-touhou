import {startTransition, StrictMode} from 'react';
import {hydrateRoot} from 'react-dom/client';
import {HydratedRouter} from 'react-router/dom';
import {initialUiLocaleAddress} from './navigation/initial-ui-locale';

async function start() {
  const href = window.location.href;
  let storage: Storage | null = null;
  try {storage = window.localStorage;} catch {}
  const target = await initialUiLocaleAddress({
    href, base: import.meta.env.BASE_URL, storage, online: navigator.onLine,
    fetchCountry: (url, options) => window.fetch(url, options),
  });
  if (target && window.location.href === href) {
    if (target.pathname !== window.location.pathname) {
      window.location.replace(target.href);
      return;
    }
    window.history.replaceState(window.history.state, '', target.href);
  }
  startTransition(() => {hydrateRoot(document, <StrictMode><HydratedRouter/></StrictMode>);});
}
void start();
