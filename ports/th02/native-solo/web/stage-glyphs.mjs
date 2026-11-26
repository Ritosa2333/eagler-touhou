import { execFileSync } from 'node:child_process';
import { mkdtempSync, readFileSync, readdirSync, rmdirSync, unlinkSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

const [exe,output] = process.argv.slice(2);
if (!exe || !output) throw Error('Usage: node web/stage-glyphs.mjs <Windows SoEW.exe> <web build>');
const build = resolve(output), src = fileURLToPath(new URL('../src/',import.meta.url));
const codes = new Set(Array.from({length:95},(_,i) => i+32));
function add(text) {
    for (const c of text) {
        const cp = c.codePointAt(0);
        if (cp >= 128 && !/\p{C}/u.test(c)) codes.add(cp);
    }
}
codes.add(0xfffd);
for (const name of readdirSync(src).filter(name => /\.(hpp|cpp)$/.test(name)))
    add(readFileSync(join(src,name),'utf8'));
const root = join(build,'content');
const manifest = JSON.parse(readFileSync(join(root,'full-files.json'),'utf8'));
const cp932 = new TextDecoder('shift_jis');
for (const name of manifest) {
    if (/\.txt$/i.test(name)) add(cp932.decode(readFileSync(join(root,name))));
    if (/\.pack$/i.test(name)) add(readFileSync(join(root,name),'utf8'));
}
const repertoire = Buffer.alloc(codes.size*4);
[...codes].sort((a,b) => a-b).forEach((cp,i) => repertoire.writeUInt32LE(cp,i*4));
const scratch = mkdtempSync(join(tmpdir(),'soew-gdf-'));
const input = join(scratch,'chars.bin');
writeFileSync(input,repertoire);
try {
    execFileSync(resolve(exe),['--export-web-glyphs',input,join(build,'soew-glyphs.bin')],{stdio:'inherit'});
} finally {
    unlinkSync(input);
    rmdirSync(scratch);
}
