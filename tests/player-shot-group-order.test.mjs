import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import * as draw from '../site/runtime/draw.mjs';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');

test('playerShotGroup follows the original ANM layer12 native priority and keeps known passes', () => {
    const fixture = JSON.parse(fs.readFileSync(
        path.join(process.env.TH128_RESEARCH_FIXTURES??path.join(root,'local/research'), 'player-shot-groups-fixtures.json'), 'utf8'));
    assert.equal(fixture.selection.layer, 12);
    const groupPriority = draw.nativeDrawPriority(12);
    assert.equal(groupPriority, 26);
    assert.notEqual(groupPriority, 25);
    assert.notEqual(groupPriority, 32);
    assert.equal(draw.nativeRecordPriority({layer: 12, drawPass: 'playerShotGroup'}), groupPriority);
    assert.equal(draw.nativeRecordPriority({layer: 12, drawPass: 'hostile-bullets'}), 32);
    assert.equal(draw.nativeRecordPriority({layer: 12, drawPass: 'player'}), 25);
    assert.equal(draw.nativeRecordPriority({layer: 12, drawPass: 'frozen-bullets'}), 24);
    const records = [
        {id: 1, layer: 12, drawPass: 'hostile-bullets'},
        {id: 2, layer: 12, drawPass: 'playerShotGroup'},
        {id: 3, layer: 12, drawPass: 'player'},
        {id: 4, layer: 12, drawPass: 'frozen-bullets'},
    ];
    assert.deepEqual(draw.orderDrawLayers(records, {world: true}).map(record => record.id), [4, 3, 2, 1]);
    assert.throws(() => draw.nativeRecordPriority({layer: 12, drawPass: 'playerShotGroupUnknown'}));
    assert.throws(() => draw.nativeRecordPriority({layer: 27, drawPass: 'playerShotGroup'}));
});
