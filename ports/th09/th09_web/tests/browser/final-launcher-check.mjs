import {spawn} from 'node:child_process';
const tasks=['tests/browser/launcher-files-check.mjs','tests/browser/launcher-acceptance-check.mjs'];
const results=await Promise.allSettled(tasks.map(file=>new Promise((resolve,reject)=>{const p=spawn(process.execPath,[file],{stdio:'inherit',windowsHide:true});p.on('error',reject);p.on('exit',code=>code?reject(Error(file+' '+code)):resolve(file));})));console.log(results);if(results.some(r=>r.status==='rejected'))process.exitCode=1;
