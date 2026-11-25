import { copyFile, mkdir, writeFile } from 'node:fs/promises';
import { resolve } from 'node:path';

const [source, output] = process.argv.slice(2);
if (!source || !output) throw Error('Usage: node web/stage-content.mjs <extracted TH02 content> <web build>');
const names = [
    'MIKO.BFT', 'MIKO16.BFT', 'MIKO32.BFT', 'MIKOFT.BFT', 'MIKO_K.MPN',
    'BOMB1.BFT', 'BOMBS.BFT', 'BOMB1.PI', 'BOMB2.PI', 'BOMB3.PI',
    'STAGE2_B.BBT', 'STAGE3_B.BFT', 'STAGE3_B.BTT', 'STAGE5B1.BFT', 'STAGE5B2.BFT',
    'MIMA.BFT', 'MIMA1.BFT', 'MIMA2.BFT',
    ...Array.from({length: 6}, (_, stage) =>
        ['BBT', 'BFT', 'BMT', 'DT1', 'MAP', 'MPN', 'TXT'].map(ext => `STAGE${stage}.${ext}`)).flat(),
].sort();
const dest = resolve(output, 'content');
await mkdir(dest, {recursive: true});
await Promise.all(names.map(name => copyFile(resolve(source, name), resolve(dest, name))));
await writeFile(resolve(dest, 'files.json'), JSON.stringify(names) + '\n');
