import {startTransition, StrictMode} from 'react';
import {hydrateRoot} from 'react-dom/client';
import {HydratedRouter} from 'react-router/dom';
import {initialUiLocaleAddress} from './navigation/initial-ui-locale';

function start() {
  const href = window.location.href;
  let storage: Storage | null = null;
  try {storage = window.localStorage;} catch {}
  const target = initialUiLocaleAddress({href, base: import.meta.env.BASE_URL, storage, document});
  if (target) {
    if (target.pathname !== window.location.pathname) {
      window.location.replace(target.href);
      return;
    }
    window.history.replaceState(window.history.state, '', target.href);
  }
  startTransition(() => {hydrateRoot(document, <StrictMode><HydratedRouter/></StrictMode>);});
}
start();
