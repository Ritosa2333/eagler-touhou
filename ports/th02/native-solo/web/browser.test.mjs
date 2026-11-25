import assert from 'node:assert/strict';
import { execFileSync } from 'node:child_process';
import { readFileSync } from 'node:fs';
import { createRequire } from 'node:module';
import { resolve } from 'node:path';
import { test } from 'node:test';
import { tmpdir } from 'node:os';
import { mkdtemp, rm } from 'node:fs/promises';
import { findChromiumExecutable } from '../../../../launcher/lib/chromium-executable.mjs';
import { Keyboard } from './input.mjs';
import { RetraceClock } from './clock.mjs';
import { previewServer } from './serve.mjs';

async function compareToolGlyphs(page, native, out) {
    const dir = await mkdtemp(resolve(tmpdir(),'soew-panel-'));
    try {
        for (const [lang,click,title] of [
            ['ja',[365,137],'東方封魔録'],
            ['en',[500,137],'Story of Eastern Wonderland'],
            ['zh-Hans',[365,154],'东方封魔录'],
            ['zh-Hant',[500,154],'東方封魔錄'],
        ]) for (const {key,args,x,y,width} of [
            {key:'F12',args:['--practice-menu','tools'],x:29,y:67,width:280},
            {key:'Backspace',args:['--stage-dump','0','--practice-assist-dump'],x:45,y:43,width:196},
        ]) {
            const bmp = resolve(dir,lang+'-'+key+'.bmp');
            execFileSync(resolve(native),['--assets',resolve(out,'content'),'--practice-language',lang,
                ...args,'--dump',bmp],{stdio:'pipe'});
            const bytes = readFileSync(bmp), start = bytes.readUInt32LE(10);
            const expected = [];
            for (let row = y; row < y+16; row++) for (let col = x; col < x+width; col++)
                expected.push(Number((bytes.readUInt32LE(start+4*(row*640+col)) & 0xffffff) === 0xffffff));
            assert(expected.some(Boolean), key+' native font was not drawn');
            await page.keyboard.press(key);
            if (key === 'F12') {
                await page.mouse.click(...click);
                await page.waitForFunction(title => document.title.startsWith(title),{timeout:5000},title);
            }
            await page.waitForFunction(({x,y,width,expected}) => {
                const pixels = document.querySelector('canvas').getContext('2d').getImageData(x,y,width,16).data;
                return expected.every((bit,i) =>
                    bit === Number(pixels[i*4]===255 && pixels[i*4+1]===255 && pixels[i*4+2]===255));
            },{timeout:5000},{x,y,width,expected}).catch(cause => {
                throw Error(lang+' '+key+' glyphs must match Windows pixel-for-pixel',{cause});
            });
            await page.keyboard.press(key);
        }
    } finally {
        await rm(dir,{recursive:true,force:true});
    }
}

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

test('Full SoEW draws the playfield and preserves the Win32 practice glyphs', { timeout: 30_000 }, async () => {
    const [out,,native] = process.argv.slice(2);
    assert(out, 'Usage: node web/browser.test.mjs <web build> <native replay probe>');
    const require = createRequire(process.env.SOEW_BROWSER_DEPS ||
        new URL('../../../../launcher/package.json', import.meta.url));
    const {launch} = require('puppeteer-core');
    const server = previewServer(out);
    let browser;
    try {
        await new Promise(done => server.listen(0,'127.0.0.1',done));
        browser = await launch({executablePath: await findChromiumExecutable(), headless: true});
        const page = await browser.newPage();
        await page.setViewport({width:640,height:400});
        const errors = [];
        page.on('pageerror', error => errors.push(error.message));
        page.on('console', msg => {
            if (msg.type() === 'error' && !msg.text().startsWith('Locale overlay missing;')) errors.push(msg.text());
        });
        await page.goto(`http://127.0.0.1:${server.address().port}/`);
        await page.waitForFunction(() => document.title.includes('F8'));
        await page.click('canvas');
        for (let i = 0; i < 5 && !/ショット選択|choose shot/u.test(await page.title()); i++) {
            await page.keyboard.press('Enter',{delay: 100});
            await new Promise(done => setTimeout(done,200));
        }
        assert.match(await page.title(), /ショット選択|choose shot/u);
        await page.keyboard.press('Enter',{delay: 100});
        await page.waitForFunction(() => /一時停止|Esc: pause/u.test(document.title));
        await page.waitForFunction(() => {
            const ctx = document.querySelector('canvas').getContext('2d');
            const hasColor = (x,y,w,h) => ctx.getImageData(x,y,w,h).data.some((red,i,rgba) =>
                i%4===0 && (red!==rgba[i+1] || red!==rgba[i+2]));
            const fps = ctx.getImageData(550,375,82,17).data.some((v,i) => i%4!==3&&v>240);
            return fps && hasColor(32,32,380,200) && hasColor(432,32,144,200);
        });
        if (native) await compareToolGlyphs(page,native,out);
        await page.setViewport({width:1001,height:650,deviceScaleFactor:1.25});
        await page.waitForFunction(() => {
            const canvas = document.querySelector('canvas');
            const rect = canvas.getBoundingClientRect();
            return canvas.width === Math.round(rect.width*devicePixelRatio) &&
                canvas.height === Math.round(rect.height*devicePixelRatio) &&
                !canvas.getContext('2d').imageSmoothingEnabled;
        });
        assert.deepEqual(errors,[]);
    } finally {
        if (browser) await browser.close();
        if (server.listening) await new Promise(done => server.close(done));
    }
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
    const server = previewServer(root);
    let browser;
    try {
        const expected = JSON.parse(execFileSync(resolve(probe), [resolve(root, 'content'), rep], {encoding: 'utf8'}));
        assert.equal(expected.length, 901);
        await new Promise(done => server.listen(0, '127.0.0.1', done));
        browser = await launch({executablePath: await findChromiumExecutable(), headless: true});
        const page = await browser.newPage();
        const errors = [];
        page.on('pageerror', error => errors.push(error.message));
        await page.goto(`http://127.0.0.1:${server.address().port}/?stage1`);
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
