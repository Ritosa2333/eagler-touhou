/**
 * L3/module: execute the actual inline preload shell scripts with mocked DOM/FS.
 * Preconditions: source shells; no game assets or browser required.
 * Mutations: isolated VM filesystem, deferred restore/persist callbacks.
 * Invariant: restore fails closed; command replies wait for persistence.
 * Proves: shell callback/protocol ordering, path validation, exact bytes.
 * Does NOT prove: real IDBFS/browser persistence, WASM or gameplay.
 */
import assert from "node:assert/strict";
import { readFile } from "node:fs/promises";
import vm from "node:vm";
import { PRODUCT_GAMES, HOST_PROTOCOL } from "../lib/contracts/product-catalog.mjs";
import { workspacePath } from "../lib/workspace-layout.mjs";

const shells = {
  th06: workspacePath("th06", "resources", "shell.html"),
  th07: workspacePath("th07", "resources", "shell.html"),
};
const preloadGames = Object.entries(PRODUCT_GAMES)
  .filter(([, product]) => product.dataProvider === "emscripten-preload")
  .map(([game]) => game)
  .sort();
assert.deepEqual(Object.keys(shells).sort(), preloadGames,
  "preload Runtime storage verification must be updated when a formal emscripten-preload adapter is registered");

function harness(source, game) {
  const listeners = new Map(), timers = new Map(), dependencies = new Set();
  const replies = [], pending = [], files = new Map();
  const saveMount = { idbPersistState: 0 };
  const parent = { postMessage: message => replies.push(message) };
  const element = { addEventListener() {}, focus() {}, style: { setProperty() {} },
    getBoundingClientRect: () => ({left:0,top:0,width:640,height:480}) };
  const saveRoot = PRODUCT_GAMES[game].storage.saveRoot;
  let timerId = 0;
  const context = {
    console: { log() {}, warn() {}, error() {} }, URL, URLSearchParams, Uint8Array, TextDecoder,
    Request, Response, AbortController, performance,
    navigator: { userAgent:"contract",maxTouchPoints:0 },
    location: { search:"?hosted=1",origin:"http://test.local",href:"http://test.local/game.html?hosted=1" },
    parent, innerWidth:640,innerHeight:480,
    document: { visibilityState:"visible",documentElement:{...element,clientWidth:640,clientHeight:480},
      getElementById:()=>element,addEventListener() {} },
    addEventListener: (name, fn) => listeners.set(name, fn),
    setTimeout: fn => {timers.set(++timerId, fn);return timerId;},clearTimeout: id=>timers.delete(id),
    fetch: async()=>new Response(new Uint8Array()),
    FS: {
      mkdir() {},mkdirTree() {},mount(_type,_options,root) {assert.equal(root,saveRoot);return {mount:saveMount};},
      chdir() {throw Error("save mount must not become cwd");},
      syncfs(populate, callback) {pending.push({populate,callback});},
      writeFile(path, bytes) {files.set(path, Uint8Array.from(bytes));},
      readFile(path) {if(!files.has(path)) throw Error("missing file");return files.get(path);},
      unlink(path) {if(!files.delete(path)) throw Error("missing file");},
      readdir(path) {return [".","..",...[...files.keys()].filter(k=>k.startsWith(path+"/")).map(k=>k.slice(path.length+1))];},
      stat(path) {return {mode:0,size:files.get(path)?.length || 0};},isDir:()=>false,
    },
    IDBFS:{},addRunDependency:name=>dependencies.add(name),removeRunDependency:name=>dependencies.delete(name),
  };
  context.window=context;
  vm.runInNewContext(source,context,{filename:shells[game]});
  return {context,replies,pending,timers,dependencies,files,saveMount,
    send: (command, payload={}) => listeners.get("message")({origin:context.location.origin,source:parent,
      data:{protocol:HOST_PROTOCOL,game,command,request:`test-${replies.length}`, ...payload}})};
}

for (const [game, shell] of Object.entries(shells)) {
  const html=await readFile(shell,"utf8");
  const source=html.match(/<script>\s*([\s\S]*?)<\/script>/)?.[1];
  assert.ok(source,`${game}: inline source`);
  for (const outcome of ["success","failure","timeout"]) {
    const h=harness(source,game);
    h.context.Module.preRun.forEach(fn=>fn());
    assert.equal(h.dependencies.size,1,`${game}: restore holds run dependency`);
    assert.equal(h.pending.length,1);
    assert.equal(h.pending[0].populate,true);
    assert.equal(h.saveMount.idbPersistState,"restore",`${game}: autoPersist paused during populate`);
    assert.equal(h.replies.some(m=>m.event==="ready"),false);
    if(outcome==="timeout") {
      for(const fn of [...h.timers.values()]) fn();
      h.pending.shift().callback(null); // Late success must not reopen startup.
    } else h.pending.shift().callback(outcome==="failure" ? Error("injected restore failure") : null);
    assert.equal(h.dependencies.size,outcome==="success"?0:1,`${game}: ${outcome} barrier`);
    assert.equal(h.saveMount.idbPersistState,outcome==="success"?0:"restore",`${game}: autoPersist release follows restore success`);
    if(outcome!=="success") assert.ok(h.replies.some(m=>m.event==="error"),`${game}: failure visible to host`);
  }
  const h=harness(source,game);
  const payload=[0,1,127,128,255];
  const before=h.replies.length;
  const writing=h.send("write",{path:"contract.dat",bytes:payload});
  assert.equal(h.replies.length,before,`${game}: no premature write acknowledgement`);
  assert.equal(h.pending[0].populate,false);
  h.pending.shift().callback(null);
  await writing;
  assert.equal(h.replies.at(-1).ok,true);
  await h.send("read",{path:"contract.dat"});
  assert.deepEqual(Array.from(h.replies.at(-1).bytes),payload);
  await h.send("list");
  assert.ok(h.replies.at(-1).files.some(f=>f.path==="contract.dat" && f.size===payload.length));
  const syncing=h.send("sync");
  h.pending.shift().callback(Error("injected persistence failure"));
  await syncing;
  assert.equal(h.replies.at(-1).ok,false,`${game}: sync failure propagated`);
  const removing=h.send("remove",{path:"contract.dat"});
  h.pending.shift().callback(null);await removing;
  await h.send("read",{path:"contract.dat"});
  assert.equal(h.replies.at(-1).ok,false);
  for(const path of ["../escape","/escape","a/../escape","a\\escape","","a//b"]) {
    await h.send("write",{path,bytes:payload});
    assert.equal(h.replies.at(-1).ok,false,`${game}: reject ${JSON.stringify(path)}`);
  }
  assert.equal(h.files.size,0,`${game}: invalid writes must not mutate storage`);
  console.log(`${game}: Runtime Storage L3/module PASS`);
}
