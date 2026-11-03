import {readFile} from 'node:fs/promises';
import {extname, resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {parse} from 'acorn';

/** Parse source without importing it or running its top-level statements. */
export function checkJavaScriptSyntax(source, filename) {
  const commonjs = extname(filename) === '.cjs';
  try {parse(source, {ecmaVersion: 'latest', sourceType: commonjs ? 'script' : 'module', allowReturnOutsideFunction: commonjs, allowHashBang: true});}
  catch (cause) {throw new SyntaxError(`${filename}: ${cause.message}`, {cause});}
}

if (process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url)) {
  const files = process.argv.slice(2);
  for (const file of files) checkJavaScriptSyntax(await readFile(file, 'utf8'), file);
  console.log(`JavaScript syntax: PASS (${files.length} files)`);
}
