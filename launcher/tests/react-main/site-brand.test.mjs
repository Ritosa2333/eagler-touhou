import {test,beforeAll,afterAll,afterEach} from 'vitest';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {JSDOM} from 'jsdom';
import {SITE_BRAND_FACES,SITE_BRAND_DEFAULT,SITE_BRAND_SESSION_KEY,SITE_BRAND_PREPAINT_SCRIPT,chooseSiteBrandFace,initializeStaticSiteBrand,sessionSiteBrandFace,siteBrandPath} from '../../lib/site-brand.mjs';
import {STATIC_FRONTEND_PACKAGE_FILES,STATIC_APP_SHELL_FILES} from '../../lib/frontend-static-manifest.mjs';
import {installMountedDom} from './mounted-dom-environment.mjs';

let env,React,createRoot,renderToStaticMarkup,SiteBrand,root;

beforeAll(async()=>{
  env=installMountedDom();React=await import('react');({createRoot}=await import('react-dom/client'));({renderToStaticMarkup}=await import('react-dom/server'));
  ({SiteBrand}=await import('../../app/components/launcher/SiteBrand.tsx'));
});
afterEach(async()=>{if(root)await React.act(async()=>root.unmount());root=null;env.document.body.replaceChildren();assert.deepEqual(env.errors.splice(0),[]);});
afterAll(()=>env?.close());

class MemoryStorage {
  values = new Map();
  getItem(key) {return this.values.get(key) ?? null;}
  setItem(key, value) {this.values.set(key, String(value));}
}

function documentWithStorage(storage, count = 1) {
  const marks = Array.from({length: count}, (_, index) => `<img data-site-brand src="fallback-${index}.svg">`).join('');
  const dom = new JSDOM(`<main>${marks}</main>`, {url: 'https://launcher.example/'});
  Object.defineProperty(dom.window, 'sessionStorage', {configurable: true, value: storage});
  return dom;
}

test('approved character pool has exact weighted boundaries and reaches every ordinary character',()=>{
  assert.equal(SITE_BRAND_FACES.length,87);assert.equal(new Set(SITE_BRAND_FACES).size,87);
  assert.equal(chooseSiteBrandFace(()=>0),'face-F02B');assert.equal(chooseSiteBrandFace(()=>.099999),'face-F02B');
  assert.equal(chooseSiteBrandFace(()=>.1),'face-F055');assert.equal(chooseSiteBrandFace(()=>.199999),'face-F055');
  const ordinary=SITE_BRAND_FACES.filter(id=>!['face-F02B','face-F055'].includes(id));
  for(let i=0;i<85;i++){let call=0;assert.equal(chooseSiteBrandFace(()=>call++?.5/85+i/85:.2),ordinary[i]);}
  assert.throws(()=>siteBrandPath('unapproved'));
});

test('a new document in the same session reads the previously selected face',()=>{
  const storage = new MemoryStorage(), firstDocument = documentWithStorage(storage), nextDocument = documentWithStorage(storage);
  let draws = 0;
  const first = sessionSiteBrandFace(firstDocument.window, () => {draws++; return .15;});
  const next = sessionSiteBrandFace(nextDocument.window, () => {throw new Error('A saved session face must not be redrawn');});
  assert.equal(first, 'face-F055');
  assert.equal(next, first);
  assert.equal(storage.getItem(SITE_BRAND_SESSION_KEY), first);
  assert.equal(draws, 1);
  firstDocument.window.close(); nextDocument.window.close();
});

test('parser initializer restores the session face before any client hydration',()=>{
  const markup=renderToStaticMarkup(React.createElement(React.Fragment,null,React.createElement(SiteBrand),React.createElement(SiteBrand)));
  const dom=new JSDOM(`<!doctype html><script>sessionStorage.setItem('${SITE_BRAND_SESSION_KEY}','face-F055')</script><main>${markup}</main><script>${SITE_BRAND_PREPAINT_SCRIPT}</script><script>window.__brandBeforeHydration=[...document.querySelectorAll('.site-brand img')].map(image=>image.dataset.brandFace)</script>`,{url:'https://launcher.example/',runScripts:'dangerously'});
  const images=[...dom.window.document.querySelectorAll('.site-brand img')];
  assert.deepEqual(Array.from(dom.window.__brandBeforeHydration),['face-F055','face-F055']);
  assert.deepEqual(images.map(image=>image.dataset.brandFace),['face-F055','face-F055']);
  assert.ok(images.every(image=>/face-F055\.svg$/.test(image.getAttribute('src'))));
  dom.window.close();
});

test('document layout runs the brand initializer after rendered marks and before client scripts',async()=>{
  const source=await readFile(new URL('../../app/root.tsx',import.meta.url),'utf8');
  const children=source.indexOf('{children}'),initializer=source.search(/<script\b(?=[^>]*\bsuppressHydrationWarning\b)[^>]*>\s*\{SITE_BRAND_PREPAINT_SCRIPT\}\s*<\/script>/),clientScripts=source.indexOf('<Scripts ');
  assert.ok(children>=0&&initializer>children&&clientScripts>initializer);
});

test('multiple React brand instances share the session face without inserting scripts',async()=>{
  env.window.sessionStorage.setItem(SITE_BRAND_SESSION_KEY,'face-F055');
  const host=env.document.createElement('div');env.document.body.append(host);root=createRoot(host);
  await React.act(async()=>root.render(React.createElement(React.Fragment,null,React.createElement(SiteBrand),React.createElement(SiteBrand))));
  assert.deepEqual([...env.document.querySelectorAll('.site-brand img')].map(image=>image.dataset.brandFace),['face-F055','face-F055']);
});

test('all brand marks in one document share a face and a failed image resets the session',()=>{
  const storage = new MemoryStorage(), dom = documentWithStorage(storage, 3);
  initializeStaticSiteBrand(dom.window.document, 'https://launcher.example/assets/site-brand.mjs');
  const marks = [...dom.window.document.querySelectorAll('img[data-site-brand]')];
  assert.deepEqual(marks.map(mark => mark.dataset.brandFace), [storage.getItem(SITE_BRAND_SESSION_KEY), storage.getItem(SITE_BRAND_SESSION_KEY), storage.getItem(SITE_BRAND_SESSION_KEY)]);
  assert.equal(new Set(marks.map(mark => mark.src)).size, 1);
  marks[1].onerror();
  assert.equal(marks[1].dataset.brandFace, SITE_BRAND_DEFAULT);
  assert.equal(storage.getItem(SITE_BRAND_SESSION_KEY), SITE_BRAND_DEFAULT);
  dom.window.close();
});

test('invalid or unavailable session storage falls back safely and deterministically',()=>{
  const invalid = new MemoryStorage(); invalid.setItem(SITE_BRAND_SESSION_KEY, 'unapproved');
  const repairedDocument = {sessionStorage: invalid};
  assert.equal(sessionSiteBrandFace(repairedDocument, () => .15), 'face-F055');
  assert.equal(invalid.getItem(SITE_BRAND_SESSION_KEY), 'face-F055');

  const blockedStorage = {getItem() {throw new Error('storage disabled');}, setItem() {throw new Error('storage disabled');}};
  for (const view of [{sessionStorage: blockedStorage}, {sessionStorage: blockedStorage}]) {
    assert.equal(sessionSiteBrandFace(view, () => {throw new Error('disabled storage must use the stable fallback');}), SITE_BRAND_DEFAULT);
  }
  const readOnlyStorage = {getItem() {return null;}, setItem() {throw new Error('storage is read-only');}};
  assert.equal(sessionSiteBrandFace({sessionStorage: readOnlyStorage}, () => .15), SITE_BRAND_DEFAULT);
});

test('every advertised logo ships with attribution, face geometry and both theme palettes',async()=>{
  for(const id of SITE_BRAND_FACES){
    const path=siteBrandPath(id),svg=await readFile(new URL('../../public/'+path,import.meta.url),'utf8');
    assert.ok(STATIC_FRONTEND_PACKAGE_FILES.includes(path));
    assert.ok(svg.includes('plasticparakeet')&&svg.includes('CC BY-NC-SA 3.0'));
    assert.ok(svg.includes('data-face='));
    assert.ok(svg.includes('<style>.wordmark{fill:#202124}@media(prefers-color-scheme:dark){.wordmark{fill:#f0f0f2}}</style>'));
    assert.match(svg,/viewBox="0 0 1000 320"/);
  }
  assert.ok(STATIC_APP_SHELL_FILES.includes(siteBrandPath(SITE_BRAND_DEFAULT)));
  assert.equal(SITE_BRAND_FACES.filter(id=>STATIC_APP_SHELL_FILES.includes(siteBrandPath(id))).length,1);
});

test('published static entry points use the shared session-persistence logic',async()=>{
  const [shared, published, migration] = await Promise.all([
    readFile(new URL('../../lib/site-brand.mjs', import.meta.url), 'utf8'),
    readFile(new URL('../../public/assets/site-brand.mjs', import.meta.url), 'utf8'),
    readFile(new URL('../../public/migrate.html', import.meta.url), 'utf8'),
  ]);
  assert.equal(published.replace(/\s*initializeStaticSiteBrand\(document,import\.meta\.url\);\s*$/, '').trimEnd(), shared.trimEnd());
  assert.match(migration, /const SITE_BRAND_SESSION_KEY="eagler\.site\.brand\.face"/);
  assert.match(migration, /sessionSiteBrandFace\(document\.defaultView\)/);
});
