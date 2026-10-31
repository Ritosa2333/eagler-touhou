import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {spawnSync} from 'node:child_process';
test('gameplay easing matches all original samples in the live D3D9 precision',()=>{
  const fixture=JSON.parse(fs.readFileSync(new URL('fixtures/gameplay-easing-original.json',import.meta.url)));
  assert.equal(fixture.x87ControlWord,'0x007f');
  assert.equal(fixture.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
  fs.mkdirSync('artifacts/easing-tests',{recursive:true});
  const compiled=spawnSync('g++',['tests/native/native-easing.cpp','-std=c++20','-O2','-ffp-contract=off','-o','artifacts/easing-tests/probe.exe'],{encoding:'utf8'});
  assert.equal(compiled.status,0,compiled.stderr);
  const run=spawnSync('artifacts/easing-tests/probe.exe',[],{encoding:'utf8',input:fixture.rows.map(r=>`${r.mode} ${r.inputBits}`).join('\n')+'\n'});
  assert.equal(run.status,0,run.stderr);const bits=run.stdout.trim().split(/\s+/).map(Number);
  assert.equal(bits.length,fixture.rows.length);
  fixture.rows.forEach((row,i)=>assert.equal(bits[i],row.outputBits,`mode${row.mode}, inputbits${row.inputBits}`));
});
