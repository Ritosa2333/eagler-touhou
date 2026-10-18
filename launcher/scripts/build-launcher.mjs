#!/usr/bin/env node
import {ensureClientServicesBuild} from '../lib/client-services-build.mjs';
import {spawn} from 'node:child_process';
import {fileURLToPath} from 'node:url';

const result = await ensureClientServicesBuild({force:process.argv.includes('--force')});
console.log(`Client services: ${result.sourceCount} TypeScript modules`);
await new Promise((yes,no)=>{
  const child=spawn(process.execPath,[fileURLToPath(new URL('../node_modules/@react-router/dev/bin.cjs',import.meta.url)),'build'],{stdio:'inherit'});
  child.once('error',no);child.once('exit',code=>code===0?yes():no(Error(`React build failed (${code})`)));
});
