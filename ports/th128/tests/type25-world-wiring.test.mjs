import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const source = fs.readFileSync(path.join(root, 'src/runtime/WorldExports.cpp'), 'utf8');

test('type25 WorldExports secondary create/render/cancel wiring is enabled', () => {
  assert.match(source, /info\.secondaryScript=secondary/);
  assert.match(source, /info\.identitySpriteRemap=profile->second\.identitySpriteRemap\(\)/);
  assert.match(source, /birth\.type==25/);
  assert.match(source, /animationState\(found->second,true\)/);
  assert.match(source, /0x80000000u\+bullet\.id/);
  assert.equal((source.match(/hasSecondary&&bullet\.type!=25/g) ?? []).length, 3);
  assert.match(source, /hostile-bullets/);
  assert.match(source, /supportedBulletTypes\\":\[4,8,10,18,20,23,25,29\]/);
  for (const variant of ['ja', 'zh']) {
    const asset = path.join(root, 'local/generations/8c71c316a4b24e68827f9fc560242050',
      variant, 'animations/bullet-type25-profile.json');
    assert.ok(fs.existsSync(asset), asset);
    const profile = JSON.parse(fs.readFileSync(asset, 'utf8'));
    assert.equal(profile.type, 25);
    assert.equal(profile.secondaryScript, 103);
    assert.equal(profile.identitySpriteRemap, true);
  }
});
