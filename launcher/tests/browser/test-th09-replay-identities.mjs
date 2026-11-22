import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';

const origin = new URL(process.argv[2] || 'http://127.0.0.1:5173/');
const screenshot = process.argv[3];
const duelIds = [
  '72aeaea1-dd24-43cb-ad47-68ced29d8881',
  '72aeaea1-dd24-43cb-ad47-68ced29d8882',
  '72aeaea1-dd24-43cb-ad47-68ced29d8883',
];
const files = [[0,5,3],[0,5,2],[0,5,2],[0,5,1],[13,9,2],[13,5,4],[0,12,2]]
  .map(([p1,p2,minutes], i) => ({
    roundId: '1:' + (1000 + i * 600), characters:[p1,p2], frames:minutes * 3600,
  }));
const longId = 'replay-opponent-with-a-long-id';
const replayLists = [
  {playerUsernames:['alice','bob'], files},
  {playerUsernames:[longId,'Alice'], files:files.slice(0,1)},
  {files:files.slice(0,1)},
];
const playedAt = Date.parse('2026-11-17T05:26:34Z');
const duels = duelIds.map((duelId, i) => ({
  duelId, playedAt:playedAt - i * 60_000, wins:i ? 0 : 2, losses:i ? 1 : 5,
  durationFrames:replayLists[i].files.reduce((total, file) => total + file.frames, 0),
  replayCount:replayLists[i].files.length,
  opponent:{username:'alice',displayName:'Alice'},
}));
const group = '.profile-th09-replays';
const browser = await puppeteer.launch({headless:true, executablePath:await findChromiumExecutable()});

async function openProfile({width, member, scale = 1}) {
  const page = await browser.newPage();
  await page.setViewport({width, height:820, deviceScaleFactor:scale});
  await page.emulateMediaFeatures([
    {name:'prefers-color-scheme',value:'dark'},
    {name:'prefers-reduced-motion',value:'reduce'},
  ]);
  await page.evaluateOnNewDocument(() => localStorage.setItem('eagler-touhou-first-use-notice-seen-v1', '1'));
  await page.setRequestInterception(true);
  page.on('request', request => {
    const url = new URL(request.url());
    if (url.origin !== origin.origin || !url.pathname.startsWith('/api/')) return request.continue();
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

async function expand(page, index) {
  let groups = await page.$$(group);
  if (!groups[index]) {
    await page.click('button[aria-controls="profile-th09-duel-list"]');
    await page.waitForFunction(({group,index}) => !!document.querySelectorAll(group)[index], {}, {group,index});
    groups = await page.$$(group);
  }
  const set = groups[index];
  await (await set.$('summary')).click();
  await set.waitForSelector('ol > li');
  return set;
}

const sides = set => set.$$eval('ol > li:first-child .th09-replay-side', nodes => nodes.map(node => ({
  text:node.textContent.trim(), self:node.hasAttribute('data-self'),
})));

try {
  const desktop = await openProfile({width:1440, member:true, scale:2});
  const first = await expand(desktop, 0);
  assert.deepEqual(await sides(first), [
    {text:'灵梦 @alice',self:true}, {text:'琪露诺 @bob',self:false},
  ], "The viewer's 1P side is highlighted on the other player's profile");
  assert.deepEqual(await first.$$eval('ol > li:first-child .th09-replay-actions > button', buttons => buttons.map(node => node.textContent.trim())),
    ['播放','下载']);
  assert.equal(await first.$$eval('.th09-replay-actions', rows => rows.every(row =>
    row.firstElementChild.getBoundingClientRect().top === row.lastElementChild.getBoundingClientRect().top)), true);
  if (screenshot) {
    await desktop.evaluate(() => document.fonts.ready);
    const card = await first.evaluateHandle(node => node.closest('.profile-th09-history > li'));
    await card.asElement().screenshot({path:screenshot});
  }
  await desktop.close();

  const narrow = await openProfile({width:390, member:true});
  const second = await expand(narrow, 1);
  assert.deepEqual(await sides(second), [
    {text:'灵梦 @' + longId,self:false}, {text:'琪露诺 @Alice',self:true},
  ]);
  assert.equal(await second.$eval('ol', list => list.scrollWidth <= list.clientWidth + 1), true);
  const legacy = await expand(narrow, 2);
  assert.deepEqual(await sides(legacy), [{text:'灵梦',self:false}, {text:'琪露诺',self:false}]);
  await narrow.close();

  const guest = await openProfile({width:960, member:false});
  assert.deepEqual(await sides(await expand(guest, 0)), [
    {text:'灵梦 @alice',self:false}, {text:'琪露诺 @bob',self:false},
  ]);
  await guest.close();
  console.log('PASS: TH09 seat identities, viewer highlight, narrow layout, and legacy fallback');
} finally {
  await browser.close();
}
