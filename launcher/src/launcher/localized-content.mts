import {renderContentFragment} from './content-fragment.mjs';
import {translateUi, withUiLanguage, type UiLocale} from './i18n.mjs';

export type AuthoredContent = 'faq' | 'firstUse' | 'multiplayer' | 'about';
const normalize = (html: string) => html.replace(/\r\n?/g, '\n').trim();

/** Local help preserves language; external links and in-page anchors stay intact. */
export function localizeHelpLinks(target: HTMLElement, locale: UiLocale): void {
  const base = new URL(target.ownerDocument.baseURI);
  for (const link of target.querySelectorAll<HTMLAnchorElement>('a[href]')) {
    const original = link.getAttribute('href')!;
    if (original.startsWith('#')) continue;
    const href = new URL(original, base);
    if (href.origin !== base.origin) continue;
    if (href.pathname === base.pathname && href.search === base.search && href.hash) continue;
    if (/\/compatibility(?:\.(?:en|ja))?\.html$/.test(href.pathname)) {
      href.pathname = href.pathname.replace(/compatibility(?:\.(?:en|ja))?\.html$/, `compatibility${locale === 'zh-CN' ? '' : '.' + locale}.html`);
      link.href = href.href;
    } else if (/\/(?:faq|about|guide|multiplayer|lobby|profile|login)\.html$/.test(href.pathname)) link.href = withUiLanguage(href.href, locale);
  }
}

/** Only the matching authored source selects a translation. */
export function resolveLocalizedContent(kind: AuthoredContent, locale: UiLocale, sourceHtml?: string) {
  const key = `content.${kind}` as const;
  if (sourceHtml !== undefined && normalize(sourceHtml) !== normalize(translateUi('zh-CN', key))) {
    return {html: sourceHtml, locale: 'zh-CN' as const};
  }
  return {html: translateUi(locale, key), locale};
}

export function renderLocalizedContent(target: HTMLElement, kind: AuthoredContent, locale: UiLocale, sourceHtml?: string): UiLocale {
  const content = resolveLocalizedContent(kind, locale, sourceHtml);
  renderContentFragment(target, content.html, target.ownerDocument);
  target.lang = content.locale;
  localizeHelpLinks(target, locale);
  return content.locale;
}
