// Rebuild this patch from the audited pre-change WASM: npm ci --prefix ports/th01/scripts
// node ports/th01/scripts/patch-launch.cjs <original.wasm> <output.wasm>
// The multi-orb reset spreads up to eight balls into two rows and assigns
// distinct upward velocities; the one-orb path remains the original code.
// Multi-orb floor bounces preserve the shared card combo until stage reset.
const fs=require('fs');
const crypto=require('crypto');
const input=process.argv[2],output=process.argv[3];
if(!input||!output)throw Error('Usage: node patch-launch.cjs <original.wasm> <output.wasm>');
const bytes=fs.readFileSync(input);
if(crypto.createHash('sha256').update(bytes).digest('hex')!=='10ddef930adc2527f34f8584dc7383b825bb916d52782bd66919f7a653a56eee')throw Error('Unsupported TH01 core');
require('wabt')().then(w=>{
const original=w.readWasm(bytes,{readDebugNames:true});original.generateNames();original.applyNames();
const wat=original.toText({foldExprs:false,inlineExport:false});
const begin=wat.indexOf('  (func $th01::orbs_reset__');
const end=wat.indexOf('  (func $th01::orbs_tick_forces__',begin);
let body=wat.slice(begin,end);
const target=`      local.get $l0\n      i32.const 48\n      i32.add`;
const patch=`      local.get $l1
      i32.const 1
      i32.gt_u
      if
        local.get $l0
        i32.const 600
        local.get $l1
        i32.const 6
        i32.eq
        if (result i32)
          local.get $l5
          i32.const 3
          i32.rem_u
        else
          local.get $l5
          local.get $l1
          i32.const 4
          i32.eq
          if (result i32)
            i32.const 1
          else
            i32.const 3
          end
          i32.and
        end
        i32.const 32
        i32.mul
        i32.sub
        local.tee $l8
        i32.store
        local.get $l0
        i32.const 4
        i32.add
        local.get $l8
        i32.store
        local.get $l0
        i32.const 8
        i32.add
        i32.const 280
        local.get $l1
        i32.const 6
        i32.eq
        if (result i32)
          local.get $l5
          i32.const 3
          i32.div_u
        else
          local.get $l5
          local.get $l1
          i32.const 4
          i32.eq
          if (result i32)
            i32.const 1
          else
            i32.const 2
          end
          i32.shr_u
        end
        i32.const 32
        i32.mul
        i32.add
        local.tee $l8
        i32.store
        local.get $l0
        i32.const 12
        i32.add
        local.get $l8
        i32.store
        local.get $l6
        local.get $l1
        i32.const 6
        i32.eq
        if (result i32)
          local.get $l5
          i32.const 3
          i32.rem_u
        else
          local.get $l5
          local.get $l1
          i32.const 4
          i32.eq
          if (result i32)
            i32.const 1
          else
            i32.const 3
          end
          i32.and
        end
        i32.const 1
        i32.add
        i32.const -512
        i32.mul
        local.tee $l8
        i32.store
        local.get $l7
        i32.const 3
        i32.const 1
        local.get $l8
        i32.const -1536
        i32.lt_s
        select
        i32.store
        f64.const -8
        local.set $l9
        local.get $l0
        i32.const 24
        i32.add
        local.get $l9
        f64.store
      end
`;
if(body.split(target).length!==2)throw Error('ambiguous');
body=body.replace(target,patch+target);
let patched=wat.slice(0,begin)+body+wat.slice(end);
const boundsStart=patched.indexOf('  (func $th01::orb_multi_bounds__');
const boundsEnd=patched.indexOf('  (func $th01::orb_fixed_pair_resolve_',boundsStart);
let bounds=patched.slice(boundsStart,boundsEnd);
const comboReset='      i32.const 0\n      i32.const 0\n      i32.store offset=2538596\n';
if(bounds.split(comboReset).length!==2)throw Error('Unexpected multi-orb combo reset');
bounds=bounds.replace(comboReset,'');
patched=patched.slice(0,boundsStart)+bounds+patched.slice(boundsEnd);
const m=w.parseWat('th01.wat',patched,{sat_float_to_int:true,bulk_memory:true});m.resolveNames();m.validate();fs.writeFileSync(output,Buffer.from(m.toBinary({write_debug_names:true}).buffer));}).catch(error=>{console.error(error);process.exitCode=1;});
