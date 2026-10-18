import {mkdir,open,unlink} from 'node:fs/promises';
import {resolve} from 'node:path';

/** Concurrent invocations must never replace each other's evidence. */
export async function acquireOutputLock(output) {
  await mkdir(output,{recursive:true});
  const path=resolve(output,'.running.json');
  let handle;
  try {handle=await open(path,'wx');}
  catch (error) {
    if (error.code === 'EEXIST') throw new Error(`Test output is already owned by another invocation: ${path}`);
    throw error;
  }
  await handle.writeFile(JSON.stringify({pid:process.pid,startedAt:new Date().toISOString()})+'\n');
  return async () => {await handle.close();await unlink(path);};
}
