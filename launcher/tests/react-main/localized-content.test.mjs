import {test, beforeAll as before, afterAll as after} from 'vitest';
import assert from 'node:assert/strict';
import {readFile, mkdtemp, rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {JSDOM} from 'jsdom';
import {parse} from 'acorn';
import {build} from 'esbuild';
import {ensureI18nBuild} from '../../lib/i18n-build.mjs';
import {localizeCompatibilityHtml, loadCompatibilityTranslations, compatibilityThemeHtml} from '../../lib/localized-compatibility.mjs';

const project = fileURLToPath(new URL('../../', import.meta.url));
let work, api, compatibility, literal, entries;
before(async () => {
  await ensureI18nBuild();
  work = await mkdtemp(resolve(project, '.cache/localized-content-'));
  const outfile = resolve(work, 'actual.mjs');
  await build({stdin: {resolveDir: project, loader: 'ts', contents: 'export * from "./src/launcher/i18n.mts"; export * from "./src/launcher/localized-content.mts"; export * from "./src/launcher/multiplayer-guide-content.mts";'}, outfile, bundle: true, platform: 'node', format: 'esm', logLevel: 'silent'});
  api = await import(pathToFileURL(outfile).href);
  compatibility = await readFile(resolve(project, 'public/compatibility.html'), 'utf8');
  entries = JSON.parse(await readFile(resolve(project, 'messages/compatibility.json'), 'utf8'));
  literal = await loadCompatibilityTranslations();
});
after(async () => {if (work) await rm(work, {recursive: true, force: true});});

test('standalone compatibility literals match compiled messages and reject interpolation', () => {
  for (const locale of ['en', 'ja']) for (const {key} of entries) assert.equal(literal(locale, key), api.translateUi(locale, key));
  assert.throws(() => literal('en', 'status.roomCreated'), /literal message/);
  assert.throws(() => literal('en', 'unknown'), /literal message/);
});

test('compatibility variants survive CRLF and execute their diagnostics as ES5', () => {
  for (const locale of ['en', 'ja']) {
    const source = compatibility.replace(/\r\n?/g, '\n');
    const html = localizeCompatibilityHtml(source, locale, literal);
    assert.equal(html, localizeCompatibilityHtml(source.replaceAll('\n', '\r\n'), locale, literal));
    const dom = new JSDOM(html, {url: `https://eagler.invalid/compatibility.${locale}.html`, runScripts: 'outside-only'});
    try {
      const {document, HTMLCanvasElement} = dom.window;
      HTMLCanvasElement.prototype.getContext = () => null;
      for (const script of document.scripts) {parse(script.textContent, {ecmaVersion: 5}); dom.window.eval(script.textContent);}
      assert.equal(document.documentElement.lang, locale);
      assert.equal(document.title, api.translateUi(locale, 'compatibility.title'));
      assert.equal(document.querySelector('nav').getAttribute('aria-label'), api.translateUi(locale, 'ui.language'));
      assert.equal(document.querySelectorAll('nav a').length, 3);
      assert.ok(document.querySelectorAll('#checks li').length >= 3);
      assert.ok(document.querySelector('#result').textContent.trim());
      assert.equal(document.querySelector('#result').hasAttribute('hidden'), false);
      assert.match(document.querySelector('#try-anyway').getAttribute('onclick'), new RegExp(`${locale}\\.html\\?compat=continue`));
      assert.equal(document.querySelector('script[src],script[type="module"],link[rel="stylesheet"]'), null);
    } finally {dom.window.close();}
  }
});

test('compatibility translations cannot escape HTML or inline script strings', () => {
  const payload = '</script><script>alert("x")</script>&\u2028\u2029';
  const html = localizeCompatibilityHtml(compatibility, 'en', () => payload);
  const dom = new JSDOM(html);
  try {
    assert.equal(dom.window.document.scripts.length, 2);
    assert.equal(dom.window.document.querySelector('h1').textContent, payload);
    for (const script of dom.window.document.scripts) parse(script.textContent, {ecmaVersion: 5});
  } finally {dom.window.close();}
});

test('authored help renders all three locales and keeps local language links', () => {
  const dom = new JSDOM('<main></main>', {url: 'https://eagler.invalid/lobby.html?lang=en'});
  dom.window.HTMLElement.prototype.scrollIntoView = function () {};
  const target = dom.window.document.querySelector('main');
  try {
    for (const kind of ['faq', 'firstUse', 'multiplayer']) for (const locale of ['zh-CN', 'en', 'ja']) {
      const source = '\r\n' + api.translateUi('zh-CN', `content.${kind}`).replace(/\r?\n/g, '\r\n') + '\r\n';
      assert.equal(api.renderLocalizedContent(target, kind, locale, source), locale);
      assert.equal(target.lang, locale);
      assert.ok(target.textContent.trim().length > 100);
      assert.equal(target.querySelector('script,[onclick]'), null);
      const expected = new JSDOM(api.translateUi(locale, `content.${kind}`));
      try {assert.equal(target.querySelector('h1,h2,h3').textContent, expected.window.document.querySelector('h1,h2,h3').textContent);}
      finally {expected.window.close();}
      if (kind === 'multiplayer') {
        api.buildRuleGuide(target, dom.window.document, 'th10', locale);
        assert.equal(target.querySelectorAll('[role="tab"]').length, 7);
        assert.equal(target.querySelector('[role="tab"][aria-selected="true"]').textContent, api.translateUi(locale, 'guide.th10'));
        assert.equal(target.querySelectorAll('.multiplayer-rule-panel:not([hidden])').length, 1);
        assert.ok(target.querySelector('.multiplayer-rule-panel:not([hidden])').textContent.length > 100);
      }
    }
  } finally {dom.window.close();}
});

test('operator help edits use actual sanitized content instead of a stale translation', () => {
  const dom = new JSDOM('<main></main>', {url: 'https://eagler.invalid/faq.html?lang=en'});
  const target = dom.window.document.querySelector('main');
  try {
    const content = '<h2 onclick="run()">Operator update</h2><script>run()</script><p><a href="compatibility.html#phone">Help</a><a href="about.html?keep=a%20b#contact">About</a><a href="#here">Anchor</a><a href="mailto:hello@example.org">Email</a><a href="https://example.org/path?x=1">External</a><img src="javascript:run()"></p>';
    assert.equal(api.renderLocalizedContent(target, 'firstUse', 'ja', content), 'zh-CN');
    assert.equal(target.lang, 'zh-CN');
    assert.equal(target.querySelector('h2').textContent, 'Operator update');
    assert.equal(target.querySelector('script,[onclick],img[src]'), null);
    const links = [...target.querySelectorAll('a')].map(link => link.getAttribute('href'));
    assert.deepEqual(links, ['https://eagler.invalid/compatibility.ja.html#phone', 'https://eagler.invalid/about.html?keep=a%20b&lang=ja#contact', 'https://eagler.invalid/faq.html?lang=en#here', 'mailto:hello@example.org', 'https://example.org/path?x=1']);
  } finally {dom.window.close();}
});

test('standalone compatibility colors follow both canonical themes without CSS variable support', async () => {
  const tokens = await readFile(resolve(project, 'app/ui/tokens.css'), 'utf8');
  const controls = await readFile(resolve(project, 'app/ui/document-controls.css'), 'utf8');
  for (const [theme, expected] of [['dark', 'rgb(23, 24, 27)'], ['light', 'rgb(247, 247, 248)']]) {
    const html = compatibilityThemeHtml(compatibility, tokens, controls);
    assert.equal(html, compatibility, 'checked-in help uses current tokens');
    const dom = new JSDOM(html, {url: 'https://eagler.invalid/compatibility.html', runScripts: 'outside-only'});
    try {
      dom.window.localStorage.setItem('eagler.book.theme', JSON.stringify(theme));
      dom.window.eval(dom.window.document.scripts[0].textContent);
      const style = dom.window.getComputedStyle(dom.window.document.body);
      assert.equal(style.backgroundColor, expected);
      assert.equal(dom.window.document.querySelector('meta[name="theme-color"]').content, theme === 'light' ? '#f7f7f8' : '#17181b');
      assert.doesNotMatch(dom.window.document.getElementById('siteTheme').textContent, /var\(/);
      const changed = compatibilityThemeHtml(html, tokens.replace('--kit-background:#f7f7f8', '--kit-background:#e0e1e2'), controls);
      assert.match(changed, /\.light body\{[^}]*background:#e0e1e2/);
    } finally {dom.window.close();}
  }
});
