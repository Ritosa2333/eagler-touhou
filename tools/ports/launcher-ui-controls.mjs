import assert from 'node:assert/strict';

/** Open the real disclosure and wait for an actionable control inside its body. */
export async function openAdvancedSettings(page){
  const advanced=page.locator('#advancedOptions');
  if(await advanced.getAttribute('open')===null)await advanced.locator('summary').click();
  const ready=advanced.locator('#gameVersionOption .mizuki-select-trigger');
  await ready.click({trial:true});
  const result={advancedOpen:await advanced.getAttribute('open')!==null,readyControlVisible:await ready.isVisible()};
  assert.equal(result.advancedOpen,true,'Launcher advanced settings did not stay open');
  assert.equal(result.readyControlVisible,true,'Launcher advanced settings body is not visible');
  return result;
}

/** Choose through the published custom menu, preserving its actual UI behavior. */
export async function chooseSetting(page,id,value){
  const select=page.locator(`#${id}`);
  if(await select.inputValue()===value)return;
  await openAdvancedSettings(page);
  const control=page.locator('.mizuki-select').filter({has:select});
  await control.locator('.mizuki-select-trigger').click();
  await page.locator(`.mizuki-select-menu:visible [data-value="${value}"]`).click();
  assert.equal(await select.inputValue(),value,`Launcher did not select ${id}=${value}`);
}
