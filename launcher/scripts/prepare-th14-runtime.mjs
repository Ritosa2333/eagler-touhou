import {createHash} from 'node:crypto';
import {mkdir, readFile, writeFile, copyFile} from 'node:fs/promises';
import {resolve, dirname} from 'node:path';
import {transform} from 'esbuild';
import {runtimeFileNames} from '../lib/runtime-release.mjs';

const args = Object.fromEntries(process.argv.slice(2).map(argument => {
  const match = /^--(input|output)=(.+)$/.exec(argument);
  if (!match) throw Error('Use --input=RUNTIME_DIRECTORY --output=EMPTY_DIRECTORY');
  return [match[1], resolve(match[2])];
}));
if (!args.input || !args.output || args.input === args.output) throw Error('Supply distinct --input and --output directories');
const digest = bytes => createHash('sha256').update(bytes).digest('hex');
const declared = JSON.parse(await readFile(resolve(args.input, 'runtime-files.json'), 'utf8'));
if (declared.schema !== 'eagler-touhou/runtime-directory/1') throw Error('Missing TH14 runtime directory manifest');
const files = {}, adaptations = [];
for (const name of runtimeFileNames('th14', declared.files)) {
  const source = resolve(args.input, name), target = resolve(args.output, name);
  let bytes = await readFile(source);
  const expected = declared.files[name];
  if (bytes.length !== expected.bytes || digest(bytes) !== expected.sha256) throw Error('Stale runtime input: ' + name);
  await mkdir(dirname(target), {recursive: true});
  if (name === 'archive-decoder.mjs') {
    const original = digest(bytes);
    const guard = 'globalThis.process?.versions?.node&&globalThis.process?.type!="renderer"';
    const sourceText = bytes.toString('utf8');
    if (!sourceText.includes('var ENVIRONMENT_IS_NODE=' + guard + ';')) throw Error('Archive decoder environment guard changed');
    // This published variant runs only in a browser or browser worker. Remove
    // the universal loader's Node branch; keep the decoder Wasm unchanged.
    bytes = Buffer.from((await transform(sourceText.replace('var ENVIRONMENT_IS_NODE=' + guard + ';', '')
      .replace(/\bENVIRONMENT_IS_NODE\b/g, 'false'), {format: 'esm', target: 'es2022', minifySyntax: true})).code);
    if (/import\(["']node:/.test(bytes.toString('utf8'))) throw Error('Node dependency survived browser preparation');
    await writeFile(target, bytes);
    adaptations.push({file: name, inputSha256: original, outputSha256: digest(bytes)});
  } else {
    await copyFile(source, target);
  }
  files[name] = {bytes: bytes.length, sha256: digest(bytes)};
}
await writeFile(resolve(args.output, 'runtime-files.json'), JSON.stringify({
  schema: declared.schema, game: 'th14', files,
}, null, 2) + '\n');
console.log(JSON.stringify({game: 'th14', output: args.output, adaptations, wasm: files['th14-sdl.wasm']}));
