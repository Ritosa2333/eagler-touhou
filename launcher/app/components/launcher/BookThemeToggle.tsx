import {Button as UiButton} from '../../ui/button';
import {useEffect, useRef, useState} from 'react';
import {useLocale} from '../../i18n';
import {BOOK_PREFERENCES_EVENT, readBookPreference, writeBookPreference} from './book-preferences';
import {NavigationIcon} from './NavigationIcon';
import {impressTheme} from './ink-feedback';
import {MastheadTooltip} from './MastheadTooltip';
import {BackgroundSettings, type BackgroundSurface} from './CustomBackground';

let themeTransitionTimer: number | undefined;
let pendingTheme: 'light' | 'dark' | undefined;
let themeTransition: ViewTransition | undefined;
let themeChangeSerial = 0;
function applyTheme(value: 'light' | 'dark', animate: boolean) {
  const root = document.documentElement;
  if ((pendingTheme ?? root.dataset.bookTheme) === value) return;
  const serial = ++themeChangeSerial;
  window.clearTimeout(themeTransitionTimer);
  themeTransition?.skipTransition();
  themeTransition = undefined;
  pendingTheme = value;
  const commit = () => {
    if (serial !== themeChangeSerial) return;
    root.dataset.bookTheme = value;
    root.classList.toggle('dark', value === 'dark');
    root.classList.toggle('light', value === 'light');
    pendingTheme = undefined;
  };
  const reduced = window.matchMedia?.('(prefers-reduced-motion: reduce)').matches || document.body.classList.contains('less-motion');
  if (!animate || reduced || typeof document.startViewTransition !== 'function') {
    root.classList.remove('ui-theme-changing');
    commit();
    return;
  }
  root.classList.add('ui-theme-changing');
  themeTransitionTimer = window.setTimeout(() => {
    if (serial !== themeChangeSerial) return;
    let transition: ViewTransition;
    try {transition = document.startViewTransition(commit);}
    catch {commit(); root.classList.remove('ui-theme-changing'); return;}
    themeTransition = transition;
    void transition.finished.finally(() => {
      if (serial === themeChangeSerial) {root.classList.remove('ui-theme-changing'); themeTransition = undefined;}
    }).catch(() => {});
  }, 180);
}

export function BookThemeToggle({interactive = true, row = false, tooltip = false, surfaces}: {
  interactive?: boolean; row?: boolean; tooltip?: boolean; surfaces?: readonly BackgroundSurface[];
}) {
  const {locale} = useLocale();
  const [dark, setDark] = useState(true);
  const [open, setOpen] = useState(false);
  const trigger = useRef<HTMLButtonElement>(null);
  const [ready, setReady] = useState(false);
  useEffect(() => {
    let initialized = false;
    const refresh = () => {
      const value = readBookPreference('theme', 'dark', (item): item is 'light' | 'dark' => item === 'light' || item === 'dark');
      setDark(value === 'dark'); setReady(true); applyTheme(value, initialized); initialized = true;
    };
    refresh(); window.addEventListener('storage', refresh); window.addEventListener(BOOK_PREFERENCES_EVENT, refresh);
    return () => {window.removeEventListener('storage', refresh); window.removeEventListener(BOOK_PREFERENCES_EVENT, refresh);};
  }, []);
  const label = locale === 'zh-CN' ? '主题设置' : locale === 'ja' ? 'テーマ設定' : 'Theme settings';
  const button = <UiButton ref={trigger} type="button" variant="ghost" size={row ? "row" : "icon"} className={row ? "masthead-overview-theme" : "masthead-action book-theme-toggle [&_svg]:size-5"} disabled={!interactive || !ready}
    aria-label={label} aria-haspopup="dialog" onClick={() => setOpen(true)} title={tooltip ? undefined : label}><NavigationIcon name={dark ? 'moon' : 'sun'}/>{row && <span>{label}</span>}</UiButton>;
  return <>{tooltip ? <MastheadTooltip label={label}>{button}</MastheadTooltip> : button}
    <BackgroundSettings open={open} onOpenChange={setOpen} dark={dark} surfaces={surfaces} onThemeChange={value=>{if ((value==='dark')===dark) return; setDark(value==='dark');applyTheme(value,true);writeBookPreference('theme',value);if(trigger.current)impressTheme(trigger.current);}}/>
  </>;
}
