// Remove the original keyboard combination shortcuts while retaining the dedicated bomb input.
const fs=require('node:fs'),crypto=require('node:crypto');
const [source,target]=process.argv.slice(2);if(!source||!target)throw Error('usage: node patch-bomb-input.cjs <input.wasm> <output.wasm>');
const bytes=fs.readFileSync(source),expected='6263d75348bee374c67c99a978057b53908a6faf54c9aebf60f77309131c15f6';
if(crypto.createHash('sha256').update(bytes).digest('hex')!==expected)throw Error('unexpected core');
require('wabt')().then(w=>{
 const original=w.readWasm(bytes,{readDebugNames:true});original.generateNames();original.applyNames();let wat=original.toText({foldExprs:false,inlineExport:false});
 const begin=wat.indexOf('  (func $th01::coop_input_th01::CoopPlayer&__unsigned_short_');const end=wat.indexOf('\n  (func ',begin+1);let body=wat.slice(begin,end);
 const a=body.indexOf('    block $B8\n'),b=body.indexOf('    local.get $p0\n    local.get $p1\n    i32.store16 offset=40',a);
 if(a<0||b<0)throw Error('bomb input branch missing');
 const originalBombBranch=body.slice(a,b);
 body=body.slice(0,a)+`    i32.const 0
    i32.load offset=2536992
    i32.const 1
    i32.gt_s
    if
    local.get $l2
    i32.const 16
    i32.and
    if
      local.get $p0
      i32.const 1
      i32.store16 offset=38
    end
    else
`+originalBombBranch+`    end
`+body.slice(b);
 wat=wat.slice(0,begin)+body+wat.slice(end);const mod=w.parseWat('bomb.wat',wat,{sat_float_to_int:true,bulk_memory:true});mod.resolveNames();mod.validate();fs.writeFileSync(target,Buffer.from(mod.toBinary({write_debug_names:true}).buffer));
}).catch(error=>{console.error(error);process.exitCode=1});
