import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';

const origin = new URL(process.argv[2] || 'http://127.0.0.1:5173/');
const username = 'navigation-tester';
const user = {username, name:'Navigation Tester', emailVerified:true};
const profile = '/u/' + username;
const apiProfile = '/api/v1/profiles/' + username;
const date = new Date('2026-11-20').toISOString();
const duelId = '00000000-0000-4000-8000-000000000001';
const ratingHistory = [{at:Date.parse(date),rating:1700},{at:Date.parse(date) + 86400000,rating:1750}];
const workStatistics = {
  game:'th07',parserVersion:1,format:'native-th07',complete:true,
  playtimeSeconds:7380,applicationTimeSeconds:7380,playCount:3,clearCount:1,
  warnings:[],loadouts:[],
  progress:[{character:0,shot:0,difficulty:'easy',cleared:true,oneCreditClear:true,clearCount:1,playCount:3,bestScore:1000,playtimeSeconds:7380}],
  spellCards:[{number:3,name:'Test Spell',attempts:[3,0,0,0,0,0],captures:[2,0,0,0,0,0]}],
};
const fixtures = {
  '/release-catalog.json': {schema:'eagler-touhou/release-catalog/1', games:{}},
  '/api/ready': {ready:true},
  '/api/auth/get-session': {user},
  '/api/v1/me/cloud-saves': {
    products:[{id:'th07',game:'th07',multiplayer:false,files:[],requiredFiles:[],revision:1}],
    saves:[{product:'th07',revision:1,uploadedAt:date,files:[],statistics:workStatistics}],
    limits:{fileBytes:1,totalBytes:1,files:1},
  },
  [apiProfile]: {username, nickname:user.name, joinedAt:date, details:{bio:'Fixture', links:[], display:{gameDataDefault:'th09'}}},
  [apiProfile + '/social']: {visible:true, following:null, friends:[{username:'active-friend',nickname:'Active Friend'}], followers:[{username:'missing-player',nickname:'Missing Player'}], followingUsers:[], followingCount:32, followerCount:48, friendCount:1},
  '/api/v1/profiles/active-friend/statistics': {
    career:true, records:true, playtimeSeconds:7380,
    games:[{product:'th07',uploadedAt:date,statistics:workStatistics}],
  },
  [apiProfile + '/th09-duels']: {
    summary:{games:68,wins:43}, ratingHistory, rankHistory:ratingHistory.map(({at},i)=>({at,rank:12-i})),
    characters:[{character:0,games:68,wins:43}],
    characterRatings:[{character:0,games:68,rating:1801,interval:13,history:ratingHistory}],
    duels:[{duelId,playedAt:Date.parse(date),wins:2,losses:1,durationFrames:32400,replayCount:1,opponent:null}],
  },
  [apiProfile + '/statistics']: {
    career:true, records:true, playtimeSeconds:8400,
    games:[{product:'th07',uploadedAt:date,statistics:workStatistics},
      ...['th06','th08','th09'].map(game => ({
        product:game, uploadedAt:date, statistics:{
          ...workStatistics, game, format:'native-' + game, playtimeSeconds:340, spellCards:undefined,
        },
      }))],
  },
  ['/api/v1/th09/duels/' + duelId + '/replays']: {
    files:[{roundId:'1:1',frames:7200,characters:[0,1]}],
  },
};

const browser = await puppeteer.launch({headless:true,executablePath:await findChromiumExecutable()});
try {
  for (const [width,motion] of [[320,'normal'],[390,'normal'],[390,'system'],[390,'launcher'],[1440,'normal']]) {
    const context = await browser.createBrowserContext();
    try {
      const page = await context.newPage();
      const errors = [];
      let signedIn = true;
      const pausedFixtures = new Map();
      const pauseFixture = path => {
        const {promise, resolve} = Promise.withResolvers();
        pausedFixtures.set(path, promise);
        return () => {pausedFixtures.delete(path); resolve();};
      };
      page.on('pageerror', error => errors.push(error.message));
      const mobile = width <= 390;
      await page.setViewport({width,height:720,isMobile:mobile,hasTouch:mobile,deviceScaleFactor:mobile?2:1});
      await page.emulateMediaFeatures([{name:'prefers-reduced-motion',value:motion === 'system' ? 'reduce' : 'no-preference'}]);
      await page.evaluateOnNewDocument(mode => {
        localStorage.setItem('eagler-touhou-first-use-notice-seen-v1','1');
        if (mode === 'launcher') localStorage.setItem('eagler-touhou-less-motion-v1','1');
      },motion);
      await page.setRequestInterception(true);
      page.on('request', async request => {
        const url = new URL(request.url());
        if (url.origin !== origin.origin || (!url.pathname.startsWith('/api/') && url.pathname !== '/release-catalog.json')) return request.continue();
        const paused = pausedFixtures.get(url.pathname);
        if (paused) await paused;
        const data = url.pathname === '/api/auth/get-session' && !signedIn ? {user:null} : fixtures[url.pathname];
        return request.respond({status:data === undefined?404:200,contentType:'application/json',body:JSON.stringify(data??{})});
      });
      await page.goto(origin.href, {waitUntil:'domcontentloaded'});
      await page.waitForFunction(() => window.__eaglerBoot?.done === true && !document.querySelector('#eaglerPreload'));
      await page.evaluate(() => document.querySelector('#siteNotice:not([hidden]) #siteNoticeClose')?.click());

      const wait = (selector, hidden = false) => page.waitForSelector(selector,{visible:!hidden,hidden,timeout:10000});
      const checkThemeControl = async selector => {
        const group = await wait(selector);
        const layers = await group.evaluate(element => ({
          track: parseFloat(getComputedStyle(element,'::before').height),
          indicator: parseFloat(getComputedStyle(element,'::after').height),
          choices: [...element.querySelectorAll('button')].map(button => ({
            height: button.offsetHeight,
            fill: getComputedStyle(button).backgroundColor,
            label: getComputedStyle(button).fontSize,
            icon: parseFloat(getComputedStyle(button.querySelector('svg')).width),
          })),
        }));
        assert.deepEqual(layers, {
          track:32, indicator:26,
          choices:Array.from({length:2}, () => ({height:44,fill:'rgba(0, 0, 0, 0)',label:'12px',icon:14})),
        },'the 44px targets share one compact track and selection indicator');
      };
      const tap = async selector => {
        const target = await wait(selector);
        await page.evaluate(async () => {
          const finite = document.getAnimations().filter(animation => animation.effect?.getTiming().iterations !== Infinity);
          await Promise.allSettled(finite.map(animation => animation.finished));
        });
        await target.scrollIntoView();
        await page.waitForFunction(element => {
          const {x,y,width,height} = element.getBoundingClientRect();
          const hit = document.elementFromPoint(x + width / 2, y + height / 2);
          return element.contains(hit);
        }, {timeout:6000}, target);
        if (mobile) await target.tap();
        else await target.click();
      };
      const checkSpellMap = async () => {
        const table = await wait('.spell-map');
        assert.equal(await table.evaluate(element => {
          const tiles = [...element.querySelectorAll('button.spell-tile')];
          const widths = tiles.map(tile => tile.getBoundingClientRect().width);
          const bossStarts = [...element.querySelectorAll('tbody td:nth-child(3) .spell-tile:first-child')]
            .map(tile => tile.getBoundingClientRect().left);
          const scroller = element.parentElement;
          return tiles.length > 1 && Math.max(...widths) - Math.min(...widths) < .1 &&
            Math.max(...bossStarts) - Math.min(...bossStarts) < 1 && scroller.scrollWidth <= scroller.clientWidth + 1;
        }), true, 'spell tiles share one width and the Boss column stays aligned');
      };
      if (!mobile) {
        await page.goto(new URL('?game=th07',origin).href,{waitUntil:'domcontentloaded'});
        const desktopSettings = '.book-game-detail-settings';
        const settingsBack = '.book-game-settings-back';
        await wait('.book-game-detail-content > .game-detail-friends');
        const actionRow = await page.$eval('.book-game-detail-hero', hero =>
          ['#launch','#gamePackageImport','#gameSettingsOpen','.game-detail-favorite'].map(selector => {
            const {left, top, height} = hero.querySelector(selector).getBoundingClientRect();
            return {left, top, height};
          }));
        assert.equal(new Set(actionRow.map(item => item.top)).size,1);
        assert.equal(new Set(actionRow.map(item => item.height)).size,1);
        assert.ok(actionRow.every((item,index) => index === 0 || actionRow[index - 1].left < item.left));
        await tap('#gameSettingsOpen');
        await wait(desktopSettings);
        await wait('#launch',true);
        const measureRail = () => page.evaluate(() => {
          const rail = document.querySelector('.book-sidebar').getBoundingClientRect();
          const header = document.querySelector('.book-masthead').getBoundingClientRect();
          const search = document.querySelector('.book-search-slot').getBoundingClientRect();
          return {
            scroll:window.scrollY,
            maxScroll:document.scrollingElement.scrollHeight - innerHeight,
            topGap:rail.top - header.bottom,
            bottomGap:innerHeight - rail.bottom,
            searchOffset:search.top - header.bottom,
          };
        });
        const desktopViewport = page.viewport();
        await page.setViewport({...desktopViewport,height:930});
        const collapsed = await measureRail();
        assert.deepEqual([collapsed.maxScroll,collapsed.topGap,collapsed.bottomGap],[0,0,0],
          'short settings fill the area below the masthead without making an empty scroll range');
        assert.ok(collapsed.searchOffset > 0,'the masthead leaves the sidebar search visible');
        await page.setViewport(desktopViewport);
        await tap('#advancedOptions button.options-group-head');
        const version = await wait('#gameVersionSelect-trigger');
        assert.equal(await version.evaluate(button => {
          const row = button.closest('.settings-choice-row');
          const label = row.querySelector('.ui-field-label').getBoundingClientRect();
          const control = button.getBoundingClientRect();
          return label.right < control.left && label.top < control.bottom && label.bottom > control.top;
        }),true,'wide settings keep the game version control beside its label');
        await page.setViewport({...desktopViewport,height:435});
        await page.evaluate(() => window.scrollTo({top:document.scrollingElement.scrollHeight,behavior:'instant'}));
        const expanded = await measureRail();
        assert.ok(expanded.scroll > 0,'settings can scroll to the bottom in a short window');
        for (const key of ['topGap', 'bottomGap', 'searchOffset']) {
          assert.ok(Math.abs(expanded[key] - collapsed[key]) < 1,
            `the sidebar stays pinned when settings scroll (${key}: ${collapsed[key]} → ${expanded[key]})`);
        }
        await page.setViewport(desktopViewport);
        await tap(settingsBack);
        await wait(desktopSettings,true);
        assert.equal(await page.evaluate(() => document.activeElement?.id),'gameSettingsOpen');
        await tap('#gameSettingsOpen');
        await wait(desktopSettings);
        await page.setViewport({width:390,height:720});
        await wait('.mobile-game-settings');
        await page.setViewport({width,height:720});
        await wait(desktopSettings);
        await page.goBack();
        await wait(desktopSettings,true);
        await wait('#gameSettingsOpen');
        await tap('#gameSettingsOpen');
        await tap('#fileOptions button.options-group-head');
        await tap('[data-action="manage-replay"]');
        await tap('#player .replay-back');
        await wait(desktopSettings);
        await tap(settingsBack);
        await wait(desktopSettings,true);
        await wait('.game-detail-friends-list a[href*="active-friend"]');
        await tap('.game-detail-friends a[href*="social=friends"]');
        await wait('.profile-social-page');
        await tap('.profile-social-back');
        await wait('.book-game-detail-hero');
        await checkSpellMap();
        const spell = await wait('.spell-map button.spell-tile[data-capture="captured"]');
        await spell.scrollIntoView();
        await page.evaluate(async () => {
          await new Promise(resolve => requestAnimationFrame(() => requestAnimationFrame(resolve)));
        });
        await spell.evaluate(button => button.focus({preventScroll:true}));
        assert.equal(await spell.evaluate(button => document.activeElement === button),true,'the spell button receives keyboard focus');
        await wait('.spell-tooltip[data-state$="open"]');
        await page.keyboard.press('Escape');
        await wait('.spell-tooltip[data-state$="open"]',true);
        await tap('.book-game-detail-back');
        await wait('article[data-game-detail]',true);
        await tap('#bookGameGrid .book-game-link[data-game="th07"]');
        await wait('.book-game-detail-back');
        await page.goBack();
        await wait('article[data-game-detail]',true);
        await page.goto(new URL(profile + '?lang=zh-CN',origin).href,{waitUntil:'domcontentloaded'});
        await wait('.profile-th09-history-meta button[aria-expanded="false"]');
        await tap('.profile-actions a[href$="#game-settings"]');
        await wait('.account-settings');
        await tap('.account-settings-nav a[href$="#security"]');
        await wait('.account-settings-nav a[aria-current="page"][href$="#security"]');
        await tap('.account-settings-back');
        await wait('.player-profile');
        assert.equal(new URL(page.url()).hash,'');
      } else {
        const overview = '#mastheadMenuPanel';
        await tap('#mastheadMenuToggle');
        await wait(overview);
        await tap(overview + ' [data-overview-panel="language"]');
        await wait(overview + ' .masthead-overview-subheading');
        await page.goBack();
        await wait(overview + ' .masthead-overview-subheading',true);
        await tap(overview + ' [data-overview-panel="about"]');
        await tap(overview + ' .masthead-overview-subheading button');
        await wait(overview + ' .masthead-overview-subheading',true);
        await wait(overview);
        await tap(overview + ' .masthead-overview-theme');
        await wait('.custom-background-dialog');
        await checkThemeControl('.custom-background-dialog .custom-theme-switch');
        await page.goBack();
        await wait('.custom-background-dialog',true);
        await wait(overview);
        await page.goBack();
        await wait(overview,true);

        const home = '.launcher-bottom-navigation a[href*="section=home"]';
        const homePanel = '.launcher-home';
        const settingsTrigger = homePanel + ' .settings-disclosure:last-child > button';
        const settingsBack = '.account-settings-back';
        await tap(home);
        await tap(homePanel + ' .settings-disclosure:nth-child(2) > button');
        await checkThemeControl(homePanel + ' .custom-theme-switch');
        await tap(homePanel + ' .masthead-overview-theme');
        await wait('.custom-background-dialog');
        const closeBackground = '.custom-background-dialog .ui-kit-dialog-close';
        await tap(closeBackground);
        await wait('.custom-background-dialog',true);
        await wait(homePanel);
        await tap(settingsTrigger);
        await tap(homePanel + ' a[href$="#security"]');
        await wait(settingsBack);
        await tap(settingsBack);
        await wait(homePanel);
        assert.equal(new URL(page.url()).searchParams.get('section'),'home');
        await tap(homePanel + ' a[href$="#security"]');
        await wait(settingsBack);
        await page.goBack();
        await wait(homePanel);
        assert.equal(new URL(page.url()).searchParams.get('section'),'home');
        const shelf = '.launcher-bottom-navigation a:first-child';
        const shelfScroll = '.book-compact-categories .book-categories';
        const game = '#bookGameGrid .book-game-link[data-game="th07"]';
        const gameBack = '.mobile-game-toolbar #libraryBack';
        await tap(shelf);
        await wait('.book-library');
        const shelfKey = await page.evaluate(() => history.state.key);
        const categories = await page.$eval(shelfScroll, rail => {
          const rows = [...rail.querySelectorAll('button')].map(node => Math.round(node.getBoundingClientRect().top));
          return {rows: new Set(rows).size, overflows: rail.scrollWidth > rail.clientWidth,
            scrollable: getComputedStyle(rail).overflowX === 'auto'};
        });
        assert.equal(categories.rows, 1, 'mobile library categories stay on one line');
        assert.ok(!categories.overflows || categories.scrollable, 'extra categories are reachable by horizontal scrolling');
        for (const back of [() => tap(gameBack), () => page.goBack()]) {
          await tap(game);
          await wait(gameBack);
          await back();
          await wait(game);
          assert.equal(await page.evaluate(() => history.state.key), shelfKey,
            'game Back returns to the exact shelf after visiting Home');
        }
        const bookSheet = '.tools[data-book-sheet][data-state="open"]';
        const gameSettings = '.mobile-game-settings #displayOptions';
        assert.equal(await page.$(gameSettings),null,'the library does not mount game settings');
        const resumeStatistics = width === 390 && motion === 'normal' ? pauseFixture('/api/v1/me/cloud-saves') : undefined;
        try {
          await tap(game);
          await wait(bookSheet);
          if (resumeStatistics) {
            await wait('.mobile-game-progress .book-game-stats-message[role="status"]');
            assert.equal(await page.$('.mobile-game-progress .game-detail-friends'),null,
              'friends wait for the space above them to settle');
          }
        } finally { resumeStatistics?.(); }
        const viewport = await page.$eval(bookSheet, panel => ({
          panel: {width:panel.offsetWidth, height:panel.offsetHeight},
          heroWidth:panel.querySelector('.game-detail-hero').offsetWidth,
          page: {width:Math.round(visualViewport.width), height:Math.round(visualViewport.height)},
        }));
        assert.equal(viewport.panel.width, viewport.page.width, 'game detail spans the phone viewport');
        assert.ok(viewport.page.height - viewport.panel.height >= 16, 'the sheet leaves the shelf visible above it');
        assert.equal(viewport.heroWidth,viewport.panel.width,'the detail artwork extends to both phone edges');
        assert.equal(await page.$(gameSettings),null,'the detail does not mount game settings');
        await tap('#gamePackageImport');
        await tap('#gameDataImportClose');
        await wait('#gameDataImportWindow[data-state="open"]',true);
        await wait(bookSheet);
        await tap('#gamePackageImport');
        await wait('#gameDataImportClose');
        await page.goBack();
        await wait('#gameDataImportWindow[data-state="open"]',true);
        await wait(bookSheet);
        await tap('.spell-map button.spell-tile[data-capture="captured"]');
        const hint = await wait('.spell-tooltip[data-state$="open"]');
        assert.equal(await hint.evaluate(node => {
          const [number, name, count] = [...node.children].slice(0, 3).map(child => child.getBoundingClientRect());
          return name.left - number.right >= 7 && count.left - name.right >= 7;
        }), true, 'spell tooltip separates the number, name and count');
        await checkSpellMap();
        await tap('.spell-view-switch button:last-child');
        await wait('.spell-map',true);
        await tap('#gameSettingsOpen');
        await wait(gameSettings);
        assert.equal(await page.$eval(gameSettings, group => {
          const heading = group.querySelector('.options-group-head > span');
          const label = group.querySelector('.settings-field:not([hidden]) .ui-field-label');
          const range = document.createRange();
          range.selectNode(heading.lastChild);
          return Math.abs(range.getBoundingClientRect().left - label.getBoundingClientRect().left) < 1;
        }),true,'nested settings labels align with the section title');
        await tap('#mobileOptionsToggle');
        await tap('#touchLayoutEdit');
        await tap('#touchLayoutExit');
        await wait(gameSettings);
        await wait('#touchLayoutEditor',true);
        await tap('#touchLayoutEdit');
        await wait('#touchLayoutExit');
        await page.goBack();
        await wait('#touchLayoutEditor',true);
        await wait(gameSettings);
        await tap('#fileOptions button.options-group-head');
        await tap('[data-action="manage-replay"]');
        await tap('#player .replay-back');
        await wait(gameSettings);
        await wait('#player .replay-back',true);
        assert.equal(await page.$eval('#fileOptions button.options-group-head', button => button.getAttribute('aria-expanded')),'true');
        await tap('[data-action="manage-replay"]');
        await wait('#player .replay-back');
        await page.goBack();
        await wait('#player .replay-back',true);
        await wait(gameSettings);
        await tap('#libraryBack');
        await wait(gameSettings,true);
        await wait('#gameSettingsOpen');
        await wait('.spell-map',true);
        await tap('#gameSettingsOpen');
        await wait(gameSettings);
        assert.equal(await page.$eval('#fileOptions button.options-group-head', button => button.getAttribute('aria-expanded')),'true',
          'the settings page restores the open save and replay section');
        await page.goBack();
        await wait(gameSettings,true);
        await wait('#gameSettingsOpen');
        await tap('#libraryBack');
        await wait(bookSheet,true);
        await tap(game);
        await wait('#libraryBack');
        await page.goBack();
        await wait(bookSheet,true);
        await wait(game);

        await tap(home);
        const socialLink = '.launcher-home .launcher-social-counts a[href*="social=followers"]';
        await tap(socialLink);
        await wait('.profile-social-page');
        await tap('.profile-social-tabs a[href*="social=following"]');
        await page.goBack();
        await wait(homePanel);
        await tap(socialLink);
        await tap('.profile-social-users a[href*="missing-player"]');
        await wait('.player-profile [role="alert"]');
        await tap('.player-profile .launcher-back');
        await wait('.profile-social-page');
        await tap('.profile-social-back');
        await wait(homePanel);
        await tap('.launcher-home .launcher-account-link');
        await tap('.profile-page-back');
        await wait(homePanel);

        const lobby = '.launcher-bottom-navigation a[href*="lobby"]';
        await tap(lobby);
        await wait('.book-directory');
        await tap('.lobby-help-trigger');
        await tap('.lobby-help-menu [role="menuitem"]:has(.lucide-activity)');
        await wait('#lobbyNetworkDialog[open]');
        await page.goBack();
        await wait('#lobbyNetworkDialog[open]',true);
        await wait('.book-directory');
        await wait('.book-masthead',true);
        await tap('.launcher-bottom-navigation a:first-child');
        await wait('.book-library');
        await tap('.masthead-inbox a[href*="section=announcements"]');
        await wait('.message-center');
        await tap('.message-center nav a[href*="section=messages"]');
        await tap('.message-center .launcher-back');
        await wait('.book-library');
        await tap('#mastheadMenuToggle');
        await tap(overview + ' .launcher-account-link');
        await tap('.profile-page-back');
        await wait('.book-library');
        await wait(overview,true);
      }

      const resumeSocial = width === 390 && motion === 'normal' ? pauseFixture(apiProfile + '/social') : undefined;
      try {
        await page.goto(new URL(profile + '?lang=zh-CN',origin).href,{waitUntil:'domcontentloaded'});
        await wait('.profile-th09-character');
        if (resumeSocial) {
          await wait('#profile-overview .profile-action-buttons');
          const content = await wait('.profile-content');
          const before = await content.evaluate(element => element.getBoundingClientRect().top);
          resumeSocial();
          await wait('.profile-social-summary .launcher-social-counts a');
          const after = await content.evaluate(element => element.getBoundingClientRect().top);
          assert.ok(Math.abs(after-before) < 1,'profile statistics stay in place when social counts arrive');
        }
      } finally { resumeSocial?.(); }
      const plot = await wait('.profile-th09-whr-plot');
      await plot.evaluate(element => element.focus({preventScroll:true}));
      await page.keyboard.press('Home');
      const start = await wait('.profile-th09-whr-tooltip[data-visible]');
      const left = await start.evaluate(element => element.getBoundingClientRect().x);
      await page.keyboard.press('End');
      assert.ok(await start.evaluate(element => element.getBoundingClientRect().x) > left + 100,
        'the rating tooltip moves with the selected point');
      await page.keyboard.press('Escape');
      await wait('.profile-th09-whr-tooltip[data-visible]',true);
      assert.equal(await plot.evaluate(element => {
        const line = element.querySelector('polyline');
        const probe = document.createElement('span');
        probe.style.color = 'var(--kit-brand)';
        element.append(probe);
        const matches = getComputedStyle(line).stroke === getComputedStyle(probe).color;
        probe.remove();
        return matches;
      }), true, 'overall WHR uses the theme accent');
      const comparisonViewport = page.viewport();
      if (mobile) await page.setViewport({...comparisonViewport,height:460});
      const comparisonTrigger = await wait('.profile-th09-rating-toggle');
      const documentScroll = () => page.evaluate(() => window.scrollY);
      await comparisonTrigger.scrollIntoView();
      await comparisonTrigger.evaluate(button => button.addEventListener('click', () => {
        button.dataset.scrollBeforeComparison = String(window.scrollY);
      }));
      const scrollBeforeComparison = () => comparisonTrigger.evaluate(button => Number(button.dataset.scrollBeforeComparison));
      const openComparison = async () => {
        if (mobile) await comparisonTrigger.tap();
        else await comparisonTrigger.click();
        await wait('.profile-th09-comparison-back');
        await wait('.profile-th09-whr-plot polyline[data-muted]');
        if (mobile) {
          await wait('.profile-th09-comparison-page-heading');
          await wait('.profile-page-banner',true);
          await wait('.profile-th09-duels',true);
          assert.ok(await scrollBeforeComparison() > 0,'the profile was scrolled before entering the comparison');
          assert.equal(await documentScroll(),0,'the comparison starts at the top of its own page');
        }
      };
      const verifyComparisonReturn = async () => {
        await wait('.profile-th09-comparison-back',true);
        await wait('.player-profile');
        if (mobile) {
          assert.equal(await documentScroll(),await scrollBeforeComparison(),'returning from the comparison restores the original position');
          assert.equal(await comparisonTrigger.evaluate(button=>document.activeElement === button),true);
        }
      };
      await openComparison();
      await page.goBack();
      await verifyComparisonReturn();
      await openComparison();
      await tap('.profile-th09-comparison-back');
      await verifyComparisonReturn();
      if (mobile) {
        const characterPicker = '#profileTh09Character-trigger';
        await tap(characterPicker);
        const pickerScroll = await documentScroll();
        await tap('[role="option"][data-value="0"]');
        await wait('.profile-th09-comparison-page-heading');
        await page.goBack();
        await wait('.profile-th09-comparison-back',true);
        assert.equal(await documentScroll(),pickerScroll,'the picker returns to the same position');
        assert.equal(await page.evaluate(() => document.activeElement?.id),'profileTh09Character-trigger');
        await page.setViewport(comparisonViewport);
      }

      await tap('.profile-th09-history-meta button');
      await wait('.profile-th09-replays[data-open=true] .th09-replay-row');
      await wait('.profile-th09-replays .th09-replay-actions button');
      assert.equal(new URL(page.url()).pathname,profile,'rounds expand inside the current profile');
      await tap('.profile-th09-history-meta button');
      await wait('.profile-th09-replays[data-open=true]',true);

      const gameDataTab = name => '.profile-game-data-switch button[aria-label="' + name + '"]';
      await page.$eval(gameDataTab('全部作品'), button => button.scrollIntoView({block:'center',behavior:'instant'}));
      await tap(gameDataTab('全部作品'));
      {
        const viewport = page.viewport();
        await page.setViewport({...viewport,height:480});
        await wait('#profile-career .clear-matrix');
        const scrollTo = top => page.evaluate(top => {
          window.scrollTo({top,behavior:'instant'});
          return window.scrollY;
        },top);
        const tab = async (name, view, visible) => {
          await page.$eval(gameDataTab(name), button => button.click());
          await wait('[data-game-data-view="' + view + '"] ' + visible);
          return page.evaluate(() => window.scrollY);
        };
        await page.evaluate(() => {
          window.profileTabFades = 0;
          const animate = Element.prototype.animate;
          Element.prototype.animate = function (...args) {
            if (this.matches('.profile-game-data-body')) window.profileTabFades++;
            return animate.apply(this,args);
          };
        });
        const allTop = await scrollTo(85);
        assert.equal(await tab('花映塚','th09','.profile-th09-duels'), allTop);
        const versusTop = await scrollTo(155);
        assert.deepEqual([allTop,versusTop],[85,155],'both fixtures allow scrolling');
        assert.equal(await tab('全部作品','all','#profile-career'), versusTop);
        assert.equal(await tab('花映塚','th09','.profile-th09-duels'), versusTop);
        assert.equal(await tab('全部作品','all','#profile-career'), versusTop);
        assert.equal(await page.evaluate(() => window.profileTabFades), motion === 'normal' ? 4 : 0);
        await page.setViewport(viewport);
      }
      const overviewMatrix = '#profile-career .clear-matrix[data-loadouts="false"]';
      const work = overviewMatrix + ' tbody button';
      const matrix = await wait(overviewMatrix);
      const viewport = page.viewport();
      await page.setViewport({...viewport,height:490});
      const position = () => page.evaluate(() => ({
        scroll:window.scrollY,
        aside:document.querySelector('.profile-aside').getBoundingClientRect().top,
        heading:document.querySelector('.profile-game-data-heading').getBoundingClientRect().top,
      }));
      const detail = '#profile-career .clear-matrix[data-loadouts="true"]';
      const returnButton = '.profile-game-return';
      const selectWork = await wait(work);
      const innerScroll = await matrix.evaluate(node => {
        const list = node.querySelector('.clear-matrix-scroll');
        list.scrollTop = 12;
        return list.scrollTop;
      });
      assert.ok(innerScroll > 0,'the work list can scroll independently');
      await page.evaluate(() => {
        const heading = document.querySelector('.profile-game-data-heading');
        window.scrollTo({top:window.scrollY + heading.getBoundingClientRect().top - 105,behavior:'instant'});
      });
      const profilePosition = await position();
      assert.ok(profilePosition.scroll > 0,'the profile is scrolled before opening a work');
      const activate = element => mobile ? element.tap() : element.click();
      await selectWork.evaluate(button => {
        window.workSwitchFrames = [];
        button.addEventListener('click',() => {
          const sample = () => {
            window.workSwitchFrames.push({
              scroll:window.scrollY,
              aside:document.querySelector('.profile-aside').getBoundingClientRect().top,
              heading:document.querySelector('.profile-game-data-heading').getBoundingClientRect().top,
            });
            if (window.workSwitchFrames.length < 10) requestAnimationFrame(sample);
          };
          requestAnimationFrame(sample);
        },{once:true});
      });
      await activate(selectWork);
      await wait(detail);
      await page.waitForFunction(() => window.workSwitchFrames.length === 10);
      assert.deepEqual(await page.evaluate(() => window.workSwitchFrames),Array(10).fill(profilePosition),
        'the profile and its sidebar stay in place throughout the detail transition');
      assert.deepEqual(await position(),profilePosition,'opening a short work keeps the profile and its sidebar in place');
      await wait(overviewMatrix,true);
      assert.equal(await page.evaluate(() => document.activeElement?.matches('.profile-game-return')),true);
      await page.goBack();
      await wait(overviewMatrix);
      assert.deepEqual(await position(),profilePosition,'browser Back keeps the profile in place');
      assert.equal(await matrix.$eval('.clear-matrix-scroll',node=>node.scrollTop),innerScroll);
      assert.equal(await selectWork.evaluate(button=>button === document.activeElement),true);
      await activate(selectWork);
      await activate(await wait(returnButton));
      await wait(overviewMatrix);
      assert.deepEqual(await position(),profilePosition,'the inline return keeps the profile in place');
      await page.setViewport(viewport);

      if (!mobile) {
        const restingViewport = page.viewport();
        for (const [path, sidebar, content] of [
          ['/', '.book-sidebar', '.book-content'],
          ['/?game=th07', '.book-sidebar', 'article[data-game-detail]'],
          ['/lobby.html', '.lobby-game-sidebar', '.lobby-room-list'],
        ]) {
          await page.setViewport({...restingViewport,width:path === '/' ? 900 : width,height:390});
          await page.goto(new URL(path,origin).href,{waitUntil:'domcontentloaded'});
          await page.waitForFunction(selector => {
            const nav = document.querySelector(selector + ' nav');
            return nav?.clientHeight > 0 && getComputedStyle(nav).overflowY === 'auto' &&
              !nav.closest('[inert]') && !!nav.querySelector('.book-category-choice:not(:disabled)');
          },{timeout:10000},sidebar);
          const rail = await wait(sidebar);
          const lobby = path === '/lobby.html';
          if (lobby) {
            // Exercise the real lobby scroller without depending on an online relay.
            await page.$eval(content,node => {
              const surface = node.closest('.lobby-list-surface');
              surface.dataset.hasRooms = 'true';
              for (const notice of surface.querySelectorAll('.lobby-connection-warning, .lobby-empty')) notice.hidden = true;
              node.hidden = false;
              const rows = document.createElement('li');
              rows.style.minHeight = '1200px';
              node.append(rows);
            });
          }
          const panel = await wait(content);
          await page.evaluate(() => document.fonts.ready);
          const railPosition = () => rail.evaluate(node => ({
            top:node.getBoundingClientRect().top, list:node.querySelector('nav').scrollTop,
            left:node.getBoundingClientRect().left, bottom:node.getBoundingClientRect().bottom,
          }));
          const before = await railPosition();
          const wheel = async (target, deltaY) => {
            const {x,y} = await target.evaluate(node => {
              const bounds = node.getBoundingClientRect();
              const start = Math.max(bounds.top,70);
              return {x:bounds.left + bounds.width / 2, y:(start + Math.min(bounds.bottom,innerHeight)) / 2};
            });
            await page.mouse.move(x,y);
            await page.mouse.wheel({deltaY});
          };
          const destination = lobby
            ? await panel.evaluateHandle(node => node)
            : await page.evaluateHandle(() => document.scrollingElement);
          const scrollerTop = () => destination.evaluate(node => node.scrollTop);
          const rightTop = await scrollerTop();
          await wheel(panel,185);
          await page.waitForFunction((node,top) => node.scrollTop > top,{timeout:4000},destination,rightTop);
          assert.deepEqual(await railPosition(),before,path + ' right-side scrolling keeps the rail in place');
          const nav = await rail.$('nav');
          assert.ok(await nav.evaluate(node => node.scrollHeight > node.clientHeight),path + ' has enough works for the rail to scroll');
          await wheel(nav,300);
          await page.waitForFunction(node => node.scrollTop > 0,{timeout:4000},nav);
          const held = await scrollerTop();
          await wheel(nav,900);
          await page.waitForFunction(node => node.scrollTop >= node.scrollHeight - node.clientHeight - 1,{timeout:4000},nav);
          await wheel(nav,400);
          await page.evaluate(() => new Promise(resolve => requestAnimationFrame(() => requestAnimationFrame(resolve))));
          assert.equal(await scrollerTop(),held,path + ' wheel over the bottom of the rail stays on the left');
          assert.equal((await railPosition()).top,before.top);
          if (path === '/') {
            const beforeFilter = await railPosition();
            await page.$eval('#librarySearch',input => input.focus({preventScroll:true}));
            await page.keyboard.type('no matching title 64137');
            await wait('#bookGameGrid[data-empty="true"]');
            assert.deepEqual(await railPosition(),beforeFilter,'an empty search leaves the sidebar and its scroll position in place');
          }
          await destination.dispose();
          console.log(`PASS ${path}: independent desktop sidebar scrolling`);
        }
        await page.setViewport(restingViewport);
      }

      if (mobile) {
        signedIn = false;
        await page.goto(origin,{waitUntil:'domcontentloaded'});
        await page.waitForFunction(() => window.__eaglerBoot?.done === true && !document.querySelector('#eaglerPreload'));
        await wait('.book-library');
        await tap('#mastheadMenuToggle');
        await tap('#mastheadMenuPanel .launcher-account-sign-in a:first-child');
        await wait('.account-page');
        await tap('.account-forgot');
        await wait('.account-back');
        assert.equal(await page.$('.account-page .launcher-mobile-back'),null,'password reset has one visible return control');
        await page.goBack();
        await wait('.account-forgot');
        await tap('.account-forgot');
        await tap('.account-back');
        await wait('.account-forgot');
        await tap('.account-page .launcher-mobile-back');
        await wait('.book-library');
        await page.goto(new URL('login.html?mode=reset&lang=zh-CN',origin).href,{waitUntil:'domcontentloaded'});
        await tap('.account-back');
        await wait('.account-forgot');
        assert.equal(new URL(page.url()).searchParams.has('mode'),false,'direct recovery returns to login');
        await tap('.account-page .launcher-mobile-back');
        await wait('.book-library');
        assert.equal(new URL(page.url()).pathname,'/');
      }
      assert.deepEqual(errors,[]);
      console.log(`PASS ${width}px, motion ${motion}: return paths`);
    } finally {await context.close();}
  }
} finally {await browser.close();}
