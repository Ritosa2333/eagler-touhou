import assert from 'node:assert/strict';
import { execFileSync } from 'node:child_process';
import { createReadStream, readFileSync, statSync } from 'node:fs';
import { createServer } from 'node:http';
import { createRequire } from 'node:module';
import { resolve, sep } from 'node:path';
import { test } from 'node:test';
import { tmpdir } from 'node:os';
import { mkdtemp, rm } from 'node:fs/promises';
import { findChromiumExecutable } from '../../../../launcher/lib/chromium-executable.mjs';
import { Keyboard } from './input.mjs';
import { RetraceClock } from './clock.mjs';

test('keyboard samples quick Z edges and retains the other Shift key', () => {
    const input = new Keyboard();
    input.event('KeyZ', true);
    input.event('KeyZ', false);
    input.event('ShiftLeft', true);
    input.event('ShiftRight', true);
    input.event('ShiftLeft', false);
    assert.deepEqual(input.sample(), [1040, true]);
    assert.deepEqual(input.sample(), [1024, false]);
    input.clear();
    assert.deepEqual(input.sample(), [0, false]);
});

test('clock follows PC-98 retraces on 60 Hz and does not replay time spent suspended', () => {
    const clock = new RetraceClock(1000 * 9328 / 526315);
    let ticks = 0;
    const tick = () => ++ticks;
    for (let i = 0; i <= 600; ++i) clock.frame(i * 1000 / 60, () => 1, tick);
    assert.equal(ticks, 565);
    clock.frame(40000, () => 2, tick);
    assert.equal(ticks, 565);
    clock.frame(40020, () => 2, tick);
    assert.equal(ticks, 566);
});

test('Stage 1 browser, keyboard and 900 native replay samples', { timeout: 60_000 }, async () => {
    const [out, probe] = process.argv.slice(2);
    assert(out && probe, 'Usage: node web/browser.test.mjs <web build> <native replay probe>');
    const root = resolve(out);
    const require = createRequire(process.env.SOEW_BROWSER_DEPS ||
        new URL('../../../../launcher/package.json', import.meta.url));
    const {launch} = require('puppeteer-core');
    const dir = await mkdtemp(resolve(tmpdir(), 'soew-replay-'));
    const rep = resolve(dir, 'stage1.rep');
    const server = createServer((request, response) => {
        const pathname = decodeURIComponent(new URL(request.url, 'http://localhost').pathname);
        const path = resolve(root, '.' + (pathname === '/' ? '/index.html' : pathname));
        if (!path.startsWith(root + sep) || !statSync(path, {throwIfNoEntry: false})?.isFile()) {
            response.writeHead(404).end();
            return;
        }
        const ext = path.split('.').at(-1);
        response.setHeader('Content-Type', {
            html: 'text/html', mjs: 'text/javascript', wasm: 'application/wasm', json: 'application/json',
        }[ext] ?? 'application/octet-stream');
        createReadStream(path).pipe(response);
    });
    let browser;
    try {
        const expected = JSON.parse(execFileSync(resolve(probe), [resolve(root, 'content'), rep], {encoding: 'utf8'}));
        assert.equal(expected.length, 901);
        await new Promise(done => server.listen(0, '127.0.0.1', done));
        browser = await launch({executablePath: await findChromiumExecutable(), headless: true});
        const page = await browser.newPage();
        const errors = [];
        page.on('pageerror', error => errors.push(error.message));
        await page.goto(`http://127.0.0.1:${server.address().port}/`);
        await page.waitForFunction(() => {
            const c = document.querySelector('canvas');
            return c.getContext('2d').getImageData(32, 16, 384, 368).data.some((byte, i) => i % 4 !== 3 && byte !== 0);
        });
        await page.keyboard.down('ArrowLeft');
        await page.keyboard.down('KeyZ');
        await page.waitForFunction(() => document.querySelector('canvas').getContext('2d')
            .getImageData(80, 328, 88, 48).data.some((byte, i) => i % 4 !== 3 && byte > 192));
        await page.keyboard.up('KeyZ');
        await page.keyboard.up('ArrowLeft');
        if (process.env.SOEW_SCREENSHOT) await page.screenshot({path: process.env.SOEW_SCREENSHOT});

        const result = await page.evaluate(async ({bytes, expected}) => {
            const {createGame} = await import('./shell.mjs');
            const canvas = document.createElement('canvas');
            canvas.width = 640;
            canvas.height = 400;
            const game = await createGame(canvas);
            game.start();
            const x = game.playerX;
            for (let i = 0; i < 4; i++) game.step(4);
            if (game.playerX >= x) throw Error('Left did not move the original player');
            game.openReplay(new Uint8Array(bytes));
            let running = true;
            for (let i = 0; i < expected.length; i++) {
                if (game.ticks !== i || game.digest !== expected[i]) {
                    throw Error(`Replay mismatch at ${i}: ${game.digest} != ${expected[i]}`);
                }
                if (i + 1 < expected.length) running = game.step();
            }
            game.draw();
            const pixels = canvas.getContext('2d').getImageData(0, 0, 640, 400).data;
            let field = false, hud = false;
            for (let p = 0; p < pixels.length; p += 4) {
                if (!(pixels[p] || pixels[p + 1] || pixels[p + 2])) continue;
                const x = (p / 4) % 640;
                if (x >= 32 && x < 416) field = true;
                if (x >= 448 && x < 576) hud = true;
            }
            return {running, field, hud};
        }, {bytes: [...readFileSync(rep)], expected});
        assert.deepEqual(result, {running: false, field: true, hud: true});
        assert.deepEqual(errors, []);
    } finally {
        await browser?.close();
        if (server.listening) await new Promise(done => server.close(done));
        await rm(dir, {recursive: true, force: true});
    }
});
