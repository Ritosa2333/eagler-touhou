import {execFileSync} from 'node:child_process';
import {mkdtemp, writeFile, rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {join,dirname,resolve} from 'node:path';

// Workspace-only checks run real portable headers. CXX is a compiler executable,
// not a shell command. All generated inputs and programs stay in system temp.
export async function runNativeCpp(source, {includes = []} = {}) {
  const directory = await mkdtemp(join(tmpdir(), 'eagler-native-check-'));
  try {
    const input = join(directory, 'check.cpp');
    const output = join(directory, process.platform === 'win32' ? 'check.exe' : 'check');
    await writeFile(input, source);
    execFileSync(process.env.CXX || 'g++', ['-std=c++17', '-O2', ...includes.flatMap(path => ['-I', path]), input, '-o', output],
      {encoding:'utf8', windowsHide:true, timeout:60000});
    return execFileSync(output, [], {encoding:'utf8', windowsHide:true, timeout:30000});
  } finally {
    if (dirname(resolve(directory)) !== resolve(tmpdir())) throw new Error('Unexpected native-check temporary directory');
    await rm(directory, {recursive:true, force:true});
  }
}
