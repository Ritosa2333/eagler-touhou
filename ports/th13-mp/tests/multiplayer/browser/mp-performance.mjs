// Production configuration multiplayer performance gate (4x CPU throttle).
// Per-frame audit is disabled (production mode); 60-frame wire digests remain
// active and mismatch-free. Pilot debug inspections are deferred outside the
// timed C++ callback window while production shell.mjs onGameFrame stays timed.
// Does not replace formal-flow.mjs (per-frame full-state audit).
import {createRequire} from 'node:module';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {writeFile, readFile, readlink, appendFile, mkdir} from 'node:fs/promises';
import {createHash} from 'node:crypto';
import assert from 'node:assert/strict';

import {installPilot} from './formal-pilot.mjs';
import {installWireConditions} from './formal-network.mjs';
import {startCpuProfiles} from './formal-profile.mjs';
import {assertHardwareIdentity} from './hardware-identity.mjs';
import {persistIndependent, terminalStream} from './persist-independent.mjs';
import {boundedOperation} from './bounded-operation.mjs';
import {freezeFrameWork} from './freeze-frame-work.mjs';
import {createInspectionGuard, settleInspections} from './inspection-serialization.mjs';
import {assertBuildDelivery} from './formal-delivery.mjs';
import {assertRuntimeBuildBinding} from './formal-required-acceptance.mjs';
import {
  evaluateMpPerformance,
  verifyProductionWireDigest,
  installUntimedPilot,
} from './mp-performance-eval.mjs';

const require = createRequire(import.meta.url);
const {chromium} = require(
  process.env.PLAYWRIGHT_MODULE || '/opt/shot/node_modules/playwright-core',
);

const workspace = fileURLToPath(new URL('../../../../../../', import.meta.url));
const dir = resolve(
  process.env.TH13_PERF_ARTIFACTS ||
    resolve(workspace, 'artifacts/th13-mp-performance'),
  process.env.TH13_CASE || 'lunatic-rtc-0',
);
const origin = process.env.TH13_ORIGIN || 'http://127.0.0.1:18760';
await mkdir(resolve(dir, '..'), {recursive: true});
await mkdir(dir); // Never overwrite or append to prior run evidence.

let interruptedSignal = null;
const sleepWaiters = new Set();
for (const sig of ['SIGTERM', 'SIGINT']) {
  process.once(sig, () => {
    interruptedSignal = sig;
    for (const wake of sleepWaiters) wake();
    sleepWaiters.clear();
  });
}
const interruptibleSleep = ms => new Promise(done => {
  if (interruptedSignal) return done();
  const wake = () => {
    clearTimeout(timer);
    sleepWaiters.delete(wake);
    done();
  };
  const timer = setTimeout(wake, ms);
  sleepWaiters.add(wake);
});

const expectedFiles = [];
let runtimeManifestBytes;
const hash = bytes => createHash('sha256').update(bytes).digest('hex');
const report = {
  started: new Date().toISOString(),
  mode: 'production-audit-disabled',
  scope: 'Production configuration multiplayer performance acceptance (4x CPU throttle, per-frame audit disabled, 60-frame wire digest active, untimed pilot outside C++ work callback). Does not replace formal-flow.mjs per-frame audit or independent original GPU pixel comparison.',
  errors: [],
  events: [],
  delivery: [],
  expectedFiles,
  conditions: Object.fromEntries([
    'TH13_ROUTE', 'TH13_RTT', 'TH13_JITTER', 'TH13_LOSS',
    'TH13_DUPLICATE_EVERY', 'TH13_BURST_EVERY', 'TH13_BURST_MS', 'TH13_DIFFICULTY',
  ].map(key => [key, process.env[key] || '0'])),
};
report.conditions.TH13_ROUTE = process.env.TH13_ROUTE || 'rtc';
report.conditions.TH13_DIFFICULTY = process.env.TH13_DIFFICULTY || '3';

const helperFiles = [
  'mp-performance.mjs', 'mp-performance-eval.mjs', 'frame-work.mjs',
  'formal-delivery.mjs', 'formal-required-acceptance.mjs', 'formal-pilot.mjs',
  'formal-network.mjs', 'formal-profile.mjs', 'hardware-identity.mjs',
  'persist-independent.mjs', 'bounded-operation.mjs', 'freeze-frame-work.mjs',
  'inspection-serialization.mjs',
];

const launchOptions = {
  headless: process.env.TH13_HEADFUL !== '1',
  channel: 'chromium',
  args: [
    '--no-sandbox',
    '--disable-dev-shm-usage',
    '--ignore-gpu-blocklist',
    '--disable-gpu-sandbox',
    '--use-angle=' + (process.env.TH13_ANGLE || 'gl-egl'),
    '--disable-software-rasterizer',
    '--enable-gpu',
    '--autoplay-policy=no-user-gesture-required',
    '--enable-logging=stderr',
    '--disable-background-timer-throttling',
    '--disable-backgrounding-occluded-windows',
    '--disable-renderer-backgrounding',
  ],
};

const PLAYER_COUNT = Number(process.env.TH13_PLAYERS || 3);
const sharedBrowser = process.env.TH13_SHARED_BROWSER === '1';
const browsers = [];
report.browserTopology = {sharedBrowser, launchOptions, seats: [], snapshots: []};
const pages = [];
const cpuSessions = [];
const workCaptures = [];
const workErrors = [];
const deliveryTasks = new Set();
const deliveryRequests = new Set();
const deliveryRequestIdentity = new WeakMap();
const deliveryFrames = new WeakMap();
const deliveryEpochs = new WeakMap();
const gameFrames = [];
const gameFrameGuids = [];
const gameDocumentEpochs = [];
report.playerCount = PLAYER_COUNT;
report.frameGuids = gameFrameGuids;
report.documentEpochs = gameDocumentEpochs;

let deliveryFrameSequence = 0;
const deliveryFrameGuid = frame => {
  if (!deliveryFrames.has(frame)) deliveryFrames.set(frame, `frame-${++deliveryFrameSequence}`);
  return deliveryFrames.get(frame);
};
const runtimeResponse = url => {
  try { return /\/th13-sdl\.(mjs|wasm)$/.test(new URL(url).pathname); }
  catch { return false; }
};
async function settleRuntimeDelivery(label) {
  await boundedOperation(label, async () => {
    while (deliveryTasks.size || deliveryRequests.size) {
      await Promise.all([...deliveryTasks]);
      if (deliveryRequests.size) await new Promise(r => setTimeout(r, 10));
    }
  }, 15000);
}

const inspectTimedOut = new Set();
const guardInspection = createInspectionGuard();
let closingBrowsers = false;
const workCursors = Array.from({length: PLAYER_COUNT}, () => ({failure: null}));
const requireRollback = Number(process.env.TH13_REQUIRE_ROLLBACK || 0) > 0;
report.cpuThrottlingRate = 4;
report.requestedCharacters = [0, 1, 3].slice(0, PLAYER_COUNT);
let profileCapture;

async function captureBrowserIdentity(browser, browserIndex, phase) {
  const session = await browser.newBrowserCDPSession();
  try {
    const [version, system, {processInfo}] = await Promise.all([
      session.send('Browser.getVersion'),
      session.send('SystemInfo.getInfo'),
      session.send('SystemInfo.getProcessInfo'),
    ]);
    const processes = await Promise.all(processInfo
      .filter(info => ['browser', 'GPU', 'renderer'].includes(info.type))
      .map(async info => ({
        ...info,
        executable: await readlink(`/proc/${info.id}/exe`),
        arguments: (await readFile(`/proc/${info.id}/cmdline`, 'utf8'))
          .split('\0').filter(Boolean),
      })));
    const contexts = await Promise.all(report.browserTopology.seats
      .filter(row => row.browserIndex === browserIndex)
      .map(async ({seat}) => ({seat, ...await pages[seat].evaluate(() => {
        const w = document.querySelector('#gameFrame')?.contentWindow;
        const canvas = w?.document.querySelector('canvas');
        const gl = canvas?.getContext('webgl2');
        if (!gl) return {error: 'Game has no WebGL2 context'};
        const info = gl.getExtension('WEBGL_debug_renderer_info');
        return {
          version: gl.getParameter(gl.VERSION),
          renderer: info && gl.getParameter(info.UNMASKED_RENDERER_WEBGL),
          vendor: info && gl.getParameter(info.UNMASKED_VENDOR_WEBGL),
        };
      })})));
    const identity = {
      phase, at: new Date().toISOString(), browserIndex,
      seats: report.browserTopology.seats.filter(row => row.browserIndex === browserIndex)
        .map(row => row.seat),
      version, gpu: system.gpu, processes, contexts,
    };
    report.browserTopology.snapshots.push(identity);
    await appendFile(dir + '/browser-identity.ndjson', JSON.stringify(identity) + '\n');
    return identity;
  } finally {
    await session.detach();
  }
}

const inspect = async (page, {expired = () => false} = {}) => {
  const state = await page.evaluate(({playerCount}) => {
    const frame = document.querySelector('#gameFrame');
    const w = frame?.contentWindow;
    const m = w?.Module;
    const read = name => {
      if (!m?.[name]) return null;
      const text = m.UTF8ToString(m[name]());
      return text ? JSON.parse(text) : {empty: true};
    };
    return {
      work: w?.__frameWork ? {
        capture: {...w.__frameWork.capture},
        samples: w.__frameWork.samples.slice(),
        errors: [...w.__frameWork.errors],
        attached: !m || m.onGameWorkFrame === w.__frameWork.observer,
      } : null,
      pilotDeferred: w?.__pilotDeferred ? {
        executed: w.__pilotDeferred.executed,
        hasPending: w.__pilotDeferred.pending !== null,
        attached: m?.onGameFrame === w.__pilotDeferred.wrapper,
      } : null,
      url: location.href,
      frame: frame?.src,
      visibility: document.visibilityState,
      wire: w?.__wireConditions ? {...w.__wireConditions, records: undefined} : null,
      roomVisible: !!document.querySelector('#mpRoomView .room-workspace-header')?.getClientRects().length,
      readyEnabled: !!document.querySelector('#mpReady:not(:disabled)')?.getClientRects().length,
      contextLoss: w?.__contextLoss,
      nativeError: m?.UTF8ToString(m?._th13_error?.() || 0),
      raf: w?.__rafProbe,
      pilot: w?.__pilot ? {
        stage: w.__pilot.stage,
        lastFrame: w.__pilot.lastFrame,
        decisions: w.__pilot.decisions,
        movementDecisions: w.__pilot.movementDecisions,
        eventsCount: w.__pilot.events?.length ?? 0,
      } : null,
      probe: m
        ? Array.from(m.HEAP32.subarray(
            m._th13_probe_state() >>> 2,
            (m._th13_probe_state() >>> 2) + 16,
          ))
        : null,
      room: JSON.parse(sessionStorage.getItem('eagler-touhou-th13mp-room-v1') || 'null'),
      runtime: m
        ? {
            net: read('_th13_debug_mp_net_state'),
            pending: read('_th13_debug_pending'),
            pause: read('_th13_debug_pause_state'),
            result: read('_th13_debug_result_state'),
            audit: read('_th13_mp_audit_take'),
            diagnostic: read('_th13_mp_diagnostic_report'),
            rollbackMetrics: read('_th13_debug_mp_net_rollback'),
            rollback: Array.from(
              m.HEAPU32.subarray(
                m._th13_mp_rollback_status() >>> 2,
                (m._th13_mp_rollback_status() >>> 2) + 9,
              ),
            ),
            stageFrame: m._th13_debug_stage_frame(),
            hostFrame: m._th13_frame(),
            seats: Array.from({length: playerCount}, (_, i) =>
              JSON.parse(m.UTF8ToString(m._th13_debug_mp_seat_state(i))),
            ),
            error: m.UTF8ToString(m._th13_mp_error()),
            route: w.__eaglerPeerTransport?.route,
            options: m.eaglerOptions,
          }
        : null,
    };
  }, {playerCount: PLAYER_COUNT});
  if (expired()) throw Error('Evidence inspection expired before persistence');
  const seat = pages.indexOf(page);
  await persistIndependent([
    ['work', async () => {
      if (state.work) {
        const cursor = workCursors[seat];
        await terminalStream(cursor, 'Work', async () => {
          const rows = state.work.samples;
          if (rows.length) {
            await appendFile(dir + `/work-${seat}.ndjson`,
              rows.map(row => JSON.stringify(row) + '\n').join(''));
            await page.evaluate(({count, first, last}) => {
              const work = document.querySelector('#gameFrame')?.contentWindow?.__frameWork;
              if (!work || work.samples[0]?.sequence !== first || work.samples[count - 1]?.sequence !== last)
                throw Error('Work evidence changed before persistence acknowledgement');
              work.samples.splice(0, count);
            }, {count: rows.length, first: rows[0].sequence, last: rows.at(-1).sequence});
          }
          workCaptures[seat] = state.work.capture;
          if (!state.work.attached) workErrors.push({seat, error: 'Work observer was replaced'});
          workErrors.push(...state.work.errors.map(error => ({seat, error})));
          state.work = {
            capture: state.work.capture,
            count: rows.length,
            errors: state.work.errors,
            attached: state.work.attached,
          };
        });
      }
    }],
  ]);
  return state;
};

const inspectBounded = async page => {
  const seat = pages.indexOf(page);
  if (inspectTimedOut.has(seat)) throw Error(`Seat ${seat} prior inspection timed out; tail incomplete`);
  try {
    return await boundedOperation(`seat ${seat} inspect`,
      expired => guardInspection(seat, () => inspect(page, {expired})), 15000);
  } catch (error) {
    if (String(error).includes('timed out')) inspectTimedOut.add(seat);
    throw error;
  }
};
const inspectAll = () => settleInspections(pages.map(page => () => inspectBounded(page)));

async function dismiss(page) {
  await page.waitForFunction(
    () =>
      localStorage.getItem('eagler-touhou-first-use-notice-seen-v1') === '1' ||
      document.querySelector('#firstUseNoticeDialog')?.open === true,
  );
  if (await page.locator('#decisionDialog[open]:not(.closing)').count())
    await page.locator('#decisionConfirm').click();
}

try {
  if (![2, 3].includes(PLAYER_COUNT)) throw Error('TH13_PLAYERS must be 2 or 3');
  if (!['0', '1', '2', '3', '4'].includes(report.conditions.TH13_DIFFICULTY))
    throw Error('TH13_DIFFICULTY must be 0, 1, 2, 3 or 4');
  report.helperHashes = {};
  await Promise.all(helperFiles.map(async name => {
    const bytes = await readFile(new URL(name, import.meta.url));
    await writeFile(resolve(dir, name), bytes);
    report.helperHashes[name] = hash(bytes);
  }));
  expectedFiles.push(...await Promise.all(['th13-sdl.mjs', 'th13-sdl.wasm'].map(async name => {
    const bytes = await readFile(process.env.TH13_MP_BUILD
      ? resolve(process.env.TH13_MP_BUILD, name)
      : new URL('../../../build-eagler-multiplayer/' + name, import.meta.url));
    return {name, bytes: bytes.length, sha256: hash(bytes)};
  })));
  runtimeManifestBytes = await readFile(process.env.TH13_MP_BUILD
    ? resolve(process.env.TH13_MP_BUILD, 'build.json')
    : new URL('../../../build-eagler-multiplayer/build.json', import.meta.url));
  const binding = assertRuntimeBuildBinding({expectedFiles, runtimeManifestBytes});
  await writeFile(resolve(dir, 'runtime-build.json'), runtimeManifestBytes);
  report.runtimeBuild = {path: 'runtime-build.json', sha256: binding.sha256};

  for (let seat = 0; seat < PLAYER_COUNT; seat++) {
    const browser = sharedBrowser && browsers.length
      ? browsers[0] : await chromium.launch(launchOptions);
    if (!browsers.includes(browser)) {
      browsers.push(browser);
      browser.on('disconnected', () => {
        if (!closingBrowsers)
          report.errors.push({message: 'Browser disconnected', browserIndex: browsers.indexOf(browser)});
      });
    }
    const browserIndex = browsers.indexOf(browser);
    report.browserTopology.seats.push({seat, browserIndex});
    const context = await browser.newContext({
      viewport: {width: 1280, height: 900},
      screen: {width: 640, height: 480},
      hasTouch: true,
      isMobile: true,
    });
    if (process.env.TH13_RELAY_PORT) {
      await context.addInitScript(port => {
        const Original = WebSocket;
        globalThis.WebSocket = class extends Original {
          constructor(url, ...args) {
            super(String(url).replace(':18761', ':' + port), ...args);
          }
        };
      }, process.env.TH13_RELAY_PORT);
    }
    await context.addInitScript(installWireConditions, {
      route: process.env.TH13_ROUTE || 'rtc',
      rtt: +(process.env.TH13_RTT || 0),
      jitter: +(process.env.TH13_JITTER || 0),
      loss: +(process.env.TH13_LOSS || 0),
      duplicateEvery: +(process.env.TH13_DUPLICATE_EVERY || 0),
      burstEvery: +(process.env.TH13_BURST_EVERY || 0),
      burstMs: +(process.env.TH13_BURST_MS || 0),
    });
    await context.addInitScript(() => {
      const work = (globalThis.__frameWork = {
        samples: [],
        errors: [],
        capture: {first: null, last: null, count: 0},
      });
      let moduleValue;
      Object.defineProperty(globalThis, 'Module', {
        configurable: true,
        get: () => moduleValue,
        set: value => {
          moduleValue = value;
          if (!value || typeof value !== 'object') return;
          const previous = value.onGameWorkFrame;
          work.observer = value.onGameWorkFrame = row => {
            work.samples.push({...row});
            if (!work.capture.count) work.capture.first = row.sequence;
            work.capture.last = row.sequence;
            ++work.capture.count;
            previous?.(row);
            const deferred = globalThis.__pilotDeferred;
            if (deferred && deferred.pending !== null) {
              const args = deferred.pending;
              deferred.pending = null;
              try {
                deferred.run(...args);
                ++deferred.executed;
              } catch (error) {
                work.errors.push(`Deferred pilot failed: ${error?.stack || error}`);
              }
            }
          };
        },
      });
      localStorage.setItem(
        'eagler-touhou-game-options-v1-th13mp',
        JSON.stringify({options: {renderScale: 1, thpracEnabled: false}}),
      );
      localStorage.setItem(
        'eagler-touhou-game-options-v1-th13',
        JSON.stringify({options: {renderScale: 1, thpracEnabled: false}}),
      );
      window.addEventListener(
        'webglcontextlost',
        e => {
          window.__contextLoss = {message: e.statusMessage, t: performance.now()};
        },
        true,
      );
      const original = window.requestAnimationFrame;
      window.__rafProbe = {calls: 0, scheduled: 0, first: 0, last: 0};
      window.requestAnimationFrame = function (cb) {
        ++window.__rafProbe.scheduled;
        return original.call(this, t => {
          const p = window.__rafProbe;
          ++p.calls;
          p.first ||= t;
          p.last = t;
          cb(t);
        });
      };
    });

    const page = await context.newPage();
    pages.push(page);
    page.on('crash', () => report.errors.push({seat, message: 'Browser page crashed'}));
    page.on('close', () => {
      if (!closingBrowsers) report.errors.push({seat, message: 'Browser page closed'});
    });
    page.on('framenavigated', frame => {
      deliveryEpochs.set(frame, (deliveryEpochs.get(frame) || 0) + 1);
      if (gameFrames[seat] === frame)
        report.errors.push({seat, message: 'Runtime iframe navigated after room identity binding'});
    });
    const cpu = await context.newCDPSession(page);
    await cpu.send('Emulation.setCPUThrottlingRate', {rate: 4});
    cpu.on('disconnected', () => {
      if (!closingBrowsers)
        report.errors.push({seat, message: 'CPU throttle session disconnected'});
    });
    cpuSessions.push(cpu);
    page.setDefaultTimeout(10000);
    await page.addLocatorHandler(page.locator('#firstUseNoticeDialog[open]'),
      async () => { await page.locator('#firstUseNoticeClose').click(); });
    await page.addLocatorHandler(page.locator('#decisionDialog[open]:not(.closing)'),
      async () => { await page.locator('#decisionConfirm').click(); });
    page.on('requestfailed', r => {
      deliveryRequests.delete(r);
      report.errors.push({seat, url: r.url(), message: r.failure()?.errorText});
    });
    page.on('request', request => {
      if (runtimeResponse(request.url())) {
        deliveryRequests.add(request);
        try {
          const frame = request.frame();
          deliveryRequestIdentity.set(request, {
            frameGuid: deliveryFrameGuid(frame),
            documentEpoch: deliveryEpochs.get(frame) || 0,
          });
        } catch (error) {
          report.errors.push({seat, url: request.url(), message: `Runtime request frame: ${error}`});
        }
      }
    });
    page.on('requestfinished', request => deliveryRequests.delete(request));
    page.on('response', r => {
      if (r.status() >= 400) report.errors.push({seat, url: r.url(), status: r.status()});
    });
    page.on('pageerror', e => report.errors.push({seat, message: e.message, stack: e.stack}));
    page.on('console', m => {
      if (m.type() === 'error') report.errors.push({seat, message: m.text()});
    });
    page.on('response', response => {
      if (runtimeResponse(response.url())) {
        const task = (async () => {
          const identity = deliveryRequestIdentity.get(response.request());
          assert(identity, 'Runtime response has no observed request identity');
          const bytes = await response.body();
          const row = {
            seat,
            url: response.url(),
            ...identity,
            bytes: bytes.length,
            sha256: hash(bytes),
          };
          report.delivery.push(row);
          const expected = expectedFiles.find(file => new URL(row.url).pathname.endsWith('/' + file.name));
          assert(expected, 'Unexpected runtime response filename');
          assert.equal(row.bytes, expected.bytes, `Seat ${seat}: runtime response size`);
          assert.equal(row.sha256, expected.sha256, `Seat ${seat}: runtime response hash`);
          if (gameFrameGuids[seat])
            assert.equal(row.frameGuid, gameFrameGuids[seat], `Seat ${seat}: runtime iframe changed`);
          if (gameDocumentEpochs[seat] !== undefined)
            assert.equal(row.documentEpoch, gameDocumentEpochs[seat], `Seat ${seat}: runtime document changed`);
        })().catch(error => {
          report.errors.push({seat, url: response.url(), message: `Runtime delivery body: ${error}`});
        });
        deliveryTasks.add(task);
        task.finally(() => deliveryTasks.delete(task));
      }
    });

    await page.goto(origin + '/?renderScale=1');
    await page.waitForFunction(() => window.__eaglerBoot?.done === true, {}, {timeout: 60000});
    await dismiss(page);
    await page.getByRole('link', {name: '联机', exact: true}).click();
    await page.goto(origin + '/lobby.html?game=th13mp');
    await page.waitForFunction(() => !document.querySelector('#createButton')?.disabled);
    await dismiss(page);
    await page.locator(seat ? '#codeButton' : '#createButton').click();
    if (!seat) {
      await page.locator('#capacitySelect').selectOption(String(PLAYER_COUNT));
      await page.locator('#difficultySelect').selectOption(process.env.TH13_DIFFICULTY || '3');
    } else {
      await page.locator('#roomCodeInput').fill(String(report.room));
    }
    await page.locator('#submitRoom').click();
    await page.waitForSelector('#mpRoomView:not([hidden])', {timeout: 60000});
    await dismiss(page);
    await page.waitForFunction(() => {
      if (document.querySelector('#decisionDialog[open]:not(.closing)'))
        document.querySelector('#decisionConfirm').click();
      return !!document.querySelector('#mpReady:not(:disabled)') &&
        !!document.querySelector('#gameFrame')?.contentWindow?.Module?._th13_probe_state;
    }, {}, {timeout: 120000});

    const desired = report.requestedCharacters[seat];
    const characterField = page.locator('#mpLoadoutField0');
    if (await characterField.count()) {
      await characterField.selectOption(String(desired));
      await page.waitForFunction(value =>
        document.querySelector('#mpLoadoutField0')?.value === String(value) &&
        !document.querySelector('#mpLoadoutField0')?.disabled, desired);
    } else {
      await page.locator(`[data-mp-seat="${seat}"] .mp-seat-edit`).click();
      for (let step = 0; step < desired; ++step) {
        await page.locator('#mpLoadoutNextSeat').click();
        await page.waitForTimeout(100);
      }
      await page.locator('#mpRoomPanelClose').click();
    }
    await page.evaluate(() => {
      // Production config: ensure per-frame audit stays explicitly off (0).
      const m = document.querySelector('#gameFrame')?.contentWindow?.Module;
      m?._th13_mp_audit?.(0);
      const u = new URL(location.href);
      u.searchParams.set('renderScale', '1');
      history.replaceState(null, '', u);
    });

    const state = await inspectBounded(page);
    const frameElement = await page.locator('#gameFrame').elementHandle();
    const gameFrame = await frameElement.contentFrame();
    assert(gameFrame && gameFrame !== page.mainFrame(), `Seat ${seat}: missing game iframe`);
    gameFrames[seat] = gameFrame;
    gameFrameGuids[seat] = deliveryFrameGuid(gameFrame);
    gameDocumentEpochs[seat] = deliveryEpochs.get(gameFrame) || 0;
    report.events.push({seat, at: 'room', state});
    if (!seat) report.room = (await page.locator('#mpRoomCode').innerText()).trim();
    await captureBrowserIdentity(browser, browserIndex, 'room');
    console.log('room entered', seat, report.room);
  }

  for (const page of pages) {
    await page.waitForFunction(() => {
      if (document.querySelector('#decisionDialog[open]:not(.closing)'))
        document.querySelector('#decisionConfirm').click();
      return !!document.querySelector('#mpReady:not(:disabled)');
    }, {}, {timeout: 60000});
    await page.locator('#mpReady').click({timeout: 60000});
    await page.waitForFunction(
      () =>
        document.querySelector('#decisionDialog')?.open ||
        JSON.parse(sessionStorage.getItem('eagler-touhou-th13mp-room-v1') || '{}').ready === true,
      {},
      {timeout: 60000},
    );
    if (await page.locator('#decisionDialog[open]:not(.closing)').count())
      await page.locator('#decisionConfirm').click();
    await page.waitForFunction(
      () => JSON.parse(sessionStorage.getItem('eagler-touhou-th13mp-room-v1') || '{}').ready === true,
      {},
      {timeout: 60000},
    );
  }

  report.events.push({at: 'ready', states: await inspectAll()});
  await settleRuntimeDelivery('Initial runtime response bodies');
  report.initialDelivery = assertBuildDelivery({
    expectedFiles,
    delivery: report.delivery,
    playerCount: PLAYER_COUNT,
    frameGuids: gameFrameGuids,
    documentEpochs: gameDocumentEpochs,
  });
  assert.equal(report.errors.length, 0, 'Browser errors before starting');

  await pages[0].locator('#mpStartGame').click({timeout: 30000});
  console.log('started');
  await Promise.all(
    pages.map(async page => {
      await page.waitForFunction(
        () => {
          const d = document.querySelector('#decisionDialog[open]:not(.closing)');
          if (d) document.querySelector('#decisionConfirm').click();
          const m = document.querySelector('#gameFrame')?.contentWindow?.Module;
          return m && JSON.parse(m.UTF8ToString(m._th13_debug_mp_net_state())).sim_frame > 0;
        },
        {},
        {timeout: 60000},
      );
    }),
  );

  await Promise.all(browsers.map((browser, index) =>
    captureBrowserIdentity(browser, index, 'gameplay')));
  for (const identity of report.browserTopology.snapshots.filter(row => row.phase === 'gameplay')) {
    identity.hardwareVerification = assertHardwareIdentity(identity);
  }
  assert.equal(report.browserTopology.snapshots.filter(row => row.phase === 'gameplay').length,
    browsers.length, 'Every browser has a gameplay identity');

  const activeStates = await inspectAll();
  for (const state of activeStates) {
    assert.deepEqual(
      state.runtime.seats.map(player => player.character),
      report.requestedCharacters,
      'Actual runtime character seats',
    );
  }

  const pilotSource = String(installPilot);
  for (let seat = 0; seat < pages.length; seat++) {
    await pages[seat].evaluate(installUntimedPilot, {seat, pilotSource});
  }
  profileCapture = await startCpuProfiles({pages, sessions: cpuSessions, directory: dir});

  for (const page of pages) {
    assert.equal(
      await page.evaluate(
        () => document.querySelector('#gameFrame').contentWindow.Module.eaglerOptions.netplayInputDelay,
      ),
      0,
      'Default input delay',
    );
  }

  let lastProgress = Date.now();
  let lastFrame = 0;
  const stageSet = new Set();
  const start = Date.now();
  let seenEnding = false;
  let seenExtraEnd = false;
  const routes = Array.from({length: PLAYER_COUNT}, () => new Set());
  const maxCorrections = Array(PLAYER_COUNT).fill(0);
  const maxWireDigests = Array.from({length: PLAYER_COUNT}, () => ({sent: 0, received: 0, confirmed: 0}));
  let initialStates = activeStates;
  const timeoutMs = Number(process.env.TH13_TIMEOUT_MINUTES || 90) * 60 * 1000;

  while (Date.now() - start < timeoutMs) {
    if (interruptedSignal) {
      throw Error(`Interrupted by ${interruptedSignal} (bounded prefix run; never a formal pass)`);
    }
    if (!initialStates) await interruptibleSleep(2000);
    if (interruptedSignal) {
      throw Error(`Interrupted by ${interruptedSignal} (bounded prefix run; never a formal pass)`);
    }
    const states = initialStates || await inspectAll();
    initialStates = null;

    for (let seat = 0; seat < PLAYER_COUNT; seat++) {
      const state = states[seat];
      const runtime = state.runtime;
      assert(runtime, 'Missing runtime');
      if (runtime.net.sim_frame > 0 && runtime.route) routes[seat].add(runtime.route);
      maxCorrections[seat] = Math.max(maxCorrections[seat], runtime.rollback[2]);
      assert.equal(state.nativeError, '', 'Native game error');
      assert(!state.contextLoss, 'WebGL context lost');

      const wireCheck = verifyProductionWireDigest({runtime, seat, playerCount: PLAYER_COUNT});
      assert(wireCheck.passed, wireCheck.errors.join('; '));
      const wd = runtime.diagnostic?.wireDigest;
      if (wd) {
        maxWireDigests[seat].sent = Math.max(maxWireDigests[seat].sent, wd.sent || 0);
        maxWireDigests[seat].received = Math.max(maxWireDigests[seat].received, wd.received || 0);
      }
      if (Number.isInteger(runtime.diagnostic?.confirmedThrough)) {
        maxWireDigests[seat].confirmed = Math.max(maxWireDigests[seat].confirmed, runtime.diagnostic.confirmedThrough);
      }
      await appendFile(dir + `/health-${seat}.ndjson`, JSON.stringify(state) + '\n');
    }

    report.current = states;
    report.wireDigestProgress = maxWireDigests;
    for (const s of states) {
      if (s.probe?.[0] === 1) stageSet.add(s.probe[2]);
      if (s.probe?.[0] === 2) seenEnding = true;
      if (s.runtime.pause.screen === 4) seenExtraEnd = true;
    }
    if (states[0].runtime.hostFrame !== lastFrame) {
      lastFrame = states[0].runtime.hostFrame;
      lastProgress = Date.now();
    }
    console.log(JSON.stringify({
      wireDigests: maxWireDigests,
      clients: states.map(s => ({
        stage: s.probe?.[2],
        frame: s.runtime.stageFrame,
        pause: s.runtime.pause,
        net: s.runtime.net,
        rollback: s.runtime.rollbackMetrics,
        seats: s.runtime.seats,
      })),
    }));
    await writeFile(dir + '/progress.json', JSON.stringify(report, null, 2));
    assert(Date.now() - lastProgress < 30000, 'No simulation progress for 30 seconds');
    assert.equal(report.errors.length, 0, 'Browser errors');

    if (
      states.every(s => s.probe?.[0] === 0 && s.probe?.[8] === 1 && s.roomVisible && s.readyEnabled) &&
      maxWireDigests.every(w => w.confirmed > 5000)
    ) {
      report.completedStages = [...stageSet];
      report.passed = true;
      break;
    }
  }

  assert(report.passed, 'Full flow did not complete and return to the ready room');
  report.transport = [];
  for (let seat = 0; seat < PLAYER_COUNT; seat++) {
    const wire = await pages[seat].evaluate(() => document.querySelector('#gameFrame').contentWindow.__wireConditions);
    const expectedRoute = process.env.TH13_ROUTE || 'rtc';
    assert.deepEqual([...routes[seat]], [expectedRoute], 'Actual transport route');
    assert(wire.sent > 0 && wire.delivered > 0, 'Transport must carry real packets');
    const expectedSent = Math.floor(maxWireDigests[seat].confirmed / 60);
    assert(maxWireDigests[seat].sent >= expectedSent,
      `Seat ${seat}: wire digests sent ${maxWireDigests[seat].sent} below ${expectedSent}`);
    assert(maxWireDigests[seat].received >= (PLAYER_COUNT - 1) * Math.max(1, expectedSent - 1),
      `Seat ${seat}: wire digests received ${maxWireDigests[seat].received} below the 60-frame cadence`);
    if (+process.env.TH13_LOSS > 0 && expectedRoute === 'rtc') assert(wire.dropped > 0, 'Input loss must occur');
    if (+process.env.TH13_DUPLICATE_EVERY > 0) assert(wire.duplicated > 0, 'Duplicates must occur');
    if (+process.env.TH13_BURST_EVERY > 0) assert(wire.bursts > 0, 'Burst delay must occur');
    if (+process.env.TH13_JITTER > 0) assert(wire.reordered > 0, 'Jitter must reorder deliveries');
    if (+process.env.TH13_REQUIRE_ROLLBACK > 0) assert(maxCorrections[seat] > 0, 'Rollback must actually occur');
    report.transport.push({
      seat,
      routes: [...routes[seat]],
      maxCorrections: maxCorrections[seat],
      wireDigests: maxWireDigests[seat],
      wire,
      datagramLossApplied: expectedRoute === 'rtc' && +process.env.TH13_LOSS > 0,
    });
  }

  const requiredStages = process.env.TH13_DIFFICULTY === '4' ? [7] : [1, 2, 3, 4, 5, 6];
  assert.deepEqual(report.completedStages, requiredStages, 'Required stages');
  assert(
    process.env.TH13_DIFFICULTY === '4' ? seenExtraEnd : seenEnding,
    'Clear result/Ending must be observed',
  );

  await settleRuntimeDelivery('Final runtime response bodies');
  for (const [seat, page] of pages.entries()) {
    const element = await page.locator('#gameFrame').elementHandle();
    assert.equal(await element.contentFrame(), gameFrames[seat], `Seat ${seat}: game iframe replaced`);
  }
  report.finalDelivery = assertBuildDelivery({
    expectedFiles,
    delivery: report.delivery,
    playerCount: PLAYER_COUNT,
    frameGuids: gameFrameGuids,
    documentEpochs: gameDocumentEpochs,
  });

  report.saved = [];
  let referenceInputFrames = null;
  for (let seat = 0; seat < PLAYER_COUNT; seat++) {
    const saved = await pages[seat].evaluate(async () => {
      const w = document.querySelector('#gameFrame').contentWindow;
      const m = w.Module;
      const names = m.FS.readdir('/save/replay').filter(n => /^th13mp_\d{2}\.rpy$/.test(n));
      if (!names.length) throw Error('No replay saved through result menu');
      const request = 'full-flow-sync';
      const epoch = Number(new URL(w.location.href).searchParams.get('runtimeEpoch'));
      const ack = await new Promise((resolveAck, rejectAck) => {
        const timer = setTimeout(() => rejectAck(Error('Replay sync timed out')), 10000);
        const on = e => {
          if (e.source === w && e.data.request === request) {
            clearTimeout(timer);
            removeEventListener('message', on);
            resolveAck(e.data);
          }
        };
        addEventListener('message', on);
        w.postMessage({protocol: 'eagler-touhou/1', game: 'th13', epoch, request, command: 'sync'}, location.origin);
      });
      if (!ack.ok) throw Error('Replay sync failed: ' + ack.error);
      const db = await new Promise((resolveDb, rejectDb) => {
        const r = w.indexedDB.open('th13mp.replays.v1', 1);
        r.onsuccess = () => resolveDb(r.result);
        r.onerror = () => rejectDb(r.error);
      });
      const records = [];
      for (const name of names) {
        const record = await new Promise((resolveRec, rejectRec) => {
          const r = db.transaction('files').objectStore('files').get(name);
          r.onsuccess = () => resolveRec(r.result);
          r.onerror = () => rejectRec(r.error);
        });
        const bytes = m.FS.readFile('/save/replay/' + name);
        if (!record || record.bytes.length !== bytes.length || record.bytes.some((b, i) => b !== bytes[i]))
          throw Error('Durable replay differs: ' + name);
        records.push({name, bytes: Array.from(bytes)});
      }
      db.close();
      return records;
    });
    const files = [];
    for (const file of saved) {
      const bytes = Buffer.from(file.bytes);
      const words = 22;
      const replayBody = 12 + bytes.readUInt32LE(8);
      assert(replayBody + 36 <= bytes.length, 'Replay body is complete');
      const inputFrames = bytes.readUInt32LE(replayBody + 32);
      assert(inputFrames > 5000, 'Saved replay input history is non-trivial');
      if (referenceInputFrames === null) referenceInputFrames = inputFrames;
      assert.equal(inputFrames, referenceInputFrames, 'Saved replay input history matches across seats');
      assert(maxWireDigests[seat].sent >= Math.floor(referenceInputFrames / 60),
        `Seat ${seat}: wire digests do not cover the saved input history`);
      assert.equal(bytes.readUInt32LE(12 + words * 4 + 8 + 4), 1, 'Replay must contain completed clear');
      const nameLength = bytes.readUInt32LE(12 + words * 4 + 8 + 4 + 4);
      let offset = 12 + words * 4 + 8 + 4 + 4 + 4 + nameLength;
      const count = bytes.readUInt32LE(offset);
      const chapters = [];
      offset += 4;
      for (let i = 0; i < count; i++) {
        chapters.push(bytes.readUInt32LE(offset));
        offset += 8 + bytes.readUInt32LE(offset + 4);
      }
      assert.deepEqual(chapters, report.completedStages, 'Saved replay chapters');
      await writeFile(dir + `/seat-${seat}-` + file.name, bytes);
      files.push({
        name: file.name,
        bytes: bytes.length,
        sha256: hash(bytes),
        chapters,
        inputFrames,
      });
    }
    report.saved.push({seat, files});
  }
} catch (error) {
  report.passed = false;
  report.failure = error.stack;
  console.error(error.stack);
} finally {
  const freezes = await Promise.allSettled(pages.map((page, seat) =>
    inspectTimedOut.has(seat)
      ? Promise.reject(Error('Prior inspection timed out; tail incomplete'))
      : boundedOperation(`seat ${seat} freeze`, () => page.evaluate(freezeFrameWork), 15000)));
  const boundaries = freezes.map((result, seat) => {
    if (result.status === 'fulfilled') return result.value;
    workErrors.push({seat, error: `Final evidence freeze: ${result.reason}`});
    return null;
  });
  if (profileCapture) {
    try {
      report.cpuProfiles = await boundedOperation('finish CPU profiles', () => profileCapture.finish(), 35000);
      if (!report.cpuProfiles.passed) workErrors.push({error: 'CPU profiling incomplete'});
    } catch (error) {
      report.profileError = String(error);
      workErrors.push({error: String(error)});
    }
  }
  report.performance = [];
  const finalFlushes = await Promise.allSettled(pages.map(async (page, seat) => {
    const boundary = boundaries[seat];
    const final = await inspectBounded(page);
    if (!boundary || boundary.unmatchedCandidate || !final.work ||
        boundary.count !== final.work.capture.count ||
        boundary.first !== final.work.capture.first ||
        boundary.last !== final.work.capture.last) {
      throw Error('Frozen work boundary differs from persisted capture');
    }
    if (final.pilotDeferred?.hasPending) {
      throw Error('Unconsumed deferred pilot tick at freeze boundary');
    }
    if (!final.pilotDeferred?.attached || !(final.pilotDeferred.executed > 0)) {
      throw Error('Untimed pilot was not attached outside the timed window');
    }
    (report.finalBoundaries ||= [])[seat] = boundary;
  }));

  const expectedStages = process.env.TH13_DIFFICULTY === '4' ? [7] : [1, 2, 3, 4, 5, 6];
  for (let seat = 0; seat < pages.length; ++seat) {
    if (finalFlushes[seat].status === 'rejected')
      workErrors.push({seat, error: `Final evidence flush: ${finalFlushes[seat].reason}`});
    try {
      const samples = (await readFile(dir + `/work-${seat}.ndjson`, 'utf8'))
        .split('\n').filter(Boolean).map(JSON.parse);
      report.performance.push({
        seat,
        ...evaluateMpPerformance(samples, {
          capture: workCaptures[seat],
          expectedFirstSequence: 1,
          expectedStages,
          expectedPhases: ['menu', 'loading', 'active'],
          requireZero: true,
          requireCatchup: true,
          requireRollback,
          minForwardFps: 55,
          minForwardFramesPerStage: 1000,
        }),
      });
    } catch (error) {
      report.performance.push({seat, passed: false, error: String(error)});
    }
  }

  report.gameplayPassed = report.passed === true;
  report.workErrors = workErrors;
  report.passed = report.gameplayPassed &&
    report.performance.length === PLAYER_COUNT &&
    report.performance.every(row => row.passed) &&
    !sharedBrowser &&
    !workErrors.length &&
    !report.errors.length;

  try {
    const identityResults = await Promise.allSettled(browsers.map((browser, index) =>
      boundedOperation(`browser ${index} final identity`, () => captureBrowserIdentity(browser, index, 'final'), 15000)));
    report.browserTopology.finalIdentityErrors = identityResults.flatMap((result, browserIndex) =>
      result.status === 'rejected' ? [{browserIndex, error: String(result.reason)}] : []);
    if (report.browserTopology.finalIdentityErrors.length) report.passed = false;
    for (const identity of report.browserTopology.snapshots.filter(row => row.phase === 'final')) {
      try {
        identity.hardwareVerification = assertHardwareIdentity(identity);
      } catch (error) {
        report.errors.push({message: `Final hardware identity: ${error}`});
        report.passed = false;
      }
    }
  } finally {
    closingBrowsers = true;
    const closed = await Promise.allSettled(browsers.map((browser, index) =>
      boundedOperation(`browser ${index} close`, () => browser.close(), 15000)));
    for (const [index, result] of closed.entries()) {
      if (result.status === 'rejected') {
        report.errors.push({browserIndex: index, message: `Browser close failed: ${result.reason}`});
        report.passed = false;
      }
    }
    try {
      await settleRuntimeDelivery('Closed runtime response bodies');
      report.closedDelivery = assertBuildDelivery({
        expectedFiles,
        delivery: report.delivery,
        playerCount: PLAYER_COUNT,
        frameGuids: gameFrameGuids,
        documentEpochs: gameDocumentEpochs,
      });
      report.deliveryBoundary = {
        closed: closed.length === (sharedBrowser ? 1 : PLAYER_COUNT) &&
          closed.every(result => result.status === 'fulfilled'),
        settled: true,
        pendingRequests: deliveryRequests.size,
        pendingBodies: deliveryTasks.size,
      };
    } catch (error) {
      report.errors.push({message: `Final runtime delivery: ${error}`});
      report.passed = false;
    }
    if (report.errors.length) report.passed = false;
    report.finished = new Date().toISOString();
    await writeFile(dir + '/mp-performance.json', JSON.stringify(report, null, 2) + '\n');
  }
}

if (!report.passed) process.exitCode = 1;
