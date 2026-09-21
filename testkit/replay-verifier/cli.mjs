#!/usr/bin/env node
import {readFile, writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {compareTraces} from './compare.mjs';
import {readTraceJsonLines} from './trace-jsonl.mjs';
import {exitCodeForResult, formatResult} from './report.mjs';
import {corpusReadiness} from './corpus.mjs';

function usage() {
  return 'usage:\n  node cli.mjs compare --expected <trace.jsonl> --actual <trace.jsonl> [--report <result.json>] [--require a,b]\n  node cli.mjs corpus-status --corpus <corpus.json> [--report <status.json>]\n';
}

function argumentsFor(argv) {
  const [command, ...rest] = argv;
  const options = {};
  for (let i = 0; i < rest.length; i++) {
    const token = rest[i];
    if (!token.startsWith('--') || i + 1 >= rest.length) throw new Error(`Invalid argument ${token}`);
    options[token.slice(2)] = rest[++i];
  }
  return {command, options};
}

try {
  const {command, options} = argumentsFor(process.argv.slice(2));
  if (command === 'corpus-status') {
    if (!options.corpus) throw new Error(usage().trim());
    const result = corpusReadiness(JSON.parse(await readFile(resolve(options.corpus), 'utf8')));
    if (options.report) await writeFile(resolve(options.report), `${JSON.stringify(result, null, 2)}\n`);
    process.stdout.write(`${JSON.stringify(result, null, 2)}\n`);
    process.exitCode = result.ready ? 0 : 1;
  } else if (command === 'compare') {
    if (!options.expected || !options.actual) throw new Error(usage().trim());
  const requiredCategories = options.require ? options.require.split(',').filter(Boolean) : undefined;
  const result = await compareTraces(readTraceJsonLines(resolve(options.expected)), readTraceJsonLines(resolve(options.actual)), {requiredCategories});
  if (options.report) await writeFile(resolve(options.report), `${JSON.stringify(result, null, 2)}\n`);
  process.stdout.write(formatResult(result));
  process.exitCode = exitCodeForResult(result);
  } else throw new Error(usage().trim());
} catch (error) {
  process.stderr.write(`${error.stack ?? error}\n${usage()}`);
  process.exitCode = 2;
}
