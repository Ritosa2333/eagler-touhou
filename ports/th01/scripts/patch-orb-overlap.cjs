// Build from the stacked-launch core: node patch-orb-overlap.cjs <input.wasm> <output.wasm>
const {readFileSync, writeFileSync} = require('node:fs');
const {createHash} = require('node:crypto');
const [source, target] = process.argv.slice(2);
if (!source || !target) throw Error('usage: node patch-orb-overlap.cjs <input.wasm> <output.wasm>');
const bytes = readFileSync(source);
if (createHash('sha256').update(bytes).digest('hex') !== 'e483102f58a4f815315ff73c57156906296052d40aecfb7c57c63ed7b2e42703') {
  throw Error('expected the TH01 stacked-launch core');
}

require('wabt')().then(wabt => {
  const original = wabt.readWasm(bytes, {readDebugNames: true});
  original.generateNames();
  original.applyNames();
  const wat = original.toText({foldExprs: false, inlineExport: false});
  const start = wat.indexOf('  (func $th01::orb_fixed_pair_resolve_');
  const end = wat.indexOf('\n  (func ', start + 1);
  const body = wat.slice(start, end);
  const unarmed = `    block $B3
      local.get $p2
      i32.load
      br_if $B3
      i32.const 0
      return
    end`;
  if (body.split(unarmed).length !== 2) throw Error('unexpected TH01 pair collision gate');

  // Pair state starts at 0. Negative values count overlapping substeps; 1
  // means armed, just as when the balls reach normal separation. After 48
  // substeps (12 physics ticks), resolve even if their paths never diverged.
  // This field already participates in the snapshot and state checksum.
  const limitedGrace = `    block $B3
      local.get $p2
      i32.load
      local.tee $l8
      i32.const 0
      i32.gt_s
      br_if $B3
      local.get $p2
      local.get $l8
      i32.const 1
      i32.sub
      local.tee $l8
      i32.store
      local.get $l8
      i32.const -48
      i32.le_s
      if
        local.get $p2
        i32.const 1
        i32.store
        br $B3
      end
      i32.const 0
      return
    end`;
  const patched = wabt.parseWat('orb-overlap.wat', wat.slice(0, start) + body.replace(unarmed, limitedGrace) + wat.slice(end), {
    sat_float_to_int: true, bulk_memory: true,
  });
  patched.resolveNames();
  patched.validate();
  writeFileSync(target, Buffer.from(patched.toBinary({write_debug_names: true}).buffer));
}).catch(error => {
  console.error(error);
  process.exitCode = 1;
});
