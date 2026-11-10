import {resolve} from 'node:path';
import {fileURLToPath,pathToFileURL} from 'node:url';

const pending=new Map();
const defaultProject=fileURLToPath(new URL('..',import.meta.url));
export function buildAuthoredContent(project=defaultProject){
  project=resolve(project);
  if(pending.has(project))return pending.get(project);
  const work=import(pathToFileURL(resolve(project,'scripts/build-content-pages.mjs')).href)
    .then(compiler=>compiler.buildContentPages({project,quiet:true}))
    .finally(()=>{if(pending.get(project)===work)pending.delete(project);});
  pending.set(project,work);return work;
}
