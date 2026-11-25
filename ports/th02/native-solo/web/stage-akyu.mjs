import {createHash} from 'node:crypto';
import {mkdir, readFile, writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';

const [source, build] = process.argv.slice(2);
if (!source || !build) throw Error('Usage: node web/stage-akyu.mjs <old TH02 web/bgm> <web build>');

// MUSIC.TXT order, not album order. Use the old DOS port's original map.
const slots = ['op', 'stage0', 'stage1', 'stage2', 'stage3', 'stage4',
    'boss1', 'boss4', 'boss2', 'boss3', 'mima', 'end1', 'ending', 'stage5', 'boss5'];
const manifest = JSON.parse(await readFile(resolve(source, 'akyu/manifest.json'), 'utf8'));
const catalog = slots.map(slot => manifest.tracks[slot + '-m86']);
const dest = resolve(build, 'content');
const filesPath = resolve(dest, 'full-files.json');
const files = JSON.parse(await readFile(filesPath, 'utf8'));
await mkdir(resolve(dest, 'akyu'), {recursive: true});
for (const track of catalog) {
    const bytes = await readFile(resolve(source, track.url));
    if (bytes.length !== track.bytes || createHash('sha256').update(bytes).digest('hex') !== track.audioSha256)
        throw Error('Album track differs from old TH02 manifest: ' + track.url);
    await writeFile(resolve(dest, track.url), bytes);
}
await writeFile(resolve(dest, 'akyu.pack'), JSON.stringify(catalog.map(({url, loopStart, loopEnd}) =>
    ({url, loopStart, loopEnd}))) + '\n');
await writeFile(filesPath, JSON.stringify([...new Set([...files, 'akyu.pack'])].sort()) + '\n');
console.log(`Staged ${catalog.length} Akyu's Untouched Score Vol. 3 tracks`);
