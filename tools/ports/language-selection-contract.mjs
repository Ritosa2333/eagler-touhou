import assert from 'node:assert/strict';

/** Produce real selector changes when the chosen language is only a UI-locale default. */
export function explicitLanguageSelectionSteps({current,target,stored,options}){
  assert.ok(typeof target==='string'&&target.length,'A selected language is required');
  assert.ok(Array.isArray(options)&&options.includes(target),'The selected language is not available in the Launcher');
  if(current!==target)return[target];
  if(stored===target)return[];
  const alternate=options.find(value=>value!==target);
  assert.ok(alternate,'The default language has no alternate Launcher choice to produce a persisted change');
  return[alternate,target];
}

/** Normal Runtime preferenceId is the game id; this mirrors the Launcher storage-key owner. */
export function normalGameLanguagePreferenceKey(gameId){
  assert.ok(typeof gameId==='string'&&/^[a-z0-9]+$/.test(gameId),'Normal Runtime preference needs a product game id');
  return`eagler-touhou-language-v1-${gameId}`;
}
