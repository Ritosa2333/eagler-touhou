import {createHash} from 'node:crypto';
import {readFile, writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';

const [source, build] = process.argv.slice(2);
if (!source || !build) throw Error('Usage: node web/stage-midi.mjs <GeneralUser GS SoftSynth v1.44.sf2> <web build>');
const name = 'GeneralUser-GS-SoftSynth-1.44.sf2';
const sha256 = '00725296486f652ba42bdbf2098254762ca80239353d04b55a27454fc8edb8a8';
const data = await readFile(source);
if (createHash('sha256').update(data).digest('hex') !== sha256) throw Error('Expected the original GeneralUser GS SoftSynth v1.44 SoundFont');
const content = resolve(build, 'content');
const path = resolve(content, 'full-files.json');
const files = new Set(JSON.parse(await readFile(path, 'utf8')));
for (const song of ['OP', 'END1', 'ENDING', 'MIMA', ...Array.from({length:6}, (_, i) => 'STAGE' + i),
    ...Array.from({length:5}, (_, i) => 'BOSS' + (i + 1))]) {
    if (!files.has(song + '.MMD')) throw Error('Missing TH02 MIDI song: ' + song + '.MMD');
}
await writeFile(resolve(content,name), data);
files.add(name);
await writeFile(path, JSON.stringify([...files].sort()) + '\n');
console.log('Staged the verified GeneralUser GS SoftSynth v1.44 SoundFont for all 15 MIDI tracks');
