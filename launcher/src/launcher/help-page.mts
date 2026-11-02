import {isUiLocale, localeHome, UI_LOCALE_STORAGE_KEY, withUiLanguage, translateUi, type UiLocale} from './i18n.mjs';
import {renderLocalizedContent, type AuthoredContent} from './localized-content.mjs';

/** Standalone help uses the same messages and sanitizer as in-app help. */
export function initializeHelpPage(document: Document): () => void {
  const window = document.defaultView;
  const kind = document.body.dataset.helpPage as AuthoredContent | undefined;
  const target = document.querySelector<HTMLElement>('[data-localized-content]');
  if (!window || !target || (kind !== 'faq' && kind !== 'about')) return () => {};
  const source = target.innerHTML;
  const authored = document.createElement('template');
  authored.innerHTML = translateUi('zh-CN', `content.${kind}`);
  const known = source.trim() === authored.innerHTML.trim();
  const readLocale = (): UiLocale => {
    const explicit = new URL(window.location.href).searchParams.get('lang');
    if (isUiLocale(explicit)) return explicit;
    try {const saved = window.localStorage.getItem(UI_LOCALE_STORAGE_KEY); if (isUiLocale(saved)) return saved;} catch {}
    return 'zh-CN';
  };
  const render = () => {
    const locale = readLocale(), t = (key: Parameters<typeof translateUi>[1]) => translateUi(locale, key);
    renderLocalizedContent(target, kind, locale, known ? undefined : source);
    document.documentElement.lang = locale;
    document.documentElement.dataset.uiLocale = locale;
    document.title = t(kind === 'faq' ? 'content.faqTitle' : 'content.aboutTitle');
    document.querySelector('meta[name="description"]')?.setAttribute('content', t(kind === 'faq' ? 'content.faqDescription' : 'content.aboutDescription'));
    if (kind === 'faq') target.setAttribute('aria-label', t('content.faqAria'));
    const back = document.querySelector<HTMLAnchorElement>('.about-back a');
    if (back) {back.textContent = t('content.back'); back.href = new URL(localeHome(locale), window.location.href).href;}
    const languages = document.querySelector<HTMLElement>('.about-languages');
    languages?.setAttribute('aria-label', t('ui.language'));
    for (const link of languages?.querySelectorAll<HTMLAnchorElement>('a[lang]') ?? []) {
      const language = link.lang;
      if (!isUiLocale(language)) continue;
      link.href = withUiLanguage(window.location.href, language);
      if (language === locale) link.setAttribute('aria-current', 'page'); else link.removeAttribute('aria-current');
    }
    const navigation = document.querySelector<HTMLElement>('.about-navigation');
    navigation?.setAttribute('aria-label', t('nav.help'));
    for (const link of navigation?.querySelectorAll<HTMLAnchorElement>('[data-help-link]') ?? []) {
      const key = link.dataset.helpLink;
      if (key === 'nav.faq' || key === 'nav.about' || key === 'nav.serviceStatus') {
        link.textContent = t(key);
        link.href = withUiLanguage(link.href, locale);
      }
    }
  };
  const storage = (event: StorageEvent) => {if (event.key === UI_LOCALE_STORAGE_KEY || event.key === null) render();};
  render();
  window.addEventListener('popstate', render);
  window.addEventListener('storage', storage);
  return () => {window.removeEventListener('popstate', render); window.removeEventListener('storage', storage);};
}
