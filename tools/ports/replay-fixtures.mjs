import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {readFile} from 'node:fs/promises';
import {isAbsolute} from 'node:path';

/** Load an explicit, independently sourced replay for real Runtime file operations. */
export async function loadRuntimeReplayFixture(target) {
  const fixture=target.replay;
  assert.ok(fixture&&typeof fixture==='object',`${target.id}: missing replay fixture; provide replay.path, replay.bytes and replay.sha256`);
  assert.ok(typeof fixture.path==='string'&&isAbsolute(fixture.path),`${target.id}: replay.path must be absolute`);
  assert.ok(Number.isSafeInteger(fixture.bytes)&&fixture.bytes>0,`${target.id}: replay.bytes must be positive`);
  assert.match(fixture.sha256??'',/^[a-f0-9]{64}$/,`${target.id}: replay.sha256 is required`);
  const contents=await readFile(fixture.path);
  const identity={path:fixture.path,bytes:contents.length,sha256:createHash('sha256').update(contents).digest('hex')};
  assert.equal(identity.bytes,fixture.bytes,`${target.id}: replay fixture size changed`);
  assert.equal(identity.sha256,fixture.sha256,`${target.id}: replay fixture bytes changed`);
  return {contents,identity};
}
