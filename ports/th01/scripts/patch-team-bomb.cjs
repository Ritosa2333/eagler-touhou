// Build from the audited orb-overlap core with WABT installed.
// node patch-team-bomb.cjs <input.wasm> <output.wasm>
const {readFileSync, writeFileSync} = require('node:fs');
const {createHash} = require('node:crypto');
const [source, target] = process.argv.slice(2);
if (!source || !target) throw Error('usage: node patch-team-bomb.cjs <input.wasm> <output.wasm>');
const bytes = readFileSync(source);
if (createHash('sha256').update(bytes).digest('hex') !== '68e218573c8be89da609b17af0324566c3ae84ab7341a8ba9913a3f0fe9511af') {
  throw Error('expected the TH01 orb-overlap core');
}

require('wabt')().then(wabt => {
  const original = wabt.readWasm(bytes, {readDebugNames: true});
  original.generateNames();
  original.applyNames();
  const wat = original.toText({foldExprs: false, inlineExport: false});
  const start = wat.indexOf('  (func $th01::coop_frame_end__ ');
  const end = wat.indexOf('\n  (func ', start + 1);
  let body = wat.slice(start, end);
  function replace(from, to) {
    if (body.split(from).length !== 2) throw Error('unexpected TH01 co-op hit routine');
    body = body.replace(from, to);
  }
  // The co-op core records the bomb owner until the native effect ends.
  // Shared protection is derived from that checkpointed state, not a timer.
  replace('(local $l33 i32)\n    block $B0', `(local $l33 i32) (local $teamBomb i32)
    i32.const 0
    i32.load offset=2536992
    i32.const 1
    i32.gt_s
    i32.const 0
    i32.load offset=2536996
    i32.const 0
    i32.ge_s
    i32.and
    local.set $teamBomb
    block $B0`);
  // Discard debug hit intents, then drop native collision flags without
  // touching personal respawn timers, ghost state or either player's stock.
  replace('            block $B4\n              local.get $l4', `            block $B4
              local.get $teamBomb
              br_if $B4
              local.get $l4`);
  replace('              br_if $B6\n              local.get $l0\n              i32.const 28', `              br_if $B6
              local.get $teamBomb
              if
                local.get $l3
                i32.const 0
                i32.store
                br $B6
              end
              local.get $l0
              i32.const 28`);
  replace('      local.get $l2\n      i32.store8 offset=2545752', `      local.get $l2
      local.get $teamBomb
      i32.eqz
      i32.and
      i32.store8 offset=2545752`);
  const patched = wabt.parseWat('team-bomb.wat', wat.slice(0, start) + body + wat.slice(end), {
    sat_float_to_int: true, bulk_memory: true,
  });
  patched.resolveNames();
  patched.validate();
  writeFileSync(target, Buffer.from(patched.toBinary({write_debug_names: true}).buffer));
}).catch(error => {
  console.error(error);
  process.exitCode = 1;
});
