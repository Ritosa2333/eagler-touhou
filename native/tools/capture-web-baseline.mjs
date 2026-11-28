import assert from 'node:assert/strict';
import {createRequire} from 'node:module';
import {mkdir, readFile, writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';

const require = createRequire(new URL('../../launcher/package.json', import.meta.url));
const puppeteer = require('puppeteer-core');
const options = JSON.parse(await readFile(new URL('../docs/web-baseline.json', import.meta.url)));
const [output, subset] = process.argv.slice(2);
if (!output) throw new Error('Usage: node native/tools/capture-web-baseline.mjs <output-directory> [comma-separated-screen-ids]');
if (!process.env.CHROME_PATH) throw new Error('Set CHROME_PATH to a Chromium browser executable.');
const directory = resolve(output);
await mkdir(directory, {recursive:true});
const rows = [];
const realProgress = process.env.TOUHOU_PROGRESS_INPUT
  ? JSON.parse(await readFile(process.env.TOUHOU_PROGRESS_INPUT))
  : null;
const realCloudFixture = realProgress && {
  limits:{fileBytes:4194304,totalBytes:8388608,files:6},
  products:Object.keys(realProgress).map(game => ({
    id:game,game,multiplayer:false,files:[],requiredFiles:[],revision:1,
  })),
  saves:Object.entries(realProgress).map(([game,statistics]) => ({
    product:game,revision:1,uploadedAt:'2026-11-24T10:00:00Z',files:[],statistics,
  })),
};

const browser = await puppeteer.launch({executablePath:process.env.CHROME_PATH, headless:true, args:['--no-first-run']});
const profileEditor = JSON.parse(await readFile(new URL('profile-editor-fixture.json', import.meta.url)));
const directoryFixture = JSON.parse(await readFile(new URL('lobby-directory-fixture.json', import.meta.url)));
const lobbyRatings = JSON.parse(await readFile(new URL('lobby-ratings-fixture.json', import.meta.url)));
const profileTh09 = JSON.parse(await readFile(new URL('profile-th09-fixture.json', import.meta.url)));
const profileSocial = JSON.parse(await readFile(new URL('profile-social-fixture.json', import.meta.url)));
const occupiedDirectoryFixture = {
  ...directoryFixture,
  mine: {product:'th06mp', code:'4162', recoveryToken:'fixture-previous-room'},
};
const cappedDirectoryFixture = {
  ...directoryFixture,
  rooms: [
    ...Array.from({length:200}, (_, index) => ({
      ...directoryFixture.rooms.find(room => room.product === 'th06mp'),
      code: String(2000 + index),
    })),
    directoryFixture.rooms.find(room => room.product === 'th09mp'),
  ],
};
const roomFixture = JSON.parse(await readFile(new URL('room-fixture.json', import.meta.url)));
const {product: timedProduct, ...timingState} = roomFixture.timing;
const timedRoomFixture = {
  ...roomFixture, product: timedProduct,
  state: {...roomFixture.state, room: {...roomFixture.state.room, ...timingState}},
};
const ratedRoomFixture = {
  ...roomFixture,
  product: 'th09mp',
  state: {
    ...roomFixture.state,
    room: {
      ...roomFixture.state.room,
      seats: roomFixture.state.room.seats.map((seat, index) => ({
        ...seat, profile:{username:index === 0 ? 'rated' : 'other', th09:{games:index === 0 ? 26 : 23, wins:index === 0 ? 13 : 11}},
      })),
      spectators: roomFixture.state.room.spectators.map(person => ({
        ...person, name:'妖梦', profile:{username:'unrated'},
      })),
    },
  },
};
const cloudFixture = JSON.parse(await readFile(new URL('game-progress-fixture.json', import.meta.url)));
const recentFixture = JSON.parse(await readFile(new URL('recent-library-fixture.json', import.meta.url)));
const friends = JSON.parse(await readFile(new URL('game-friends-fixture.json', import.meta.url)));

function installRelayFixture(directory, room, disconnect, includeChat) {
  if (room) localStorage.setItem('eagler-touhou-touch-options-v1',
    JSON.stringify({touchMovementMode:room.movementMode??'joystick'}));
  const original = window.WebSocket;
  class DirectorySocket extends EventTarget {
    constructor(value, history, invitedSpectator) {
      super();
      this.value = value;
      this.product = '';
      this.invitedSpectator = invitedSpectator;
      this.readyState = original.CONNECTING;
      setTimeout(() => {
        if (this.readyState !== original.CONNECTING) return;
        this.readyState = original.OPEN;
        this.notify('open', new Event('open'));
        this.snapshot();
        if (history) setTimeout(() => {
          if (this.readyState === original.OPEN) {
            this.notify('message', new MessageEvent('message', {data:JSON.stringify(history)}));
          }
        }, 250);
        if (disconnect) setTimeout(() => this.close(1006), 300);
      }, 50);
    }

    notify(type, event) {
      this.dispatchEvent(event);
      this['on' + type]?.(event);
    }
    snapshot() {
      if (this.readyState === original.OPEN) {
        const value = this.value;
        const rooms = value.type === 'directory'
          ? value.rooms.filter(room => !this.product || room.product === this.product)
          : null;
        const snapshot = rooms
          ? {...value, product:this.product, total:rooms.length, rooms:rooms.slice(0, 200)}
          : value;
        this.notify('message', new MessageEvent('message', {data:JSON.stringify(snapshot)}));
      }
    }
    send(message) {
      const request = JSON.parse(message);
      if (request.type === 'refresh') {
        this.product = request.product || '';
        queueMicrotask(() => this.snapshot());
      } else if (request.type === 'release-membership' &&
        request.recoveryToken === this.value.mine?.recoveryToken) {
        this.value = {...this.value, mine:null};
        queueMicrotask(() => this.snapshot());
      } else if (request.type === 'spectate' && this.invitedSpectator) {
        const room = this.value.room;
        if (!room.spectators.some(entry => entry.clientId === this.invitedSpectator.clientId)) {
          room.spectators.push(this.invitedSpectator);
          room.spectatorCount = room.spectators.length;
        }
        queueMicrotask(() => this.snapshot());
      }
    }
    close(code = 1000) {
      if (this.readyState === original.CLOSED) return;
      this.readyState = original.CLOSED;
      this.notify('close', new CloseEvent('close', {code}));
    }
  }
  window.WebSocket = new Proxy(original, {
    construct(Socket, params) {
      const query = new URL(params[0]).searchParams;
      if (query.get('directory') === '1') return new DirectorySocket(directory);
      if (room && query.has('lobby')) {
        const state = structuredClone(room.state);
        if (query.get('room') !== room.product+'-'+room.code) {
          return new DirectorySocket({type:'error',error:'Unexpected fixture room'});
        }
        const player = state.room.seats[0];
        player.clientId = query.get('lobby');
        if (room.watch) {
          state.room.seats[0] = null;
          return new DirectorySocket(state, null, player);
        }
        return new DirectorySocket(state, includeChat && room.chatHistory);
      }
      return Reflect.construct(Socket, params);
    },
  });
}
const selected = subset ? new Set(subset.split(',')) : null;

const fixtureThreads = [
  {username:'second',nickname:'第二位玩家',unread:0,
    lastMessage:{id:2,body:'准备开始了吗？',outgoing:false,sentAt:Date.parse('2026-11-24T10:00:00Z')}},
  {username:'peer',nickname:'另一位玩家',unread:1,
    lastMessage:{id:1,body:'第一条测试私信',outgoing:false,sentAt:Date.parse('2026-11-24T16:00:00Z')}},
];
let fixtureRead = false;

function replyToAccountFixture(request, fixture) {
  const inbox = fixture === 'private-inbox';
  const {pathname, searchParams} = new URL(request.url());
  const method = request.method();
  const fixtureReceipt = inbox && method === 'POST' && pathname === '/api/v1/me/messages/peer/read';
  if (method !== 'GET' && method !== 'HEAD' && method !== 'OPTIONS' && !fixtureReceipt)
    return request.abort();
  if (!pathname.startsWith('/api/')) return request.continue();
  const {username, draft, review} = profileEditor;
  const headers = {
    'Access-Control-Allow-Origin': options.origin,
    'Access-Control-Allow-Credentials': 'true',
    'Access-Control-Allow-Methods': inbox ? 'GET, POST, OPTIONS' : 'GET, OPTIONS',
    'Access-Control-Allow-Headers': 'Content-Type',
  };
  if (method === 'OPTIONS') return request.respond({status:204,headers});
  const responses = {
    '/api/ready': {ready:true},
    '/api/auth/get-session': {user:{username,name:draft.name}},
    ['/api/v1/profiles/'+username]: {username,nickname:draft.name,details:draft,
      joinedAt:null,reviewPending:false,roomAvatarReady:false},
    '/api/v1/me/profile': {draft,review},
    '/api/v1/me/cloud-saves': fixture === 'real-progress' ? realCloudFixture : cloudFixture,
  };
  if (fixture === 'profile-th09') {
    responses['/api/auth/get-session'] = {};
    responses['/api/v1/profiles/'+username+'/th09-duels'] = profileTh09.history;
    responses['/api/v1/th09/duels/'+profileTh09.history.duels[0].duelId+'/replays'] = profileTh09.replays;
  }
  if (fixture === 'profile-social') {
    responses['/api/auth/get-session'] = {};
    responses['/api/v1/profiles/peer'] = {username:'peer',nickname:'Peer',details:draft};
    const tab = searchParams.get('list');
    const list = Object.hasOwn(profileSocial, tab) && Array.isArray(profileSocial[tab])
      ? {[tab === 'following' ? 'followingUsers' : tab]: profileSocial[tab]}
      : {};
    responses['/api/v1/profiles/'+username+'/social'] = {...profileSocial.summary,...list};
    responses['/api/v1/profiles/peer/social'] = profileSocial.private;
  }
  if (['lobby-ratings','room-ratings','board-ratings','board-locate'].includes(fixture)) {
    responses['/api/auth/get-session'] = {user:{username:'fixture',name:'测试玩家'}};
    responses['/api/v1/th09/ratings'] = lobbyRatings.ratings;
    const {board:[first, target]} = lobbyRatings.board;
    responses['/api/v1/th09/ratings/board'] = fixture === 'board-locate'
      ? {...lobbyRatings.board, board:[
          ...Array.from({length:64}, (_, index) => ({
            ...first, player:'fixture-'+index, rating:1700-index, profile:undefined,
          })),
          target,
        ]}
      : lobbyRatings.board;
    for (const [username, profile] of Object.entries(lobbyRatings.profiles)) {
      responses['/api/v1/profiles/'+username] = profile;
    }
  }
  if (inbox) {
    const viewer = {username:'fixture',name:'测试玩家'};
    responses['/api/auth/get-session'] = {user:viewer};
    responses['/api/v1/announcements'] = {items:[],nextBefore:null,canPublish:false};
    responses['/api/v1/me/messages'] = {
      threads: searchParams.has('summary') ? [] : fixtureThreads.map(thread => ({
        ...thread, unread: fixtureRead ? 0 : thread.unread,
      })),
      unread: fixtureRead ? 0 : 1, nextBefore:null,
    };
    const peer = fixtureThreads[1];
    responses['/api/v1/me/messages/peer'] = {
      peer:{username:peer.username,nickname:peer.nickname}, messages:[peer.lastMessage], nextBefore:null,
    };
    if (fixtureReceipt) {
      fixtureRead = true;
      responses[pathname] = {ok:true};
    }
  }
  if (fixture === 'game-friends') {
    responses['/api/v1/profiles/'+username+'/social'] = {
      visible:true, following:null, followers:[], followingUsers:[],
      friends:Object.entries(friends).map(([username, {nickname}]) => ({username,nickname})),
    };
    for (const [username, sample] of Object.entries(friends)) {
      const {game='th07',career=true,records=true,playtimeSeconds=null,playCount=null,cleared=false,bestScore=null} = sample;
      responses['/api/v1/profiles/'+username+'/statistics'] = {
        career, records, playtimeSeconds:records?playtimeSeconds:null,
        games:career||records?[{
          product:game, uploadedAt:cloudFixture.saves[0].uploadedAt,
          statistics:{
            game, parserVersion:1, format:'fixture', complete:true, warnings:[], loadouts:[],
            playtimeSeconds:records?playtimeSeconds:null, applicationTimeSeconds:null,
            playCount:records?playCount:null, clearCount:null,
            progress:career?[{
              difficulty:'normal', character:0, shot:null, cleared, oneCreditClear:false,
              clearCount:null, playCount:null, bestScore:records?bestScore:null, playtimeSeconds:null,
            }]:[],
          },
        }]:[],
      };
    }
  }
  const result = responses[pathname] ?? {};
  return request.respond({status:200,headers,contentType:'application/json',body:JSON.stringify(result)});
}
if (selected) for (const id of selected) assert(options.screens.some(screen=>screen.id===id), 'Unknown screen: '+id);
async function activate(page, selector) {
  const target = await page.waitForSelector(selector, {visible:true});
  await target.evaluate(node=>node.scrollIntoView({block:'nearest'}));
  await page.waitForFunction(selector=>{
    const node=document.querySelector(selector);
    if (!node) return false;
    const rect=node.getBoundingClientRect();
    return node.contains(document.elementFromPoint(rect.x+rect.width/2,rect.y+rect.height/2));
  }, {}, selector);
  await target.click();
}
async function expandSettingsGroup(page, selector) {
  const heading = await page.waitForSelector(selector, {visible:true});
  const expanded = await heading.evaluate(node => node.getAttribute('aria-expanded'));
  await heading.dispose();
  if (expanded === 'false') await activate(page, selector);
}
async function applyAction(page, action) {
  if (action === 'profile-crop-avatar' || action === 'profile-crop-cover') {
    const kind = action === 'profile-crop-avatar' ? 'avatar' : 'cover';
    const input = await page.waitForSelector('#settings-' + kind);
    const sample = new URL('../archive/web-only/assets/account-background.webp', import.meta.url);
    await input.uploadFile(fileURLToPath(sample));
    await input.dispose();
    return;
  }
  if (action==='drawer') return activate(page, '.masthead-menu-toggle');
  if (action==='announcement-preview' || action==='inbox-preview-guest') {
    const trigger = action==='announcement-preview' ? ':first-child' : ':last-child';
    return activate(page, '.masthead-inbox .masthead-inbox-action'+trigger);
  }
  if (action==='interface-language') {
    await activate(page, '.masthead-menu-toggle');
    return activate(page, page.viewport().isMobile
      ? 'button[data-overview-panel="language"]'
      : '.masthead-language-trigger');
  }
  if (action==='background-settings') {
    if (page.viewport().isMobile) {
      await activate(page, '.masthead-menu-toggle');
      await activate(page, '.masthead-overview-theme');
    } else {
      await activate(page, '.book-theme-toggle');
    }
    const grid = await page.waitForSelector('.background-preset[data-preset="grid"]', {visible:true});
    if (await grid.evaluate(button => button.getAttribute('aria-pressed') !== 'true')) {
      await activate(page, '.background-preset[data-preset="grid"]');
    }
    await grid.dispose();
    return;
  }
  if (action==='home-language') {
    return activate(page, '.launcher-home .settings-disclosure-trigger[aria-expanded="false"]');
  }
  if (action==='site-donation') return activate(page, '.book-top-donation');
  if (action==='registration-terms') return activate(page, '#registration-terms');
  if (action==='room-rules') return activate(page, '#roomWorkspaceTab-game');
  if (action==='room-game-display') {
    await activate(page, '#mpSettingsRoomDrawerToggle');
    await expandSettingsGroup(page, '#mpDisplayOptions > .options-group-head');
  }
  if (action==='network-check') return activate(page, '#lobbyNetworkCheck');
  if (action==='lobby-rating-card') return activate(page, '[data-key="th09mp-4201"] .lobby-seat[data-registered]');
  if (action==='room-rating-card') return activate(page, '[data-mp-seat-glyph="0"][data-registered]');
  if (action==='board-rating-card') return activate(page, '.player-card-avatar-button[aria-label="灵梦"]');
  if (action==='board-filters') return activate(page, '.rating-filter-trigger');
  if (action==='board-columns') {
    await page.waitForSelector('.rating-table tbody th[scope="row"]');
    return activate(page, '.rating-table th[data-column="winRate"] button');
  }
  if (action==='board-locate') {
    const search = '.rating-search input';
    await page.waitForSelector(search, {visible:true});
    await page.type(search, '@rated');
    await activate(page, '.rating-search button[aria-label="在全榜定位"]:enabled');
    await page.waitForFunction(() => document.activeElement?.matches('tr[data-located]'));
    return;
  }
  if (action==='room-quick-picker') {
    if (!page.viewport().isMobile) return activate(page, 'button[title="快捷发言"]');
    const selector = '#mpRoomChat button[title="快捷发言"]';
    const trigger = await page.waitForSelector(selector, {visible:true});
    try {
      await trigger.evaluate(node => node.scrollIntoView({block:'center'}));
      await page.waitForFunction(selector => !document.querySelector(selector)?.disabled,
        {timeout:5000}, selector);
      await trigger.evaluate(node => node.click());
      if (await trigger.evaluate(node => node.getAttribute('aria-expanded') !== 'true')) {
        await page.evaluate(() => document.activeElement?.blur());
        await page.keyboard.press('Alt');
      }
    } finally {
      await trigger.dispose();
    }
    return;
  }
  if (action==='profile-all-games') {
    return activate(page, '.profile-game-data-switch [aria-label="全部作品"]');
  }
  if (action==='profile-duel-replays') {
    return activate(page, '.profile-th09-history-meta button[aria-expanded="false"]');
  }
  if (action==='profile-th09-rank') {
    return activate(page, '.profile-th09-series button:nth-child(2)');
  }
  if (action==='game-settings') return activate(page, '#gameSettingsOpen');
  if (action==='game-settings-display') {
    await activate(page, '#gameSettingsOpen');
    await expandSettingsGroup(page, '#displayOptions > .options-group-head');
  }
  if (['game-keyboard','game-gamepad','th09-local-duel'].includes(action)) {
    await activate(page, '#gameSettingsOpen');
    await expandSettingsGroup(page, '.game-settings-page .input-settings-heading');
    const entry='.game-settings-page .input-settings-entry';
    await activate(page, entry+':first-child');
    if (action==='game-gamepad') {
      await activate(page, '.input-device-tabs [role="tab"]:last-child');
    } else if (action==='th09-local-duel') {
      await activate(page, '.input-settings-dialog button[id$="-scope-trigger"]');
      await activate(page, '[role="option"][data-value="local"]');
    }
    return;
  }
  if (action==='game-data-import' || action==='game-package-reference') {
    await page.waitForNetworkIdle({idleTime:800,timeout:10000}).catch(()=>{});
    await activate(page, '#gamePackageImport');
    if (action==='game-data-import') return;
    await page.waitForSelector('#transferDownload:enabled', {visible:true,timeout:6000});
    await page.focus('#transferDownload');
    return page.keyboard.press('Enter');
  }
  if (['game-files','game-save-confirm','replay-manager-empty'].includes(action)) {
    await activate(page, '#gameSettingsOpen');
    await expandSettingsGroup(page, '#fileOptions > .options-group-head');
    if (action==='replay-manager-empty') {
      return activate(page, '#replayFileTool [data-action="manage-replay"]');
    }
    if (action==='game-save-confirm') {
      return activate(page, '#saveFileTool [data-action="import-save"]');
    }
    const rows = await page.waitForSelector('#replayFileTool', {visible:true});
    await rows.evaluate(node=>node.scrollIntoView({block:'end'}));
    return;
  }
  if (action==='touch-layout' || action==='touch-sensitivity-drag') {
    await activate(page, '#gameSettingsOpen');
    if (await page.$('#mobileOptionsToggle[aria-expanded="false"]')) {
      await activate(page, '#mobileOptionsToggle[aria-expanded="false"]');
    }
    await activate(page, '#touchLayoutEdit');
    if (action==='touch-layout') return;
    await page.waitForSelector('#touchSensitivityPreview', {visible:true});
    if (page.viewport().hasTouch) {
      await activate(page, '#touchLayoutEditor [aria-controls="touchWorkbenchBody"]');
      await page.waitForSelector('#touchLayoutEditor.is-collapsed', {visible:true});
    }
    await page.evaluate(() => new Promise(resolve =>
      requestAnimationFrame(() => requestAnimationFrame(resolve))));
    const {x, y, left, top} = await page.$eval('#touchSensitivityPreview', target => {
      const player = target.closest('.player');
      const bounds = player.getBoundingClientRect();
      const blocked = '.touch-layout-editor,.touch-layout-settings,[data-ui-surface="application"],button,select,input,.mizuki-select-menu,[data-touch-layout-control]';
      for (const fractionY of [.26, .42, .58, .74]) {
        for (const fractionX of [.48, .32, .66, .18, .8]) {
          const x = Math.round(bounds.left + bounds.width * fractionX);
          const y = Math.round(bounds.top + bounds.height * fractionY);
          const hit = document.elementFromPoint(x, y);
          if (player.contains(hit) && !hit.closest(blocked)) {
            return {x, y, left:bounds.width / 2 + 15, top:bounds.height / 2 + 37.5};
          }
        }
      }
      throw new Error('No free touch preview area');
    });
    const finger = page.viewport().hasTouch
      ? await page.touchscreen.touchStart(x, y) : null;
    if (!finger) {
      await page.mouse.move(x, y);
      await page.mouse.down();
    }
    const release = () => finger ? finger.end() : page.mouse.up();
    try {
      await page.waitForSelector('#touchSensitivityPreview.active', {timeout:4000});
      if (finger) await finger.move(x + 10, y + 25);
      else await page.mouse.move(x + 10, y + 25);
      await page.waitForFunction((left, top) => {
        const target = document.querySelector('#touchSensitivityPreview.active');
        return target && Math.abs(parseFloat(target.style.left) - left) < 1
          && Math.abs(parseFloat(target.style.top) - top) < 1;
      }, {timeout:4000}, left, top);
    } catch (error) {
      await release();
      throw error;
    }
    return release;
  }
  if (action==='library-list') return activate(page, '.book-view-switch button:last-child');
  if (action==='library-sort-keyboard') {
    const focusedOption = text => page.waitForFunction(label => {
      const current = document.activeElement;
      return current?.getAttribute('role') === 'option' &&
        current.textContent.includes(label);
    }, {timeout:5000}, text);
    const focusedSort = () => page.waitForFunction(() =>
      document.activeElement?.matches('.book-sort') &&
      document.activeElement.getAttribute('aria-expanded') === 'false',
    {timeout:5000});
    await page.focus('.book-sort');
    await page.keyboard.press('Enter');
    await focusedOption('按作品编号');
    await page.keyboard.press('ArrowDown');
    await focusedOption('编号倒序');
    await page.keyboard.press('Enter');
    await focusedSort();
    await page.keyboard.press('ArrowDown');
    await focusedOption('编号倒序');
    await page.keyboard.press('Escape');
    await focusedSort();
    await page.keyboard.press('Enter');
    return focusedOption('编号倒序');
  }
  if (action==='library-reverse' || action==='library-list-reverse') {
    if (action==='library-list-reverse') {
      await activate(page, '.book-view-switch button:last-child');
    }
    await activate(page, '.book-sort');
    const reverse = await page.waitForFunction(() =>
      [...document.querySelectorAll('[role="option"]')]
        .find(option => option.textContent.trim() === '编号倒序'),
      {timeout: 8000},
    );
    await reverse.asElement().click();
    await reverse.dispose();
    return page.waitForFunction(() =>
      document.querySelector('.book-sort')?.textContent.includes('编号倒序'),
      {timeout: 8000},
    );
  }
  if (action==='spell-list') return activate(page, '.spell-view-switch button:last-child');
  if (action==='cloud-files') {
    await applyAction(page, 'cloud-saves');
    return activate(page, '.decision-version details summary');
  }
  if (action==='cloud-saves') {
    const row = await page.waitForSelector('.cloud-save-row', {visible:true});
    try {
      if (await row.evaluate(button => button.getAttribute('aria-expanded') === 'true')) return;
      if (await page.evaluate(() => navigator.maxTouchPoints > 0)) await row.tap();
      else await activate(page, '.cloud-save-row[aria-expanded="false"]');
      if (await row.evaluate(button => button.getAttribute('aria-expanded') === 'false')) {
        await row.focus();
        await page.keyboard.press('Enter');
      }
    } finally {
      await row.dispose();
    }
  }
}
const inspect = () => {
  const visible = element => element.getClientRects().length && !element.closest('[hidden], [inert]') &&
    getComputedStyle(element).visibility === 'visible';
  const selector='header,main,footer,nav,aside,h1,h2,h3,button,a,input,select,textarea,details,summary,[role="switch"],[role="option"],[role="tablist"],[role="dialog"],[role="table"],[data-book-card]';
  const elements=[...document.querySelectorAll(selector)].filter(visible).map(element=>{
    const rect=element.getBoundingClientRect(), style=getComputedStyle(element);
    return {tag:element.tagName.toLowerCase(), role:element.getAttribute('role'), id:element.id||null,
      label:(element.getAttribute('aria-label')??element.innerText??element.getAttribute('placeholder')??'').trim().replace(/\s+/gu,' ').slice(0,160),
      href:element.getAttribute('href'), state:{expanded:element.getAttribute('aria-expanded') ??
        (element instanceof HTMLDetailsElement ? String(element.open) : null),
        pressed:element.getAttribute('aria-pressed'), checked:element.getAttribute('aria-checked'),
        focused:element === document.activeElement,
        disabled:element.hasAttribute('disabled')||element.getAttribute('aria-disabled')==='true'},
      rect:[rect.x,rect.y,rect.width,rect.height].map(value=>Math.round(value*10)/10),
      font:[style.fontSize,style.fontWeight], overflow:[style.overflowX,style.overflowY]};
  });
  const zone=document.querySelector('#touchLayoutSafeZone');
  const touchGeometry=zone&&visible(zone)?(()=>{
    const surface=zone.parentElement;
    const controls=[...surface.querySelectorAll('[data-touch-layout-control]')];
    const old=controls.map(node=>[node,node.hidden,node.getAttribute('style')]);
    const custom=surface.classList.contains('touch-layout-custom');
    surface.classList.add('touch-layout-capturing');
    surface.classList.remove('touch-layout-custom');
    for (const node of controls) { node.hidden=false; node.removeAttribute('style'); }
    try {
      const safe=zone.getBoundingClientRect();
      const point=v=>Math.round(Math.min(1,Math.max(0,v))*10000)/10000;
      return {width:safe.width,height:safe.height,controls:Object.fromEntries(controls.map(node=>{
        const rect=node.getBoundingClientRect();
        return [node.dataset.touchLayoutControl,{
          x:point((rect.left+rect.width/2-safe.left)/safe.width),
          y:point((rect.top+rect.height/2-safe.top)/safe.height),
          width:rect.width,height:rect.height,
        }];
      }))};
    } finally {
      for (const [node,hidden,style] of old) {
        node.hidden=hidden;
        if (style===null) node.removeAttribute('style'); else node.setAttribute('style',style);
      }
      surface.classList.remove('touch-layout-capturing');
      surface.classList.toggle('touch-layout-custom',custom);
    }
  })():null;
  return {address:location.pathname+location.search+location.hash,lang:document.documentElement.lang,
    theme:document.documentElement.dataset.bookTheme,viewport:[innerWidth,innerHeight],
    scroll:[scrollX,scrollY],documentSize:[document.documentElement.scrollWidth,document.documentElement.scrollHeight],elements,
    ...(touchGeometry&&{touchGeometry})};
};
try {
  const context = await browser.createBrowserContext();
  const manifestPage = await context.newPage();
  await manifestPage.setBypassServiceWorker(true);
  await manifestPage.goto(options.origin,{waitUntil:'domcontentloaded',timeout:45000});
  const revision=await manifestPage.evaluate(async()=>{
    const response=await fetch('/release-manifest.json',{cache:'no-store'});
    if (!response.ok) throw new Error('Cannot read release manifest');
    return (await response.json()).sources.launcher.revision;
  });
  assert.equal(revision,options.revision,'The production website differs from the baseline revision.');
  await manifestPage.close();
  let fixture;
  const accountFixtures = new Set(['profile-editor','profile-th09','profile-social','cloud-saves','private-inbox','game-friends','real-progress','lobby-ratings','room-ratings','board-ratings','board-locate']);
  const needsAccountFixture = options.screens.some(screen =>
    (!selected || selected.has(screen.id)) && accountFixtures.has(screen.fixture));
  for(const viewport of options.viewports){
    const mobile=!viewport.device.startsWith('desktop');
    const page = await context.newPage();
    await page.setViewport({width:viewport.width,height:viewport.height,isMobile:mobile,hasTouch:mobile,deviceScaleFactor:1});
    await page.setBypassServiceWorker(true);
    await page.evaluateOnNewDocument(()=>{
      localStorage.setItem('eagler-touhou-first-use-notice-seen-v1','1');
      localStorage.setItem('eagler-touhou-less-motion-v1','1');
    });
    if (needsAccountFixture) {
      await page.setRequestInterception(true);
      page.on('request', request => accountFixtures.has(fixture)
        ? replyToAccountFixture(request, fixture)
        : request.continue());
    }
    for(const screen of options.screens){
      if (selected && !selected.has(screen.id)) continue;
      if ((screen.devices || viewport.optIn) && !screen.devices?.includes(viewport.device)) continue;
      if(screen.fixture==='real-progress'&&!realCloudFixture) {
        assert(!selected, 'Set TOUHOU_PROGRESS_INPUT for real-save screenshots.');
        continue;
      }
      await page.emulateMediaFeatures([{name:'prefers-color-scheme',value:screen.colorScheme??'dark'}]);
      const themeScript = screen.colorScheme
        ? await page.evaluateOnNewDocument(theme => {
            localStorage.setItem('eagler.book.theme', JSON.stringify(theme));
          }, screen.colorScheme)
        : null;
      const stem=viewport.device+'-'+screen.id;
      fixture=screen.fixture;
      fixtureRead = false;
      const injected = fixture === 'recent-library'
        ? await page.evaluateOnNewDocument(dates => {
            localStorage.setItem('eagler.book.recent', JSON.stringify(Object.keys(dates)));
            localStorage.setItem('eagler.book.last-played', JSON.stringify(dates));
          }, recentFixture.dates)
        : ['lobby-directory', 'lobby-capped', 'lobby-membership', 'lobby-ratings', 'room', 'room-ratings'].includes(fixture)
        ? await page.evaluateOnNewDocument(installRelayFixture,
          fixture === 'lobby-capped' ? cappedDirectoryFixture
            : fixture === 'lobby-membership' ? occupiedDirectoryFixture
            : fixture === 'lobby-ratings' ? {...directoryFixture, rooms:lobbyRatings.rooms} : directoryFixture,
          fixture === 'room' ? (screen.roomWatch ? {...roomFixture, watch:true}
              : screen.roomTiming ? timedRoomFixture
                : {...roomFixture, product:screen.roomProduct??roomFixture.product,
                    movementMode:screen.roomMovementMode})
            : fixture === 'room-ratings' ? ratedRoomFixture : null,
          !!screen.disconnect, !!screen.roomChat)
        : null;
      let releaseInput;
      try {
        await page.goto(new URL(screen.path,options.origin).href,{waitUntil:'domcontentloaded',timeout:45000});
        if (fixture === 'profile-social' || fixture === 'profile-editor') {
          await page.waitForFunction(()=>window.__eaglerBoot?.done,{timeout:18000});
        }
        if (screen.action) {
          if (!['registration-terms','profile-all-games','profile-duel-replays','profile-th09-rank','profile-crop-avatar','profile-crop-cover','cloud-saves','cloud-files','room-rules','room-game-display','room-quick-picker','network-check','lobby-rating-card','room-rating-card','board-rating-card','board-locate','board-filters','board-columns'].includes(screen.action)) {
            await page.waitForFunction(()=>window.__eaglerBoot?.done,{timeout:18000});
          }
          releaseInput = await applyAction(page,screen.action);
        }
        await page.waitForSelector(screen.selector,{visible:true,timeout:screen.timeoutMs??18000});
        if (fixture === 'profile-social' ||
            (fixture === 'profile-editor' && screen.selector === '#settings-name')) {
          await page.waitForFunction(selector => {
            const element = document.querySelector(selector);
            if (!element) return false;
            const rect = element.getBoundingClientRect();
            return element.contains(document.elementFromPoint(
              rect.left + rect.width / 2, rect.top + rect.height / 2,
            ));
          }, {timeout:18000}, screen.selector);
        }
        await page.evaluate(()=>document.fonts.ready);
        if (screen.scrollTop) {
          await page.evaluate(()=>window.scrollTo({top:0,behavior:'instant'}));
        }
        if (screen.scrollTo) {
          const target = await page.waitForSelector(screen.scrollTo,{visible:true});
          await target.evaluate(node=>node.scrollIntoView({block:'start'}));
          await target.dispose();
        }
        const expectedLang=screen.lang??'zh-CN';
        await page.waitForFunction(lang=>document.documentElement.lang===lang,{timeout:12000},expectedLang);
        if (screen.colorScheme) {
          await page.waitForFunction(theme=>document.documentElement.dataset.bookTheme===theme,
            {timeout:12000},screen.colorScheme);
        }
        const frame=await page.evaluate(inspect);
        assert.equal(frame.lang,expectedLang);
        await page.screenshot({path:resolve(directory,stem+'.png')});
        await writeFile(resolve(directory,stem+'.json'),JSON.stringify(frame,null,2)+'\n');
        rows.push({screen:screen.id,device:viewport.device,address:frame.address,pencil:screen.pencil[viewport.device]??null,
          screenshot:stem+'.png',inventory:stem+'.json',observed:frame.elements.length,
          ...(fixture&&{fixture}), ...(screen.colorScheme&&{colorScheme:screen.colorScheme})});
        console.log(JSON.stringify(rows.at(-1)));
      } catch(error){
        rows.push({screen:screen.id,device:viewport.device,error:String(error).split('\n')[0]});
        console.error(JSON.stringify(rows.at(-1)));
      } finally {
        if (typeof releaseInput === 'function') await releaseInput();
        if (themeScript) {
          await page.removeScriptToEvaluateOnNewDocument(themeScript.identifier);
          await page.evaluate(()=>localStorage.removeItem('eagler.book.theme'));
        }
        if (injected) await page.removeScriptToEvaluateOnNewDocument(injected.identifier);
        if (fixture === 'recent-library') await page.evaluate(() => {
          localStorage.removeItem('eagler.book.recent');
          localStorage.removeItem('eagler.book.last-played');
        });
      }
    }
    await page.close();
  }
  await context.close();
} finally {
  await browser.close();
  await writeFile(resolve(directory,'manifest.json'),JSON.stringify({revision:options.revision,origin:options.origin,captures:rows},null,2)+'\n');
}
if (rows.some(row=>row.error)) process.exitCode=1;
