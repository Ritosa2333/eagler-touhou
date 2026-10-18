/** DOM/setup evidence only. JSDOM is not a browser navigation/Runtime gate. */
import test from 'node:test';
import assert from 'node:assert/strict';
import {execFileSync} from 'node:child_process';
import {readFileSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {parse} from 'acorn';
import {JSDOM} from 'jsdom';
import {runtimeDocumentObservationScript, installRuntimeDocumentObservation} from './runtime-document-observation.mjs';
import {normalizeSourceText} from '../react-main/source-text.mjs';

const project = fileURLToPath(new URL('../../', import.meta.url));
const baseline = 'edee9633e5e3ee79cd2e1aa334f84f6caf755090';
const paths = ['tests/test-th15-launcher-browser.mjs', 'tests/test-runtime-storage-conformance.py', 'tests/test-mp-runtime-exit-room.py'];
const read = path => normalizeSourceText(readFileSync(new URL(`../../${path}`, import.meta.url), 'utf8'));
const pinned = path => execFileSync('git', ['show', `${baseline}:${path}`], {cwd: project, encoding: 'utf8'});
const helper = 'globalThis.__originalRuntimeDocumentObservation';
const selected = runtimeDocumentObservationScript({EAGLER_RUNTIME_TEST_OBSERVATION: 'document'});
const python = source => JSON.parse(execFileSync('python', ['-c', source], {cwd: project, encoding: 'utf8', env: {...process.env, PYTHONDONTWRITEBYTECODE: '1'}}));

function assertionCalls(source) {
  const found = [];
  function visit(value) {
    if (!value || typeof value !== 'object') return;
    if (value.type === 'CallExpression' && value.callee.type === 'MemberExpression' && value.callee.object.name === 'assert') found.push(source.slice(value.start, value.end));
    for (const item of Object.values(value)) if (Array.isArray(item)) item.forEach(visit); else if (item && typeof item === 'object') visit(item);
  }
  visit(parse(source, {ecmaVersion: 'latest', sourceType: 'module'})); return found;
}
function fixture({install = true, src} = {}) {
  const dom = new JSDOM('<section id="player"></section><iframe id="gameFrame"></iframe>', {url: 'https://launcher.test/base/', runScripts: 'outside-only'});
  const frame = dom.window.document.getElementById('gameFrame');
  if (src) frame.src = src;
  if (install) dom.window.eval(selected);
  return {dom, window: dom.window, frame, observer: dom.window.__originalRuntimeDocumentObservation};
}
const storageStrings = python(`import ast,json\nfrom pathlib import Path\nt=ast.parse(Path('tests/test-runtime-storage-conformance.py').read_text())\nprint(json.dumps([n.value for n in ast.walk(t) if isinstance(n,ast.Constant) and isinstance(n.value,str)]))`);
const closePredicates = storageStrings.filter(value => value.startsWith('!document.getElementById') && value.includes('hasAttribute'));
const storageCommand = storageStrings.find(value => value.startsWith('async ({game,command,payload})'));
const storageEpoch = storageCommand.slice(storageCommand.indexOf('{\n') + 1, storageCommand.indexOf('          const request'));

test('default mode installs nothing; explicit document mode is identical for either frontend and independent of artifact root', async () => {
  assert.equal(runtimeDocumentObservationScript({}), null);
  assert.equal(runtimeDocumentObservationScript({EAGLER_RUNTIME_TEST_OBSERVATION: 'attribute'}), null);
  assert.throws(() => runtimeDocumentObservationScript({EAGLER_RUNTIME_TEST_OBSERVATION: 'react'}), /must be attribute or document/);
  for (const variant of ['main', 'rewrite']) for (const root of [undefined, '/explicit/artifact']) {
    assert.equal(runtimeDocumentObservationScript({EAGLER_RUNTIME_TEST_OBSERVATION: 'document', EAGLER_LAUNCHER_TEST_VARIANT: variant, EAGLER_LAUNCHER_TEST_ROOT: root}), selected);
  }
  const prior = process.env.EAGLER_RUNTIME_TEST_OBSERVATION, scripts = [];
  try {
    delete process.env.EAGLER_RUNTIME_TEST_OBSERVATION;
    await installRuntimeDocumentObservation({evaluateOnNewDocument(source) {scripts.push(source);}});
    assert.deepEqual(scripts, []);
    process.env.EAGLER_RUNTIME_TEST_OBSERVATION = 'document';
    await installRuntimeDocumentObservation({evaluateOnNewDocument(source) {scripts.push(source);}});
    assert.deepEqual(scripts, [selected]);
  } finally {if (prior === undefined) delete process.env.EAGLER_RUNTIME_TEST_OBSERVATION; else process.env.EAGLER_RUNTIME_TEST_OBSERVATION = prior;}
  const result = python(`import sys,json\nsys.path.insert(0,'tests')\nfrom support.runtime_document_observation import runtime_document_observation_script\nvalues=[runtime_document_observation_script({}),runtime_document_observation_script({'EAGLER_RUNTIME_TEST_OBSERVATION':'document'})]\ntry: runtime_document_observation_script({'EAGLER_RUNTIME_TEST_OBSERVATION':'react'})\nexcept ValueError: values.append('rejected')\nprint(json.dumps(values))`);
  assert.deepEqual(result, [null, selected, 'rejected']);
});

test('observation adapters retain the independent original native scenario assertions', () => {
  assert.deepEqual(assertionCalls(read(paths[0])), assertionCalls(pinned(paths[0])));
  const result = python(`import ast,json,subprocess\nfrom pathlib import Path\nresults=[]\nfor path in ${JSON.stringify(paths.slice(1))}:\n old=subprocess.check_output(['git','show','${baseline}:'+path],text=True);current=Path(path).read_text()\n def assertions(source): return [ast.get_source_segment(source,n) for n in ast.walk(ast.parse(source)) if isinstance(n,ast.Assert)]\n results.append({'path':path,'count':len(assertions(old)),'equal':assertions(old)==assertions(current)})\nprint(json.dumps(results))`);
  assert.deepEqual(result.map(item => [item.count, item.equal]), [[17, true], [14, true]]);

});

test('URL reads follow the actual same-origin child document when its native History URL differs from the src attribute', () => {
  const f = fixture({src: '/runtime/th15/th15.html?runtimeEpoch=11'});
  try {
    const initialSrc = f.frame.getAttribute('src');
    f.frame.contentWindow.history.replaceState(null, '', '?runtimeEpoch=12&managedData=1');
    assert.match(f.observer.url(f.frame), /runtimeEpoch=12&managedData=1$/);
    assert.match(f.frame.src, /runtimeEpoch=11$/);
    assert.equal(f.frame.getAttribute('src'), initialSrc);
    assert.equal(f.observer.url(f.frame), f.frame.contentDocument.URL);
    assert.equal(f.observer.hasRuntimeEpoch(f.frame), true);
    assert.equal(f.observer.blank(f.frame), false);
    const declaration = read(paths[0]).split('\n').find(line => line.includes('url=new URL('));
    assert.equal(f.window.eval(`(() => {${declaration};return epoch;})()`), 12);
  } finally {f.window.close();}
});

test('the unchanged storage epoch parser and positive-safe-integer guard consume the actual child URL', () => {
  const f = fixture({src: '/runtime/th06/th06.html?runtimeEpoch=1'});
  try {
    for (const value of ['2', '9007199254740991']) {
      f.frame.contentWindow.history.replaceState(null, '', `?runtimeEpoch=${value}`);
      assert.equal(f.window.eval(`(() => {${storageEpoch};return epoch;})()`), Number(value));
    }
    for (const query of ['', '?runtimeEpoch=0', '?runtimeEpoch=-1', '?runtimeEpoch=1.5', '?runtimeEpoch=NaN', '?runtimeEpoch=9007199254740992']) {
      f.frame.contentWindow.history.replaceState(null, '', `/runtime/th06/th06.html${query}`);
      assert.throws(() => f.window.eval(`(() => {${storageEpoch};return epoch;})()`), /Runtime navigation epoch missing/);
    }
  } finally {f.window.close();}
});

test('stronger teardown predicates retain original no-src and Player-closed requirements and reject a missing/detached frame', {timeout: 5000}, async () => {
  assert.equal(closePredicates.length, 3);
  const f = fixture();
  try {
    assert.equal(f.observer.blank(f.frame), true);
    for (const predicate of closePredicates) assert.equal(f.window.eval(predicate), true);
    f.window.document.getElementById('player').classList.add('open');
    assert.equal(f.window.eval(closePredicates[0]), false);
    f.window.document.getElementById('player').classList.remove('open');
    let loaded = new Promise(resolve => f.frame.addEventListener('load', resolve, {once: true}));
    f.frame.src = 'about:blank';
    assert.equal(f.observer.blank(f.frame), false, 'an incomplete replacement document is not a completed teardown');
    await loaded;
    assert.equal(f.observer.blank(f.frame), true);
    for (const predicate of closePredicates) assert.equal(f.window.eval(predicate), false, 'original src-attribute absence is still required');
    loaded = new Promise(resolve => f.frame.addEventListener('load', resolve, {once: true}));
    f.frame.removeAttribute('src');
    await loaded;
    for (const predicate of closePredicates) assert.equal(f.window.eval(predicate), true);
    f.frame.remove();
    assert.equal(f.observer.blank(f.frame), false);
    assert.equal(f.observer.hasRuntimeEpoch(f.frame), false);
    assert.throws(() => f.observer.url(f.frame), /missing or detached/);
    for (const predicate of closePredicates) assert.equal(f.window.eval(predicate), false, 'missing frame must not satisfy opt-in teardown');
  } finally {f.window.close();}
  const original = fixture({install: false});
  try {
    original.frame.remove();
    for (const predicate of closePredicates) assert.equal(original.window.eval(predicate), true, 'default retains original observation semantics');
  } finally {original.window.close();}
});

test('unsupported/foreign documents fail closed without changing src, URL, or native frame interfaces', () => {
  const f = fixture({src: 'https://foreign.test/runtime.html?runtimeEpoch=8'});
  try {
    const source = f.frame.src, href = f.frame.contentWindow.location.href;
    assert.throws(() => f.observer.url(f.frame), /share the Launcher origin/);
    assert.equal(f.observer.blank(f.frame), false); assert.equal(f.observer.hasRuntimeEpoch(f.frame), false);
    assert.equal(f.frame.src, source); assert.equal(f.frame.contentWindow.location.href, href);
    assert.equal(Object.hasOwn(f.frame, 'src'), false);
  } finally {f.window.close();}
});
