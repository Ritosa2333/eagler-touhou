/** Reproducible publication/format tests. Synthetic programs/data only; this is
 * not gameplay or real browser acceptance. Exercises the actual generators. */
import assert from "node:assert/strict";
import { createHash } from "node:crypto";
import { cp, lstat, mkdtemp, mkdir, readFile, readdir, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { dirname, join, resolve } from "node:path";
import { execFile } from "node:child_process";
import { promisify } from "node:util";
import { fileURLToPath } from "node:url";
import { writeRuntimeGeneration, publishRuntimeManifest, readRuntimeManifest, verifyRuntimePublication,
  runtimeGenerationId, assertPortableRuntimeSources } from "../lib/runtime-generations.mjs";
import { RUNTIME_MANIFEST_FILE, RUNTIME_PREPARE, canonicalRuntimePayload, parseRuntimeGenerationPath,
  validateRuntimeManifest, validateRuntimePrepareRequest } from "../lib/contracts/runtime-generations.mjs";
import { PRODUCT_GAMES } from "../lib/contracts/product-catalog.mjs";
import { createPublicationHostSeed } from "../lib/publication-host-seed.mjs";
import { writeSyntheticRuntimeRelease } from "./support/runtime-release-fixture.mjs";
import { verifyRuntimeRelease, LEGACY_RUNTIME_RELEASE_SCHEMA, runtimeFileNames } from "../lib/runtime-release.mjs";
import { deployStaticSite } from "../lib/static-deployment.mjs";

const project = resolve(fileURLToPath(new URL("..", import.meta.url)));
const temp = await mkdtemp(join(tmpdir(), "eagler-generations-"));
const run = promisify(execFile);
const command = (...args) => run(process.execPath, args, { cwd: project, maxBuffer: 8 * 1024 * 1024 });
const json = async path => JSON.parse(await readFile(path, "utf8"));
try {
  const source = resolve(temp, "source"), site = resolve(temp, "unit-site");
  await mkdir(source, { recursive: true });
  await writeFile(resolve(source, "game.html"), '<script type="module" src="./shell.mjs"></script>');
  await writeFile(resolve(source, "shell.mjs"), 'import "./glue.mjs"; new Worker(new URL("./worker.mjs", import.meta.url), {type:"module"});');
  await writeFile(resolve(source, "worker.mjs"), 'import "./glue.mjs";');
  await writeFile(resolve(source, "glue.mjs"), 'fetch(new URL("./game.wasm", import.meta.url));');
  await writeFile(resolve(source, "game.wasm"), new Uint8Array([0,97,115,109,1,0,0,0]));
  const names = await readdir(source);
  const a = await writeRuntimeGeneration({site, source, root:"runtime/th08/", entry:"game.html", names});
  assert.equal(runtimeGenerationId(a.entry, [...a.files].reverse()), a.generation, "filesystem enumeration order is not identity");
  assert.equal((await writeRuntimeGeneration({site, source, root:"runtime/th08/", entry:"game.html", names})).generation, a.generation);
  await publishRuntimeManifest(site, [{root:"runtime/th08/",current:a}]);
  assert.equal(parseRuntimeGenerationPath(`runtime/th08/${a.generation}/game.wasm`).generation, a.generation);
  for(const bad of ["runtime/th08/../x", "runtime/th08//x", "/runtime/th08/x"]) assert.equal(parseRuntimeGenerationPath(bad), null);
  assert.throws(()=>canonicalRuntimePayload(a.entry,[...a.files,a.files[0]]), /identity/);
  assert.throws(()=>validateRuntimePrepareRequest({type:RUNTIME_PREPARE,entry:"runtime/th08/game.html",exclude:["bad"]}), /exclusions/);
  assert.throws(()=>validateRuntimePrepareRequest({type:RUNTIME_PREPARE,entry:"../private.html"}), /entry/);
  assert.equal(validateRuntimePrepareRequest({type:RUNTIME_PREPARE,entry:"runtime/th08/game.html",localOnly:true}).localOnly,true);
  assert.throws(()=>validateRuntimePrepareRequest({type:RUNTIME_PREPARE,entry:"runtime/th08/game.html",localOnly:'yes'}), /local-only/);
  assert.throws(()=>assertPortableRuntimeSources(new Map([["shell.mjs",Buffer.from('import "../mutable/glue.mjs";')]])), /escapes/);
  assert.throws(()=>assertPortableRuntimeSources(new Map([["shell.mjs",Buffer.from('new Worker("missing.mjs");')]])), /not declared/);
  await writeFile(resolve(source,"shell.mjs"), 'import "./glue.mjs"; // next generation');
  const b = await writeRuntimeGeneration({site, source, root:"runtime/th08/", entry:"game.html", names});
  await publishRuntimeManifest(site,[{root:"runtime/th08/",current:b}]);
  assert.deepEqual((await readRuntimeManifest(site)).groups[0].previous.map(x=>x.generation),[a.generation]);
  const pointer = await readFile(resolve(site,RUNTIME_MANIFEST_FILE));
  await writeFile(resolve(site,"runtime/th08",b.generation,"game.wasm"),"corrupt");
  await assert.rejects(writeRuntimeGeneration({site, source, root:"runtime/th08/", entry:"game.html", names}), /mismatch/);
  await assert.rejects(publishRuntimeManifest(site,[{root:"runtime/th08/",current:b}]), /mismatch/);
  assert.deepEqual(await readFile(resolve(site,RUNTIME_MANIFEST_FILE)),pointer,"a failed candidate must not change the live pointer");

  const release = resolve(temp,"release"), output = resolve(temp,"output");
  const runtimeRelease = await writeSyntheticRuntimeRelease(release);
  await verifyRuntimeRelease(release);
  // Legacy Runtime Release reader is bounded compatibility, not a legacy writer.
  const legacy = resolve(temp,"legacy-release");
  const old = structuredClone(runtimeRelease); old.schema = LEGACY_RUNTIME_RELEASE_SCHEMA;
  for(const [game,entry] of Object.entries(old.games)) for(const field of ["runtime","multiplayerRuntime"]) if(entry[field]) {
    const program=entry[field], target=`runtime/${game}/${field === "multiplayerRuntime" ? "multiplayer" : ""}`.replace(/\/$/,"");
    for(const name of Object.keys(program.files)) {await mkdir(dirname(resolve(legacy,target,name)),{recursive:true});await cp(resolve(release,program.root,name),resolve(legacy,target,name));}
    program.root=target;delete program.generation;
  }
  await writeFile(resolve(legacy,"runtime-release.json"),JSON.stringify(old));
  await verifyRuntimeRelease(legacy);
  const seed = createPublicationHostSeed("web-validation-generations");
  seed.schema="eagler-touhou/site-manifest/1";
  seed.shared={resourceMode:"hosted",vanillaFont:"shared/msgothic.ttc?v=f",unicodeFont:"shared/unifont.otf?v=g"};
  for(const [game,product] of Object.entries(PRODUCT_GAMES))Object.assign(seed.games[game],{
    runtime:`runtime/${game}/${game}.html?hosted=1&v=old`,
    ...(product.multiplayerRuntime?{multiplayerRuntime:`runtime/${game}/multiplayer/${game}.html?hosted=1&v=old`}:{}),
    gameData:{path:product.package.dataTarget.slice(1),bytes:4,sha256:"a".repeat(64),version:`sha256-${"a".repeat(64)}`,layout:runtimeRelease.games[game].dataLayout},
    features:{thprac:false,focusHitbox:false}, music:{midi:{files:[],...(product.musicCapabilities.midi?{}:{supported:false})}},
  });
  const features={schema:"eagler-touhou/server-features/1",resourceMode:"import",games:Object.fromEntries(Object.entries(PRODUCT_GAMES).map(([game,product])=>[game,{
    ...(product.features.languages?{languages:["ja"]}:{}),...(product.features.thprac?{thprac:false}:{})}]))};
  await writeFile(resolve(temp,"seed.json"),JSON.stringify(seed));
  await writeFile(resolve(temp,"features.json"),JSON.stringify(features));
  await command("scripts/package-server.mjs",`--output=${output}`,`--runtime-release=${release}`,`--site-manifest=${resolve(temp,"seed.json")}`,
    `--feature-config=${resolve(temp,"features.json")}`,"--profile=web-validation-generations","--games=th06,th07,th08,th10","--music=midi");
  await command("scripts/verify-server-build.mjs",output);
  const host = await json(resolve(output,"site-manifest.json"));
  const catalog = await verifyRuntimePublication(output,host);
  assert.deepEqual(catalog.groups.map(group => group.root).sort(),
    ["th06", "th07", "th08", "th10"].flatMap(game => [
      `runtime/${game}/`, ...(PRODUCT_GAMES[game].multiplayerRuntime ? [`runtime/${game}/multiplayer/`] : []),
    ]).sort(), "each selected game and declared multiplayer variant needs an independent Runtime generation");

  // A validation Host may intentionally contain one product's normal Runtime
  // only. Exercise package-server's direct-build input path with a real closed
  // directory fixture and prove the default still requires its MP counterpart.
  const th075Build = resolve(temp, "th075-normal-build");
  const th075Runtime = runtimeRelease.games.th075.runtime;
  await mkdir(th075Build, { recursive: true });
  for (const name of runtimeFileNames("th075", th075Runtime.files)) {
    await cp(resolve(release, th075Runtime.root, name), resolve(th075Build, name));
  }
  await writeFile(resolve(th075Build, "runtime-files.json"), JSON.stringify({
    schema: "eagler-touhou/runtime-directory/1", files: th075Runtime.files,
  }, null, 2));
  const defaultNeedsMultiplayer = await command("scripts/package-server.mjs",
    `--output=${resolve(temp, "default-needs-mp")}`, `--th075-build=${th075Build}`,
    `--site-manifest=${resolve(temp, "seed.json")}`, `--feature-config=${resolve(temp, "features.json")}`,
    "--profile=web-validation-generations", "--games=th075", "--music=midi")
    .then(() => "", error => `${error.stdout || ""}\n${error.stderr || ""}\n${error.message}`);
  assert.match(defaultNeedsMultiplayer, /missing --th075-multiplayer-build=PATH/,
    "default packaging continues to require both declared Runtime variants");
  const normalOnly = resolve(temp, "normal-only-output");
  await command("scripts/package-server.mjs", `--output=${normalOnly}`, `--th075-build=${th075Build}`,
    `--site-manifest=${resolve(temp, "seed.json")}`, `--feature-config=${resolve(temp, "features.json")}`,
    "--profile=web-validation-generations", "--games=th075", "--music=midi", "--runtime-variants=normal");
  await command("scripts/verify-server-build.mjs", normalOnly);
  const normalOnlyHost = await json(resolve(normalOnly, "site-manifest.json"));
  assert.equal(normalOnlyHost.games.th075.multiplayerRuntime, undefined,
    "a validation Host must not retain the seed's stale multiplayer Runtime URL");
  const normalOnlyRuntime = await verifyRuntimePublication(normalOnly, normalOnlyHost);
  assert.deepEqual(normalOnlyRuntime.groups.map(group => group.root), ["runtime/th075/"],
    "Runtime generations must include exactly the selected normal variant");
  const releaseVariantFailure = await command("scripts/package-server.mjs",
    `--output=${resolve(temp, "release-normal-only")}`, `--th075-build=${th075Build}`,
    "--profile=web-release-runtime-variants", "--games=th075", "--runtime-variants=normal")
    .then(() => "", error => `${error.stdout || ""}\n${error.stderr || ""}\n${error.message}`);
  assert.match(releaseVariantFailure, /restricted to web-validation-\* packaging/,
    "publication profiles must reject the normal-only validation option");

  for(const [game,entry] of Object.entries(host.games))for(const field of ["runtime","multiplayerRuntime"])if(entry[field]) {
    assert.equal(parseRuntimeGenerationPath(entry[field].split("?")[0]).generation,runtimeRelease.games[game][field].generation);
  }
  const generations=catalog.groups.map(group=>group.current.generation);
  await command("scripts/refresh-deployment-app-shell.mjs",output,"--frontend");
  assert.deepEqual((await readRuntimeManifest(output)).groups.map(group=>group.current.generation),generations,"Launcher-only refresh does not rebuild game generations");
  const deployment=await json(resolve(output,"deployment.json"));
  if (deployment.appShell) assert.ok(deployment.appShell.entries.every(path=>!path.startsWith("runtime/") && path!==RUNTIME_MANIFEST_FILE));
  else {
    assert.ok(deployment.files.some(file=>file.path==="app-shell-sw.js"),"disabled offline mode retains its upgrade endpoint");
    const {APP_SHELL_RETIREMENT_WORKER} = await import('../lib/app-shell-retirement.mjs');
    assert.equal(await readFile(resolve(output,"app-shell-sw.js"),"utf8"),APP_SHELL_RETIREMENT_WORKER);
  }
  await command("scripts/verify-server-build.mjs",output);
  const store=resolve(temp,"releases"),current=resolve(temp,"current");
  if (process.platform === "win32") {
    await assert.rejects(deployStaticSite({source:output,releases:store,current}),
      {code:"EAGLER_ATOMIC_DEPLOYMENT_UNSUPPORTED_PLATFORM"});
    for (const path of [store,current]) await assert.rejects(lstat(path),{code:"ENOENT"});
    console.log("Immutable Runtime generation/Host/refresh: PASS; atomic deployment: Windows early refusal PASS (POSIX switch runs in Linux CI)");
  } else {
  const first=await deployStaticSite({source:output,releases:store,current});
  assert.equal((await lstat(current)).isSymbolicLink(),true);
  const second=await deployStaticSite({source:output,releases:store,current});
  assert.equal(second.previous,first.release);
  await command("scripts/verify-server-build.mjs",current);
  assert.deepEqual((await readRuntimeManifest(current)).groups.map(group=>group.current.generation),generations);
  await writeFile(resolve(output,"runtime-manifest.json"),"broken");
  await assert.rejects(deployStaticSite({source:output,releases:store,current}),/mismatch/);
  await command("scripts/verify-server-build.mjs",current);
  console.log("Immutable Runtime protocol, generator, legacy read, declared-variant Host, refresh and atomic deployment: PASS");
  }
} finally { await rm(temp,{recursive:true,force:true}); }
