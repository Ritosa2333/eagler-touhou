import { renderContentFragment } from './content-fragment.mts';
import { translateUi, type UiLocale, type UiMessageKey } from './i18n.mts';

export type AuthoredContent = 'faq' | 'firstUse' | 'multiplayer' | 'about';

/** Rich copy is compiled by Paraglide and passes through the same sanitizer as fetched content. */
export function renderLocalizedContent(target: HTMLElement, kind: AuthoredContent, locale: UiLocale): void {
  renderContentFragment(target, translateUi(locale, `content.${kind}` as UiMessageKey), target.ownerDocument);
  if (locale !== 'zh-CN')
    for (const link of target.querySelectorAll<HTMLAnchorElement>('a[href]')) {
      const href = new URL(link.href, target.ownerDocument.baseURI);
      if (
        href.origin === new URL(target.ownerDocument.baseURI).origin &&
        href.pathname.endsWith('/compatibility.html')
      ) {
        href.pathname = href.pathname.replace(/compatibility\.html$/, `compatibility.${locale}.html`);
        link.href = href.href;
      }
    }
}

/** Help links carry locale even on a first visit with no saved preference. */
export function localizedContentHref(href: string, locale: UiLocale): string {
  const [path, hash] = href.split('#', 2);
  const localized = /([?&])lang=[^&]*/.test(path)
    ? path.replace(/([?&])lang=[^&]*/, `$1lang=${locale}`)
    : `${path}${path.includes('?') ? '&' : '?'}lang=${locale}`;
  return localized + (hash === undefined ? '' : `#${hash}`);
}
