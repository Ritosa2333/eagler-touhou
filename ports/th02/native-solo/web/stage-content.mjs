import { copyFile, mkdir, readdir, writeFile } from 'node:fs/promises';
import { dirname, resolve } from 'node:path';

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
async function walk(dir,prefix='') {
    const entries = await readdir(resolve(dir,prefix),{withFileTypes:true});
    return (await Promise.all(entries.map(entry => {
        const name = prefix + entry.name;
        if (entry.isDirectory() && (prefix || ['locale','zunsoft'].includes(entry.name))) return walk(dir,name + '/');
        if (entry.isFile() && (prefix || /\.(?:PI|BFT|BBT|BMT|BTT|MPN|MAP|DT1|TXT|RGB|M|MMD|EFC|REC|DAT|pack|sf2)$/i.test(name))) return [name];
        return [];
    }))).flat();
}
const fullNames = (await walk(source)).sort();
await Promise.all(fullNames.map(async name => {
    const path = resolve(dest,name);
    await mkdir(dirname(path),{recursive:true});
    await copyFile(resolve(source,name),path);
}));
await writeFile(resolve(dest, 'files.json'), JSON.stringify(names) + '\n');
await writeFile(resolve(dest, 'full-files.json'), JSON.stringify(fullNames) + '\n');
