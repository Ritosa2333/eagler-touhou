import {test, afterAll} from 'vitest';
import {legacyControlAttributes} from './legacy-control-attributes.mjs';
import {readUiHistoryText} from '../support/ui-history.mjs';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {join} from 'node:path';
import {JSDOM} from 'jsdom';
import {act, createRef, StrictMode} from 'react';
import {createRoot} from 'react-dom/client';
import {SiteNotice, type SiteNoticeHandle} from '@source/app/components/notices/SiteNotice.tsx';
import {LocaleProvider, translate} from '@source/app/i18n.tsx';
import {createSiteNoticeState} from '@source/app/components/notices/site-notice-state.ts';
import {createSitePreferencesModel, bindSitePreferencesToDocument, LESS_MOTION_STORAGE_KEY,
  SITE_NOTICE_STORAGE_KEY, SITE_NOTICE_DISMISSED_KEY} from '@source/app/models/site-preferences.ts';
import * as contentPolicy from '@source/src/launcher/site-notice-content.mts';
import * as reactPolicy from '@source/app/components/notices/site-notice-source.ts';
import * as pinnedNotice from '@pinned/site-notice';

function storage(initial: Record<string, string> = {}) {
  const data = new Map(Object.entries(initial));
  return {data, getItem: (key: string) => data.get(key) ?? null,
    setItem: (key: string, value: string) => {data.set(key, value);}};
}
function clock() {
  let serial = 0;
  const timers = new Map<number, {callback: () => void; delay: number}>();
  return {timers,
    setTimeoutImpl(callback: () => void, delay: number) {
      const id = ++serial; timers.set(id, {callback, delay}); return id;
    },
    clearTimeoutImpl(id: number) {timers.delete(id);},
    fire(delay: number) {
      for (const [id, item] of timers) if (item.delay === delay) {
        timers.delete(id); item.callback(); return;
      }
      throw new Error(`Missing ${delay}ms timer`);
    },
  };
}
function installDom(dom: JSDOM) {
  Object.assign(globalThis, {window: dom.window, document: dom.window.document,
    Node: dom.window.Node, HTMLElement: dom.window.HTMLElement, location: dom.window.location,
    IS_REACT_ACT_ENVIRONMENT: true});
}
function tree(node: any): any {
  if (node.nodeType === 3) return node.textContent.trim() ? node.textContent : null;
  if (node.nodeType !== 1) return null;
  return {tag: node.tagName,
    attrs: Object.fromEntries(legacyControlAttributes(node).map(attr => [attr.name, attr.value])
      .sort(([left], [right]) => left.localeCompare(right))),
    children: [...node.childNodes].map(tree).filter(Boolean)};
}
export function registerChecks(repo: string) {
let originalHtml, saved, prefs, timer, fetches, state, scroll, markup, original, dom, notice, legacyTimer, legacy, browserPrefs, detach, componentTimer, environment, ref, root, gestureDom, requests, hint, gestureClock, gestureEnvironment, gesturePrefs, handle, gestureRoot, render, bar;
function pointer(target: EventTarget, type: string, x: number, y = 20) {
    const event = new gestureDom.window.MouseEvent(type, {bubbles: true, cancelable: true, clientX: x, clientY: y, button: 0});
    Object.defineProperties(event, {pointerId: {value: 1}, isPrimary: {value: true}});
    target.dispatchEvent(event); return event;
  }
async function swipe(target: EventTarget, from: number, to: number) {
    await act(async () => {pointer(target, 'pointerdown', from); pointer(target, 'pointermove', to); pointer(target, 'pointerup', to);});
  }
test('shared parser exports the same policy and uses a four-second lifespan', async () => {
originalHtml = readUiHistoryText('edee9633e5e3ee79cd2e1aa334f84f6caf755090', 'public/index.html');

    assert.equal(contentPolicy.SITE_NOTICE_DURATION_MS, 4_000);
    for (const name of ['parseSiteNoticeText', 'siteNoticeBrandAsset', 'SITE_NOTICE_DURATION_MS', 'SITE_NOTICE_STORAGE_KEY', 'SITE_NOTICE_DISMISSED_KEY'] as const) {
      assert.equal(reactPolicy[name], contentPolicy[name]);
    }
  
});
test('independently compiled pinned parser matches shared policy across untrusted-link and origin cases', async () => {

    const corpus = [
      '', '  \r\n  ', 'plain text 中文 <script>alert(1)</script>',
      '  [FAQ](faq.html) [GitHub](https://github.com/example/repo)  \r\n [cloud](https://cloud.touhou.best/) ',
      '[unsafe](javascript:alert(1)) [data](data:text/html,hello) [mail](mailto:test@example.org) [file](file:///a) [ftp](ftp://example.org/a)',
      '[invalid](http://[) [safe](https://example.org/) trailing [bad](JAVASCRIPT:x)',
      '[relative](../faq.html?q=a%20b#section) [fragment](#local) [query](?q=a%20b) [protocol-relative](//github.com/repo)',
      '[upper](HTTPS://GITHUB.COM/repo) [lookalike](https://github.com.evil.invalid/repo) [credentials](https://github.com@evil.invalid/repo)',
      '[root](https://bilibili.com/) [sub](https://space.bilibili.com/1) [lookalike](https://bilibili.com.evil.invalid/) [trailing-dot](https://bilibili.com./)',
      '[QQ](https://qm.qq.com/q/a) [same FAQ](/nested/FAQ.HTML) [external FAQ](https://elsewhere.invalid/faq.html) [not FAQ](faq.html/more)',
      '[nested [label]](https://example.org/a(b)c) [empty]() [space](https://example.org/a b) ![image](https://example.org/a.png)',
      '[unclosed](https://example.org [escaped](https://example.org/a%29b) [unicode](https://例え.テスト/道)',
      '\uFEFF[FAQ](faq.html)\n[escaped](/%2Fexample.org)\n[backslash](https://example.org\\@github.com/)',
    ];
    for (const baseUrl of ['https://test.example/', 'https://test.example/eagler-touhou/', 'http://test.example:8080/nested/index.html?q=1#part']) {
      for (const input of corpus) {
        const expected = pinnedNotice.parseSiteNoticeText(input, baseUrl);
        const actual = contentPolicy.parseSiteNoticeText(input, baseUrl);
        assert.deepEqual(actual, expected, `${baseUrl}: ${input}`);
        assert.equal(Object.isFrozen(actual), true);
        for (const line of actual) {assert.equal(Object.isFrozen(line), true); for (const segment of line) assert.equal(Object.isFrozen(segment), true);}
      }
      for (const url of ['faq.html', '/FAQ.HTML#part', '/other/faq.html?q=1', 'https://cloud.touhou.best/', 'https://qm.qq.com/q/a',
        'https://github.com/repo', 'https://space.bilibili.com/1', 'https://bilibili.com/', 'https://bilibili.com.evil.invalid/',
        'https://elsewhere.invalid/faq.html', 'https://test.example:444/faq.html', 'data:text/plain,hi', 'http://[']) {
        const outcome = (read: () => string) => {try {return {value: read()};} catch (error) {return {error: String(error)};}};
        assert.deepEqual(outcome(() => contentPolicy.siteNoticeBrandAsset(url, baseUrl)), outcome(() => pinnedNotice.siteNoticeBrandAsset(url, baseUrl)));
      }
    }
  
});
test('parser default-base resolution preserves absent, available and denied location behavior', async () => {

    const original = Object.getOwnPropertyDescriptor(globalThis, 'location');
    try {
      for (const descriptor of [{value: undefined}, {value: {href: 'https://test.example/subdir/'}}, {get() {throw new Error('denied location');}}]) {
        Object.defineProperty(globalThis, 'location', {configurable: true, ...descriptor});
        assert.deepEqual(contentPolicy.parseSiteNoticeText('[FAQ](faq.html) [outside](https://outside.example/)'),
          pinnedNotice.parseSiteNoticeText('[FAQ](faq.html) [outside](https://outside.example/)'));
        assert.equal(contentPolicy.siteNoticeBrandAsset('faq.html'), pinnedNotice.siteNoticeBrandAsset('faq.html'));
      }
    } finally {if (original) Object.defineProperty(globalThis, 'location', original); else delete (globalThis as {location?: unknown}).location;}
  
});
test('ordinary-build defaults and construction without storage writes', async () => {
saved = storage(); prefs = createSitePreferencesModel({storage: saved});

    assert.deepEqual(prefs.getSnapshot(), {lessMotion: false, noticeEnabled: true, noticeDismissed: false});
    assert.equal(saved.data.size, 0);
  
});
test('motion persistence and exact-value cross-document parsing', async () => {

    prefs.toggleMotion(); assert.equal(saved.data.get(LESS_MOTION_STORAGE_KEY), '1');
    prefs.syncStorageEvent({key: LESS_MOTION_STORAGE_KEY, newValue: 'true'});
    assert.equal(prefs.getSnapshot().lessMotion, false);
  
});
test('denied localStorage is non-fatal for all preferences', async () => {

    const denied = createSitePreferencesModel({storage: {
      getItem() {throw new Error('blocked');}, setItem() {throw new Error('blocked');},
    }});
    denied.toggleNotice(); denied.dismissNotice(); denied.toggleMotion();
    assert.equal(denied.getSnapshot().noticeEnabled, false);
    assert.equal(denied.getSnapshot().noticeDismissed, true);
  
});
test('real NOTICE.txt fetch, BOM trim, parser and four-second lifespan', async () => {
timer = clock();
fetches = 0;
state = createSiteNoticeState(prefs, {...timer, baseUrl: 'https://test.example/',
    matchMediaImpl: () => ({matches: false}), fetchImpl: async (url, init) => {
      fetches++; assert.equal(url, 'NOTICE.txt'); assert.deepEqual(init, {cache: 'no-store'});
      return new Response('\uFEFF[FAQ](faq.html) [GitHub](https://github.com/test)\n<script>alert(1)</script> [bad](javascript:alert(1))');
    }});
state.connect();

    assert.equal(await state.load(), true); assert.equal(state.getSnapshot().durationMs, 4_000);
    assert.equal(state.getSnapshot().lines.length, 2);
    assert.equal(state.getSnapshot().lines[1][0].type, 'text');
    assert.equal(state.getSnapshot().optOutVisible, false);
  
});
test('close persists dismissal and uses original 220ms animation', async () => {

    state.dismiss(); assert.equal(saved.data.get(SITE_NOTICE_DISMISSED_KEY), '1');
    assert.equal(state.getSnapshot().closing, true); assert.equal(state.getSnapshot().hidden, false);
    timer.fire(220); assert.equal(state.getSnapshot().hidden, true);
  
});
test('reopening a dismissed notice exposes permanent opt-out', async () => {

    await state.load(); assert.equal(state.getSnapshot().optOutVisible, true);
  
});
test('scroll hiding preserves running lifespan and upward scroll restores', async () => {
scroll = {};

    state.rememberScroll(scroll, 0); state.scroll(scroll, 30);
    assert.equal(state.getSnapshot().scrollHidden, true);
    assert.ok([...timer.timers.values()].some(item => item.delay === 4_000));
    state.scroll(scroll, 24); assert.equal(state.getSnapshot().scrollHidden, false);
  
});
test('closing a scroll-hidden notice is immediate', async () => {

    state.scroll(scroll, 40); state.close(); assert.equal(state.getSnapshot().hidden, true);
  
});
test('automatic expiration completes original animated dismissal', async () => {

    await state.load(); timer.fire(4_000); assert.equal(state.getSnapshot().closing, true);
    timer.fire(220); assert.equal(state.getSnapshot().hidden, true);
  
});
test('permanent opt-out persists and prevents additional fetches', async () => {

    state.optOut(); assert.equal(saved.data.get(SITE_NOTICE_STORAGE_KEY), '0');
    const before = fetches; assert.equal(await state.load(), false); assert.equal(fetches, before);
  
});
test('reenabling immediately fetches and owner teardown clears timers', async () => {

    const before = fetches; prefs.toggleNotice(); await Promise.resolve(); await Promise.resolve();
    assert.equal(fetches, before + 1); state.destroy(); assert.equal(timer.timers.size, 0);
  
});
test('leaving Launcher hides the banner and cancels its lifespan without dismissal', async () => {

    const preference = createSitePreferencesModel({storage: storage()}), time = clock(); let calls = 0;
    const owner = createSiteNoticeState(preference, {...time, fetchImpl: async () => {calls++; return new Response('Notice');}});
    owner.connect(); await owner.load(); owner.setVisible(false);
    assert.equal(owner.getSnapshot().hidden, true); assert.equal(time.timers.size, 0);
    assert.equal(preference.getSnapshot().noticeDismissed, false);
    assert.equal(owner.isEnabled(), true); assert.equal(await owner.load(), false); assert.equal(calls, 1);
    owner.setVisible(true); assert.equal(await owner.load(), true); owner.destroy();
  
});
for (const operation of ['newer', 'close', 'disable', 'destroy', 'leave-launcher']) {
    test(`pending request invalidation: ${operation}`, async () => {
      const preference = createSitePreferencesModel({storage: storage()}), time = clock();
      const pending: ((response: Response) => void)[] = [];
      const owner = createSiteNoticeState(preference, {...time,
        fetchImpl: () => new Promise(resolve => pending.push(resolve)), baseUrl: 'https://test.example/'});
      owner.connect(); const first = owner.load(); let newer: Promise<boolean> | undefined;
      if (operation === 'newer') newer = owner.load();
      if (operation === 'close') owner.close();
      if (operation === 'disable') preference.toggleNotice();
      if (operation === 'destroy') owner.destroy();
      if (operation === 'leave-launcher') owner.setVisible(false);
      pending[0](new Response('obsolete')); assert.equal(await first, false);
      if (newer) {
        pending[1](new Response('current')); assert.equal(await newer, true);
        assert.equal(owner.getSnapshot().lines[0][0].text, 'current');
      } else assert.equal(owner.getSnapshot().hidden, true);
      owner.destroy(); assert.equal(time.timers.size, 0);
    });
  }
for (const failure of ['empty', 'http', 'offline']) {
    test(`notice fetch failure stays hidden: ${failure}`, async () => {
      const owner = createSiteNoticeState(prefs, {...clock(), fetchImpl: async () => {
        if (failure === 'offline') throw new Error('offline');
        return new Response(failure === 'empty' ? '' : 'bad', {status: failure === 'http' ? 500 : 200});
      }});
      owner.connect(); assert.equal(await owner.load(), false);
      assert.equal(owner.getSnapshot().hidden, true); owner.destroy();
    });
  }
test('StrictMode mounted output matches pinned main DOM, copy, icons, style and aria', async () => {
markup = originalHtml.slice(originalHtml.indexOf('  <div class="site-notice"'), originalHtml.indexOf('  <div class="sheet">'));
original = new JSDOM(markup + '<button id="siteNoticeToggle"></button>', {url: 'https://test.example/'});
dom = new JSDOM('<div id="mount"></div>', {url: 'https://test.example/'});
for (const window of [original.window, dom.window]) Object.defineProperty(window.HTMLImageElement.prototype, 'decoding', {
    get() {return this.getAttribute('decoding') ?? 'auto';},
    set(value) {this.setAttribute('decoding', value);}, configurable: true,
  });
installDom(dom);
notice = readFileSync(join(repo, 'NOTICE.txt'), 'utf8'); legacyTimer = clock();
legacy = pinnedNotice.createSiteNoticeController({documentObj: original.window.document, windowObj: original.window,
    storage: storage(), fetchImpl: async () => new Response(notice), ...legacyTimer, matchMediaImpl: () => ({matches: false})});
await legacy.load();
original.window.document.getElementById('siteNotice')!.style.setProperty('--site-notice-duration', '4000ms');
browserPrefs = createSitePreferencesModel({storage: storage()});
detach = bindSitePreferencesToDocument(browserPrefs); componentTimer = clock();
environment = {...componentTimer, fetchImpl: async () => new Response(notice), baseUrl: 'https://test.example/', matchMediaImpl: () => ({matches: false})};
ref = createRef<SiteNoticeHandle>(); root = createRoot(document.getElementById('mount')!);

    await act(async () => root.render(<StrictMode><LocaleProvider locale="zh-CN"><SiteNotice preferences={browserPrefs}
      autoLoad edgeGestures={false} environment={environment} ref={ref}/></LocaleProvider></StrictMode>));
    assert.deepEqual(tree(document.getElementById('siteNotice')), tree(original.window.document.getElementById('siteNotice')));
  
});
test('mounted close click and imperative reopen preserve dismissal semantics', async () => {

    await act(async () => document.getElementById('siteNoticeClose')!.dispatchEvent(new dom.window.MouseEvent('click', {bubbles: true})));
    assert.equal(browserPrefs.getSnapshot().noticeDismissed, true);
    await act(async () => componentTimer.fire(220)); assert.equal(document.getElementById('siteNotice')!.hidden, true);
    await act(async () => {await ref.current!.load();}); assert.equal(document.getElementById('siteNoticeOptOut')!.hidden, false);
  
});
test('mounted motion changes and storage events synchronize body class', async () => {

    await act(async () => browserPrefs.toggleMotion()); assert.equal(document.body.classList.contains('less-motion'), true);
    await act(async () => window.dispatchEvent(new dom.window.StorageEvent('storage', {key: LESS_MOTION_STORAGE_KEY, newValue: '0'})));
    assert.equal(document.body.classList.contains('less-motion'), false);
  
});
test('live English locale copy changes without a history write', async () => {

    const history = window.history.length;
    await act(async () => root.render(<StrictMode><LocaleProvider locale="en"><SiteNotice preferences={browserPrefs}
      edgeGestures={false} environment={environment} ref={ref}/></LocaleProvider></StrictMode>));
    assert.equal(document.getElementById('siteNotice')!.getAttribute('aria-label'), translate('en', 'notice.aria'));
    assert.equal(window.history.length, history);
  
});
test('mounted cleanup clears timers and original-controller resources', async () => {

    await act(async () => root.unmount()); assert.equal(componentTimer.timers.size, 0);
    detach(); legacy.destroy();
  
});
test('startup gate does not fetch automatically', async () => {
dom.window.close();
original.window.close();
gestureDom = new JSDOM('<div id="mount"></div>', {url: 'https://test.example/'});
installDom(gestureDom);
requests = 0; hint = '';
gestureClock = clock();
gestureEnvironment = {...gestureClock, baseUrl: 'https://test.example/', matchMediaImpl: () => ({matches: true}), fetchImpl: async () => {
    requests++; return new Response('[bad](javascript:alert(1)) <img src=x onerror=alert(1)> [GitHub](https://github.com/test)');
  }};
gesturePrefs = createSitePreferencesModel({storage: null}); handle = createRef<SiteNoticeHandle>();
gestureRoot = createRoot(document.getElementById('mount')!);
render = (edgeGestures = true) => gestureRoot.render(<LocaleProvider locale="en"><SiteNotice
    preferences={gesturePrefs} edgeGestures={edgeGestures} environment={gestureEnvironment}
    onOptOut={text => {hint = text;}} ref={handle}/></LocaleProvider>);

    await act(async () => render()); assert.equal(requests, 0);
  
});
test('real pointer events reopen notice from left viewport edge', async () => {

    await swipe(document.body, 5, 80); assert.equal(requests, 1); assert.equal(handle.current!.isOpen(), true);
  
});
test('hostile fetched content remains inert text in mounted DOM', async () => {

    assert.equal(document.querySelectorAll('#siteNoticeContent script, #siteNoticeContent [onerror]').length, 0);
    assert.equal(document.querySelectorAll('#siteNoticeContent a').length, 1);
    assert.match(document.getElementById('siteNoticeContent')!.textContent!, /javascript:alert/);
  
});
test('original edge-drawer setup can hide the native banner then reveal it again', async () => {
bar = document.getElementById('siteNotice')!;

    bar.hidden = true; assert.equal(handle.current!.isOpen(), false);
    const before = requests; await swipe(document.body, 5, 80);
    assert.equal(requests, before + 1); assert.equal(bar.hidden, false);
    assert.equal(handle.current!.isOpen(), true);
    assert.equal(gesturePrefs.getSnapshot().noticeDismissed, false);
  
});
test('reduced-motion gesture collapse is immediate and does not mark dismissal', async () => {

    await swipe(bar, 180, 80); assert.equal(bar.hidden, true);
    assert.equal(gesturePrefs.getSnapshot().noticeDismissed, false);
    await swipe(document.body, 5, 80); assert.equal(bar.hidden, false);
  
});
test('close and reopen expose opt-out in the mounted gesture surface', async () => {

    await act(async () => document.getElementById('siteNoticeClose')!.dispatchEvent(new gestureDom.window.MouseEvent('click', {bubbles: true})));
    await act(async () => {await handle.current!.load();});
    assert.equal(document.getElementById('siteNoticeOptOut')!.hidden, false);
  
});
test('opt-out click sends exact localized restore hint and disables banner', async () => {

    await act(async () => document.getElementById('siteNoticeOptOut')!.dispatchEvent(new gestureDom.window.MouseEvent('click', {bubbles: true})));
    assert.equal(hint, translate('en', 'notice.restoreHint'));
    assert.equal(gesturePrefs.getSnapshot().noticeEnabled, false); assert.equal(bar.hidden, true);
  
});
test('disabled notice rejects edge-swipe reopen', async () => {

    const before = requests; await swipe(document.body, 5, 80); assert.equal(requests, before);
  
});
test('reenabling opens banner and lobby mode disables edge gestures', async () => {

    await act(async () => gesturePrefs.toggleNotice()); assert.equal(handle.current!.isOpen(), true);
    await act(async () => handle.current!.close()); await act(async () => render(false));
    const before = requests; await swipe(document.body, 5, 80); assert.equal(requests, before);
  
});
test('unmount removes gesture listeners and all outstanding timers', async () => {

    const before = requests; await act(async () => gestureRoot.unmount());
    assert.equal(gestureClock.timers.size, 0); await swipe(document.body, 5, 80); assert.equal(requests, before);
  
});
afterAll(() => {gestureDom.window.close();
});
}
