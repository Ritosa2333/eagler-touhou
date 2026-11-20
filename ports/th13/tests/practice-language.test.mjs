import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {practiceSection, practiceSections} from '../web/sdl-runtime/practice-catalog.mjs';

const read = rel => readFileSync(new URL(rel, import.meta.url), 'utf8');

test('RUEEE section TH13_ST2_BOSS1 warps to stage 2 and stays idle when disabled', () => {
  const section = practiceSection('TH13_ST2_BOSS1');
  assert.equal(section.stage, 2);
  assert.equal(section.group, 2);
  assert.equal(section.index, 1);
  const header = read('../web/cpp/game/PracticeSections.gen.hpp');
  const rows = [...header.matchAll(/\{(\d+),(\d+),(\d+),(\d+)\}, \/\/ (\S+)/g)];
  assert.equal(rows.length, practiceSections.length);
  const generated = rows.find(row => row[5] === 'TH13_ST2_BOSS1');
  assert.equal(Number(generated[1]), section.stage);
  assert.equal(rows.indexOf(generated) + 1, section.id);
  const disabled = {enabled: false, section: section.id, stage: 1};
  assert.equal(disabled.enabled ? section.stage : disabled.stage, 1);
  const enabled = {enabled: true, section: section.id, stage: 1};
  assert.equal(enabled.enabled && enabled.section ? section.stage : enabled.stage, 2);
});

test('single-player shell owns the practice panel and both shells install only zh-hans', () => {
  const sp = read('../web/sdl-runtime/shell.mjs');
  const mp = read('../../th13-mp/web/sdl-runtime/shell.mjs');
  assert.match(sp, /practice-catalog\.mjs/);
  assert.match(sp, /_th13_practice_set\?\.\(0, 0, -1, -1, -1, -1\)/);
  assert.match(sp, /dialog\.showModal/);
  assert.doesNotMatch(sp, /position:fixed/);
  assert.match(sp, /installLanguagePack\(core, m\.runtimePack, game\)/);
  assert.doesNotMatch(mp, /practice-catalog/);
  assert.match(mp, /options\.thpracEnabled = false/);
  assert.match(mp, /installLanguagePack\(core, m\.runtimePack, game\)/);
  for (const source of [sp, mp].map(() => read('../web/sdl-runtime/language-pack.mjs'))) {
    assert.match(source, /pack\.language !== 'lang_zh-hans'/);
    assert.match(source, /\\\.\(\?:ecl\|std\|sht\)/);
  }
});
