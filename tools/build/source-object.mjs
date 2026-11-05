import {createHash} from 'node:crypto';
import {relative,resolve} from 'node:path';

/** Absolute shared inputs must never replace the selected object directory. */
export function sourceObjectPath(directory, workspace, source) {
  const name=relative(workspace,resolve(source)).replaceAll('\\','/');
  return resolve(directory,createHash('sha256').update(name).digest('hex')+'.o');
}
