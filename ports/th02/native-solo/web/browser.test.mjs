import assert from 'node:assert/strict';
import { execFileSync } from 'node:child_process';
import { existsSync, readFileSync } from 'node:fs';
import { createRequire } from 'node:module';
import { resolve } from 'node:path';
import { test } from 'node:test';
import { tmpdir } from 'node:os';
import { mkdtemp, rm } from 'node:fs/promises';
import { Keyboard } from './input.mjs';
import { RetraceClock } from './clock.mjs';
import { previewServer } from './serve.mjs';
import { glyphCells } from './glyph-cells.mjs';

function launchBrowser() {
    const require = createRequire(process.env.SOEW_BROWSER_DEPS || import.meta.url);
    const {launch} = require('puppeteer-core');
    const executablePath = process.env.SOEW_CHROME_PATH || process.env.EAGLER_CHROME_PATH;
    assert(executablePath, 'Set SOEW_CHROME_PATH to the installed Chromium executable');
    return launch({executablePath,headless:true});
}

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

async function enterShot(page, url, shot) {
    await page.goto(url);
    await page.waitForFunction(() => document.title.includes('F8'));
    await page.click('canvas');
    for (let i=0;i<5&&!/ショット選択|choose shot/u.test(await page.title());++i) {
        await page.keyboard.press('Enter',{delay:35});
        await new Promise(done => setTimeout(done,400));
    }
    await page.waitForFunction(() => /ショット選択|choose shot/u.test(document.title));
    await new Promise(done => setTimeout(done,700));
    await page.keyboard.press(shot==='A'?'ArrowLeft':'ArrowRight',{delay:100});
    await new Promise(done => setTimeout(done,100));
    await page.keyboard.press('Enter');
    await page.waitForFunction(() => /一時停止|Esc: Pause/u.test(document.title));
    await page.waitForFunction(() => {
        const ctx=document.querySelector('canvas').getContext('2d');
        return ctx.getImageData(550,375,82,17).data.some((v,i) => i%4!==3&&v>240);
    });
    await page.keyboard.press('ArrowLeft',{delay:140});
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

test('About has the bitmap glyphs for the disclaimer and credits in all languages', () => {
    const [out] = process.argv.slice(2);
    const atlas = glyphCells(Uint8Array.from(readFileSync(resolve(out,'soew-glyphs.bin'))).buffer);
    for (const [face, credits] of [
        [0,'東方Projectの非公式二次創作です。 原作：ZUN © ZUN / 上海アリス幻樂団 原作は正規に入手したものをご使用ください。原作素材の無断転載・再配布はご遠慮ください。 ReC98の解析に基づく移植 An unofficial Touhou fan project. Original game by ZUN © ZUN / Team Shanghai Alice Please use a legally obtained copy. Do not redistribute game assets without permission. Port based on ReC98 research'],
        [1,'这是东方 Project 的非官方二次创作。 原作：ZUN © ZUN / 上海爱丽丝幻乐团 请使用合法取得的原作。 未经授权，请勿转载或再分发原作资源。 基于 ReC98 的研究实现移植 查错及优化 感谢游玩！ 也谢谢你看到这里。 Esc：返回'],
        [2,'這是東方 Project 的非官方二次創作。 原作：ZUN © ZUN / 上海愛麗絲幻樂團 請使用合法取得的原作。 未經授權，請勿轉載或再散布原作資源。 基於 ReC98 的研究實作移植 查錯及最佳化 感謝遊玩！ 也謝謝你看到這裡。 Esc：返回'],
    ]) {
        const missing = [...new Set(credits)].filter(c => !atlas.has(c.codePointAt(0),face));
        assert.deepEqual(missing,[], 'Refresh the Win32 glyph atlas with web/stage-glyphs.mjs');
    }
});

test('Wiki Music Room packs contain both versions of all 15 comments and their rendered glyphs', t => {
    const [out] = process.argv.slice(2);
    const atlas = glyphCells(Uint8Array.from(readFileSync(resolve(out,'soew-glyphs.bin'))).buffer);
    const packs = [
        ['en',0,'Pure Land Mandala','FM version','MIDI version'],
        ['zh-Hans',1,'净土曼荼罗','FM版','MIDI版'],
        ['zh-Hant',2,'淨土曼荼羅','FM版','MIDI版'],
    ];
    if (!existsSync(resolve(out,'content/music-room.en.pack'))) return t.skip('Import the Wiki Music Room packs first');
    for (const [lang,face,title,fm,midi] of packs) {
        const source = readFileSync(resolve(out,`content/music-room.${lang}.pack`),'utf8');
        const lines = source.slice(0,-1).split('\n');
        assert.equal(lines.length,15*39);
        assert(lines[0].includes(title));
        for (let i=0; i<lines.length; i+=39) {
            const track = lines.slice(i,i+39);
            assert(track[0] && track[1] === fm && track[2] && track[track.indexOf(midi)+1]);
            assert(track.includes(midi), `${lang} track ${i/39+1} MIDI comment`);
        }
        const missing = [...new Set(source.replaceAll('\n',''))].filter(c => !atlas.has(c.codePointAt(0),face));
        assert.deepEqual(missing,[], lang+' must use the Win32 glyph atlas');
    }
});

test('Full SoEW plays all MIDI songs and persists the album selected in Options', {timeout:55_000}, async t => {
    const [out] = process.argv.slice(2);
    const manifest = JSON.parse(readFileSync(resolve(out,'content/full-files.json'),'utf8'));
    if (!manifest.includes('akyu.pack') || !manifest.includes('GeneralUser-GS-SoftSynth-1.44.sf2'))
        return t.skip('Stage MIDI and Akyu assets first');
    const server = previewServer(out);
    let browser;
    try {
        await new Promise(done => server.listen(0,'127.0.0.1',done));
        browser = await launchBrowser();
        const page = await browser.newPage();
        await page.setViewport({width:640,height:400});
        const sources = [], errors = [], previews = [];
        page.on('request',request => { if (request.url().endsWith('.opus')) sources.push(new URL(request.url()).pathname); });
        page.on('pageerror',error => errors.push(error.message));
        page.on('console',message => {
            if (message.type()==='error') errors.push(message.location().url+': '+message.text());
            if (message.text().startsWith('preview_track=')) previews.push(message.text());
        });
        await page.evaluateOnNewDocument(() => {
            window.audioProbe = {track:null, pcm:new Set(), albums:[], active:null, gain:null};
            const log = console.log;
            console.log = function(...args) {
                const match = /^preview_track=(\d+) file=(\S+)/.exec(args[0]);
                if (match) window.audioProbe.track = match[2].endsWith('.MMD') ? Number(match[1]) : null;
                return log.apply(this,args);
            };
            const start = AudioBufferSourceNode.prototype.start;
            AudioBufferSourceNode.prototype.start = function(...args) {
                const probe = window.audioProbe;
                if (this.loop) {
                    probe.active = this;
                    probe.albums.push([this.loopStart,this.loopEnd]);
                } else if (probe.track !== null && this.buffer.getChannelData(0).some(x => Math.abs(x)>0.0001)) {
                    probe.pcm.add(probe.track);
                }
                return start.apply(this,args);
            };
            const connect = AudioBufferSourceNode.prototype.connect;
            AudioBufferSourceNode.prototype.connect = function(target,...args) {
                if(this.loop&&target instanceof GainNode)window.audioProbe.gain=target.gain;
                return connect.call(this,target,...args);
            };
            const stop = AudioBufferSourceNode.prototype.stop;
            AudioBufferSourceNode.prototype.stop = function(...args) {
                if (window.audioProbe.active === this) window.audioProbe.active = null;
                return stop.apply(this,args);
            };
        });
        const pause = ms => new Promise(done => setTimeout(done,ms));
        const press = key => page.keyboard.press(key,{delay:45});
        const wait = (fn,...args) => page.waitForFunction(fn,{timeout:7000},...args);
        await page.goto(`http://127.0.0.1:${server.address().port}/`);
        await wait(() => document.title.includes('F8'));
        await page.click('canvas');
        for (let i=0;i<5&&!/ショット選択/u.test(await page.title());++i) {
            await press('Enter');
            await pause(400);
        }
        await wait(() => /ショット選択/u.test(document.title));
        await pause(650);
        await press('Escape');
        await pause(1100);
        for (let i=0;i<3;++i) await press('ArrowUp'); // Story → Quit → About → Music Room.
        await press('Enter');
        await wait(() => /音楽室：決定/u.test(document.title)).catch(async cause => {
            throw Error('Music Room did not open: '+await page.title(),{cause});
        });
        await pause(1100);
        await press('F5'); // FM → MIDI
        for (let track=0; track<15; ++track) {
            if (track) {
                await press('ArrowDown');
                await press('Enter');
            }
            await wait(track => window.audioProbe.pcm.has(track),track).catch(cause => {
                throw Error(`No MIDI PCM for slot ${track}: ${previews.join('; ')} | ${errors.join('; ')}`,{cause});
            });
        }
        await press('F5'); // MIDI → Akyu, native slot 15 is album track 13.
        await wait(() => window.audioProbe.albums.length===1);
        assert.deepEqual(sources,['/content/akyu/13.opus']);
        await press('F4');
        await wait(() => !window.audioProbe.active);
        await press('F4');
        await wait(() => window.audioProbe.albums.length===2);
        await press('F5'); // Akyu → FM stops the album.
        await wait(() => !window.audioProbe.active);
        await press('Escape');
        await pause(1100);
        await press('ArrowUp'); // Music Room → Options.
        await press('Enter');
        await pause(1100);
        await press('ArrowDown'); // Rank → Music.
        await press('ArrowRight'); // FM → MIDI.
        await press('ArrowRight'); // MIDI → Akyu.
        await wait(() => window.audioProbe.albums.length===3);
        assert.equal(sources.at(-1),'/content/akyu/13.opus');
        const albumLevel = () => page.evaluate(() => window.audioProbe.gain.value);
        const initialLevel = await albumLevel();
        assert(initialLevel>0 && initialLevel<0.25,'The album must use its balanced output gain');
        for(let i=0;i<5;++i)await press('ArrowDown'); // Music → Music Volume.
        for(let i=0;i<5;++i)await press('ArrowLeft');
        assert.equal(await albumLevel(),0);
        await press('ArrowRight');
        assert(Math.abs(await albumLevel()-initialLevel/5)<0.0001);
        await press('Tab'); // SFX Volume must not affect the album.
        await press('ArrowLeft');
        assert(Math.abs(await albumLevel()-initialLevel/5)<0.0001);
        assert.equal(sources.length,3,'Changing volume must not restart the album');
        await press('Escape');
        await pause(1100);
        await page.reload();
        await wait(() => document.title.includes('F8'));
        await page.click('canvas');
        for (let i=0;i<5&&!/ショット選択/u.test(await page.title());++i) {
            await press('Enter');
            await pause(400);
        }
        await wait(() => /ショット選択/u.test(document.title));
        await wait(() => window.audioProbe.albums.length>=1);
        assert.equal(sources.at(-1),'/content/akyu/01.opus');
        assert(Math.abs(await albumLevel()-initialLevel/5)<0.0001,'Music volume survives reload');
        await pause(650);
        await press('Enter');
        await wait(() => /Esc.*一時停止/u.test(document.title));
        await wait(() => window.audioProbe.albums.length>=2 && window.audioProbe.active?.context.state==='running');
        assert.equal(sources.at(-1),'/content/akyu/02.opus');
        await press('KeyP');
        await wait(() => window.audioProbe.active?.context.state==='suspended');
        await press('KeyP');
        await wait(() => window.audioProbe.active?.context.state==='running');
        assert.deepEqual(errors,[]);
    } finally {
        await browser?.close();
        await new Promise(done => server.close(done));
    }
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

test('Full SoEW fades into and out of shot selection, draws the playfield and preserves Win32 glyphs', { timeout: 35_000 }, async () => {
    const [out,,native] = process.argv.slice(2);
    assert(out, 'Usage: node web/browser.test.mjs <web build> <native replay probe>');
    const server = previewServer(out);
    let browser;
    try {
        await new Promise(done => server.listen(0,'127.0.0.1',done));
        browser = await launchBrowser();
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
        await page.evaluate(() => {
            const ctx = new OffscreenCanvas(32,20).getContext('2d',{willReadFrequently:true});
            const canvas = document.querySelector('canvas');
            window.shotFrames = {enter:[],leave:[],started:0,leaving:false};
            const frames = window.shotFrames;
            function sample(now) {
                if (/ショット選択|choose shot/u.test(document.title)) {
                    frames.started ||= now;
                    ctx.drawImage(canvas,0,0,32,20);
                    const bytes = ctx.getImageData(0,0,32,20).data;
                    let peak = 0;
                    for (let i = 0; i < bytes.length; ++i) if (i%4!==3) peak = Math.max(peak,bytes[i]);
                    (frames.leaving ? frames.leave : frames.enter).push(peak);
                }
                if (!/一時停止|Esc: Pause/u.test(document.title)) requestAnimationFrame(sample);
            }
            requestAnimationFrame(sample);
        });
        for (let i = 0; i < 5 && !/ショット選択|choose shot/u.test(await page.title()); i++) {
            await page.keyboard.press('Enter',{delay: 35});
            await new Promise(done => setTimeout(done,400));
        }
        assert.match(await page.title(), /ショット選択|choose shot/u);
        await page.keyboard.press('Enter'); // still fading in, confirmation is ignored
        await page.waitForFunction(() => window.shotFrames.started && performance.now()-window.shotFrames.started>680);
        assert.match(await page.title(), /ショット選択|choose shot/u);
        await page.evaluate(() => {window.shotFrames.leaving = true;});
        await page.keyboard.press('Enter');
        await page.waitForFunction(() => /一時停止|Esc: Pause/u.test(document.title));
        const {enter,leave} = await page.evaluate(() => window.shotFrames);
        assert(enter.some(level => level < 30) && enter.some(level => level > 165), 'fade-in did not lift black into the original page');
        assert(leave.some(level => level > 165) && leave.some(level => level < 30), 'fade-out did not return the selection page to black');
        await page.waitForFunction(() => {
            const ctx = document.querySelector('canvas').getContext('2d');
            const hasColor = (x,y,w,h) => ctx.getImageData(x,y,w,h).data.some((red,i,rgba) =>
                i%4===0 && (red!==rgba[i+1] || red!==rgba[i+2]));
            const fps = ctx.getImageData(550,375,82,17).data.some((v,i) => i%4!==3&&v>240);
            return fps && hasColor(32,32,380,200) && hasColor(432,32,144,200);
        });
        // The first playfield paint can precede the native input-release poll.
        await page.keyboard.press('ArrowLeft',{delay:120});
        await page.keyboard.press('KeyX',{delay:85});
        await page.waitForFunction(() => {
            const bytes = document.querySelector('canvas').getContext('2d').getImageData(32,16,384,368).data;
            let red = 0, white = 0;
            for (let i = 0; i < bytes.length; i+=4) {
                const r=bytes[i],g=bytes[i+1],b=bytes[i+2];
                red+=r===255&&g===0&&b===0;
                white+=r===255&&g===255&&b===255;
            }
            window.bombPeaks ??= {red:0,white:0};
            window.bombPeaks.red = Math.max(window.bombPeaks.red,red);
            window.bombPeaks.white = Math.max(window.bombPeaks.white,white);
            return red>=128&&white>30000;
        },{timeout:4500}).catch(async cause => {
            throw Error('Missing shot B bomb overlay: '+JSON.stringify(await page.evaluate(() => window.bombPeaks)),{cause});
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

test('Full SoEW shows A’s purple-to-white flash and C’s columns-to-portrait strobe', {timeout:45_000}, async () => {
    const [out]=process.argv.slice(2);
    const server=previewServer(out);
    let browser;
    try {
        await new Promise(done => server.listen(0,'127.0.0.1',done));
        browser=await launchBrowser();
        const url=`http://127.0.0.1:${server.address().port}/`;
        for (const shot of ['A','C']) {
            const page=await browser.newPage();
            await page.setViewport({width:640,height:400});
            await enterShot(page,url,shot);
            const monitor=page.waitForFunction(shot => {
                const watch=window.bombWatch??={phase:0,white:false,redraw:false};
                const ctx=document.querySelector('canvas').getContext('2d');
                const a=ctx.getImageData(36,28,376,344).data;
                let white=0,purple=0,red=0,darkRed=0,grey=0;
                for (let i=0;i<a.length;i+=4) {
                    const r=a[i],g=a[i+1],b=a[i+2];
                    white+=r===255&&g===255&&b===255;
                    purple+=(r===138||r===117)&&g===0&&r===b;
                    red+=r===255&&g===0&&b===0;
                    darkRed+=r===170&&g===0&&b===0;
                    grey+=r===223&&g===223&&b===223;
                }
                if (shot==='A') {
                    if (!watch.phase&&purple>100000) watch.phase=1;
                    else if (watch.phase===1&&white===a.length/4) watch.phase=2;
                    return watch.phase===2;
                }
                if (!watch.phase&&darkRed>5000&&red>10000) watch.phase=1;
                if (watch.phase===1&&white>125000) watch.white=true;
                if (watch.white&&white>110000&&grey>1500&&red>400) watch.redraw=true;
                return watch.redraw;
            },{timeout:8800},shot).catch(async cause => {
                throw Error('Missing original shot '+shot+' bomb phases: '+JSON.stringify(await page.evaluate(() => window.bombWatch)),{cause});
            });
            await page.keyboard.press('KeyX',{delay:100});
            await monitor;
            await page.close();
        }
    } finally {
        await browser?.close();
        if (server.listening) await new Promise(done => server.close(done));
    }
});

test('Full SoEW fades between title pages, animates the music room and browses replays by mode', {timeout: 45_000}, async () => {
    const [out] = process.argv.slice(2);
    const server = previewServer(out);
    let browser;
    try {
        await new Promise(done => server.listen(0, '127.0.0.1', done));
        browser = await launchBrowser();
        const page = await browser.newPage();
        await page.setViewport({width:640,height:400});
        const url = `http://127.0.0.1:${server.address().port}/`;
        const pause = ms => new Promise(done => setTimeout(done,ms));
        const press = key => page.keyboard.press(key,{delay:35});
        const color = (x,y,r,g,b) => page.waitForFunction(({x,y,r,g,b}) => {
            const p = document.querySelector('canvas').getContext('2d').getImageData(x,y,1,1).data;
            return p[0]===r && p[1]===g && p[2]===b;
        },{timeout:5000},{x,y,r,g,b});
        const evidence = process.env.SOEW_UI_EVIDENCE;
        const screenshot = name => evidence && page.screenshot({path:resolve(evidence,name+'.png')});
        const sampleSwitch = async (key, duration) => {
            await page.evaluate(duration => {
                const ctx = new OffscreenCanvas(32,20).getContext('2d',{willReadFrequently:true});
                const canvas = document.querySelector('canvas');
                const frames = [];
                const start = performance.now();
                function sample(now) {
                    ctx.drawImage(canvas,0,0,32,20);
                    let peak = 0;
                    let signature = 0;
                    for (const [i,byte] of ctx.getImageData(0,0,32,20).data.entries()) {
                        if (i%4===3) continue;
                        peak = Math.max(peak,byte);
                        signature = (Math.imul(signature,31)+byte)|0;
                    }
                    frames.push({peak,signature});
                    if (now-start<duration) requestAnimationFrame(sample);
                    else window.pageFadeDone = frames;
                }
                window.pageFadeDone = null;
                sample(start);
            },duration);
            await press(key);
            await page.waitForFunction(() => window.pageFadeDone!==null);
            return page.evaluate(() => window.pageFadeDone);
        };
        const switchPage = async (key, duration=1040) => {
            const frames = (await sampleSwitch(key,duration)).map(frame => frame.peak);
            const black = frames.findIndex(peak => peak<8);
            assert(black>=2 && black<frames.length-2, key+' did not transition through black');
            assert(frames.slice(0,black).some(peak => peak>48), key+' did not fade out the old page');
            assert(frames.slice(black+1).some(peak => peak>48), key+' did not fade in the new page');
        };
        const switchWithoutFade = async (key, label) => {
            const frames = await sampleSwitch(key,540);
            assert.notEqual(frames.at(-1).signature,frames[0].signature,label+' did not change the rendered menu');
            assert(frames.every(({peak}) => peak>48),label+' faded the whole screen');
        };
        await page.goto(url);
        await page.waitForFunction(() => document.title.includes('F8'));
        await page.click('canvas');
        for (let i=0;i<5&&!/ショット選択|choose shot/u.test(await page.title());++i) {
            await press('Enter');
            await pause(400);
        }
        await page.waitForFunction(() => /ショット選択|choose shot/u.test(document.title));
        await pause(700);
        await switchPage('Escape'); // Shot select → Title.
        await press('ArrowDown'); // Story → Practice
        await switchPage('Enter'); // Practice shot select
        for (let shot=0;shot<3;++shot) {
            if(shot)await press('ArrowRight');
            await switchWithoutFade('Enter','Practice shot → stage list');
            if(shot<2)await switchWithoutFade('Escape','Practice stage list → shot');
        }
        await switchWithoutFade('Enter','Practice stage list → setup');
        await press('ArrowUp'); // Start → New Seed.
        await press('ArrowUp'); // New Seed → Enter Seed.
        await press('4'); // Typing opens the field and replaces the current seed.
        await page.keyboard.type('294967295',{delay:30});
        await press('Enter');
        await press('ArrowDown');
        await press('ArrowDown');
        await switchPage('Enter',2400); // Practice starts at the selected boss.
        await page.waitForFunction(() => /(?:練習 \| シード|Practice \| Seed) 4294967295/u.test(document.title));
        await press('Escape'); // Pause, then exit to practice setup.
        await switchPage('KeyQ');
        await switchWithoutFade('Escape','Practice setup → stage list');
        await switchWithoutFade('Escape','Practice stage list → shot');
        await switchPage('Escape');
        for (let i=0;i<3;++i) await press('ArrowUp'); // Practice → Story → Quit → About.
        await switchPage('Enter');
        await screenshot('about');
        await switchPage('Escape');
        await press('ArrowUp'); // About → Music Room.
        await switchPage('Enter');
        await page.waitForFunction(() => /音楽室：決定|Music room: confirm/u.test(document.title));
        await page.waitForFunction(() => {
            const pixels = document.querySelector('canvas').getContext('2d').getImageData(16,16,608,42).data;
            window.firstMusicFrame ??= pixels;
            let changed = 0;
            for (let i=0;i<pixels.length;i+=4)
                changed += pixels[i]!==window.firstMusicFrame[i] ||
                    pixels[i+1]!==window.firstMusicFrame[i+1] || pixels[i+2]!==window.firstMusicFrame[i+2];
            return changed>128;
        });
        await screenshot('music-room');
        await switchPage('Escape');
        await press('ArrowUp'); // Music Room → Options.
        await switchWithoutFade('Enter','Title → Options');
        await switchWithoutFade('Escape','Options → Title (Escape)');
        await switchWithoutFade('Enter','Title → Options');
        await press('ArrowUp'); // Rank → Back.
        await switchWithoutFade('Enter','Options → Title (Back)');
        await press('ArrowUp'); // Options → HiScore.
        await switchPage('Enter');
        await page.waitForFunction(() => /ローカルハイスコア|Local high scores/u.test(document.title));
        const rank = () => page.evaluate(() => [...document.querySelector('canvas').getContext('2d').getImageData(320,32,128,16).data]);
        const normal = await rank();
        await press('ArrowRight');
        await pause(75);
        assert.notDeepEqual(await rank(),normal);
        await screenshot('hiscore');
        await switchPage('Escape');
        await press('ArrowUp'); // HiScore → Replay.
        await switchPage('Enter');
        await color(24,93,255,255,0);
        await press('ArrowRight');
        await color(224,93,255,255,0);
        await press('ArrowRight');
        await color(424,93,255,255,0);
        await press('ArrowRight');
        await color(24,93,255,255,0);
        await screenshot('replay-browser');
        await switchPage('Escape');

        await enterShot(page,url,'A');
        await pause(1200);
        await press('Escape');
        await press('ArrowDown');
        await press('ArrowDown');
        await press('Enter');
        await color(192,106,255,255,0); // First name cell.
        await press('Enter'); // A.
        await color(216,106,255,255,0);
        await screenshot('replay-name');
        await press('ArrowLeft');
        await press('ArrowUp');
        await press('Enter');
        await color(32,105,255,255,0); // First numbered save slot.
        await press('Enter');
        await page.waitForFunction(() => /リプレイを保存しました|Replay saved/u.test(document.title));
        for (let i=0;i<2;++i) await press('ArrowDown'); // Story → Practice → Replay, with Extra locked.
        await switchPage('Enter',1200);
        await color(27,97,255,255,0); // Saved Story replay.
        await screenshot('replay-browser');
        await press('Enter');
        await color(40,60,26,33,53); // Replay details panel.
        await screenshot('replay-details');
    } finally {
        await browser?.close();
        if (server.listening) await new Promise(done => server.close(done));
    }
});

test('Stage 1 browser, keyboard and 900 native replay samples', { timeout: 60_000 }, async () => {
    const [out, probe] = process.argv.slice(2);
    assert(out && probe, 'Usage: node web/browser.test.mjs <web build> <native replay probe>');
    const root = resolve(out);
    const dir = await mkdtemp(resolve(tmpdir(), 'soew-replay-'));
    const rep = resolve(dir, 'stage1.rep');
    const server = previewServer(root);
    let browser;
    try {
        const expected = JSON.parse(execFileSync(resolve(probe), [resolve(root, 'content'), rep], {encoding: 'utf8'}));
        assert.equal(expected.length, 901);
        await new Promise(done => server.listen(0, '127.0.0.1', done));
        browser = await launchBrowser();
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
