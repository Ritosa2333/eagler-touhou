import { JSDOM } from 'jsdom';
import { describe, expect, it } from 'vitest';
import { auditUiCopy } from '../../scripts/audit-ui-copy.mjs';
import { renderContentFragment } from '../../src/launcher/content-fragment.mts';
import { translateUi, UI_LOCALES } from '../../src/launcher/i18n.mts';
import { localizedContentHref, renderLocalizedContent } from '../../src/launcher/localized-content.mts';
import { buildRuleGuide } from '../../src/launcher/multiplayer-guide-content.mts';

function documentForContent() {
  return new JSDOM('<!doctype html><div id="content"></div>', { url: 'https://example.test/touhou/' }).window.document;
}
const links = (target: HTMLElement) =>
  [...target.querySelectorAll<HTMLAnchorElement>('a[href]')]
    .map((link) => link.getAttribute('href')!.replace(/compatibility\.(?:en|ja)\.html$/, 'compatibility.html'))
    .sort();

describe('localized rich help content', () => {
  it('carries the entry language to help without relying on saved preferences', () => {
    expect(localizedContentHref('faq.html', 'ja')).toBe('faq.html?lang=ja');
    expect(localizedContentHref('about.html?lang=en#contact', 'ja')).toBe('about.html?lang=ja#contact');
    expect(localizedContentHref('https://example.test/sub/faq.html?x=1#save', 'en')).toBe(
      'https://example.test/sub/faq.html?x=1&lang=en#save',
    );
  });
  it.each(['faq', 'firstUse', 'multiplayer', 'about'] as const)(
    '%s keeps structure and links across all locales',
    (kind) => {
      const counts: number[] = [],
        hrefs: string[][] = [];
      for (const locale of UI_LOCALES) {
        const doc = documentForContent(),
          target = doc.getElementById('content')!;
        renderLocalizedContent(target, kind, locale);
        counts.push(target.querySelectorAll('h1,h2,h3,h4').length);
        hrefs.push(links(target));
        expect(target.textContent!.length).toBeGreaterThan(150);
        expect(target.querySelectorAll('script,iframe,style,[onclick]').length).toBe(0);
        for (const link of target.querySelectorAll('a[target="_blank"]'))
          expect(link.getAttribute('rel')).toBe('noopener noreferrer');
      }
      expect(new Set(counts).size).toBe(1);
      expect(hrefs[1]).toEqual(hrefs[0]);
      expect(hrefs[2]).toEqual(hrefs[0]);
    },
  );
  it.each(UI_LOCALES)('builds functional game tabs from translated headings (%s)', (locale) => {
    const doc = documentForContent(),
      target = doc.getElementById('content')!;
    renderLocalizedContent(target, 'multiplayer', locale);
    // jsdom does not implement scrolling; interaction and selection remain real DOM events.
    doc.defaultView!.HTMLElement.prototype.scrollIntoView = () => {};
    buildRuleGuide(target, doc, 'th15', locale);
    expect(target.querySelector('[role="tablist"]')!.getAttribute('aria-label')).toBe(
      translateUi(locale, 'guide.games'),
    );
    expect(target.querySelector('[role="tab"][aria-selected="true"]')!.getAttribute('data-game')).toBe('th15');
    expect(target.querySelector('.multiplayer-rule-panel[data-game="th15"]')!.textContent).toContain('1,250,000');
    const button = target.querySelector<HTMLButtonElement>('button[data-game="th07"]')!;
    button.click();
    expect(button.getAttribute('aria-selected')).toBe('true');
    expect(target.querySelector<HTMLElement>('.multiplayer-rule-panel[data-game="th07"]')!.hidden).toBe(false);
  });
  it('sanitizes translated content with the original URL policy', () => {
    const doc = documentForContent(),
      target = doc.getElementById('content')!;
    renderContentFragment(
      target,
      '<script>bad()</script><a href="javascript:bad()" onclick="bad()">bad</a><img src="data:text/html,bad"><a href="mailto:test@example.test">mail</a>',
      doc,
    );
    expect(target.querySelector('script,img,[onclick],a[href^="javascript:"]')).toBe(null);
    expect(target.querySelector('a[href^="mailto:"]')).not.toBe(null);
  });
  it('contains real English and Japanese FAQ copy rather than the Chinese fallback', () => {
    expect(translateUi('en', 'content.faq')).toContain('Frequently asked questions');
    expect(translateUi('ja', 'content.faq')).toContain('よくある質問');
    expect(translateUi('en', 'content.firstUse')).toContain('Offline access');
    expect(translateUi('ja', 'content.firstUse')).toContain('オフライン');
  });
});

it('has no unregistered hardcoded React UI copy', () => {
  expect(auditUiCopy()).toEqual([]);
});

it('detects future hardcoded text and accessible labels', async () => {
  const { mkdtempSync, writeFileSync, rmSync } = await import('node:fs');
  const { tmpdir } = await import('node:os');
  const { join } = await import('node:path');
  const root = mkdtempSync(join(tmpdir(), 'touhou-copy-audit-'));
  try {
    writeFileSync(
      join(root, 'Fixture.tsx'),
      '<button aria-label="Hardcoded accessible label">Hardcoded UI copy</button>',
    );
    expect(
      auditUiCopy(root)
        .map((item) => item.kind)
        .sort(),
    ).toEqual(['jsx-attribute', 'jsx-text']);
  } finally {
    rmSync(root, { recursive: true, force: true });
  }
});
