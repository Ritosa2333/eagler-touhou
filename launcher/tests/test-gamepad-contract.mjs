import {test} from 'vitest';
import assert from 'node:assert/strict';
import {access, readFile} from 'node:fs/promises';
import {resolve, relative} from 'node:path';
import {targets, root} from '../../tools/ports/inputs.mjs';
import {runNativeCpp} from './support/native-cpp.mjs';

test("test-gamepad-contract", async () => {
/** Preconditions: monorepo sources and a C++17 compiler (CXX or g++).
 * Inputs: each logical gamepad bit, combined bits, neutral/disconnected snapshots.
 * Invariant: canonical virtual/scancode keys and independent keyboard ownership.
 * Proves: shared native mapping behavior and identity of its generated copies.
 * Device sampling and gameplay effects use the browser/Runtime acceptance tests.
 */






const header = 'browser-gamepad-input.hpp';
const canonical = resolve(root, 'common/browser', header);
const source = (await readFile(canonical, 'utf8')).replaceAll('\r\n', '\n');
const copies = new Map();
for (const target of targets()) for (const subdir of ['src', 'web/cpp/sdl']) {
  const path = resolve(root, target.directory, subdir, header);
  try {await access(path);} catch (error) {if (error.code === 'ENOENT') continue; throw error;}
  const labels = copies.get(path) ?? [];
  labels.push(`${target.game}/${target.variant}`); copies.set(path, labels);
}
assert.ok(copies.size, 'No generated native gamepad mappings found');
for (const [path, labels] of copies) {
  assert.equal((await readFile(path, 'utf8')).replaceAll('\r\n', '\n'), source,
    `${labels.join(', ')}: ${relative(root, path)} differs from the shared gamepad mapper`);
}
await runNativeCpp(`#include "browser-gamepad-input.hpp"
#include <array>
#include <cassert>
using namespace eagler::input;
int main(){
  constexpr unsigned vk[]={90,88,16,27,38,40,37,39,17,13,67,82};
  constexpr unsigned scans[]={44,45,42,1,200,208,203,205,29,28,46,19};
  for(unsigned bits=0;bits<4096;++bits){
    std::array<unsigned char,256> keys{},scan{};
    ApplyGamepadVirtualKeys(keys,bits);ApplyGamepadScanKeys(scan,bits);
    for(unsigned i=0;i<256;++i){
      bool expectedKey=false,expectedScan=false;
      for(unsigned bit=0;bit<12;++bit)if(bits&(1u<<bit)){
        expectedKey|=i==vk[bit]||(bit==2&&i==160)||(bit==8&&i==162);
        expectedScan|=i==scans[bit];
      }
      assert(bool(keys[i])==expectedKey);assert(bool(scan[i])==expectedScan);
    }
  }
  std::array<unsigned char,256> held{};held[90]=128;held[160]=128;
  const auto before=held;
  for(int bits:{-1,0}){ApplyGamepadVirtualKeys(held,bits);assert(held==before);ApplyGamepadScanKeys(held,bits);assert(held==before);}
}`, {includes: [resolve(root, 'common/browser')]});
console.log(JSON.stringify({nativeGamepadMapping: 'PASS', combinations: 4096,
  copies: [...copies].map(([path, targets]) => ({path: relative(root, path).replaceAll('\\', '/'), targets}))}));
});
