import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';
import {mkdir, writeFile} from 'node:fs/promises';

const browser = await puppeteer.launch({executablePath: process.env.CHROME_PATH || 'C:/Program Files/Google/Chrome/Application/chrome.exe', headless: true});
const base = process.argv[2] || 'http://127.0.0.1:5173/';
const viewportSizes = process.env.ACCOUNT_TEST_FAST ? [[320, 640]] : [[1440, 900], [1280, 800], [900, 600], [390, 844], [320, 640]];
const selectMode = async (page, mode) => {
  const register = mode === 'register';
  const field = register ? '[name=username]' : '[name=identity]';
  await page.waitForSelector('.account-switch');
  if (!await page.$(field)) await page.click('.account-switch');
  await page.waitForSelector(field);
  await page.evaluate(async () => {
    await new Promise(resolve => requestAnimationFrame(() => requestAnimationFrame(resolve)));
    const content = document.querySelector('.account-mode-content');
    if (content) await Promise.all(content.getAnimations().map(animation => animation.finished.catch(() => {})));
  });
  await page.waitForFunction(() => {
    const content = document.querySelector('.account-mode-content');
    return content && Number(getComputedStyle(content).opacity) >= 0.999 &&
      content.getAnimations().every(animation => animation.playState === 'finished');
  });
};
const settle = page => page.evaluate(() => Promise.all(document.getAnimations().filter(animation => animation.effect?.getTiming().iterations !== Infinity).map(animation => animation.finished.catch(() => {}))));
const clearFocus = page => page.evaluate(() => {if (document.activeElement instanceof HTMLElement) document.activeElement.blur();});
try {
  const screenshotDirectory = process.env.ACCOUNT_SCREENSHOT_DIR || '.cache/account-after';
  await mkdir(screenshotDirectory, {recursive: true});
  const page = await browser.newPage(), errors = [], submissions = [];
  await page.evaluateOnNewDocument(() => {
    try {localStorage.setItem('eagler-touhou-first-use-notice-seen-v1','1');} catch {}
  });
  page.on('pageerror', error => errors.push(error.message));
  let loginAttempts = 0, challengeStarts = 0, verificationAttempts = 0, captchaEnabled = false;
  let sessionUser = null, resetCodeRequests = 0, resetPasswordAttempts = 0, holdResetCodeRequest = false, heldResetCodeRequest = null;
  const resetCodeBodies = [], resetPasswordBodies = [], cancelledRequests = [];
  const avatar = 'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAIAAAACCAIAAAD91JpzAAAAEklEQVR4nGO8pOTCwMDAxAAGAA9YATw80XvmAAAAAElFTkSuQmCC';
  await page.setRequestInterception(true);
  page.on('request', request => {
    const url = new URL(request.url());
    if (url.href === 'https://cv.naids.com/sdk/load.min.js') {
      void request.respond({status:200, contentType:'text/javascript', body:`window.CaptchaSDK = {
        async open({appId}) {
          if (appId !== 'test-app') throw Error('wrong application');
          if (window.cancelCaptcha) throw Object.assign(Error('cancelled'), {code:'CAPTCHA_CANCELLED'});
          return 'one-time-proof';
        }, close() {}, isCancelled(error) {return error.code === 'CAPTCHA_CANCELLED';}
      }`});
      return;
    }
    if (url.pathname === '/api/auth/email-otp/request-password-reset' && holdResetCodeRequest) {
      heldResetCodeRequest = request;
      return;
    }
    if (url.pathname === '/api/ready') {
      void request.respond({status: 200, contentType: 'application/json', body: JSON.stringify({ready: true, captchaAppId: captchaEnabled ? 'test-app' : undefined})});
      return;
    }
    if (url.pathname.startsWith('/api/')) {
      const entry = {path: url.pathname, method: request.method(), body: request.postData()};
      if (request.method() === 'POST') submissions.push(entry);
      let status = 200, body = {};
      if (url.pathname === '/api/auth/get-session') body = {user: sessionUser};
      if (url.pathname === '/api/v1/profiles/reimu') body = {
        username: 'reimu', nickname: '博丽灵梦', joinedAt: '2025-01-01T00:00:00.000Z',
        details: {name: '博丽灵梦', bio: '', region: 'Japan', avatar, cover: '', links: [],
          display: {visible: {friends: true, career: true, records: true, content: true}, order: ['career', 'records', 'content', 'friends', 'favorites']}},
      };
      if (url.pathname === '/api/account/register/start') {
        challengeStarts++;
        body = {challengeId: `challenge-${challengeStarts}`, expiresAt: 1_893_456_000_000};
      }
      if (url.pathname === '/api/account/register/resend')
        body = {expiresAt: 1_893_456_000_000};
      if (url.pathname === '/api/account/register/verify') {
        if (verificationAttempts++ === 0) {status = 410; body = undefined;}
      }
      if (url.pathname === '/api/auth/sign-in/email' && loginAttempts++ === 0) {
        status = 401; body = {code: 'INVALID_EMAIL_OR_PASSWORD', message: 'Raw server error must stay hidden'};
      }
      if (url.pathname === '/api/auth/email-otp/request-password-reset') {
        resetCodeRequests++;
        resetCodeBodies.push(JSON.parse(request.postData() || '{}'));
        if (resetCodeRequests === 1) {status = 503; body = {code: 'SERVICE_UNAVAILABLE'};}
        else body = {success: true};
      }
      if (url.pathname === '/api/auth/email-otp/reset-password') {
        resetPasswordAttempts++;
        resetPasswordBodies.push(JSON.parse(request.postData() || '{}'));
        if (resetPasswordAttempts === 1) {status = 400; body = {code: 'INVALID_OTP'};}
        else body = {success: true};
      }
      if (url.pathname === '/api/auth/sign-out') sessionUser = null;
      void request.respond({status, contentType: 'application/json', body: body === undefined ? '' : JSON.stringify(body)});
      return;
    }
    void request.continue();
  });
  page.on('requestfailed', request => {
    if (new URL(request.url()).pathname.startsWith('/api/')) cancelledRequests.push({path:new URL(request.url()).pathname,error:request.failure()?.errorText});
  });
  const responseFor = (path,status) => page.waitForResponse(response => {
    const url=new URL(response.url());
    return url.pathname===path&&response.status()===status;
  },{timeout:10000});
  await page.setViewport({width: 1440, height: 1000});
  const screenshotAddress = new URL('login.html',base);
  screenshotAddress.searchParams.set('lang','zh-CN');
  const goto = async address => {
    await page.goto(address,{waitUntil:'networkidle2'});
  };
  await page.evaluateOnNewDocument(() => localStorage.setItem('eagler-touhou-first-use-notice-seen-v1','1'));
  for (const width of [1440, 390]) {
    await page.setViewport({width, height:900});
    await page.goto(base, {waitUntil:'domcontentloaded'});
    await page.waitForSelector('.account-login-link', {visible:true});
    if (width > 780) await page.addStyleTag({content:'html[data-ui-route-transition="page"] {--launcher-route-duration: 2s !important}'});
    await page.evaluate(() => {
      window.loginHeader = document.querySelector('.book-masthead');
      window.loginSnapshots = 0;
      const start = document.startViewTransition.bind(document);
      document.startViewTransition = (...args) => {window.loginSnapshots++; return start(...args);};
    });
    await page.click('.account-login-link');
    await page.waitForSelector('.account-art');
    const {sameHeader, headerHeight, artClip, snapshots} = await page.evaluate(() => {
      const header = document.querySelector('.book-masthead');
      return {sameHeader: header === window.loginHeader, headerHeight: header.getBoundingClientRect().height,
        artClip: getComputedStyle(document.querySelector('.account-art')).clipPath, snapshots:window.loginSnapshots};
    });
    assert.ok(sameHeader && headerHeight > 0, `${width}px: the live masthead stays mounted`);
    if (width > 780) assert.equal(parseFloat(artClip.match(/^inset\(([^\s)]+)/)?.[1]), headerHeight, 'artwork snapshot starts below the masthead');
    else assert.equal(snapshots, 0, 'mobile sign-in navigates without a page snapshot');
  }
  await page.setViewport({width:1440, height:1000});
  await goto(screenshotAddress.href);
  await page.evaluate(() => document.fonts.ready);
  const layoutRecords = [];
  const layout = () => page.evaluate(() => {
    const box = selector => document.querySelector(selector)?.getBoundingClientRect().toJSON() ?? null;
    const art = document.querySelector('.account-art'), shell = document.querySelector('.account-shell');
    const content = document.querySelector('.account-content');
    const fixedContainingBlock = (() => {
      for (let node = art.parentElement; node; node = node.parentElement) {
        const style = getComputedStyle(node);
        if (style.transform !== 'none' || style.perspective !== 'none' || style.filter !== 'none' ||
            /(?:layout|paint|strict|content)/.test(style.contain) || /transform|perspective|filter/.test(style.willChange))
          return node.className?.toString() || node.tagName.toLowerCase();
      }
      return null;
    })();
    return {
      viewport: {width: innerWidth, rootWidth: document.documentElement.getBoundingClientRect().width, height: innerHeight},
      header: box('.book-masthead'), page: box('.account-page'), shell: box('.account-shell'),
      art: box('.account-art'), artPosition: getComputedStyle(art).position,
      artDisplay: getComputedStyle(art).display, fixedContainingBlock,
      artworkFit: getComputedStyle(document.querySelector('.account-art-image')).objectFit,
      supportsBackdropFilter: CSS.supports('backdrop-filter','blur(12px)') || CSS.supports('-webkit-backdrop-filter','blur(12px)'),
      panelBackdrop: getComputedStyle(shell).backdropFilter,
      panelBackground: getComputedStyle(shell).backgroundColor,
      heading: box('.account-title') ?? box('#account-title'),
      submit: box('.account-submit'),
      document: {width: Math.max(document.documentElement.scrollWidth, document.body.scrollWidth), height: document.documentElement.scrollHeight},
      content: {overflowY: getComputedStyle(content).overflowY, clientHeight: content.clientHeight, scrollHeight: content.scrollHeight},
    };
  });
  const assertLayout = async (mode, width, height) => {
    const current = await layout();
    assert.ok(current.document.width <= width, `${width}x${height} ${mode}: no horizontal overflow`);
    assert.ok(Math.abs(current.page.left) < 1 && Math.abs(current.page.right-current.viewport.rootWidth) < 1,
      `${width}x${height} ${mode}: account page fills the content viewport`);
    assert.ok(Math.abs(current.page.top-current.header.bottom) < 1, `${width}x${height} ${mode}: account page starts at the masthead bottom`);
    assert.ok(Math.abs(current.shell.right-current.page.right) < 1, `${width}x${height} ${mode}: panel is flush with the right edge`);
    if (width > 780) {
      const expectedPanelWidth=Math.min(720,Math.max(480,current.viewport.rootWidth*.44));
      assert.ok(Math.abs(current.shell.width-expectedPanelWidth)<1,`${width}x${height} ${mode}: desktop panel follows the 480px to 720px width range`);
      assert.ok(Math.abs(current.shell.top-current.page.top)<1,`${width}x${height} ${mode}: desktop panel begins below the masthead`);
      assert.ok(current.shell.height>=height-current.header.bottom-1&&current.shell.bottom>=height-1,
        `${width}x${height} ${mode}: desktop panel fills the remaining viewport height`);
    } else {
      assert.ok(Math.abs(current.shell.left-current.page.left)<1&&Math.abs(current.shell.width-current.page.width)<1,
        `${width}x${height} ${mode}: mobile panel spans the available page width`);
    }
    assert.equal(current.content.overflowY, 'visible', `${width}x${height} ${mode}: form uses document scrolling`);
    assert.ok(current.content.scrollHeight <= current.content.clientHeight + 1, `${width}x${height} ${mode}: content has no nested scroller`);
    assert.equal(current.artPosition, 'fixed', `${width}x${height} ${mode}: background stays fixed to the viewport`);
    assert.equal(current.artDisplay, 'block', `${width}x${height} ${mode}: artwork remains visible on every screen size`);
    assert.equal(current.fixedContainingBlock, null, `${width}x${height} ${mode}: no ancestor traps the fixed background`);
    assert.ok(Math.abs(current.art.left) < 1 && Math.abs(current.art.top) < 1 &&
      Math.abs(current.art.right-current.viewport.rootWidth) < 1 && Math.abs(current.art.bottom-height) < 1,
      `${width}x${height} ${mode}: background covers the full viewport`);
    assert.equal(current.artworkFit, 'cover', `${width}x${height} ${mode}: background fills the viewport`);
    if (current.supportsBackdropFilter) {
      assert.match(current.panelBackdrop,/12px/,`${width}x${height} ${mode}: glass panel applies the requested blur`);
      assert.match(current.panelBackground,/rgba|color\(/,`${width}x${height} ${mode}: glass panel surface remains translucent`);
    }
    if (width < 960) {
      const fontSizes=await page.$$eval('.account-content input:not([type="hidden"])',elements=>elements.map(element=>getComputedStyle(element).fontSize));
      assert.ok(fontSizes.length>0&&fontSizes.every(size=>size==='16px'),`${width}x${height} ${mode}: all text inputs avoid mobile zoom (${fontSizes.join(', ')})`);
    }
    await page.evaluate(() => window.scrollTo(0, document.documentElement.scrollHeight));
    const bottom = await page.$eval('.account-submit', element => {
      const rect = element.getBoundingClientRect();
      return {top:rect.top,bottom:rect.bottom};
    });
    assert.ok(bottom.top >= -1 && bottom.bottom <= height + 1, `${width}x${height} ${mode}: primary action is reachable by document scrolling`);
    await page.evaluate(() => window.scrollTo(0, 0));
    return current;
  };
  for (const [width,height] of viewportSizes) {
    await page.setViewport({width,height});
    for (const theme of ['dark', 'light']) {
      await page.evaluate(value => document.documentElement.setAttribute('data-book-theme', value), theme);
      await settle(page);
      await selectMode(page, 'login');
      const login = await assertLayout('login',width,height);
      const headingAnchor = login.heading.top - login.shell.top;
      const artSignature = await page.$eval('.account-art-image', image => ({
        src: image.currentSrc, fit: getComputedStyle(image).objectFit,
        position: getComputedStyle(image).objectPosition, filter: getComputedStyle(image).filter,
      }));
      await clearFocus(page);
      await page.screenshot({path: `${screenshotDirectory}/${width}x${height}-login-${theme}.png`,fullPage:false});
      await selectMode(page, 'register'); await settle(page);
      const registrationArt = await page.$eval('.account-art-image', image => ({
        src: image.currentSrc, fit: getComputedStyle(image).objectFit,
        position: getComputedStyle(image).objectPosition, filter: getComputedStyle(image).filter,
      }));
      assert.deepEqual(registrationArt,artSignature,`${width}x${height}: stable artwork source and presentation`);
      const registration = await assertLayout('register',width,height);
      assert.ok(Math.abs((registration.heading.top-registration.shell.top)-headingAnchor) <= 4, `${width}x${height}: title stays anchored when switching account mode`);
      await clearFocus(page);
      await page.screenshot({path: `${screenshotDirectory}/${width}x${height}-register-${theme}.png`,fullPage:false});
      layoutRecords.push({width,height,theme,login,register:registration});
    }
  }
  await writeFile(`${screenshotDirectory}/layout.json`,JSON.stringify(layoutRecords,null,2));
  for (const locale of ['en','ja','zh-CN']) {
    const address = new URL('login.html',base);
    address.searchParams.set('lang',locale);
    await goto(address.href);
    await page.waitForSelector('.account-page');
    for (const [width,height] of viewportSizes) {
      await page.setViewport({width,height});
      for (const theme of locale==='zh-CN'?['dark','light']:['dark']) {
        await page.evaluate(value=>document.documentElement.setAttribute('data-book-theme',value),theme);
        await settle(page);
        for (const mode of ['login','register']) {
          await selectMode(page,mode);
          await assertLayout(`${mode}/${locale}/${theme}`,width,height);
          const title = await page.$eval('#account-title',element=>({text:element.textContent,clipped:element.scrollWidth>element.clientWidth+1}));
          assert.equal(title.clipped,false,`${width}x${height} ${locale}: title text is not clipped`);
          assert.ok(title.text.trim(),`${width}x${height} ${locale}: translated title is present`);
          if(locale==='zh-CN'&&[1440,390].includes(width)) {await clearFocus(page); await page.screenshot({path:`${screenshotDirectory}/${width}x${height}-${mode}-zh-CN-${theme}.png`,fullPage:false});}
        }
      }
    }
  }
  if (process.env.ACCOUNT_LAYOUT_ONLY === '1') {
    console.log('PASS: account layout matrix, locale titles and full-viewport background');
    await browser.close();
    process.exit(0);
  }
  const resetAddress = new URL('login.html',base);
  resetAddress.searchParams.set('lang','ja');
  resetAddress.searchParams.set('next','/profile.html#settings');
  await goto(resetAddress.href);
  await page.click('.account-forgot');
  await page.waitForSelector('.account-back');
  let currentUrl = new URL(page.url());
  assert.equal(currentUrl.searchParams.get('mode'),'reset');
  assert.equal(currentUrl.searchParams.get('lang'),'ja');
  assert.equal(currentUrl.searchParams.get('next'),'/profile.html#settings');
  await page.type('[name=email]','player@example.com');
  const firstCodeFailure=responseFor('/api/auth/email-otp/request-password-reset',503);
  await page.click('.account-submit');
  await firstCodeFailure;
  await page.waitForSelector('.account-page [role=alert]');
  assert.equal(resetCodeRequests,1,'a failed verification-code request is recorded once');
  assert.ok((await page.$eval('.account-page [role=alert]',element=>element.textContent)).trim());
  const codeRetry=responseFor('/api/auth/email-otp/request-password-reset',200);
  await page.click('.account-submit');
  await codeRetry;
  await page.waitForSelector('[name=code]');
  assert.equal(resetCodeRequests,2,'the visitor can retry a failed code request');
  assert.deepEqual(resetCodeBodies,[{email:'player@example.com'},{email:'player@example.com'}]);
  assert.equal(await page.$eval('.account-code button',element=>element.disabled),true,'the resend action observes its 60-second cooldown');
  assert.equal(await page.evaluate(()=>document.activeElement?.getAttribute('name')),'code','the code field receives focus after dispatch');
  await page.type('[name=code]','111111');
  await page.type('[name=password]','Tengu-password-42');
  await page.type('[name=confirmation]','different-password');
  await page.click('.account-submit');
  await page.waitForSelector('.account-page [role=alert]');
  assert.equal(await page.$eval('.account-page [role=alert]',element=>element.textContent),'パスワードが一致しません。');
  assert.equal(resetPasswordAttempts,0,'a mismatched password is rejected before an API request');
  await page.$eval('[name=confirmation]',element=>element.select());
  await page.type('[name=confirmation]','Tengu-password-42');
  const invalidOtpResponse=responseFor('/api/auth/email-otp/reset-password',400);
  await page.click('.account-submit');
  await invalidOtpResponse;
  await page.waitForFunction(()=>document.querySelector('.account-page [role=alert]')?.textContent==='確認コードが無効か、有効期限が切れています。');
  assert.equal(resetPasswordAttempts,1,'an invalid code response stays visible for correction');
  await page.$eval('[name=code]',element=>element.select());
  await page.type('[name=code]','222222');
  const resetSuccess=responseFor('/api/auth/email-otp/reset-password',200);
  await page.click('.account-submit');
  await resetSuccess;
  await page.waitForSelector('.account-reset-success [role=status]');
  assert.equal(resetPasswordAttempts,2,'reset can be retried after a server error');
  assert.deepEqual(resetPasswordBodies,[
    {email:'player@example.com',otp:'111111',password:'Tengu-password-42'},
    {email:'player@example.com',otp:'222222',password:'Tengu-password-42'},
  ]);
  assert.equal(page.url().includes('mode=reset'),true,'successful reset waits for an explicit return to sign in');
  assert.equal(submissions.filter(request=>request.path==='/api/auth/sign-in/email').length,0,'reset never signs in automatically');
  await page.click('.account-reset-success .account-submit');
  await page.waitForSelector('[name=identity]');
  currentUrl = new URL(page.url());
  assert.equal(currentUrl.pathname,'/login.html');
  assert.equal(currentUrl.searchParams.get('mode'),null);
  assert.equal(currentUrl.searchParams.get('lang'),'ja','returning to login preserves locale');
  assert.equal(currentUrl.searchParams.get('next'),'/profile.html#settings','returning to login preserves destination');
  assert.equal(await page.evaluate(password=>JSON.stringify({...localStorage,...sessionStorage}).includes(password),'Tengu-password-42'),false,
    'reset credentials are not persisted');

  await page.click('.account-forgot');
  await page.waitForSelector('.account-back');
  await page.type('[name=email]','cancel@example.com');
  holdResetCodeRequest=true;
  await page.click('.account-submit');
  const waitFor = async (predicate,label) => {
    const deadline=Date.now()+5000;
    while(Date.now()<deadline){if(predicate())return;await new Promise(resolve=>setTimeout(resolve,20));}
    throw Error(`Timed out waiting for ${label}`);
  };
  await waitFor(()=>heldResetCodeRequest!==null,'pending reset-code request');
  await page.click('.account-back');
  await page.waitForSelector('.account-forgot');
  await waitFor(()=>cancelledRequests.some(request=>request.path==='/api/auth/email-otp/request-password-reset'),'aborted reset-code request');
  holdResetCodeRequest=false;
  for (const [width,height] of viewportSizes) {
    const address = new URL('login.html',base);
    address.searchParams.set('lang','zh-CN');
    address.searchParams.set('mode','reset');
    await page.setViewport({width,height});
    for (const theme of ['dark','light']) {
      await goto(address.href);
      await page.evaluate(value=>document.documentElement.setAttribute('data-book-theme',value),theme);
      await settle(page);
      await assertLayout('reset/request/zh-CN',width,height);
      if ([1440,390].includes(width)) await page.screenshot({path:`${screenshotDirectory}/${width}x${height}-reset-request-zh-CN-${theme}.png`,fullPage:false});
      await page.type('[name=email]','player@example.com');
      const codeResponse=responseFor('/api/auth/email-otp/request-password-reset',200);
      await page.click('.account-submit');
      try {await codeResponse;} catch(cause) {console.error('reset-code response timeout', {width,height,theme,url:page.url(),email:await page.$eval('[name=email]',element=>element.value),alert:await page.$eval('.account-page',element=>element.innerText)});throw cause;}
      await page.waitForSelector('[name=code]');
      const resetLayout=await assertLayout('reset/code/zh-CN',width,height);
      assert.ok(resetLayout.heading.width>0,`${width}x${height}: reset heading is visible`);
      if ([1440,390].includes(width)) await page.screenshot({path:`${screenshotDirectory}/${width}x${height}-reset-code-zh-CN-${theme}.png`,fullPage:true});
    }
  }
  await page.setViewport({width:1440,height:900});
  await goto(new URL('login.html',base).href);
  await selectMode(page,'register');

  const registrationPassword = 'Tengu-password-42';
  await page.type('[name=username]', 'reimu');
  await page.type('[name=nickname]', '博丽灵梦');
  await page.type('[name=email]', 'player@example.com');
  await page.type('[name=password]', registrationPassword);
  await page.type('[name=confirmation]', 'wrong-password');
  await page.click('.account-submit');
  assert.match(await page.$eval('.account-page [role=alert]', el => el.textContent), /不一致/);
  assert.equal(submissions.filter(request=>request.path==='/api/account/register/start').length, 0, 'invalid registration draft is not sent');
  await page.$eval('[name=confirmation]', el => {el.select();});
  await page.type('[name=confirmation]', registrationPassword);
  captchaEnabled = true;
  await page.evaluate(() => {window.cancelCaptcha = true;});
  await page.click('.account-submit');
  await page.waitForFunction(() => window.CaptchaSDK && !document.querySelector('.account-submit').disabled);
  assert.equal(challengeStarts, 0, 'closing the slider sends no email request');
  await page.evaluate(() => {window.cancelCaptcha = false;});
  await page.click('.account-submit');
  await page.waitForSelector('.account-page [role=status]');
  assert.match(await page.$eval('.account-page [role=status]', el => el.textContent), /验证码已发送/);
  assert.equal(await page.$eval('.account-code button', el => el.disabled), true);
  for (const name of ['username', 'nickname', 'email', 'password', 'confirmation'])
    assert.equal(await page.$eval(`[name=${name}]`, el => el.disabled), true, `${name} is locked after the OTP request`);
  await selectMode(page, 'login');
  await selectMode(page, 'register');
  assert.equal(await page.$eval('[name=username]', el => el.value), 'reimu', 'registration draft survives mode changes');
  assert.equal(await page.$eval('[name=email]', el => el.value), 'player@example.com');
  assert.equal(await page.$eval('[name=username]', el => el.disabled), true, 'registration draft remains locked');
  await page.type('[name=code]', '123456');
  await page.click('.account-submit');
  await page.waitForSelector('.account-page [role=status]');
  assert.match(await page.$eval('.account-page [role=status]', el => el.textContent), /已过期/);
  assert.equal(await page.$eval('[name=email]', el => el.disabled), true, 'an expired challenge stays bound until the visitor restarts it');
  await page.click('.account-edit-details');
  assert.equal(await page.$eval('[name=email]', el => el.disabled), false, 'editing is available after explicitly abandoning the expired challenge');
  await page.$eval('[name=email]', el => {el.select();});
  await page.type('[name=email]', 'player2@example.com');
  await page.click('.account-submit');
  await page.waitForSelector('.account-page [role=status]');
  assert.match(await page.$eval('.account-page [role=status]', el => el.textContent), /验证码已发送/);
  await page.type('[name=code]', '123456');
  await page.click('.account-submit');
  await page.waitForFunction(() => location.pathname === '/profile.html' || (location.pathname === '/login.html' && new URLSearchParams(location.search).has('next')));
  const signUps = submissions.filter(request => request.path === '/api/account/register/start');
  assert.equal(signUps.length, 2);
  const signUp = signUps[0];
  assert.deepEqual(JSON.parse(signUp.body), {username: 'reimu', name: '博丽灵梦', email: 'player@example.com', password: registrationPassword, captchaToken:'one-time-proof'});
  assert.deepEqual(JSON.parse(signUps[1].body), {username: 'reimu', name: '博丽灵梦', email: 'player2@example.com', password: registrationPassword, captchaToken:'one-time-proof'});
  const verifications = submissions.filter(request => request.path === '/api/account/register/verify');
  assert.deepEqual(JSON.parse(verifications[0].body), {challengeId: 'challenge-1', email: 'player@example.com', otp: '123456', password: registrationPassword});
  assert.deepEqual(JSON.parse(verifications[1].body), {challengeId: 'challenge-2', email: 'player2@example.com', otp: '123456', password: registrationPassword});
  assert.equal(page.url().includes(registrationPassword), false, 'credentials never enter the URL');
  await goto(new URL('/profile.html', base).href);
  await page.waitForFunction(() => location.pathname === '/login.html' && new URLSearchParams(location.search).get('next') === '/profile.html');
  await page.waitForSelector('.account-page');
  await goto(new URL('login.html', base).href);
  await page.type('[name=identity]', 'player2@example.com'); await page.type('[name=password]', 'incorrect-password');
  await page.click('.account-password-toggle');
  assert.equal(await page.$eval('[name=password]', el => el.type), 'text');
  await page.click('.account-submit');
  await page.waitForSelector('.account-page [role=alert]');
  const loginError = await page.$eval('.account-page [role=alert]', el => el.textContent);
  assert.match(loginError, /用户名、邮箱或密码不正确/);
  assert.equal(loginError.includes('Raw server error'), false, 'backend text is not shown to users');
  await page.$eval('[name=password]', el => {el.select();});
  await page.type('[name=password]', registrationPassword);
  await page.click('.account-submit');
  await page.waitForFunction(() => location.pathname === '/profile.html' || (location.pathname === '/login.html' && new URLSearchParams(location.search).has('next')));
  assert.equal(await page.evaluate(password => JSON.stringify({...localStorage, ...sessionStorage}).includes(password), registrationPassword), false);
  const loginRequest = submissions.find(request => request.path === '/api/auth/sign-in/email');
  assert.deepEqual(JSON.parse(loginRequest.body), {email: 'player2@example.com', password: 'incorrect-password'});
  assert.equal(submissions.some(request => new URL(request.path, base).search.includes('password')), false);
  sessionUser = {username: 'reimu', name: '博丽灵梦'};
  await goto(new URL('/u/reimu', base).href);
  await page.waitForSelector('.profile-page');
  await page.waitForFunction(() => document.querySelector('.masthead-account-entry .account-avatar img')?.naturalWidth > 0);
  assert.equal(await page.$eval('.profile-avatar img', image => image.naturalWidth > 0), true, 'uploaded avatar appears on the profile');
  const memberTrigger = await page.$eval('.masthead-account-entry', el => ({
    label: el.getAttribute('aria-label'), text: el.textContent.trim(), avatarWidth: el.querySelector('.account-avatar').getBoundingClientRect().width,
  }));
  assert.match(memberTrigger.label, /博丽灵梦/, 'the member avatar has the account name as its accessible label');
  assert.equal(memberTrigger.text, '', 'the masthead shows only the member avatar');
  assert.equal(memberTrigger.avatarWidth, 28);
  assert.equal(await page.$('.profile-section-nav a[href*="profile-content"]'), null, 'removed profile content tab stays hidden');
  await page.setViewport({width:320,height:640});
  const compactMenuTrigger = await page.$eval('.masthead-account-entry',element=>{
    const rect=element.getBoundingClientRect(),style=getComputedStyle(element);
    return {left:rect.left,right:rect.right,top:rect.top,bottom:rect.bottom,display:style.display,visibility:style.visibility,overflow:document.documentElement.scrollWidth>innerWidth};
  });
  assert.equal(compactMenuTrigger.visibility,'visible','320px account menu trigger remains visible');
  assert.equal(compactMenuTrigger.overflow,false,'320px profile header has no horizontal overflow');
  await page.screenshot({path:`${screenshotDirectory}/320x640-profile-menu-closed.png`,fullPage:false});
  await page.click('.masthead-account-entry');
  await page.waitForSelector('.account-menu-panel',{timeout:5000});
  const compactMenu = await page.$eval('.account-menu-panel',element=>{
    const rect=element.getBoundingClientRect();
    return {left:rect.left,right:rect.right,top:rect.top,bottom:rect.bottom,width:rect.width,height:rect.height,overflowY:getComputedStyle(element).overflowY};
  });
  assert.ok(compactMenu.left>=0&&compactMenu.right<=320,`320px account menu fits viewport: ${JSON.stringify(compactMenu)}`);
  assert.ok(compactMenu.top>=0&&compactMenu.bottom<=640,`320x640 account menu is reachable: ${JSON.stringify(compactMenu)}`);
  await page.screenshot({path:`${screenshotDirectory}/320x640-profile-menu-open.png`,fullPage:false});
  await page.keyboard.press('Escape');
  await page.waitForSelector('.account-menu-panel',{hidden:true,timeout:5000});
  await page.setViewport({width:1440,height:900});
  await page.click('.masthead-account-entry');
  await page.waitForSelector('.account-menu-panel');
  assert.equal(await page.$eval('.account-menu-identity img', image => image.naturalWidth > 0), true, 'uploaded avatar appears in the account menu');
  assert.equal(await page.$eval('.account-menu-identity strong', el => el.textContent), '博丽灵梦', 'the menu shows the member name');
  const profileLink = await page.$eval('.account-menu-panel a', link => link.getAttribute('href'));
  assert.match(profileLink, /\/u\/reimu/);
  await page.click('.account-menu-panel a[href$="#edit"]');
  await page.waitForFunction(() => location.pathname === '/u/reimu' && location.hash === '#edit');
  await page.waitForSelector('#profile-settings-title');
  await goto(new URL('/u/reimu', base).href);
  await page.waitForSelector('.profile-page');
  await page.click('.masthead-account-entry');
  await page.waitForSelector('.account-menu-panel');
  await page.click('.account-menu-panel a[href$="#settings"]');
  await page.waitForFunction(() => location.pathname === '/u/reimu' && location.hash === '#settings');
  await page.waitForSelector('#security-settings-title');
  await goto(new URL('/login.html?next=%2Fu%2Freimu%23settings', base).href);
  await page.waitForFunction(() => location.pathname === '/u/reimu' && location.hash === '#settings');
  await page.waitForSelector('#security-settings-title');
  await page.click('.masthead-account-entry');
  await page.waitForSelector('.account-menu-panel');
  const menuItems = await page.$$('.account-menu-panel [role="menuitem"]');
  await menuItems.at(-1).click();
  await page.waitForFunction(() => location.pathname === '/');
  sessionUser = null;
  await page.waitForSelector('.account-login-link');
  await page.setViewport({width: 1440, height: 900});
  assert.ok(await page.$('.account-login-link > .book-navigation-icon'), 'guest header uses the navigation icon');
  assert.ok(await page.$eval('.account-login-link', link => link.textContent.trim() === link.getAttribute('aria-label')), 'desktop login entry displays its label');
  assert.equal(await page.$('.masthead-links a[href*="mode=register"]'), null);
  await page.setViewport({width: 320, height: 720});
  const compactHeader = await page.evaluate(() => {
    const brand = document.querySelector('.book-brand'), login = document.querySelector('.account-login-link');
    const drawer = document.querySelector('#mastheadMenuToggle'), loginRect = login.getBoundingClientRect(), drawerRect = drawer.getBoundingClientRect();
    const loginIcon = login.querySelector('.book-navigation-icon'), drawerIcon = drawer.querySelector('.book-navigation-icon');
    const actions = [...document.querySelectorAll('.masthead-links > .masthead-action')].filter(node => node.getClientRects().length);
    return {overflow: document.documentElement.scrollWidth > innerWidth, brandRight: brand.getBoundingClientRect().right,
      accountLeft: loginRect.left, loginLabel: login.querySelector('span').getBoundingClientRect().width > 0 && login.textContent.trim(), actions: actions.length,
      loginIconLarger: loginIcon.getBoundingClientRect().width > drawerIcon.getBoundingClientRect().width &&
        getComputedStyle(loginIcon).strokeWidth === getComputedStyle(drawerIcon).strokeWidth,
      loginTargetSize: loginRect.height === drawerRect.height && loginRect.width >= 44};
  });
  assert.equal(compactHeader.overflow, false, '320px header has no horizontal overflow');
  assert.ok(compactHeader.accountLeft - compactHeader.brandRight >= 8, `320px brand and account controls have a gutter: ${JSON.stringify(compactHeader)}`);
  assert.equal(compactHeader.actions, 2, 'the narrow masthead shows the account and overview triggers');
  assert.ok(compactHeader.loginLabel, 'the narrow login link displays its label');
  assert.equal(compactHeader.loginIconLarger, true, 'the guest icon is slightly larger and shares the drawer icon stroke weight');
  assert.equal(compactHeader.loginTargetSize, true, 'narrow account icon keeps a 44px target');
  await page.click('#mastheadMenuToggle');
  await page.waitForSelector('#mastheadMenuPanel[role="dialog"][data-state="open"]');
  await page.waitForFunction(() => Math.abs(document.querySelector('#mastheadMenuPanel').getBoundingClientRect().right - innerWidth) < 0.5);
  const theme = await page.$eval('html', node => node.dataset.bookTheme);
  await page.click('.masthead-overview-theme');
  await page.waitForFunction(previous => document.documentElement.dataset.bookTheme !== previous, {}, theme);
  assert.ok(await page.$('#mastheadMenuPanel a[href*="faq.html"]'), 'help is available inside the overview');
  await page.keyboard.press('Escape');
  await page.waitForSelector('#mastheadMenuPanel', {hidden: true});
  assert.deepEqual(errors, []);
  console.log('PASS: aligned viewport layout, stable artwork, immutable OTP challenge, expiry recovery, localized login errors, profile navigation and no credential persistence');
} finally {await browser.close();}
