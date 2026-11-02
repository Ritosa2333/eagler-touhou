import {test, beforeAll as before, afterAll as after} from 'vitest';
import assert from 'node:assert/strict';
import {mkdtemp, rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {build} from 'esbuild';
const project = fileURLToPath(new URL('../../', import.meta.url));
let api, work;
before(async () => {
  work = await mkdtemp(resolve(project, '.cache/career-chart-'));
  const outfile = resolve(work, 'records.mjs');
  await build({absWorkingDir: project, stdin: {resolveDir: project, contents: "export * from './app/components/launcher/tracker-records.ts'; export * from './app/components/launcher/tracker-grid.ts'; export * from './app/components/launcher/tracker-renderer.ts';"}, outfile, bundle: true, platform: 'node', format: 'esm', logLevel: 'silent'});
  api = await import(pathToFileURL(outfile).href);
});
after(async () => {await rm(work, {recursive: true, force: true});});

test('full chart includes all reference works and TH20 independent of playable platform games', () => {
  assert.equal(api.TRACKER_GAMES.length, 33);
  assert.equal(api.TRACKER_CELLS.length, 1110);
  assert.equal(new Set(api.TRACKER_CELLS.map(cell => cell.key)).size, 1110);
  for (const game of ['HRTP', 'MS', 'UFO', 'TD', 'DDC', 'HSIFS', 'WBAWC', 'UM', 'UDOALG', 'STB', 'DS', 'ISC', 'VD', '100BM', 'IAMP', 'SWR', 'H.SOKU', 'HM', 'ULIL', 'AOCF', 'GI']) {
    assert.ok(api.TRACKER_CELLS.some(cell => cell.game.name === game), game);
  }
  const has = key => api.TRACKER_CELLS.some(cell => cell.key === key);
  for (const key of ['PCB-Ra-P', 'IN-BT-A-N', 'IN-BT-B-N', 'LOLK-Rp-N', 'LOLK-Rl-N', 'GFW-EX-N', 'TD-Re1-X', 'UM-Re1-X']) assert.ok(has(key), key);
  for (const key of ['LOLK-Rp-X', 'GFW-EX-L', 'TD-Re2-X', 'UM-Re2-X', 'HSIFS-Rsm-X']) assert.equal(has(key), false, key);
});

test('backup validation rejects unknown cells, invalid counts and unsafe color values', () => {
  const key = 'UFO-Ra-N', record = {...api.blankRecord(key), done: true, misses: '0', bombs: '2', lives: '3', focus: true, note: 'first clear'};
  assert.equal(api.validCareerRecords({[key]: record}), true);
  for (const override of [{name: 'EOSD-Ra-N'}, {done: 1}, {misses: '12'}, {bombs: -1}, {lives: 'NaN'}, {cellColour: 'url(evil)'}, {textColour: '#fff'}, {note: 'x'.repeat(241)}, {challenge: 'x'.repeat(25)}]) {
    assert.equal(api.validCareerRecords({[key]: {...record, ...override}}), false, JSON.stringify(override));
  }
  assert.equal(api.validCareerRecords({'UNKNOWN-R-N': api.blankRecord('UNKNOWN-R-N')}), false);
  assert.equal(api.validCareerRecords([]), false);
  assert.equal(api.validCareerRecords(null), false);
  assert.equal(api.validCareerRecords({}), true);
});

test('chart options require boolean flags; labels identify full works and distinct routes', () => {
  assert.equal(api.validOptions(api.DEFAULT_OPTIONS), true);
  assert.equal(api.validOptions({...api.DEFAULT_OPTIONS, easy: 'yes'}), false);
  assert.equal(api.validOptions({}), false);
  assert.match(api.cellLabel('UFO-Ra-N'), /东方星莲船.*Normal/);
  assert.match(api.cellLabel('PCB-Ra-P'), /Phantasm/);
  assert.notEqual(api.cellLabel('IN-BT-A-N'), api.cellLabel('IN-BT-B-N'));
});

test('adjacent squares share one aligned border while exposed edges retain thick outlines', () => {
  const edges = api.trackerCellEdges([{x: 0, y: 0}, {x: 17, y: 0}]);
  assert.equal(edges.length, 7);
  assert.equal(edges.filter(edge => edge.outer).length, 6);
  assert.deepEqual(edges.filter(edge => !edge.outer), [{x1: 17, y1: 0, x2: 17, y2: 17, outer: false}]);
  const stepped = api.trackerCellEdges([{x: 0, y: 17}, {x: 17, y: 17}, {x: 17, y: 0}]);
  assert.equal(stepped.length, 10);
  assert.equal(stepped.filter(edge => edge.outer).length, 8);
  assert.equal(stepped.filter(edge => !edge.outer).length, 2);
  assert.equal(stepped.some(edge => edge.x1 === 0 && edge.y1 === 0), false);
  const fractional = api.trackerCellEdges([{x: 0.1, y: 0.9}, {x: 17.1, y: 0.9}]);
  assert.equal(fractional.length, 7);
  assert.equal(fractional.filter(edge => !edge.outer).length, 1);
});


test('three chronological sections keep game tables separate and inside the chart', () => {
  for (const easy of [true, false]) for (const extra of [true, false]) {
    const layout = api.trackerLayout({...api.DEFAULT_OPTIONS, easy, extra});
    assert.deepEqual(layout.sections.map(section => section.titleKey), ['career.section.main', 'career.section.versus', 'career.section.spinOff']);
    assert.deepEqual(layout.sections[1].games.map(block => block.game.name), ['PODD', 'POFV', 'UDOALG']);
    const blocks = layout.sections.flatMap(section => section.games);
    assert.equal(blocks.length, extra ? 33 : 26);
    assert.equal(new Set(blocks.map(block => block.game.name)).size, blocks.length);
    for (const [index, block] of blocks.entries()) {
      assert.ok(block.x >= 24 && block.x + block.width <= 776);
      assert.ok(block.y + block.height < layout.height);
      for (const other of blocks.slice(index + 1)) {
        assert.ok(block.x + block.width <= other.x || other.x + other.width <= block.x ||
          block.y + block.height <= other.y || other.y + other.height <= block.y);
      }
    }
  }
});


test('TH20 has five difficulties and four colors for both R and M in the integer section', () => {
  const game = api.TRACKER_GAMES.find(game => game.name === 'TH20');
  assert.equal(api.trackerDisplayName(game), 'th20');
  assert.deepEqual(game.difficulties, ['E', 'N', 'H', 'L', 'X']);
  assert.deepEqual(game.characters, ['Rr', 'Ry', 'Rb', 'Rg', 'Mr', 'My', 'Mb', 'Mg']);
  assert.equal(api.TRACKER_CELLS.filter(cell => cell.game.name === 'TH20').length, 40);
  assert.equal(api.trackerLayout(api.DEFAULT_OPTIONS).sections[0].games.at(-1).game.name, 'TH20');
  assert.equal(api.cellLabel('TH20-Rr-X'), '东方锦上京 | Rr | Extra');
});


test('compact chart shares aligned difficulty rows and fits versus games on one row', () => {
  const layout = api.trackerLayout(api.DEFAULT_OPTIONS);
  assert.ok(layout.height < 1400, `chart height: ${layout.height}`);
  const versus = layout.sections[1];
  assert.equal(versus.rows.length, 1);
  assert.equal(versus.games[0].y + versus.games[0].difficulties.indexOf('N') * 17,
    versus.games[1].y + versus.games[1].difficulties.indexOf('N') * 17);
  assert.equal(versus.games[0].x + versus.games[0].width, versus.games[1].x);
});


test('shared difficulty labels align every standard game including games without Easy', () => {
  const layout = api.trackerLayout(api.DEFAULT_OPTIONS);
  for (const section of layout.sections) for (const block of section.games.filter(block => block.standard && !block.ownLabels)) {
    const row = section.rows.find(row => block.difficulties.every((difficulty, index) =>
      row.labels.includes(difficulty) && row.y + row.labels.indexOf(difficulty) * 17 === block.y + index * 17));
    assert.ok(row, block.game.name);
  }
});


test('every work uses a unique th number for its display label', () => {
  const labels = api.TRACKER_GAMES.map(api.trackerDisplayName);
  assert.equal(new Set(labels).size, 33);
  assert.ok(labels.every(label => /^th[0-9]{2}(?:\.[0-9])?$/.test(label)));
  assert.equal(api.trackerDisplayName({name: 'STB'}), 'th09.5');
  assert.equal(api.trackerDisplayName({name: 'H.SOKU'}), 'th12.3');
});


test('decimal tables form two gameplay rows and one compact scene row', () => {
  const section = api.trackerLayout(api.DEFAULT_OPTIONS).sections[2];
  assert.equal(section.rows.length, 3);
  const expected = [['IAMP', 'SWR', 'H.SOKU', 'HM'], ['ULIL', 'AOCF', 'GI'], ['GFW', 'STB', 'DS', 'ISC', 'VD', '100BM']];
  for (const [index, names] of expected.entries()) {
    const blocks = section.games.filter(block => names.includes(block.game.name));
    assert.deepEqual(blocks.map(block => block.game.name), names);
    assert.equal(new Set(blocks.map(block => block.y + block.difficulties.length * 17)).size, 1);
    assert.ok(blocks.every(block => block.y >= section.rows[index].y));
  }
  assert.ok(section.games.filter(block => ['GFW', 'GI', 'STB', 'DS', 'ISC', 'VD', '100BM'].includes(block.game.name)).every(block => block.drawDifficulties));
});


test('fixed difficulty colors follow routes and provide distinct theme palettes', () => {
  for (const dark of [false, true]) {
    const colors = ['E', 'N', 'H', 'L', 'X', 'P'].map(key => api.trackerDifficultyColour(key, dark));
    assert.equal(new Set(colors).size, 6);
    assert.equal(api.trackerDifficultyColour('B-N', dark), colors[1]);
    assert.equal(api.trackerDifficultyColour('A-N', dark), colors[1]);
  }
  assert.notEqual(api.trackerDifficultyColour('N', true), api.trackerDifficultyColour('N', false));
});
