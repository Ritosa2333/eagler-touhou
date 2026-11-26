"""Add a bounded first-write page journal to the cooperative browser engine."""

import os
import re
import subprocess
import sys
from pathlib import Path

source, output, tools = map(Path, sys.argv[1:])
suffix = ".exe" if os.name == "nt" else ""
flags = ["--all-features", "--disable-compact-imports"]
wasm_opt = tools / ("wasm-opt" + suffix)
wasm_as = tools / ("wasm-as" + suffix)
directory = output.parent
text = directory / "ph3-preview-instrumented.wat"
instrumented = directory / "ph3-preview-journal.wat"
built = directory / "ph3-preview-journal.unoptimized.wasm"
subprocess.run([wasm_opt, source, *flags, "--instrument-memory", "-S", "-o", text], check=True)

journal = """
 (memory $thibp_journal 304 304)
 (global $thibp_undo_active (mut i32) (i32.const 0))
 (global $thibp_undo_epoch (mut i32) (i32.const 0))
 (global $thibp_undo_count (mut i32) (i32.const 0))
 (global $thibp_undo_limit (mut i32) (i32.const 0))
 (func $thibp_journal_fail
  (global.set $thibp_undo_limit (i32.const 1))
  (unreachable)
 )
 (func (export "thibp_journal_begin")
  (if (i32.eqz (i32.add (global.get $thibp_undo_epoch) (i32.const 1))) (then
   (memory.fill $thibp_journal (i32.const 0) (i32.const 0) (i32.const 2097152))
   (global.set $thibp_undo_epoch (i32.const 0))
  ))
  (global.set $thibp_undo_count (i32.const 0))
  (global.set $thibp_undo_limit (i32.const 0))
  (global.set $thibp_undo_epoch (i32.add (global.get $thibp_undo_epoch) (i32.const 1)))
  (global.set $thibp_undo_active (i32.const 1))
 )
 (func (export "thibp_journal_failed") (result i32) (global.get $thibp_undo_limit))
 (func (export "thibp_journal_restore") (result i32)
  (local $index i32)
  (global.set $thibp_undo_active (i32.const 0))
  (block $done
   (loop $restore
    (br_if $done (i32.eq (local.get $index) (global.get $thibp_undo_count)))
    (memory.copy $0 $thibp_journal
      (i32.load $thibp_journal offset=2097152 (i32.shl (local.get $index) (i32.const 2)))
      (i32.add (i32.const 2162688) (i32.shl (local.get $index) (i32.const 12)))
      (i32.const 4096)
    )
    (local.set $index (i32.add (local.get $index) (i32.const 1)))
    (br $restore)
   )
  )
  (i32.shl (local.get $index) (i32.const 12))
 )
 (func $thibp_mark (export "thibp_journal_mark") (param $start i32) (param $length i32)
  (local $page i32) (local $end i32) (local $stamp i32) (local $slot i32)
  (if (i32.eqz (local.get $length)) (then (return)))
  ;; Most stores touch a page that the running transaction already owns.
  (local.set $page (i32.shr_u (local.get $start) (i32.const 12)))
  (if (i32.and (i32.lt_u (local.get $page) (i32.const 524288))
               (i32.le_u (local.get $length) (i32.sub (i32.const 4096) (i32.and (local.get $start) (i32.const 4095)))))
   (then
    (if (i32.eq (i32.load $thibp_journal (i32.shl (local.get $page) (i32.const 2))) (global.get $thibp_undo_epoch))
     (then (return))
    )
   )
  )
  (local.set $end (i32.add (local.get $start) (i32.sub (local.get $length) (i32.const 1))))
  (if (i32.or (i32.lt_u (local.get $end) (local.get $start)) (i32.ge_u (i32.shr_u (local.get $end) (i32.const 16)) (memory.size $0)))
   (then (call $thibp_journal_fail))
  )
  (local.set $end (i32.shr_u (local.get $end) (i32.const 12)))
  (local.set $page (i32.shr_u (local.get $start) (i32.const 12)))
  (loop $save
   (local.set $stamp (i32.shl (local.get $page) (i32.const 2)))
   (if (i32.ne (i32.load $thibp_journal (local.get $stamp)) (global.get $thibp_undo_epoch))
    (then
     (local.set $slot (global.get $thibp_undo_count))
     (if (i32.ge_u (local.get $slot) (i32.const 4096)) (then (call $thibp_journal_fail)))
     (local.set $start (i32.shl (local.get $page) (i32.const 12)))
     (i32.store $thibp_journal (local.get $stamp) (global.get $thibp_undo_epoch))
     (i32.store $thibp_journal offset=2097152 (i32.shl (local.get $slot) (i32.const 2)) (local.get $start))
     (memory.copy $thibp_journal $0
       (i32.add (i32.const 2162688) (i32.shl (local.get $slot) (i32.const 12)))
       (local.get $start) (i32.const 4096))
     (global.set $thibp_undo_count (i32.add (local.get $slot) (i32.const 1)))
    )
   )
   (br_if $save (i32.le_u (local.tee $page (i32.add (local.get $page) (i32.const 1))) (local.get $end)))
  )
 )
 (func $store_ptr (param i32) (param $width i32) (param $offset i32) (param $address i32) (result i32)
  (if (global.get $thibp_undo_active) (then
   (call $thibp_mark (i32.add (local.get $address) (local.get $offset)) (local.get $width))
  ))
  (local.get $address)
 )
 (func $memory_grow_pre (param i32) (param $pages i32) (result i32)
  (if (global.get $thibp_undo_active) (then (call $thibp_journal_fail)))
  (local.get $pages)
 )
 (func $memory_grow_post (param i32) (param $previous i32) (result i32) (local.get $previous))
 (func $load_ptr (param i32 i32 i32 i32) (result i32) (local.get 3))
"""
for direction in ("load", "store"):
    for ty in ("i32", "i64", "f32", "f64"):
        journal += f" (func ${direction}_val_{ty} (param i32 {ty}) (result {ty}) (local.get 1))\n"
bulk = {
    "memory.copy": "copy",
    "memory.fill": "fill",
}
for mnemonic, suffix in bulk.items():
    journal += f""" (func $thibp_bulk_{suffix} (param $to i32) (param $arg i32) (param $size i32)
  (if (global.get $thibp_undo_active) (then (call $thibp_mark (local.get $to) (local.get $size))))
  ({mnemonic} {"$0 $0" if suffix == "copy" else "$0"} (local.get $to) (local.get $arg) (local.get $size))
 )
"""
forbidden = re.compile(r"^ +\((?:memory\.(?:init|atomic\S*)|data\.drop|table\.(?:set|grow|fill|copy|init)|elem\.drop|v128\.\S*store)(?:\s|\))")
atomic = re.compile(r"^( +)\((i32\.atomic\.rmw\.(?:add|xchg|cmpxchg))(?: offset=(\d+))?(?: align=(\d+))?\s*$")
atomic_names = {}

def instrument_atomic(match):
    indent, mnemonic, offset, alignment = match.groups()
    offset = int(offset or 0)
    alignment = int(alignment or 4)
    key = (mnemonic, offset, alignment)
    if key not in atomic_names:
        atomic_names[key] = f"$thibp_atomic_{len(atomic_names)}"
    return indent + "(call " + atomic_names[key] + "\n"

def atomic_helpers():
    for (mnemonic, offset, alignment), name in atomic_names.items():
        args = "(param $replacement i32)" if mnemonic.endswith("cmpxchg") else ""
        passed = "(local.get $replacement)" if args else ""
        yield f""" (func {name} (param $addr i32) (param $val i32) {args} (result i32)
  (if (global.get $thibp_undo_active) (then
   (call $thibp_mark (i32.add (local.get $addr) (i32.const {offset})) (i32.const 4))
  ))
  ({mnemonic} offset={offset} align={alignment} (local.get $addr) (local.get $val) {passed})
 )
"""

global_lines = []
declarations = []
bulk_ops = re.compile(r"^( +)\((memory\.(?:" + "|".join(bulk.values()) + r"))(?=\s|\))")
with text.open(encoding="utf-8") as src, instrumented.open("w", encoding="utf-8", newline="\n") as dst:
    for line in src:
        if line == ")\n":
            dst.write(journal + "".join(atomic_helpers()) + line)
            break
        if line.startswith(' (import "env" '):
            name = line.split('"')[3]
            if not name.startswith(("memory_grow_", "load_", "store_", "struct_", "array_")):
                raise RuntimeError(f"Unexpected instrumentation import: {name}")
            continue
        if line.startswith(" (global ") and "(mut " in line:
            global_lines.append(line)
        if line.startswith(" (memory "):
            declarations.append(line)
        if forbidden.match(line):
            raise RuntimeError(f"Untracked Wasm state write: {line[:120]}")
        if ".atomic.rmw." in line:
            match = atomic.match(line)
            if not match:
                raise RuntimeError(f"Untracked operation: {line[:120]}")
            line = instrument_atomic(match)
        line = bulk_ops.sub(lambda match: match[1] + "(call $thibp_bulk_" + bulk[match[2]], line)
        dst.write(line)
    else:
        raise RuntimeError("Binaryen module close is missing")

assert len(global_lines) == 1 and "$__stack_pointer" in global_lines[0], global_lines
assert len(declarations) == 1 and "(memory $0 " in declarations[0], declarations
subprocess.run([wasm_as, instrumented, *flags, "-o", built], check=True)
subprocess.run([wasm_opt, built, *flags, "--inlining-optimizing",
                "--optimize-instructions", "--vacuum", "--remove-unused-module-elements", "-o", output], check=True)
for intermediate in (text, instrumented, built):
    intermediate.unlink()
print(f"THIBP rollback module: {output.stat().st_size:,} bytes")
