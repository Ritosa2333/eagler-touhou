import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';

const origin = new URL(process.argv[2] || 'http://127.0.0.1:5173/');
const screenshot = process.argv[3];
const narrowScreenshot = process.argv[4];
const duelIds = [
  '72aeaea1-dd24-43cb-ad47-68ced29d8881',
  '72aeaea1-dd24-43cb-ad47-68ced29d8882',
  '72aeaea1-dd24-43cb-ad47-68ced29d8883',
  '72aeaea1-dd24-43cb-ad47-68ced29d8884',
  '72aeaea1-dd24-43cb-ad47-68ced29d8885',
];
const files = [[0,5,3],[0,5,2],[0,5,2],[0,5,1],[13,9,2],[13,5,4],[0,12,2]]
  .map(([p1,p2,minutes], i) => ({
    roundId: '1:' + (1000 + i * 600), characters:[p1,p2], frames:minutes * 3600,
  }));
const longId = 'replay-opponent-with-a-long-id';
const alice = {username:'alice', displayName:'Alice', avatar:'1111111111111111'};
const bob = {username:'bob', displayName:'Bob', avatar:'2222222222222222'};
const replayLists = [
  {playerProfiles:[alice,bob], files},
  {playerProfiles:[{...bob, username:longId},{...alice, username:'Alice'}], files:files.slice(0,1)},
  {playerUsernames:[null,'alice'], files:files.slice(0,1)},
  {playerProfiles:[null,null], playerUsernames:['alice','bob'], files:files.slice(0,1)},
  {files:files.slice(0,1)},
];
const playedAt = Date.parse('2026-11-17T05:26:34Z');
const duels = duelIds.map((duelId, i) => ({
  duelId, playedAt:playedAt - i * 60_000, wins:i ? 0 : 2, losses:i ? 1 : 5,
  durationFrames:replayLists[i].files.reduce((total, file) => total + file.frames, 0),
  replayCount:replayLists[i].files.length,
  opponent:alice,
}));
const group = '.profile-th09-replays';
const browser = await puppeteer.launch({headless:true, executablePath:await findChromiumExecutable()});

async function openProfile({width, member, scale = 1}) {
  const page = await browser.newPage();
  const mobile = width <= 390;
  await page.setViewport({width, height:width >= 1440 ? 1080 : 820, deviceScaleFactor:scale, isMobile:mobile, hasTouch:mobile});
  await page.emulateMediaFeatures([
    {name:'prefers-color-scheme',value:'dark'},
    {name:'prefers-reduced-motion',value:'reduce'},
  ]);
  await page.evaluateOnNewDocument(() => localStorage.setItem('eagler-touhou-first-use-notice-seen-v1', '1'));
  await page.setRequestInterception(true);
  page.on('request', request => {
    const url = new URL(request.url());
    if (url.origin !== origin.origin) return request.continue();
    if (url.pathname === '/release-catalog.json') return request.respond({status:200, contentType:'application/json',
      body:JSON.stringify({schema:'eagler-touhou/release-catalog/1', games:{}})});
    if (!url.pathname.startsWith('/api/')) return request.continue();
    const avatar = url.pathname.split('/').at(-1);
    if (avatar === alice.avatar || avatar === bob.avatar) {
      const palette = avatar === alice.avatar ? ['#842249','#ffc3d1','#ed6084'] : ['#235b9d','#c3edff','#54a5f0'];
      return request.respond({status:200,contentType:'image/svg+xml',body:
        `<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64"><rect width="64" height="64" fill="${palette[0]}"/><path d="M14 18 26 27 38 27 50 18 48 40Q48 53 32 53T16 40Z" fill="${palette[1]}"/><path d="M24 38h2m12 0h2" stroke="${palette[0]}" stroke-width="4" stroke-linecap="round"/><path d="M28 46q4 4 8 0" fill="none" stroke="${palette[2]}" stroke-width="2"/></svg>`});
    }
    const replay = duelIds.findIndex(duelId => url.pathname === `/api/v1/th09/duels/${duelId}/replays`);
    let body;
    if (replay >= 0) body = replayLists[replay];
    else if (url.pathname === '/api/ready') body = {ready:true};
    else if (url.pathname === '/api/auth/get-session') body = member ? {user:{username:'alice',name:'Alice'}} : null;
    else if (url.pathname === '/api/v1/profiles/bob') body = {
      username:'bob', nickname:'Bob', joinedAt:null, details:{display:{
        gameDataDefault:'th09', order:['career','versus','records','content','friends','favorites'],
        visible:{career:false,versus:true,records:false,content:false,friends:false,favorites:false},
      }},
    };
    else if (url.pathname === '/api/v1/profiles/bob/social') body = {followers:0,following:0};
    else if (url.pathname === '/api/v1/profiles/bob/th09-duels') body = {
      summary:{games:9,wins:2}, durationFrames:55_000, duels, nextCursor:null,
    };
    return request.respond({status:body === undefined ? 404 : 200, contentType:'application/json',
      body:JSON.stringify(body ?? null)});
  });
  await page.goto(new URL('u/bob?lang=zh-CN', origin), {waitUntil:'domcontentloaded'});
  await page.waitForSelector(group);
  return page;
}

async function toggle(set) {
  const trigger = await set.evaluateHandle(panel => document.querySelector(`button[aria-controls="${panel.id}"]`));
  await trigger.asElement().click();
}

async function expand(page, index) {
  let groups = await page.$$(group);
  if (!groups[index]) {
    await page.click('button[aria-controls="profile-th09-duel-list"]');
    await page.waitForFunction(({group,index}) => !!document.querySelectorAll(group)[index], {}, {group,index});
    groups = await page.$$(group);
  }
  const set = groups[index];
  await toggle(set);
  await set.waitForSelector('ol > li', {visible:true});
  assert.equal(await set.evaluate(panel => panel.dataset.open), 'true');
  return set;
}

const sides = set => set.$$eval('ol > li:first-child .th09-replay-side', nodes => nodes.map(node => ({
  text:node.textContent.trim(), self:node.hasAttribute('data-self'),
})));

async function layout(set) {
  return set.$$eval('.th09-replay-row', rows => rows.every(row => {
    const rect = element => element.getBoundingClientRect();
    const actions = row.querySelector('.th09-replay-actions');
    const sides = [...row.querySelectorAll('.th09-replay-side')];
    const vs = row.querySelector('.th09-replay-vs');
    const avatars = [...row.querySelectorAll('.th09-replay-avatar')];
    const center = node => (rect(node).left + rect(node).right) / 2;
    return rect(sides[0]).top === rect(vs).top && rect(vs).top === rect(sides[1]).top &&
      rect(actions).top >= Math.max(...sides.map(side => rect(side).bottom)) &&
      Math.abs(rect(actions).right - rect(row).right) <= 1 && Math.abs(rect(actions).bottom - rect(row).bottom) <= 1 &&
      rect(actions.firstElementChild).top === rect(actions.lastElementChild).top &&
      avatars.every(avatar => {
        const side = sides.find(node => node.dataset.seat === avatar.dataset.seat);
        return rect(avatar).top >= rect(side).bottom && Math.abs(center(avatar) - center(side)) <= 1 &&
          avatar.hasAttribute('data-self') === side.hasAttribute('data-self');
      });
  }));
}

async function captureCard(page, set, path) {
  await page.evaluate(() => document.fonts.ready);
  const card = await set.evaluateHandle(node => node.closest('.profile-th09-history > li'));
  const element = card.asElement();
  await element.scrollIntoView();
  await element.$$eval('.th09-replay-avatar img, .profile-th09-opponent img', images => Promise.all(images.map(image => image.decode())));
  await element.screenshot({path});
}

try {
  const desktop = await openProfile({width:1440, member:true, scale:2});
  const first = await expand(desktop, 0);
  assert.deepEqual(await sides(first), [
    {text:'灵梦',self:true}, {text:'琪露诺',self:false},
  ], "The viewer's 1P side is highlighted on the other player's profile");
  assert.deepEqual(await first.$$eval('ol > li:first-child .th09-replay-actions > button', buttons => buttons.map(node => node.textContent.trim())),
    ['播放','下载']);
  assert.equal(await layout(first), true, 'Desktop avatars align with their characters and actions sit bottom-right');
  assert.equal(await first.$$eval('.th09-replay-avatar', avatars => avatars.length), files.length * 2);
  if (screenshot) await captureCard(desktop, first, screenshot);
  const bobAvatar = await first.$('li:first-child a.th09-replay-avatar[data-seat="1"]');
  await bobAvatar.hover();
  const card = await desktop.waitForSelector('.player-profile-popover .player-profile-card-name h3');
  assert.equal(await card.evaluate(node => node.textContent), 'Bob');
  const home = await bobAvatar.evaluate(node => node.href);
  assert.equal(new URL(home).pathname, '/u/bob');
  const [homeTab] = await Promise.all([
    browser.waitForTarget(target => target.type() === 'page' && target.opener() === desktop.target() && target.url() === home),
    bobAvatar.click(),
  ]);
  await (await homeTab.page()).close();
  await desktop.close();

  const narrow = await openProfile({width:390, member:true, scale:2});
  const second = await expand(narrow, 1);
  assert.deepEqual(await sides(second), [
    {text:'灵梦',self:false}, {text:'琪露诺',self:true},
  ]);
  assert.equal(await second.$eval('ol', list => list.scrollWidth <= list.clientWidth + 1), true);
  assert.equal(await layout(second), true, 'Narrow cards keep the same alignment even with a long profile ID');
  if (narrowScreenshot) await captureCard(narrow, second, narrowScreenshot);
  const legacy = await expand(narrow, 2);
  assert.deepEqual(await sides(legacy), [{text:'灵梦',self:false}, {text:'琪露诺',self:true}]);
  assert.equal(await legacy.$$eval('.th09-replay-avatar', nodes => nodes.length), 1);
  assert.ok(await legacy.$('.th09-replay-avatar .account-avatar'), 'Older APIs use the default avatar');
  assert.equal(await legacy.$('.th09-replay-avatar .participant-avatar-image'), null);
  const hidden = await expand(narrow, 3);
  assert.deepEqual(await sides(hidden), [{text:'灵梦',self:false}, {text:'琪露诺',self:false}]);
  assert.equal(await hidden.$('.th09-replay-avatar'), null, 'Do not fall back to IDs when the profiles are hidden');
  const noIdentity = await expand(narrow, 4);
  assert.equal(await noIdentity.$('.th09-replay-avatar'), null, 'Do not guess seats if the replay API returns no identities');
  await toggle(noIdentity);
  await narrow.waitForFunction(id => document.getElementById(id).inert, {}, await noIdentity.evaluate(panel => panel.id));
  assert.equal(new URL(narrow.url()).pathname, '/u/bob', 'Expanding and closing rounds stays on the profile');
  await narrow.close();

  const guest = await openProfile({width:960, member:false});
  assert.deepEqual(await sides(await expand(guest, 0)), [
    {text:'灵梦',self:false}, {text:'琪露诺',self:false},
  ]);
  await guest.close();
  console.log('PASS: TH09 seat avatars, hover and click, viewer highlight, desktop and narrow layouts, and legacy fallback');
} finally {
  await browser.close();
}
