import {execFileSync} from 'node:child_process';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import type {Plugin} from 'vite';
import {helpDocuments} from '../../app/components/documents/catalog.ts';

export interface DocumentRevision {author?:string;date?:string;commit?:string;pending?:boolean}
export function readDocumentHistory(project:string) {
  const git=(...args:string[])=>execFileSync('git',args,{cwd:project,encoding:'utf8',stdio:['ignore','pipe','ignore']}).trim();
  let repository=false;try{repository=git('rev-parse','--is-inside-work-tree')==='true';}catch{}
  const output:Record<string,Record<string,DocumentRevision>>={};
  for(const locale of ['zh-CN','en','ja']) {
    const entries=output[locale]={} as Record<string,DocumentRevision>;
    for(const doc of helpDocuments) {
      try {
        if(locale==='zh-CN') {
          const [commit,author,date]=git('log','-1','--follow','--format=%H%x00%aN%x00%aI','--',doc.source).split('\0');
          if(!commit) {entries[doc.id]={pending:true};continue;}
          let pending=false;try{git('diff','--quiet','HEAD','--',doc.source);}catch{pending=true;}
          entries[doc.id]={author,date,commit,pending};
        }else {
          const file=`messages/${locale}.json`,key=doc.content.replaceAll('.','_');
          const line=readFileSync(resolve(project,file),'utf8').split(/\r?\n/).findIndex(text=>text.trimStart().startsWith(`"${key}":`))+1;
          if(!line)continue;
          const blame=git('blame','--line-porcelain','-L',`${line},${line}`,'--',file);
          const commit=blame.split(' ')[0];
          if(/^0+$/.test(commit)){entries[doc.id]={pending:true};continue;}
          const author=/^author (.*)$/m.exec(blame)?.[1],timestamp=/^author-time (\d+)$/m.exec(blame)?.[1];
          entries[doc.id]={author,commit,date:timestamp?new Date(Number(timestamp)*1000).toISOString():undefined};
        }
      }catch{entries[doc.id]=repository?{pending:true}:{};}
    }
  }
  return output;
}
export function documentHistory(project:string):Plugin {
  const id='virtual:document-history';
  return {name:'documentation-history',resolveId(source){if(source===id)return '\0'+id;},load(source){
    if(source!=='\0'+id)return;
    for(const doc of helpDocuments)this.addWatchFile(resolve(project,doc.source));
    for(const locale of ['en','ja'])this.addWatchFile(resolve(project,`messages/${locale}.json`));
    return `export default ${JSON.stringify(readDocumentHistory(project))}`;
  }};
}
