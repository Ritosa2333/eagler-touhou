import {test} from 'vitest';
import assert from "node:assert/strict";
import { execFileSync, spawnSync } from "node:child_process";
import { mkdtemp, mkdir, copyFile, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";

test("test-publication-audit", async () => {
/** L3/tool behavior. Synthetic temporary Git repo only. Proves ignored local
 * bytes don't become source candidates, force-added bytes and concrete exports
 * are rejected. Does not authorize publishing or inspect real game data. */






const root=await mkdtemp(join(tmpdir(),"eagler-publication-audit-"));
await mkdir(join(root,"scripts"));
await copyFile(new URL("../scripts/audit-publication.mjs",import.meta.url),join(root,"scripts/audit-publication.mjs"));
await writeFile(join(root,".gitignore"),"*.dat\n");
await writeFile(join(root,"private.dat"),"synthetic data, not game content");
execFileSync("git",["init","--quiet",root]);
const run=args=>spawnSync(process.execPath,[join(root,"scripts/audit-publication.mjs"),...args],{encoding:"utf8"});
assert.equal(run([]).status,0,"ignored private fixture is not a source candidate");
const exported=run([`--directory=${root}`]);
assert.notEqual(exported.status,0,"concrete export containing ignored private bytes must fail");
assert.match(exported.stderr,/private\.dat/);
await mkdir(join(root,"public","assets"),{recursive:true});
await writeFile(join(root,"public","assets","room-user.svg"),"<svg/>");
assert.equal(run([]).status,0,"reviewed Phosphor controls may be source-published");
await writeFile(join(root,"public","assets","unreviewed.svg"),"<svg/>");
const unknownAsset=run([]);
assert.notEqual(unknownAsset.status,0,"an unknown SVG remains blocked");
assert.match(unknownAsset.stderr,/unreviewed\.svg/);
await rm(join(root,"public","assets","unreviewed.svg"));
await writeFile(join(root,"public","assets","th06-card.webp"),"synthetic host-owned artwork");
const publicHostArtwork=run([]);
assert.notEqual(publicHostArtwork.status,0,"host-generated artwork must remain forbidden under public/assets");
assert.match(publicHostArtwork.stderr,/public[\\/]assets[\\/]th06-card\.webp/);
await rm(join(root,"public"),{recursive:true,force:true});
execFileSync("git",["-C",root,"add","--force","private.dat"]);
const tracked=run([]);
assert.notEqual(tracked.status,0,"force-added ignored private file must fail");
assert.match(tracked.stderr,/private\.dat/);
console.log("Publication audit scope: PASS (ignored / exported / public-host-artwork / force-added)");
});
