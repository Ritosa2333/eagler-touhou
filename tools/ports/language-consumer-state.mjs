import assert from 'node:assert/strict';

/** Titles supply field positions; the reader exposes only observed navigation state. */
export function normalizeLanguageConsumerState(raw,contract){
  assert.equal(raw?.available,true,raw?.reason);const words=raw.words,spec=contract.uiSnapshot;
  assert.equal(words.length,spec.wordCount);assert.ok(words.every(Number.isSafeInteger));assert.equal(words[0],1);assert.equal(words[1],1);
  const fields=Object.fromEntries(Object.entries(spec.fields).map(([key,index])=>{assert.ok(Number.isSafeInteger(index)&&index>=2&&index<words.length);return[key,words[index]];}));
  for(const key of spec.booleans){assert.ok(Object.hasOwn(fields,key));assert.ok(fields[key]===0||fields[key]===1);fields[key]=!!fields[key];}
  return fields;
}
