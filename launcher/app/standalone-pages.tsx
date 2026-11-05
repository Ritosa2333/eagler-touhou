import {useEffect, useRef, useState, useSyncExternalStore, type ReactNode} from 'react';
import {useHref, useLocation, useNavigate} from 'react-router';
import {LauncherMasthead} from './components/launcher/LauncherMasthead';
import {MainSelect} from './components/launcher/MainSelect';
import {DonationDialog} from './components/notices/DonationDialog';
import {FirstUseNotice, type FirstUseNoticeHandle} from './components/notices/FirstUseNotice';
import {LocaleProvider, useLocale, type UiMessageKey} from './i18n';
import {useDocumentUiLocale} from './navigation/document-locale';
import {UI_LOCALES, UI_LOCALE_LABELS, UI_LOCALE_STORAGE_KEY, isUiLocale, localeHome, withUiLanguage} from '../src/launcher/i18n.mts';
import {createSitePreferencesModel, bindSitePreferencesToDocument} from './models/site-preferences';
import {RUNTIME_DIAGNOSTICS_STORAGE_KEY} from './models/runtime-diagnostics';
import {validateSiteManifest, siteOriginMigrationAvailable} from '../src/contracts/site-manifest.mts';

function localStorageOrNull() {try {return window.localStorage;} catch {return null;}}

/** Read-only site pages share controls and preferences, without creating a game session. */
export function StandalonePageShell({children, titleKey = 'nav.serviceStatus', mainClassName = 'uptime-page'}: {children: ReactNode; titleKey?: UiMessageKey; mainClassName?: string}) {
  return <LocaleProvider locale={useDocumentUiLocale()}><StandaloneShell titleKey={titleKey} mainClassName={mainClassName}>{children}</StandaloneShell></LocaleProvider>;
}

function StandaloneShell({children, titleKey, mainClassName}: {children: ReactNode; titleKey: UiMessageKey; mainClassName: string}) {
  const {locale, t} = useLocale(), location = useLocation(), navigate = useNavigate(), base = useHref('/');
  const assetUrl = (path: string) => base + path;
  const [preferences, setPreferences] = useState(() => createSitePreferencesModel({storage: null}));
  const prefs = useSyncExternalStore(preferences.subscribe, preferences.getSnapshot, preferences.getSnapshot);
  const [menu, setMenu] = useState(false), [donation, setDonation] = useState(false), [notice, setNotice] = useState(false);
  const [donationAvailable, setDonationAvailable] = useState(true), [diagnostics, setDiagnostics] = useState(false);
  const [migration, setMigration] = useState({known: false, available: false});
  const firstUse = useRef<FirstUseNoticeHandle>(null);
  useEffect(() => {
    setPreferences(createSitePreferencesModel({storage: localStorageOrNull()}));
    try {setDiagnostics(localStorageOrNull()?.getItem(RUNTIME_DIAGNOSTICS_STORAGE_KEY) === '1');} catch {}
  }, []);
  useEffect(() => bindSitePreferencesToDocument(preferences), [preferences]);
  useEffect(() => {
    if (new URLSearchParams(location.search).has('lang')) return;
    let saved: unknown; try {saved = localStorageOrNull()?.getItem(UI_LOCALE_STORAGE_KEY);} catch {}
    if (isUiLocale(saved) && saved !== locale) navigate(withUiLanguage(location.pathname + location.search + location.hash, saved), {replace: true});
  }, [location.pathname, location.search, location.hash, locale, navigate]);
  useEffect(() => {
    document.documentElement.lang = locale;
    document.documentElement.dataset.uiLocale = locale;
    document.title = `${t(titleKey)} | EAGLER TOUHOU`;
  }, [locale, t, titleKey]);
  useEffect(() => {
    const controller = new AbortController();
    void fetch(base + 'site-manifest.json', {signal: controller.signal}).then(async response => {
      if (!response.ok) return;
      const site = validateSiteManifest(await response.json());
      if (!controller.signal.aborted) setMigration({known: true, available: siteOriginMigrationAvailable(site, window.location.protocol)});
    }).catch(() => {});
    return () => controller.abort();
  }, [base]);
  const Main=mainClassName==='documentation-shell'?'div':'main';
  return <div className="standalone-page-shell">
    <LauncherMasthead variant="help" nativeLinks compactDocumentation={mainClassName==='documentation-shell'} assetUrl={assetUrl} launcherHref={assetUrl(localeHome(locale))}
      faqHref={withUiLanguage(assetUrl('faq.html'), locale)} aboutHref={withUiLanguage(assetUrl('about.html'), locale)}
      migrationPolicyKnown={migration.known} migrationHref={migration.available ? assetUrl('migrate.html') : undefined}
      menuOpen={menu} onMenuOpenChange={setMenu} lessMotion={prefs.lessMotion} onToggleMotion={preferences.toggleMotion}
      noticeEnabled={prefs.noticeEnabled} onToggleNotice={preferences.toggleNotice} diagnosticsEnabled={diagnostics}
      onToggleDiagnostics={() => {const next = !diagnostics; try {localStorageOrNull()?.setItem(RUNTIME_DIAGNOSTICS_STORAGE_KEY, next ? '1' : '0');} catch {} setDiagnostics(next);}}
      onDonation={() => setDonation(true)} donationAvailable={donationAvailable} onFirstUse={() => {void firstUse.current?.showManual();}}
      languageControl={<MainSelect id="uiLanguageSelect" aria-label={t('ui.language')} value={locale} onChange={event => {
        const value = event.target.value;
        if (!isUiLocale(value)) return;
        try {localStorageOrNull()?.setItem(UI_LOCALE_STORAGE_KEY, value);} catch {}
        navigate(withUiLanguage(location.pathname + location.search + location.hash, value));
      }}>{UI_LOCALES.map(value => <option key={value} value={value}>{t(UI_LOCALE_LABELS[value])}</option>)}</MainSelect>}/>
    <Main className={mainClassName}>{children}</Main>
    <DonationDialog open={donation} onCloseRequest={() => setDonation(false)} assetUrl={assetUrl} onArtworkUnavailable={() => setDonationAvailable(false)}/>
    <FirstUseNotice ref={firstUse} open={notice} onOpenRequest={() => setNotice(true)} onCloseRequest={() => setNotice(false)} contentUrl={assetUrl('content/FIRST_USE_NOTICE.html')} edgeGestures={false}/>
  </div>;
}
