const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');
const shell = fs.readFileSync(path.join(__dirname, '../resources/shell.html'), 'utf8');
const section = shell.split('// BEGIN NETPLAY PERFORMANCE PROFILE')[1]?.split('// END NETPLAY PERFORMANCE PROFILE')[0];
assert.ok(section);
const source = section.slice(section.indexOf('function resolveNetplayPerformanceProfile'));
const resolve = vm.runInNewContext(`(${source.trim()})`);
for (const coarse of [false, true]) {
  for (const touch of [false, true]) {
    for (const cap of [false, true]) {
      const single = resolve({touchEnabled:touch, limitPresentationTo60:cap}, coarse);
      assert.equal(single.limitPresentationTo60,cap);
      assert.equal(single.netplayInputDelayFrames,0);
      assert.equal(single.netplayRollbackPolicy,'full');
      const multi = resolve({netplayMode:'lan',touchEnabled:touch,limitPresentationTo60:cap},coarse);
      assert.equal(multi.limitPresentationTo60,cap);
      assert.equal(multi.netplayPerformanceProfile,'responsive');
      assert.equal(multi.netplayInputDelayFrames,0);
      assert.equal(multi.netplayRollbackPolicy,'full');
      assert.equal(multi.netplaySnapshotPolicy,'frontier');
      assert.equal(multi.netplaySnapshotLayout,'runs');
      assert.equal(multi.netplaySnapshotCopy,'bulk');
      assert.equal(multi.netplaySnapshotRestore,'coalesced');
      assert.equal(multi.netplayBulletSnapshot,'live');
      assert.equal(multi.netplaySnapshotCheckpointFrames,3);
      assert.equal(multi.netplayReliableInputRepair,true);
      const responsive = resolve({netplayMode:'lan',netplayInputDelayFrames:0,netplayRollbackPolicy:'full',touchEnabled:touch,limitPresentationTo60:cap},coarse);
      assert.equal(responsive.netplayInputDelayFrames,0);
      assert.equal(responsive.netplayRollbackPolicy,'full');
      assert.equal(responsive.netplayBulletSnapshot,'live');
      assert.equal(responsive.limitPresentationTo60,cap);
      assert.equal(responsive.netplayReliableInputRepair,true);
      assert.equal(responsive.netplaySnapshotCheckpointFrames,3);
      assert.equal(resolve({netplayMode:'lan',netplayInputDelayFrames:0,netplaySnapshotCheckpointFrames:1},coarse).netplaySnapshotCheckpointFrames,1);
      assert.equal(resolve({netplayMode:'lan',netplayInputDelayFrames:0,netplayReliableInputRepair:false},coarse).netplayReliableInputRepair,true);
      assert.equal(resolve({netplayMode:'lan',netplayInputDelayFrames:0,netplayBulletSnapshot:'journal'},coarse).netplayBulletSnapshot,'journal');
      assert.equal(resolve({netplayMode:'lan',netplayInputDelayFrames:0,netplaySpectator:true},coarse).netplayBulletSnapshot,'journal');
      const manual = resolve({debugHarness:'netplay-lan-stage1',netplayMode:'lan',netplayPerformanceProfile:'manual',touchEnabled:touch,limitPresentationTo60:cap},coarse);
      assert.equal(manual.limitPresentationTo60,cap);
      assert.equal(manual.netplayInputDelayFrames,0);
      assert.equal(manual.netplayRollbackPolicy,'full');
      assert.equal(manual.netplaySnapshotPolicy,'always');
      assert.equal(manual.netplaySnapshotCheckpointFrames,2);
      assert.equal(resolve({netplayMode:'lan',netplayRollbackPolicy:'buffered'},coarse).netplayRollbackPolicy,'full');
      assert.equal(resolve({netplayMode:'lan',netplayInputDelayFrames:6},coarse).netplayInputDelayFrames,0);
      assert.equal(resolve({debugHarness:'netplay-lan-stage1',netplayMode:'lan',netplayRollbackPolicy:'buffered'},coarse).netplayRollbackPolicy,'buffered');
      for (const delay of [0,1,3,6,12]) {
        assert.equal(resolve({debugHarness:'netplay-lan-stage1',netplayMode:'lan',touchEnabled:touch,netplayInputDelayFrames:delay},coarse).netplayInputDelayFrames,delay);
      }
    }
  }
}
// Module.eaglerOptions must not overwrite the resolved settings afterwards.
const assignment = shell.split('Module.eaglerOptions = {')[1].split('};')[0];
assert.ok(assignment.includes('...resolveNetplayPerformanceProfile'));
for (const key of ['netplayInputDelayFrames','netplayRollbackPolicy','netplaySnapshotPolicy','netplaySnapshotLayout','netplaySnapshotCopy','netplaySnapshotRestore','limitPresentationTo60'])
  assert.ok(!new RegExp(`\\b${key}\\s*:`).test(assignment), `profile overwritten: ${key}`);
console.log('netplay performance defaults: PASS single production 0/full profile plus diagnostic overrides');
