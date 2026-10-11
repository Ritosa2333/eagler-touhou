import {readFileSync} from 'node:fs';
import assert from 'node:assert/strict';
import vm from 'node:vm';
import {transformSync} from 'esbuild';
import {build} from 'esbuild';
import {mkdir,mkdtemp,rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
await mkdir('.cache',{recursive:true});const work=await mkdtemp(resolve('.cache/font-selection-'));
const outfile=resolve(work,'actual.mjs');
await build({stdin:{contents:"export * from './app/services/game-resources.ts';export {PRODUCT_GAMES as TEST_PRODUCTS} from 'test-products';",resolveDir:resolve(),loader:'ts'},outfile,bundle:true,platform:'node',format:'esm',logLevel:'silent',
 plugins:[{name:'test-product-port',setup(ctx){
  ctx.onResolve({filter:/^(?:test-products|.*contracts\/product-catalog\.mts)$/},args=>{if(args.path==='test-products'||args.importer.replaceAll('\\','/').endsWith('/app/services/game-resources.ts'))return{path:'products',namespace:'fixture'};});
  ctx.onLoad({filter:/.*/,namespace:'fixture'},()=>({loader:'js',contents:'export const PRODUCT_GAMES={fixture:{}};'}));
 }},{name:'authored-contracts',setup(ctx){ctx.onResolve({filter:/\.mjs$/},args=>{if(!args.importer)return;const path=resolve(args.importer,'..',args.path).replace(/\.mjs$/,'.mts');try{readFileSync(path);return{path};}catch{}});}}]});
const api=await import(pathToFileURL(outfile).href);
async function select(product,language='ja',practice=false,installed=[]){
 // The product declaration is substituted through the module seam, not an extracted legacy UI function.
 api.TEST_PRODUCTS.fixture=product;
 const {sharedResources}=api;
 const files=Object.fromEntries(installed.map((target,i)=>[i,{target}]));
 const generation=installed.length?{descriptor:{base:{files:Object.keys(files)},files},files:Object.fromEntries(Object.keys(files).map(k=>[k,{objectId:'installed'}]))}:null;
 return sharedResources({game:'fixture',language,thprac:practice,generation,host:{shared:{vanillaFont:'shared/msgothic.ttc',unicodeFont:'shared/unifont.otf'}},baseUrl:'https://mirror.invalid/',translate:x=>x}).map(x=>x.path);
}
assert.deepEqual(await select({requiredShared:['/unifont.otf']}),[],'TH11 Japanese baked fonts must not request msgothic');
assert.deepEqual(await select({requiredShared:['/unifont.otf']},'ja',true),['/unifont.otf']);
assert.deepEqual(await select({requiredShared:['/unifont.otf']},'lang_zh-hans'),['/unifont.otf']);
assert.deepEqual(await select({requiredShared:['/msgothic.ttc','/unifont.otf']},'ja',true),['/msgothic.ttc','/unifont.otf']);
assert.deepEqual(await select({},'ja'),['/msgothic.ttc'],'Legacy Runtime selection remains supported');
assert.deepEqual(await select({requiredShared:[]}),[]);
assert.deepEqual(await select({requiredShared:['/unifont.otf']},'ja',true,['/unifont.otf']),[],'Installed font must not download twice');
console.log('Shared font selection: Japanese baked fonts, practice, localization, legacy and installed-font cases PASS');

await rm(work,{recursive:true,force:true});
export { select };
