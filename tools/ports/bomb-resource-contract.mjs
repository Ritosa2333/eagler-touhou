import assert from 'node:assert/strict';

/** Native inventory stays in the title's own resource unit. Usage counters count activations. */
export function assertBombResource(resource){
  assert.ok(Number.isSafeInteger(resource?.cost)&&resource.cost>0&&resource.cost<=0xffff,'Declare the ordinary Bomb resource cost');
  assert.ok(typeof resource.unit==='string'&&resource.unit.trim(),'Declare the native Bomb resource unit');
  assert.ok(typeof resource.source==='string'&&resource.source.trim(),'Bomb resource cost needs its production source');
  return resource.cost;
}
