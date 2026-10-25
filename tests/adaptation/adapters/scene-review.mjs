import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {lstatSync,readFileSync} from 'node:fs';
import {isAbsolute} from 'node:path';

/** Native scene reviews share exact local artifact identities and bounded reads. */
export function readSceneReviewArtifact(receipt){
  assert.ok(receipt&&isAbsolute(receipt.path),'Review material needs an absolute local path');
  assert.ok(Number.isSafeInteger(receipt.bytes)&&receipt.bytes>0&&receipt.bytes<=128*1024*1024,'Invalid review material size');
  assert.match(receipt.sha256,/^[a-f0-9]{64}$/);
  const info=lstatSync(receipt.path);assert.ok(info.isFile()&&!info.isSymbolicLink());assert.equal(info.size,receipt.bytes);
  const bytes=readFileSync(receipt.path);assert.equal(bytes.length,receipt.bytes);
  assert.equal(createHash('sha256').update(bytes).digest('hex'),receipt.sha256);return bytes;
}
