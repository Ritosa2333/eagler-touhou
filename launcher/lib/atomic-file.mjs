import { randomUUID } from "node:crypto";
import { mkdir, rename, rm, writeFile } from "node:fs/promises";
import { basename, dirname, resolve } from "node:path";
import {setTimeout as delay} from 'node:timers/promises';

/** Replace one generated file without exposing a truncated intermediate file. */
export async function writeFileAtomic(path, data, options) {
  const directory = dirname(path);
  await mkdir(directory, { recursive: true });
  const temporary = resolve(directory, `.${basename(path)}.${process.pid}-${randomUUID()}.tmp`);
  try {
    const writeOptions = typeof options === "string"
      ? { encoding: options, flag: "wx" }
      : { ...options, flag: "wx" };
    await writeFile(temporary, data, writeOptions);
    for (let attempt = 0; ; attempt++) {
      try {await rename(temporary, path); break;}
      catch (error) {
        if (attempt >= 5 || !['EPERM', 'EBUSY'].includes(error.code)) throw error;
        await delay(10 * 2 ** attempt);
      }
    }
  } finally {
    await rm(temporary, { force: true });
  }
}
