import {createServer} from 'node:http';
import {readFile,readdir} from 'node:fs/promises';
import {resolve,join,extname} from 'node:path';
import {fileURLToPath} from 'node:url';
const here = fileURLToPath(new URL('.', import.meta.url));
const args = Object.fromEntries(process.argv.slice(2).map(arg => {const i=arg.indexOf('='); return [arg.slice(2,i),arg.slice(i+1)];}));
if (!args.build || !args.content || !args.ui) throw Error('使用 --build=<构建目录> --content=<游戏资源目录> --ui=<PODDUI目录>');
const build = resolve(args.build), content = resolve(args.content), ui = resolve(args.ui);
async function list(directory, prefix='') {
  const result=[];
  for (const entry of await readdir(directory,{withFileTypes:true})) {
    const name = prefix ? prefix+'/'+entry.name : entry.name;
    if (entry.isDirectory()) result.push(...await list(join(directory,entry.name),name));
    else if (entry.isFile()) result.push(name);
  }
  return result;
}
const originals = await list(content), uiFiles = await list(ui);
const fileMap = new Map([...originals.map(name => [name,join(content,name)]), ...uiFiles.map(name => ['PODDUI/'+name,join(ui,name)])]);
const files = [...fileMap.keys()];
const staticFiles = new Map([['/',join(here,'index.html')],['/game.js',join(here,'game.js')],['/fire-control.js',join(here,'../web/fire-control.js')],['/direct-drag.js',join(here,'../web/direct-drag.js')],['/classic.js',join(build,'classic.js')],['/classic.wasm',join(build,'classic.wasm')]]);
const server = createServer(async (req,res) => {
  try {
    res.setHeader('Cache-Control','no-store');
    if (req.url === '/content-list') {res.setHeader('Content-Type','application/json'); return res.end(JSON.stringify(files));}
    let path = staticFiles.get(req.url);
    if (!path && req.url.startsWith('/content/')) {const name=decodeURIComponent(req.url.slice(9)); path=fileMap.get(name);}
    if (!path) return res.writeHead(404).end();
    const type={'.html':'text/html; charset=utf-8','.js':'text/javascript','.wasm':'application/wasm'}[extname(path)];
    if (type) res.setHeader('Content-Type',type);
    res.end(await readFile(path));
  } catch {res.writeHead(500).end();}
});
server.listen(Number(args.port || 18332),'127.0.0.1',()=>console.log(`TH03 单机：http://127.0.0.1:${server.address().port}/`));
