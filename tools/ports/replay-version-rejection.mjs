import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');

/** Change only the title-declared extension version, keeping payload and its checksum intact. */
export function unsupportedReplayVersion(bytes,contract){
  assert.ok(contract?.source?.trim());assert.ok(Number.isSafeInteger(contract.versionOffsetFromEnd)&&contract.versionOffsetFromEnd>=4&&contract.versionOffsetFromEnd<=bytes.length);
  assert.ok(Number.isSafeInteger(contract.unsupportedVersion)&&contract.unsupportedVersion>=0&&contract.unsupportedVersion<=0xffffffff);
  const mutated=Buffer.from(bytes),offset=mutated.length-contract.versionOffsetFromEnd,originalVersion=mutated.readUInt32LE(offset);
  assert.notEqual(originalVersion,contract.unsupportedVersion);mutated.writeUInt32LE(contract.unsupportedVersion,offset);
  return{bytes:mutated,mutation:{offset,originalVersion,unsupportedVersion:contract.unsupportedVersion,originalSha256:hash(bytes),sha256:hash(mutated)}};
}

export function assertReplayVersionRejection(e,{contract,game,epoch,bytes}){
  const expected=unsupportedReplayVersion(bytes,contract);
  assert.equal(e.status,'passed');assert.deepEqual(e.mutation,expected.mutation);
  assert.equal(e.sent.command,'write');assert.equal(e.sent.path,contract.path);assert.equal(e.sent.protocol,'eagler-touhou/1');
  assert.equal(e.sent.game,game);assert.equal(e.sent.epoch,epoch);assert.ok(e.sent.request);
  assert.deepEqual(Buffer.from(e.sent.bytes),expected.bytes,'The rejected import did not send the version-only mutation');
  assert.equal(e.responses.length,1);const response=e.responses[0];
  assert.equal(response.protocol,e.sent.protocol);assert.equal(response.game,game);assert.equal(response.epoch,epoch);assert.equal(response.request,e.sent.request);
  assert.equal(response.ok,false);assert.equal(response.error,contract.error);
  assert.equal(e.barrier.ok,true);assert.equal(e.barrier.game,game);assert.equal(e.barrier.epoch,epoch);
  assert.notEqual(e.barrier.request,e.sent.request);assert.deepEqual(e.lists.before,e.lists.after);
  for(const list of [e.lists.before,e.lists.after])assert.ok(list.every(file=>![contract.path,contract.path.replace(/x$/,'')].includes(file.path)),'Rejected version created a user file');
  assert.deepEqual(Buffer.from(e.originalBefore),bytes);assert.deepEqual(Buffer.from(e.originalAfter),bytes,'Rejected version changed the valid Replay');
  return expected;
}
