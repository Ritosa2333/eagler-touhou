import {execFile} from 'node:child_process';
import {existsSync,mkdirSync,rmSync} from 'node:fs';
import path from 'node:path';
import {setTimeout as delay} from 'node:timers/promises';

/** Independent source consumers may bundle before the Launcher build runs. */
async function authoredMessages(project) {
  const output=path.resolve(project,'src/paraglide');
  const messages=path.resolve(output,'messages.js');
  const lock=path.resolve(project,'.cache/authored-messages.lock');
  if (existsSync(messages) && !existsSync(lock)) return messages;
  mkdirSync(path.dirname(lock),{recursive:true});
  const release=()=>rmSync(lock,{recursive:true,force:true});
  const started=Date.now();
  for (;;) {
    try {mkdirSync(lock);process.once('exit',release);break;}
    catch (error) {
      if (error.code!=='EEXIST') throw error;
      if (Date.now()-started>120000) throw new Error('Authored source message compilation is still locked');
      await delay(25);
    }
  }
  try {
    if (!existsSync(messages)) {
      try {
        await new Promise((yes,no)=>{
          execFile(process.execPath,[path.resolve(project,'scripts/compile-i18n.mjs')],
            {cwd:project,windowsHide:true,maxBuffer:8*1024*1024},(error,stdout,stderr)=>
              error ? no(new Error(`Authored source message compilation failed:\n${stdout}${stderr}`,{cause:error})) : yes());
        });
        if (!existsSync(messages)) throw new Error('Authored source message compiler did not produce messages.js');
      } catch (error) {
        rmSync(output,{recursive:true,force:true});
        throw error;
      }
    }
    return messages;
  } finally {process.off('exit',release);release();}
}

/** Resolve only existing authored siblings of NodeNext .mjs imports. Both
 * build adapters use this boundary instead of importing compiled Node facades.
 * The injectable path/existence functions also exercise Windows paths on CI. */
export function resolveAuthoredSource(project, source, importer, paths = path, exists = existsSync) {
  if (!importer || !/^\.\.?\//.test(source) || !source.endsWith('.mjs')) return;
  const root = paths.resolve(project);
  const absolute = paths.resolve(paths.dirname(importer.split('?')[0]), source);
  if (absolute.startsWith(paths.resolve(root, 'src') + paths.sep)) {
    const authored = absolute.slice(0, -4) + '.mts';
    if (exists(authored)) return authored;
  }
  for (const directory of ['', 'lib/contracts']) {
    const prefix = paths.resolve(root, directory) + paths.sep;
    if (!absolute.startsWith(prefix) || absolute.slice(prefix.length).includes(paths.sep)) continue;
    const authored = paths.resolve(root, 'src/contracts', absolute.slice(prefix.length, -4) + '.mts');
    if (exists(authored)) return authored;
  }
}

export function authoredSourcesPlugin(project) {
  return {name: 'main-authored-sources', setup(context) {
    context.onResolve({filter:/paraglide[\\/]messages\.js$/},async args=>{
      if (path.resolve(path.dirname(args.importer),args.path)!==path.resolve(project,'src/paraglide/messages.js')) return;
      return {path:await authoredMessages(project)};
    });
    context.onResolve({filter: /^\.\.?\/.*\.mjs$/}, args => {
      const authored = resolveAuthoredSource(project, args.path, args.importer);
      if (authored) return {path: authored};
    });
  }};
}
