import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';

const origin = process.env.LAUNCHER_URL || 'http://127.0.0.1:5185/';
const browser = await puppeteer.launch({headless:true, executablePath:await findChromiumExecutable()});

async function controlTipClock(page) {
  await page.evaluateOnNewDocument(() => {
    const set = window.setInterval.bind(window), clear = window.clearInterval.bind(window);
    const callbacks = new Map();
    let id = -1;
    window.setInterval = (callback, delay, ...args) => {
      if (delay !== 5000) return set(callback, delay, ...args);
      callbacks.set(id, () => callback(...args));
      return id--;
    };
    window.clearInterval = key => {
      if (!callbacks.delete(key)) clear(key);
    };
    window.tipClock = {active:() => callbacks.size, tick:() => callbacks.forEach(callback => callback())};
  });
}

async function verifyBootLoading(path, preloadId, draw, expected) {
  const page = await browser.newPage();
  try {
    await controlTipClock(page);
    await page.evaluateOnNewDocument(draw => {Math.random = () => draw;}, draw);
    await page.setRequestInterception(true);
    page.on('request', request => {
      void (request.resourceType() === 'script'
        ? request.respond({status:200, contentType:'text/javascript', body:''})
        : request.continue());
    });
    await page.goto(new URL(path,origin).href,{waitUntil:'domcontentloaded'});
    const activity = `#${preloadId} .loading-activity`;
    const tip = `${activity} .loading-activity-tip span`;
    const read = () => page.$eval(tip, node => node.textContent);
    await page.waitForSelector(tip);
    const scene = `${activity} [data-loading-scene="${expected}"]`;
    await page.waitForSelector(scene,{visible:true});
    await page.waitForFunction(selector => {
      const faces = [...document.querySelectorAll(`${selector} img`)];
      return faces.length > 0 && faces.every(face => face.complete && face.naturalWidth > 0);
    },{},scene);
    assert.equal(await page.$$eval('[data-bootstrap-scenes] img[src]', images => images.length),0,'unselected templates do not fetch portraits');
    assert.equal(await page.$eval(scene, element => element.getAnimations({subtree:true}).length > 0),true);
    const initial = await read();
    assert.ok(initial);
    assert.equal(await page.evaluate(() => window.tipClock.active()),1,'boot starts its 5s clock before the app loads');
    await page.evaluate(() => window.tipClock.tick());
    const next = await read();
    assert.notEqual(next,initial);
    const stinger = `${activity} [data-loading-stinger]`;
    if (expected === 'umbrella') {
      assert.equal(await page.$eval(stinger, node => getComputedStyle(node).animationDelay),'10s');
      await page.$eval(stinger, node => node.getAnimations().find(animation => animation.animationName === 'loading-stinger-window').finish());
      await page.waitForFunction(selector => getComputedStyle(document.querySelector(selector)).animationDelay === '27.900s' ||
        getComputedStyle(document.querySelector(selector)).animationDelay === '27.9s',{},stinger);
    }
    await page.$eval(`#${preloadId}`, node => node.classList.add('is-done'));
    assert.ok(await page.$eval(scene, element => element.getAnimations({subtree:true}).every(animation => animation.playState === 'paused')));
    if (expected === 'umbrella') assert.equal(await page.$eval(stinger, node => node.getAnimations({subtree:true}).length),0);
    await page.evaluate(() => window.tipClock.tick());
    assert.equal(await read(),next,'hidden startup tip no longer changes');
    assert.equal(await page.evaluate(() => window.tipClock.active()),0);
  } finally {await page.close();}
}

try {
  await Promise.all([verifyBootLoading('./?lang=zh-CN','eaglerPreload',0,'shrine'), verifyBootLoading('lobby.html?lang=zh-CN','lobbyPreload',.79,'umbrella')]);
  const page = await browser.newPage();
  await controlTipClock(page);
  const errors = [];
  page.on('pageerror', error => errors.push(error.message));
  const faces = '.loading-scene-portrait';
  const selector = '.loading-activity';
  const waitForFaces = () => page.waitForFunction(selector => {
    const images = [...document.querySelectorAll(selector)];
    return images.length > 0 && images.every(image => image.complete && image.naturalWidth > 0);
  }, {}, faces);
  const snapshot = () => page.$eval(selector, (loader, faces) => {
    const images = [...loader.querySelectorAll(faces)];
    const dots = [...loader.querySelectorAll('.loading-activity-dots > span')];
    const bounds = loader.getBoundingClientRect();
    return {
      images:images.map(image => image.src),
      scene:loader.querySelector('[data-loading-scene]').getAttribute('data-loading-scene'),
      dotHop:getComputedStyle(dots[0]).getPropertyValue('--loading-hop').trim(),
      crops:images.map(image => [getComputedStyle(image).objectFit, getComputedStyle(image).objectPosition]),
      dots:dots.map(dot => [dot.textContent, getComputedStyle(dot).animationName, getComputedStyle(dot).animationDelay]),
      tip:loader.querySelector('.loading-activity-tip span').textContent,
      fits:bounds.left >= 0 && bounds.right <= innerWidth,
    };
  }, faces);
  await page.emulateMediaFeatures([{name:'prefers-reduced-motion',value:'no-preference'}]);
  await page.setViewport({width:1440, height:900});
  await page.goto(new URL('ui-review.html?preview=loading',origin).href,{waitUntil:'domcontentloaded'});
  const sceneArtwork = id => `.loading-scene[data-loading-scene="${id}"]`;
  const picker = '[data-preview-scene-select]';
  const stinger = '[data-loading-stinger="kogasa"]';
  const nextScene = '[data-preview-step="next"]';
  const previousScene = '[data-preview-step="previous"]';
  const waitForPickerClose = () => page.waitForFunction(() => (
    !document.querySelector('[role=listbox]') && getComputedStyle(document.body).pointerEvents !== 'none'
  ));
  await page.waitForSelector(sceneArtwork('shrine'));
  const snapshotScene = id => page.$eval(sceneArtwork(id), node => {
    const box = node.getBoundingClientRect();
    const faces = [...node.querySelectorAll('img')];
    return {
      cast:faces.map(image => [image.dataset.character, new URL(image.src).pathname.split('/').at(-1)]),
      sizes:faces.map(image => Math.round(parseFloat(image.style.inlineSize) * 4.8)),
      uniquePositions:new Set(faces.map(image => `${image.style.left}:${image.style.top}`)).size,
      portraitMotion:faces.map(image => getComputedStyle(image).animationName),
      motion:[...node.querySelectorAll('[data-scene-motion]')]
        .map(effect => getComputedStyle(effect).animationName),
      aligned:faces.every(face => {
        const bounds = face.getBoundingClientRect();
        const expectedX = box.left + parseFloat(face.style.left) / 100 * box.width;
        const expectedY = box.top + parseFloat(face.style.top) / 100 * box.height;
        return Math.abs(bounds.left + bounds.width / 2 - expectedX) < 1
          && Math.abs(bounds.top + bounds.height / 2 - expectedY) < 1
          && Math.abs(bounds.width - box.width * parseFloat(face.style.inlineSize) / 100) < 1;
      }),
      fits:box.left >= 0 && box.right <= innerWidth && box.width > 0,
      viewportFits:document.documentElement.scrollWidth <= innerWidth,
      decorative:node.getAttribute('aria-hidden') === 'true',
    };
  });
  const casting = {
    shrine:[['reimu','reimu-hakurei.svg']],
    forest:[['marisa','face-F001.svg']],
    courtyard:[['youmu','face-F02F.svg'],['yuyuko','face-F030.svg']],
    lake:[['daiyousei','face-F021.svg'],['cirno','face-F022.svg']],
    'mansion-clock':[['sakuya','face-F026.svg']],
    library:[['patchouli','face-F025.svg'],['koakuma','face-F024.svg']],
    ferry:[['komachi','face-F046.svg']],
    pharmacy:[['eirin','face-F03D.svg'],['reisen','face-F03C.svg']],
    pump:[['nitori','face-F04F.svg']],
    puppets:Array.from({length:6}, () => ['alice','face-F01B.svg']),
    concert:[['lunasa','face-F02E.svg'],['merlin','face-F02D.svg'],['lyrica','face-F02C.svg']],
    umbrella:[['kogasa','face-F05F.svg']],
    lookout:[['momiji','face-F050.svg']],
    sunflowers:[['yuuka','face-F016.svg']],
  };
  await page.click(picker);
  await page.waitForSelector('[role=listbox]');
  assert.deepEqual(await page.$$eval('[data-preview-scene]', options => options.map(option => option.dataset.previewScene)), [...Object.keys(casting),'random']);
  await page.keyboard.press('Escape');
  await waitForPickerClose();
  const actorMotions = {
    shrine:'loading-scene-search', forest:'loading-scene-forage',
    ferry:'loading-vignette-ferry', lookout:'loading-vignette-notice',
  };
  for (const [id, cast] of Object.entries(casting)) {
    await page.waitForSelector(sceneArtwork(id));
    await page.waitForFunction(selector => {
      const images = [...document.querySelectorAll(`${selector} img`)];
      return images.length > 0 && images.every(image => image.complete && image.naturalWidth > 0);
    }, {}, sceneArtwork(id));
    const scene = await snapshotScene(id);
    assert.deepEqual(scene.cast,cast,'characters are cast by scene, not drawn randomly');
    assert.equal(scene.uniquePositions,cast.length);
    const actorMotion = actorMotions[id] ?? 'none';
    assert.deepEqual(scene.portraitMotion,id === 'puppets'
      ? ['none','loading-vignette-cue','none','none','none','none']
      : Array(cast.length).fill(actorMotion));
    assert.ok(scene.decorative && scene.fits && scene.motion.some(name => name !== 'none'));
    if (id === 'mansion-clock') {
      const [held, resumed] = await page.$eval(sceneArtwork(id), async node => {
        const pieces = [...node.querySelectorAll('[data-scene-motion^="time-"]')];
        const animations = pieces.flatMap(piece => piece.getAnimations());
        animations.forEach(animation => animation.pause());
        await Promise.all(animations.map(animation => animation.ready));
        const poseAt = time => {
          animations.forEach(animation => { animation.currentTime = time; });
          return pieces.map(piece => [getComputedStyle(piece).rotate, getComputedStyle(piece).translate]);
        };
        const frozen = poseAt(1000);
        return [JSON.stringify(frozen) === JSON.stringify(poseAt(1600)), JSON.stringify(frozen) !== JSON.stringify(poseAt(2000))];
      });
      assert.ok(held && resumed, 'the watch and tea drop stop and resume together');
    }
    if (id === 'ferry') {
      for (const width of [1440, 320]) {
        await page.setViewport({width,height:640});
        const deltas = await page.$eval(sceneArtwork(id), async node => {
          const [portrait, boat] = node.querySelectorAll('[data-scene-motion="ferry"]');
          const animations = [portrait, boat].flatMap(piece => piece.getAnimations());
          animations.forEach(animation => animation.pause());
          await Promise.all(animations.map(animation => animation.ready));
          animations.forEach(animation => { animation.currentTime = 2200; });
          const shift = piece => new DOMMatrixReadOnly(getComputedStyle(piece).transform).m41;
          return [shift(portrait) / node.getBoundingClientRect().width, shift(boat) / 480];
        });
        assert.ok(deltas[0] > 0 && Math.abs(deltas[0] - deltas[1]) < .001, `Komachi and her boat stay locked together at ${width}px`);
      }
    }
    if (id === 'puppets') {
      assert.deepEqual(scene.sizes,[48,34,32,32,32,32]);
      for (const width of [1440, 320]) {
        await page.setViewport({width,height:640});
        const attached = await page.$eval(sceneArtwork(id), async node => {
          const [rig,doll] = node.querySelectorAll('[data-scene-motion="puppet"]');
          const animations = [rig,doll].flatMap(piece => piece.getAnimations());
          animations.forEach(animation => animation.pause());
          await Promise.all(animations.map(animation => animation.ready));
          animations.forEach(animation => { animation.currentTime = 1400; });
          const bounds = doll.getBoundingClientRect();
          const anchor = new DOMPoint(0,46).matrixTransform(rig.getScreenCTM());
          return Math.abs(bounds.left + bounds.width / 2 - anchor.x) < 1
            && Math.abs(bounds.top + bounds.height / 2 - anchor.y) < 1;
        });
        assert.ok(attached, `the strings and Alice doll keep the same pivot at ${width}px`);
      }
    }
    if (id === 'umbrella') {
      for (const [width,height] of [[1440,900],[320,640],[640,320]]) {
        await page.setViewport({width,height});
        const surprise = await page.$eval(stinger, async cut => {
          const face = cut.querySelector('img');
          const animations = cut.getAnimations({subtree:true});
          const timer = animations.find(animation => animation.animationName === 'loading-stinger-window');
          const {delay, duration, iterations} = timer.effect.getTiming();
          animations.forEach(animation => animation.pause());
          await Promise.all(animations.map(animation => animation.ready));
          const frameAt = time => {
            animations.forEach(animation => { animation.currentTime = time; });
            return {
              visibility:getComputedStyle(cut).visibility,
              shade:Number(getComputedStyle(cut,'::before').opacity),
              portrait:face.getBoundingClientRect(),
            };
          };
          const before = frameAt(delay - 1);
          const entry = frameAt(delay);
          const rising = frameAt(delay + 125);
          const held = frameAt(delay + 530);
          const stillHeld = frameAt(delay + 1100);
          const retreating = frameAt(delay + 1430);
          const exit = frameAt(delay + duration - 1);
          const box = cut.getBoundingClientRect();
          const portrait = held.portrait;
          const coversViewport = box.left === 0 && box.top === 0 && box.width === innerWidth && box.height === innerHeight;
          const fillsFrame = portrait.left < 0 && portrait.right > box.right && portrait.top < 0 && portrait.bottom > box.bottom;
          return {
            delay,duration,iterations,coversViewport,fillsFrame,
            visibility:[before.visibility,entry.visibility,held.visibility,exit.visibility],
            entersFromBelow:entry.portrait.top >= box.bottom && rising.portrait.top < box.bottom && rising.portrait.top > portrait.top,
            holdsStill:stillHeld.portrait.top === portrait.top,
            exitsDownward:retreating.portrait.top > portrait.top && retreating.portrait.top < box.bottom && exit.portrait.top >= box.bottom,
            staysSameSize:[entry,rising,retreating,exit].every(frame => frame.portrait.width === portrait.width),
            shadeFades:entry.shade === 0 && rising.shade > 0 && rising.shade < 1 && held.shade === 1
              && retreating.shade > 0 && retreating.shade < 1 && exit.shade < .01,
          };
        });
        assert.equal(surprise.delay,10000);
        assert.equal(surprise.duration,1550);
        assert.equal(surprise.iterations,1);
        assert.deepEqual(surprise.visibility,['hidden','visible','visible','visible']);
        assert.ok(surprise.coversViewport && surprise.fillsFrame, `Kogasa fills the viewport at ${width}×${height}`);
        assert.ok(surprise.entersFromBelow && surprise.holdsStill && surprise.exitsDownward, `Kogasa slides in, holds and slides out at ${width}×${height}`);
        assert.ok(surprise.staysSameSize && surprise.shadeFades, 'the face does not zoom and the backdrop fades');
      }
      const finishCut = randomValue => page.$eval(stinger, async (cut, randomValue) => {
        const random = Math.random;
        Math.random = () => randomValue;
        try {
          const replaced = new Promise(resolve => {
            const observer = new MutationObserver(() => {
              if (cut.isConnected) return;
              observer.disconnect();
              resolve();
            });
            observer.observe(cut.parentNode,{childList:true});
          });
          cut.getAnimations().find(animation => animation.animationName === 'loading-stinger-window').finish();
          const after = getComputedStyle(cut).visibility;
          await replaced;
          return after;
        } finally {
          Math.random = random;
        }
      },randomValue);
      assert.equal(await finishCut(.99999),'hidden');
      assert.deepEqual(await page.$eval(stinger, cut => [getComputedStyle(cut).visibility, getComputedStyle(cut).animationDelay]), ['hidden','30s']);
      assert.equal(await finishCut(0),'hidden');
      assert.equal(await page.$eval(stinger, cut => getComputedStyle(cut).animationDelay),'20s');
    }
    await page.emulateMediaFeatures([{name:'prefers-reduced-motion',value:'reduce'}]);
    if (id === 'umbrella') {
      assert.deepEqual(await page.$eval(stinger, cut => [getComputedStyle(cut).visibility, cut.getAnimations({subtree:true}).length]), ['hidden',0]);
    }
    for (const [width,height] of [[1440,900],[515,627],[320,640],[640,320]]) {
      await page.setViewport({width,height});
      const state = await snapshotScene(id);
      assert.ok(state.fits && state.aligned && state.viewportFits, `${id} cast stays anchored at ${width}×${height}`);
      assert.ok(state.motion.every(name => name === 'none'));
    }
    await page.emulateMediaFeatures([{name:'prefers-reduced-motion',value:'no-preference'}]);
    await page.evaluate(() => document.body.classList.add('less-motion'));
    assert.ok((await snapshotScene(id)).motion.every(name => name === 'none'));
    if (id === 'umbrella') {
      assert.deepEqual(await page.$eval(stinger, cut => [getComputedStyle(cut).visibility, cut.getAnimations({subtree:true}).length]), ['hidden',0]);
    }
    await page.evaluate(() => document.body.classList.remove('less-motion'));
    await page.setViewport({width:1440,height:900});
    await page.click(nextScene);
    if (id === 'umbrella') await page.waitForSelector(stinger,{hidden:true});
  }
  await waitForFaces();
  await page.click(picker);
  await page.waitForSelector('[role=listbox]');
  await page.keyboard.press('Home');
  await page.waitForFunction(() => document.activeElement?.getAttribute('data-preview-scene') === 'shrine');
  await page.keyboard.press('Enter');
  await waitForPickerClose();
  await page.waitForSelector(sceneArtwork('shrine'));
  await page.click(previousScene);
  await waitForFaces();
  await page.click(nextScene);
  await page.waitForSelector(sceneArtwork('shrine'));
  await page.click(previousScene);
  await waitForFaces();
  const first = await snapshot();
  assert.ok(Object.hasOwn(casting,first.scene));
  assert.equal(first.dotHop,'-1px');
  assert.deepEqual(first.crops,first.images.map(() => ['cover','0% 50%']));
  assert.deepEqual(first.dots,['0s','0.3s','0.6s'].map(delay => ['.','ui-loading-hop',delay]));
  assert.ok(first.tip);
  assert.ok(first.fits);
  const seenTips = [first.tip];
  assert.equal(await page.evaluate(() => window.tipClock.active()),1);
  for (let index = 1; index <= 4; index++) {
    await page.evaluate(() => window.tipClock.tick());
    await page.waitForFunction(previous => document.querySelector('.loading-activity-tip span').textContent !== previous, {}, seenTips.at(-1));
    const current = await snapshot();
    assert.equal(current.scene,first.scene);
    assert.deepEqual(current.images,first.images,'rotating tips does not restart the artwork');
    seenTips.push(current.tip);
    if (current.tip === first.tip) break;
  }
  assert.equal(seenTips.at(-1),first.tip);
  assert.ok(seenTips.length >= 4,'the multiplayer pool has at least three tips');
  assert.equal(new Set(seenTips).size,seenTips.length - 1);
  await page.setViewport({width:320,height:640});
  assert.ok((await snapshot()).fits);
  assert.deepEqual((await snapshot()).images,first.images,'resizing does not reroll the current load');
  for (const [draw,id] of [[0,'shrine'],[.999999,'sunflowers']]) {
    await page.evaluate(draw => {Math.random = () => draw;},draw);
    await page.click('[data-preview-replay]');
    await page.waitForSelector(sceneArtwork(id));
    await waitForFaces();
    assert.equal(await page.evaluate(() => window.tipClock.active()),1,'replaying replaces the previous tip clock');
  }
  await page.emulateMediaFeatures([{name:'prefers-reduced-motion',value:'reduce'}]);
  assert.deepEqual((await snapshot()).dots.map(([dot,motion]) => [dot,motion]),Array(3).fill(['.','none']));
  await page.emulateMediaFeatures([{name:'prefers-reduced-motion',value:'no-preference'}]);
  await page.evaluate(() => document.body.classList.add('less-motion'));
  assert.deepEqual((await snapshot()).dots.map(([dot,motion]) => [dot,motion]),Array(3).fill(['.','none']));
  assert.deepEqual(errors,[]);
  console.log('PASS loading previews: casting, five-second tip rotation before and after hydration, narrow layout and reduced motion');
} finally {await browser.close();}
