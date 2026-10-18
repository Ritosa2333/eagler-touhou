import {resolve,relative,isAbsolute,sep} from 'node:path';
import {fileURLToPath} from 'node:url';

export const sourceDependencyTool=fileURLToPath(import.meta.url);

// A title's compiler closure may include the shared common owner. Other title
// sources, SDK headers and unrelated workspace files remain outside this scope.
export function isSourceDependency(root,name) {
  const path=resolve(root,name);
  if(path===sourceDependencyTool)return true;
  return [resolve(root),resolve(root,'../../common')].some(owner=>{
    const relativeName=relative(owner,path);
    return relativeName!=='..'&&!relativeName.startsWith('..'+sep)&&!isAbsolute(relativeName);
  });
}
