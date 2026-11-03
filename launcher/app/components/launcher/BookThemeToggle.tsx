import {Button as UiButton} from '../../ui/button';
import {useEffect, useState} from 'react';
import {useLocale} from '../../i18n';
import {BOOK_PREFERENCES_EVENT, readBookPreference, writeBookPreference} from './book-preferences';
import {NavigationIcon} from './NavigationIcon';
import {impressTheme} from './ink-feedback';

export function BookThemeToggle({interactive = true}: {interactive?: boolean}) {
  const {t} = useLocale();
  const [dark, setDark] = useState(true);
  const [ready, setReady] = useState(false);
  useEffect(() => {
    const refresh = () => {
      const value = readBookPreference('theme', 'dark', (item): item is 'light' | 'dark' => item === 'light' || item === 'dark');
      setDark(value === 'dark'); setReady(true); document.documentElement.dataset.bookTheme = value;
    };
    refresh(); window.addEventListener('storage', refresh); window.addEventListener(BOOK_PREFERENCES_EVENT, refresh);
    return () => {window.removeEventListener('storage', refresh); window.removeEventListener(BOOK_PREFERENCES_EVENT, refresh);};
  }, []);
  return <UiButton type="button" variant="ghost" size="icon" className="masthead-action book-theme-toggle [&_svg]:size-5" disabled={!interactive || !ready} aria-pressed={dark}
    aria-label={t('theme.dark')} onClick={(event) => {
      impressTheme(event.currentTarget);
      const next = !dark; setDark(next); document.documentElement.dataset.bookTheme = next ? 'dark' : 'light'; writeBookPreference('theme', next ? 'dark' : 'light');
    }} title={t('theme.toggle')}><NavigationIcon name={dark ? 'moon' : 'sun'}/></UiButton>;
}
