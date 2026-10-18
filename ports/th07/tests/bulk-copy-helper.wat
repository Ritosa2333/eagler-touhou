;; Experimental standalone memory.copy batch over the caller's SAME memory.
;; No allocator, data segment, threads or separately owned game state.
;; Build: wasm-as tests/bulk-copy-helper.wat --enable-bulk-memory -o <output>
(module
  (import "env" "memory" (memory 0))
  (func $add_checked (param $a i32) (param $b i32) (result i32)
    (local $sum i32)
    (local.set $sum (i32.add (local.get $a) (local.get $b)))
    (if (i32.lt_u (local.get $sum) (local.get $a)) (then (unreachable)))
    (local.get $sum))
  (func (export "copy") (param $dst i32) (param $src i32) (param $bytes i32)
    (memory.copy (local.get $dst) (local.get $src) (local.get $bytes)))
  (func (export "batch") (param $dst i32) (param $src i32) (param $regions i32) (param $count i32)
    (block $done
      (loop $next
        (br_if $done (i32.eqz (local.get $count)))
        (memory.copy
          (call $add_checked (local.get $dst) (i32.load offset=4 (local.get $regions)))
          (call $add_checked (local.get $src) (i32.load (local.get $regions)))
          (i32.load offset=8 (local.get $regions)))
        (local.set $count (i32.sub (local.get $count) (i32.const 1)))
        (local.set $regions (call $add_checked (local.get $regions) (i32.const 12)))
        (br $next)))))
