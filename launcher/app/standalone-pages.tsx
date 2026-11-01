import { useEffect, useRef, useState, useSyncExternalStore } from 'react';
import { createRoot } from 'react-dom/client';
import { siteOriginMigrationAvailable, validateSiteManifest } from '../src/contracts/site-manifest.mts';
import { isUiLocale, localeHome, UI_LOCALE_STORAGE_KEY, type UiLocale } from '../src/launcher/i18n.mts';
import { renderLocalizedContent } from '../src/launcher/localized-content.mts';
import { LauncherMasthead } from './components/launcher/LauncherMasthead';
import { MainSelect } from './components/launcher/MainSelect';
import { DonationDialog } from './components/notices/DonationDialog';
import { FirstUseNotice, type FirstUseNoticeHandle } from './components/notices/FirstUseNotice';
import { UptimePage } from './components/uptime/UptimePage';
import { LocaleProvider, useLocale } from './i18n';
import { RUNTIME_DIAGNOSTICS_STORAGE_KEY as diagnosticsKey } from './models/runtime-diagnostics';
import { bindSitePreferencesToDocument, createSitePreferencesModel } from './models/site-preferences';
import './site-navigation.css';
import './components/uptime/uptime.css';

const base = new URL('.', document.baseURI),
  url = (path: string) => new URL(path, base).href;
let storage: Storage | null = null;
try {
  storage = window.localStorage;
} catch {}
const preferences = createSitePreferencesModel({ storage });
bindSitePreferencesToDocument(preferences);
const read = (key: string) => {
  try {
    return storage?.getItem(key);
  } catch {
    return null;
  }
};
const write = (key: string, value: string) => {
  try {
    storage?.setItem(key, value);
  } catch {}
};
function StandaloneHeader() {
  const { locale, t } = useLocale(),
    prefs = useSyncExternalStore(preferences.subscribe, preferences.getSnapshot);
  const [open, setOpen] = useState(false),
    [donation, setDonation] = useState(false),
    [notice, setNotice] = useState(false);
  const [donationAvailable, setDonationAvailable] = useState(true),
    [diagnostics, setDiagnostics] = useState(read(diagnosticsKey) === '1');
  const [migration, setMigration] = useState({ known: false, available: false });
  const firstUse = useRef<FirstUseNoticeHandle>(null);
  useEffect(() => {
    document.documentElement.lang = locale;
    document.documentElement.dataset.uiLocale = locale;
    const page = document.body.dataset.sitePage;
    if (page === 'faq' || page === 'about') {
      const target = document.querySelector<HTMLElement>('[data-localized-content]');
      if (target) renderLocalizedContent(target, page, locale);
      document.title = t(page === 'faq' ? 'content.faqTitle' : 'content.aboutTitle');
      document
        .querySelector('meta[name="description"]')
        ?.setAttribute('content', t(page === 'faq' ? 'content.faqDescription' : 'content.aboutDescription'));
      if (page === 'faq') target?.setAttribute('aria-label', t('content.faqAria'));
      if (page === 'about') target?.firstElementChild?.classList.add('about-back');
      const back = document.querySelector<HTMLAnchorElement>('.about-back a');
      if (back) {
        back.textContent = t('content.back');
        back.href = url(localeHome(locale));
      }
      if (page === 'about') document.querySelector('[data-localized-content] h1')?.classList.add('brand-title');
    }
    if (page === 'uptime') document.title = `${t('nav.serviceStatus')} | EAGLER TOUHOU`;
  }, [locale, t]);
  useEffect(() => {
    const controller = new AbortController();
    void fetch(url('site-manifest.json'), { signal: controller.signal })
      .then(async (r) => {
        if (!r.ok) return;
        const site = validateSiteManifest(await r.json());
        setMigration({ known: true, available: siteOriginMigrationAvailable(site, window.location.protocol) });
      })
      .catch(() => {});
    return () => controller.abort();
  }, []);
  return (
    <>
      <LauncherMasthead
        variant="help"
        nativeLinks
        assetUrl={url}
        launcherHref={url(localeHome(locale))}
        faqHref={url('faq.html')}
        aboutHref={url('about.html')}
        migrationPolicyKnown={migration.known}
        migrationHref={migration.available ? url('migrate.html') : undefined}
        menuOpen={open}
        onMenuOpenChange={setOpen}
        lessMotion={prefs.lessMotion}
        onToggleMotion={preferences.toggleMotion}
        noticeEnabled={prefs.noticeEnabled}
        onToggleNotice={preferences.toggleNotice}
        diagnosticsEnabled={diagnostics}
        onToggleDiagnostics={() => {
          write(diagnosticsKey, diagnostics ? '0' : '1');
          setDiagnostics(!diagnostics);
        }}
        onDonation={() => setDonation(true)}
        donationAvailable={donationAvailable}
        onFirstUse={() => {
          void firstUse.current?.showManual();
        }}
        languageControl={
          <MainSelect
            id="uiLanguageSelect"
            className="option-select ui-language-select"
            aria-label={t('ui.language')}
            value={locale}
            onChange={(event) => {
              write(UI_LOCALE_STORAGE_KEY, event.target.value);
              const next = new URL(location.href);
              next.searchParams.set('lang', event.target.value);
              history.replaceState(history.state, '', next);
              window.dispatchEvent(new Event('site-pages-locale'));
            }}
          >
            <option value="zh-CN">{t('ui.language.zhCN')}</option>
            <option value="en">{t('ui.language.en')}</option>
            <option value="ja">{t('ui.language.ja')}</option>
          </MainSelect>
        }
      />
      <DonationDialog
        open={donation}
        onCloseRequest={() => setDonation(false)}
        assetUrl={url}
        onArtworkUnavailable={() => setDonationAvailable(false)}
      />
      <FirstUseNotice
        ref={firstUse}
        open={notice}
        onOpenRequest={() => setNotice(true)}
        onCloseRequest={() => setNotice(false)}
        contentUrl={url('content/FIRST_USE_NOTICE.html')}
        edgeGestures={false}
      />
    </>
  );
}
function readPageLocale(): UiLocale {
  const explicit = new URL(location.href).searchParams.get('lang');
  const saved = read(UI_LOCALE_STORAGE_KEY);
  return isUiLocale(explicit) ? explicit : isUiLocale(saved) ? saved : 'zh-CN';
}
function Page({ kind }: { kind: 'header' | 'uptime' }) {
  const [locale, setLocale] = useState<UiLocale>(() => readPageLocale());
  useEffect(() => {
    const changed = () => setLocale(readPageLocale());
    window.addEventListener('site-pages-locale', changed);
    window.addEventListener('storage', changed);
    return () => {
      window.removeEventListener('site-pages-locale', changed);
      window.removeEventListener('storage', changed);
    };
  }, []);
  return (
    <LocaleProvider locale={locale}>
      {kind === 'header' ? <StandaloneHeader /> : <UptimePage endpoint={url('uptime/summary')} />}
    </LocaleProvider>
  );
}
const header = document.getElementById('site-page-header');
if (header) createRoot(header).render(<Page kind="header" />);
const content = document.getElementById('site-page-content');
if (content && document.body.dataset.sitePage === 'uptime') createRoot(content).render(<Page kind="uptime" />);
