(module
 (import "env" "memory" (memory $live 1))
 ;; Start at 64 MiB plus the directory; the JS adapter grows the shadow
 ;; with the live heap while retaining older pages for undo.
 ;; Four directory pages cover every 4 KiB block of the 256 MiB maximum heap.
 (memory $shadow 1028)
 (export "shadow" (memory $shadow))
 (func $equal (param $page i32) (result i32) (local $at i32) (local $end i32) (local $difference v128)
  (local.set $at (i32.shl (local.get $page) (i32.const 12)))
  (local.set $end (i32.add (local.get $at) (i32.const 4096)))
  (loop $bytes
   (local.set $difference (v128.xor (v128.load $live (local.get $at)) (v128.load $shadow (i32.add (local.get $at) (i32.const 262144)))))
   (local.set $difference (v128.or (local.get $difference) (v128.xor (v128.load $live offset=16 (local.get $at)) (v128.load $shadow offset=16 (i32.add (local.get $at) (i32.const 262144))))))
   (local.set $difference (v128.or (local.get $difference) (v128.xor (v128.load $live offset=32 (local.get $at)) (v128.load $shadow offset=32 (i32.add (local.get $at) (i32.const 262144))))))
   (local.set $difference (v128.or (local.get $difference) (v128.xor (v128.load $live offset=48 (local.get $at)) (v128.load $shadow offset=48 (i32.add (local.get $at) (i32.const 262144))))))
   (if (v128.any_true (local.get $difference)) (then (return (i32.const 0))))
   (local.set $at (i32.add (local.get $at) (i32.const 64)))
   (br_if $bytes (i32.lt_u (local.get $at) (local.get $end))))
  (i32.const 1))
 (func (export "scan") (param $pages i32) (result i32) (local $page i32) (local $count i32)
  (loop $next
   (if (i32.eqz (call $equal (local.get $page)))
    (then (i32.store $shadow (i32.shl (local.get $count) (i32.const 2)) (local.get $page)) (local.set $count (i32.add (local.get $count) (i32.const 1)))))
   (local.set $page (i32.add (local.get $page) (i32.const 1)))
   (br_if $next (i32.lt_u (local.get $page) (local.get $pages))))
  (local.get $count))
 (func (export "copyPage") (param $page i32)
  (memory.copy $shadow $live (i32.add (i32.shl (local.get $page) (i32.const 12)) (i32.const 262144)) (i32.shl (local.get $page) (i32.const 12)) (i32.const 4096)))
 ;; Pack all before-images and update the shadow in one call. The JS adapter
 ;; reserves scratch storage beyond the complete live-memory shadow.
 (func (export "packPages") (param $count i32) (param $scratch i32) (local $index i32) (local $page i32) (local $before i32)
  (block $done (loop $next
   (br_if $done (i32.ge_u (local.get $index) (local.get $count)))
   (local.set $page (i32.shl (i32.load $shadow (i32.shl (local.get $index) (i32.const 2))) (i32.const 12)))
   (local.set $before (i32.add (local.get $page) (i32.const 262144)))
   (memory.copy $shadow $shadow (i32.add (local.get $scratch) (i32.shl (local.get $index) (i32.const 12))) (local.get $before) (i32.const 4096))
   (memory.copy $shadow $live (local.get $before) (local.get $page) (i32.const 4096))
   (local.set $index (i32.add (local.get $index) (i32.const 1))) (br $next))))
 (func (export "restorePages") (param $count i32) (param $scratch i32) (local $index i32) (local $page i32) (local $source i32)
  (block $done (loop $next
   (br_if $done (i32.ge_u (local.get $index) (local.get $count)))
   (local.set $page (i32.shl (i32.load $shadow (i32.shl (local.get $index) (i32.const 2))) (i32.const 12)))
   (local.set $source (i32.add (local.get $scratch) (i32.shl (local.get $index) (i32.const 12))))
   (memory.copy $live $shadow (local.get $page) (local.get $source) (i32.const 4096))
   (memory.copy $shadow $shadow (i32.add (local.get $page) (i32.const 262144)) (local.get $source) (i32.const 4096))
   (local.set $index (i32.add (local.get $index) (i32.const 1))) (br $next))))
 (func (export "init") (param $size i32)
  (memory.copy $shadow $live (i32.const 262144) (i32.const 0) (local.get $size)))
 (func (export "extend") (param $oldSize i32) (param $size i32)
  (memory.copy $shadow $live (i32.add (local.get $oldSize) (i32.const 262144)) (local.get $oldSize) (i32.sub (local.get $size) (local.get $oldSize)))))
