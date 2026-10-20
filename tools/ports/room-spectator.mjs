import assert from 'node:assert/strict';
import {resolve} from 'node:path';
import {readRoomWorldFrame} from './room-lifecycle.mjs';
import {attachRoomProtocolCapture} from './room-protocol-capture.mjs';

export function assertPlayerObservationSeats(states){
  assert.ok(states.length>=2,'Spectator isolation requires every player');
  for(const [seat,state] of states.entries())
    assert.equal(state?.observation?.localSeat,seat,`P${seat+1}: native input observation belongs to another seat`);
}

export async function admitRoomSpectator({browser,base,game,room,route,packages,record,directory,
  dismiss,waitForRoomEntry,waitForRoomResources}) {
  const context = await browser.newContext({viewport: {width: 1280, height: 900}});
  const page = await context.newPage();
  const protocolCapture=await attachRoomProtocolCapture(page,{game});
  const result = {checks: [], errors: [], consoleErrors: [], coverage: {ids: ['MP-06'], complete: false,
    scope: 'Pre-start admission through real room controls, native playback progress and physical keyboard isolation'}};
  record.spectator = result;
  try {
    if (route === 'relay') await context.addInitScript(() => Object.defineProperty(window, 'RTCPeerConnection', {value: undefined, configurable: true}));
    page.on('pageerror', error => result.errors.push(String(error)));
    page.on('crash',()=>result.errors.push('Spectator page crashed'));
    page.on('console', message => {if (message.type() === 'error') result.consoleErrors.push(message.text());});
    page.on('response',response=>{if(response.status()>=400)
      record.httpErrors.push({seat:'spectator',url:response.url(),status:response.status()});});
    page.on('requestfailed',request=>record.requestFailures.push({seat:'spectator',url:request.url(),failure:request.failure()?.errorText??'request failed'}));
    await page.goto(base); await page.waitForFunction(() => window.__eaglerBoot?.done === true); await dismiss(page);
    await page.locator(`[data-product="${game}mp"]`).click();
    await page.locator('#codeButton').click();
    await page.locator('#gameSelect').selectOption(game + 'mp');
    await page.locator('#roomCodeInput').fill(room);
    await page.locator('#submitRoom').click();
    result.roomEntry = await waitForRoomEntry(page, 0);
    if (await page.locator('#mpMusicSelect').count()) await page.locator('#mpMusicSelect').selectOption('none',{force:true});
    if (record.resourcePreparation.source === 'offline-package') {
      await page.locator('#mpSettingsRoomDrawerToggle').click();
      await page.locator('#mpGamePackageImport').click();
      await page.locator('#gameDataImportWindow').waitFor({state: 'visible'});
      const chooser = page.waitForEvent('filechooser'); await page.locator('#transferImport').click();
      await (await chooser).setFiles(packages[game]);
      await page.locator('#gameDataImportWindow').waitFor({state: 'hidden', timeout: 120000});
      await page.locator('#mpGamePackageImport').focus(); await page.keyboard.press('Escape');
      await page.locator('#mpSettingsRoomDrawer').waitFor({state: 'hidden'});
    }
    await page.locator('#mpSpectatorToggle').click();
    await page.locator('#mpSpectatorJoin').click();
    await page.waitForFunction(() => document.querySelectorAll('#mpSpectatorList .mp-spectator-entry.mine').length === 1);
    await page.locator('#mpRoomPanelClose').click();
    result.checks.push({id: 'server-acknowledged-spectator-admission', passed: true, room});
    // Spectator resource preparation is activated by its admitted role. Its
    // eventual real first frame establishes successful loading at launch.
    result.resources=await waitForRoomResources(page,record,0,{requireReadyControl:false,label:'spectator'});
    await page.screenshot({path: resolve(directory, game + '-spectator-admitted.png')});
    return {page, context, result, protocolCapture};
  } catch (error) {await context.close(); throw error;}
}

export async function exerciseRoomSpectator({viewer,game,pages,snapshot,step,directory,runtimeIdentity}) {
  const {page,result} = viewer;
  const read = async (page,seat) => {const {events,...state}=await snapshot(page,game,seat);return state;};
  await step('native-spectator-progress-and-keyboard-isolation', async () => {
    const deadline = Date.now() + 120000;
    let state;
    do {
      state = await read(page,0);
      result.lastNativeObservation=state;
      if (state?.options?.spectator === true && Number.isSafeInteger(readRoomWorldFrame(state)) && readRoomWorldFrame(state)>=60 &&
        ['stage','match'].includes(state?.observation?.phase?.name)) break;
      if (state?.failed || state?.error) throw Error('Native spectator failed: ' + JSON.stringify(state));
      await page.waitForTimeout(100);
    } while (Date.now() < deadline);
    assert.ok(state?.options?.spectator === true && Number.isSafeInteger(readRoomWorldFrame(state)) && readRoomWorldFrame(state)>=60 &&
      ['stage','match'].includes(state?.observation?.phase?.name),
      'Admitted spectator must play confirmed native history: ' + JSON.stringify(state));
    assert.equal(new URL(state.runtimeUrl).pathname,new URL(runtimeIdentity.entryUrl).pathname,
      'Spectator launched a Runtime other than the verified current MP package');
    assert.equal(state.options.players,pages.length,'Spectator must use the room player count');
    let advancing;
    const advanceDeadline=Date.now()+10000;
    do {
      await page.waitForTimeout(100); advancing=await read(page,0); result.lastNativeObservation=advancing;
      if (readRoomWorldFrame(advancing)>readRoomWorldFrame(state)) break;
    } while(Date.now()<advanceDeadline);
    assert.ok(readRoomWorldFrame(advancing)>readRoomWorldFrame(state),'Spectator native world did not apply further history');
    result.checks.push({id: 'native-spectator-playback',passed:true,before:state,after:advancing});
    const before = await Promise.all(pages.map((player,seat)=>read(player,seat)));
    assertPlayerObservationSeats(before);
    assert.ok(before.every(state=>['stage','match'].includes(state?.observation?.phase?.name) && state?.observation?.input?.value === 0),
      'Player worlds must be playable with neutral input before spectator input isolation');
    await page.bringToFront(); await page.frameLocator('#gameFrame').locator('canvas').first().focus();
    const keys = ['ArrowRight','KeyZ','ShiftLeft'];
    const samples = [];
    try {
      for (const key of keys) await page.keyboard.down(key);
      for (let index=0;index<10;index++) {
        samples.push(await Promise.all(pages.map((player,seat)=>read(player,seat))));
        await page.waitForTimeout(50);
      }
    } finally {for (const key of keys) await page.keyboard.up(key);}
    const after = await Promise.all(pages.map((player,seat)=>read(player,seat)));
    for (const states of [...samples,after]) {
      assertPlayerObservationSeats(states);
      for (const [seat,state] of states.entries())
        assert.equal(state?.observation?.input?.value,0,`Spectator keyboard entered P${seat+1}'s native input`);
    }
    assert.ok(after.every((state,seat)=>readRoomWorldFrame(state) > readRoomWorldFrame(before[seat])),
      'Players must keep advancing while spectator keys are held');
    const later = await read(page,0);
    assert.ok(readRoomWorldFrame(later)>readRoomWorldFrame(state),
      'Spectator native world stopped while physical keys were held');
    result.checks.push({id:'native-spectator-keyboard-isolation',passed:true,keys,before,samples,after,later});
    await page.screenshot({path:resolve(directory,game+'-spectator-playing.png')});
    assert.deepEqual(result.errors,[],'Spectator page errors');
    assert.deepEqual(result.consoleErrors,[],'Spectator console errors');
  });
}

export async function finishSpectatorLaunch({page,result}) {
  try {
    await page.waitForFunction(() => document.querySelector('#decisionDialog')?.open ||
      window.acceptanceEvents?.some(event=>event.event==='first-frame'), null, {timeout:120000});
    if (await page.locator('#decisionDialog[open]').count()) {
      const message=await page.locator('#decisionDialog').innerText();
      assert.match(message,/无音乐|without (?:background )?music|no (?:background )?music/i,'Unexpected spectator launch confirmation');
      result.launchConfirmation=message;
      await page.locator('#decisionConfirm').click();
    }
    await page.waitForFunction(()=>window.acceptanceEvents?.some(event=>event.event==='first-frame'),null,{timeout:120000});
  } catch(error) {
    result.launchFailure=await page.evaluate(()=>({body:document.body.innerText,events:window.acceptanceEvents??[]})).catch(()=>null);
    throw error;
  }
}
