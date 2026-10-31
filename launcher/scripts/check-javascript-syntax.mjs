import { readFile } from 'node:fs/promises';

// Bun's Node-compatible --check flag does not provide a parse-only boundary.
// Scan source directly so syntax validation never evaluates tools or fixtures.
const parser = new Bun.Transpiler({ loader: 'js', target: 'bun' });
for (const file of process.argv.slice(2)) {
  try {
    parser.scan(await readFile(file, 'utf8'));
  } catch (cause) {
    throw new SyntaxError(`${file}: ${cause.message}`, { cause });
  }
}
console.log(`JavaScript syntax: PASS (${process.argv.length - 2} files, Bun parser)`);
