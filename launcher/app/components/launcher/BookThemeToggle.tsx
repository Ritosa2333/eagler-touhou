import {useEffect, useState} from 'react';
import {useLocale} from '../../i18n';
import {BOOK_PREFERENCES_EVENT, readBookPreference, writeBookPreference} from './book-preferences';

export function BookThemeToggle({interactive = true}: {interactive?: boolean}) {
  const {locale} = useLocale();
  const [dark, setDark] = useState(true);
  useEffect(() => {
    const refresh = () => {
      const value = readBookPreference('theme', 'dark', (item): item is 'light' | 'dark' => item === 'light' || item === 'dark');
      setDark(value === 'dark'); document.documentElement.dataset.bookTheme = value;
    };
    refresh(); window.addEventListener('storage', refresh); window.addEventListener(BOOK_PREFERENCES_EVENT, refresh);
    return () => {window.removeEventListener('storage', refresh); window.removeEventListener(BOOK_PREFERENCES_EVENT, refresh);};
  }, []);
  return <button type="button" className="book-theme-toggle" disabled={!interactive} aria-pressed={dark}
    aria-label={locale === 'en' ? 'Dark appearance' : '深色外观'} onClick={() => {
      const next = !dark; setDark(next); document.documentElement.dataset.bookTheme = next ? 'dark' : 'light'; writeBookPreference('theme', next ? 'dark' : 'light');
    }}>{dark ? '☾' : '☼'}</button>;
}
