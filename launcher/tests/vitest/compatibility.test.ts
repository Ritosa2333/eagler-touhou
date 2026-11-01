import { readFileSync } from 'node:fs';
import { parse as parseJavaScript } from 'acorn';
import { JSDOM } from 'jsdom';
import { describe, expect, it } from 'vitest';
import { localizeCompatibilityHtml } from '../../lib/localized-compatibility.mjs';
import { translateUi } from '../../src/launcher/i18n.mts';

const original = readFileSync(new URL('../../public/compatibility.html', import.meta.url), 'utf8');

describe('static old-browser help', () => {
  it.each(['en', 'ja'] as const)('publishes complete %s copy with ES5 scripts', (locale) => {
    const html = localizeCompatibilityHtml(original, locale, translateUi),
      doc = new JSDOM(html).window.document;
    expect(doc.documentElement.lang).toBe(locale);
    expect(doc.querySelector('h1')!.textContent).toBe(translateUi(locale, 'compatibility.heading'));
    expect(doc.querySelectorAll('a[lang]').length).toBe(3);
    expect(doc.querySelector('nav')!.getAttribute('aria-label')).toBe(translateUi(locale, 'ui.language'));
    expect(doc.querySelectorAll('#checks,#recheck,#try-anyway,#result').length).toBe(4);
    for (const script of doc.querySelectorAll('script'))
      expect(() => parseJavaScript(script.textContent!, { ecmaVersion: 5 })).not.toThrow();
    expect(html).not.toContain('请更换浏览器');
    expect(html).not.toContain('正在检测浏览器');
    expect([...doc.querySelectorAll('a[href]')].map((link) => link.getAttribute('href'))).toEqual(
      [...new JSDOM(original).window.document.querySelectorAll('a[href]')].map((link) => link.getAttribute('href')),
    );
  });
});
