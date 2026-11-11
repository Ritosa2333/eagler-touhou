// Restores the original uninterruptible 12-frame slide, preserving its combo window.
const fs=require('fs'),crypto=require('crypto');
const [source,target]=process.argv.slice(2);if(!source||!target)throw Error('usage: node patch-slide.cjs <input.wasm> <output.wasm>');
const bytes=fs.readFileSync(source),expected='299c1dae2a8bed55d94e5754f4b2d19a099c962aeee193d098700842f028ed3c';
if(crypto.createHash('sha256').update(bytes).digest('hex')!==expected)throw Error('unexpected core');
require('wabt')().then(w=>{const original=w.readWasm(bytes,{readDebugNames:true});original.generateNames();original.applyNames();let wat=original.toText({foldExprs:false,inlineExport:false});
const begin=wat.indexOf('  (func $th01::player_unput_update_render_int_');const end=wat.indexOf('\n  (func ',begin+1);let body=wat.slice(begin,end);
const start=body.indexOf('    block $B4\n      local.get $l1\n      i32.const 2538610');const finish=body.indexOf('    block $B7\n',start);
if(start<0||finish<0)throw Error('slide cancellation block missing');
body=body.slice(0,start)+'    local.get $l1\n    i32.const 2538610\n    i32.add\n    i32.load8_u\n    local.set $l5\n'+body.slice(finish);
wat=wat.slice(0,begin)+body+wat.slice(end);const mod=w.parseWat('slide.wat',wat,{sat_float_to_int:true,bulk_memory:true});mod.resolveNames();mod.validate();fs.writeFileSync(target,Buffer.from(mod.toBinary({write_debug_names:true}).buffer));}).catch(e=>{console.error(e);process.exitCode=1});
