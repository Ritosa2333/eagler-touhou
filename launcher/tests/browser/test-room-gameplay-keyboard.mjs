import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {fileURLToPath} from 'node:url';
import {build} from 'esbuild';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';
import {authoredSourcesPlugin} from '../../lib/authored-source-resolution.mjs';

const project = fileURLToPath(new URL('../../', import.meta.url));
const fixture = `
import {createRoot} from 'react-dom/client';
import {useState} from 'react';
import {PlayerSurface} from './app/components/player/PlayerSurface.tsx';
import {createPlayerKeyboard} from './app/services/player-keyboard.ts';
import {createKeyboardPreferences} from './src/launcher/keyboard-preferences.mts';
import {deliverRuntimeInput} from './src/launcher/touch-runtime-protocol.mts';
import {createBrowserKeyboard} from '../common/browser/directory-keyboard.mjs';

const game = new URL(location.href).searchParams.get('game');
const preferences = createKeyboardPreferences(localStorage);
const th075Preferences = createKeyboardPreferences(localStorage, 'th075');
const isTh075 = game === 'th075';
const profile = isTh075 ? 'th075' : 'standard';
const saved = isTh075 ? th075Preferences : preferences;
saved.bind('left', 'Numpad4');
saved.bind('shoot', 'NumpadAdd');

const ignore = () => {};
const listeners = new Set();
const events = [];
let phase = 'preview';
function setup(frame) {
  if (!frame) return;
  frame.addEventListener('load', () => {
    const target = frame.contentWindow;
    const nativeDocument = frame.contentDocument;
    const identity = {game, protocol: 'eagler-touhou/1', epoch: 1, origin: location.origin};
    const context = () => ({...identity, target, targetOrigin: location.origin, launched: true, ready: true, spectator: phase === 'spectator'});
    const keyboard = createBrowserKeyboard({
      hostContext: {...identity, target, profile: () => profile, ready: () => true},
      send: (code, down) => events.push([code, down]),
    });
    const runtime = {
      getInputContext: context,
      getMidiEventContext: () => ({epoch: 1, game, target, document: nativeDocument, music: 'none'}),
      getSnapshot: () => ({ready: true, launched: phase === 'play', firstFrame: phase === 'play'}),
      postInput: (command, fields) => deliverRuntimeInput(context(), {...fields, ...identity, command}),
      subscribe(listener) {listeners.add(listener); return () => listeners.delete(listener);},
    };
    createPlayerKeyboard({
      runtime, window, document, frame, preferences, th075Preferences,
      playerOpen: () => true, launcherOwnsTarget: () => false,
      releaseHeldTouchFire: ignore, cancelTouchFunction: ignore, android: false,
      isPlayerFullscreen: () => false, toggleFullscreen: ignore,
    });
    window.nativeKeyEvents = 0;
    for (const name of ['keydown','keyup']) nativeDocument.addEventListener(name, event => {
      ++window.nativeKeyEvents;
      keyboard.mapNative(event, name === 'keydown');
    }, true);
    window.events = events;
    window.fixtureReady = true;
  }, {once: true});
  frame.src = '/runtime';
}
function App() {
  const [playing, setPlaying] = useState(false);
  window.setPhase = next => {
    phase = next;
    setPlaying(next === 'play');
    for (const listener of listeners) listener();
  };
  return <main data-playing={playing}>
    <PlayerSurface open editing={false} roomWorkspace roomGameplayKeyboard={playing}
      onElement={ignore} onFrame={setup}/>
  </main>;
}
createRoot(document.getElementById('root')).render(<App/>);
`;
const {outputFiles} = await build({
  absWorkingDir: project, stdin: {contents: fixture, loader: 'tsx', resolveDir: project},
  bundle: true, format: 'esm', jsx: 'automatic', write: false, outdir: '.cache/room-keyboard',
  plugins: [authoredSourcesPlugin(project)],
});
const script = outputFiles.find(file => file.path.endsWith('.js')).contents;
const server = createServer((request, response) => {
  if (request.url === '/fixture.js') {
    response.writeHead(200, {'content-type': 'text/javascript'}).end(script);
  } else {
    const markup = request.url === '/runtime' ? '<button id="game">Game</button>'
      : '<div id="root"></div><script type="module" src="/fixture.js"></script>';
    response.writeHead(200, {'content-type': 'text/html; charset=utf-8'}).end(markup);
  }
});
await new Promise(resolve => server.listen(0, '127.0.0.1', resolve));
let browser;
try {
  browser = await puppeteer.launch({headless: true, executablePath: await findChromiumExecutable()});
  for (const [game, expected] of [['th075',['Th075Action0','Th075Action4']], ['th14',['ArrowLeft','KeyZ']]]) {
    const page = await browser.newPage();
    await page.goto(`http://127.0.0.1:${server.address().port}/?game=${game}`);
    await page.waitForFunction(() => window.fixtureReady);
    const frame = page.frames().find(frame => frame.url().endsWith('/runtime'));
    await frame.focus('#game');
    await page.keyboard.press('Numpad4');
    assert.deepEqual(await page.evaluate(() => events), [], game + ' preview');
    assert.equal(await page.evaluate(() => nativeKeyEvents), 0, game + ' preview guard');

    await page.evaluate(() => setPhase('play'));
    await page.waitForSelector('main[data-playing=true]');
    await page.keyboard.press('Numpad4');
    await page.keyboard.press('NumpadAdd');
    assert.deepEqual(await page.evaluate(() => events), expected.flatMap(code => [[code,true],[code,false]]), game + ' gameplay');
    assert.equal(await page.evaluate(() => nativeKeyEvents), 4, game + ' focused iframe');

    await page.evaluate(() => {events.length = 0; setPhase('spectator');});
    await page.waitForSelector('main[data-playing=false]');
    await page.keyboard.press('Numpad4');
    assert.deepEqual(await page.evaluate(() => events), [], game + ' spectating');
    assert.equal(await page.evaluate(() => nativeKeyEvents), 4, game + ' spectator guard');
    await page.close();
  }
  console.log('PASS TH075 and TH14 remapped keypad input in the focused room iframe');
} finally {
  await browser?.close();
  server.close();
}
