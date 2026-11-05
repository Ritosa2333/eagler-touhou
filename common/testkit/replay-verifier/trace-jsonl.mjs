import {createReadStream, createWriteStream} from 'node:fs';
import {createInterface} from 'node:readline';
import {validateRecord} from './contracts.mjs';

export async function* readTraceJsonLines(path) {
  const input = createReadStream(path, {encoding: 'utf8'});
  const lines = createInterface({input, crlfDelay: Infinity});
  let lineNumber = 0;
  try {
    for await (const line of lines) {
      lineNumber++;
      if (!line.trim()) continue;
      let record;
      try { record = JSON.parse(line); }
      catch (error) { throw new SyntaxError(`${path}:${lineNumber}: ${error.message}`); }
      try { yield validateRecord(record); }
      catch (error) { throw new TypeError(`${path}:${lineNumber}: ${error.message}`); }
    }
  } finally {
    lines.close();
    input.destroy();
  }
}
export async function writeTraceJsonLines(path, records) {
  const output = createWriteStream(path, {encoding: 'utf8'});
  const closed = new Promise((resolve, reject) => {
    output.once('finish', resolve);
    output.once('error', reject);
  });
  try {
    for await (const value of records) {
      const record = validateRecord(value);
      if (!output.write(`${JSON.stringify(record)}\n`))
        await new Promise(resolve => output.once('drain', resolve));
    }
    output.end();
    await closed;
  } catch (error) {
    output.destroy(error);
    await closed.catch(() => {});
    throw error;
  }
}
