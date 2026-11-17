// Rebuild the stacked multi-orb launch from the checked-in native core.
const fs=require('fs'),crypto=require('crypto');
const [source,target]=process.argv.slice(2);
if(!source||!target)throw Error('usage: node patch-stacked-launch.cjs <input.wasm> <output.wasm>');
const bytes=fs.readFileSync(source);
if(crypto.createHash('sha256').update(bytes).digest('hex')!=='2af7d6e7a5a5434dc7916a1b746dd6b03846dc5f7f892c18687ab6f507b556fd')throw Error('unexpected TH01 core');
require('wabt')().then(w=>{
 const original=w.readWasm(bytes,{readDebugNames:true});original.generateNames();original.applyNames();let wat=original.toText({foldExprs:false,inlineExport:false});
 const begin=wat.indexOf('  (func $th01::orbs_reset__'),end=wat.indexOf('\n  (func $th01::orbs_tick_forces__',begin);
 let body=wat.slice(begin,end);
 const marker='      local.get $l0\n      i32.const 48\n      i32.add\n      i64.const 307863255930880';
 if(body.split(marker).length!==2)throw Error('launch initialization not found');
 const patch=`      local.get $l1
      i32.const 1
      i32.gt_s
      if
        local.get $l0
        i32.const 600
        i32.store
        local.get $l0
        i32.const 4
        i32.add
        i32.const 600
        i32.store
        local.get $l0
        i32.const 8
        i32.add
        i32.const 280
        i32.store
        local.get $l0
        i32.const 12
        i32.add
        i32.const 280
        i32.store
        local.get $l6
        i32.const -1024
        local.get $l5
        i32.const 2
        i32.mul
        local.get $l1
        i32.const 1
        i32.sub
        i32.sub
        i32.const 32
        i32.mul
        i32.add
        i32.store
        f64.const -8
        local.get $l5
        i32.const 2
        i32.mul
        local.get $l1
        i32.const 1
        i32.sub
        i32.sub
        f64.convert_i32_s
        f64.const 0.0625
        f64.mul
        f64.add
        local.set $l9
        local.get $l0
        i32.const 24
        i32.add
        local.get $l9
        f64.store
      end
`;
 body=body.replace(marker,patch+marker);wat=wat.slice(0,begin)+body+wat.slice(end);
 const mod=w.parseWat('stacked-launch.wat',wat,{sat_float_to_int:true,bulk_memory:true});mod.resolveNames();mod.validate();fs.writeFileSync(target,Buffer.from(mod.toBinary({write_debug_names:true}).buffer));
}).catch(e=>{console.error(e);process.exitCode=1;});
