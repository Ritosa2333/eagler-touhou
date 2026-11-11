// Recognize TH03's side-effect-free vertical-interrupt wait in ordinary RAM.
// Only real/VM86 mode with identity pages, no DMA/traps, exact code and operands.
function transform(wat,game='03'){
  const start=wat.indexOf('  (func $f584 '),end=wat.indexOf('\n  (func ',start+1);
  let step=wat.slice(start,end);
  const anchor='    block $B2\n';
  if(!step.includes(anchor))throw Error('exec_1step layout changed');
  step=step.replace(anchor,`    local.get $l0
    i32.const ${game==='04'?114:119}
    i32.eq
    if
      call $idle_wait${game}
      drop
    end
${anchor}`);
  wat=wat.slice(0,start)+step+wat.slice(end);
  const distance=game==='04'?7:9,cycles=game==='04'?17:19;
  const delay=game==='04'?21392:26009,counter=game==='04'?10930:4586;
  const checks=[
    `(i32.ne (i32.and (i32.load (i32.const 4590044)) (i32.const 833)) (i32.const ${game==='04'?513:512}))`,
    '(i32.load8_u (i32.const 4590370))', // trap
    '(i32.and (i32.load8_u (i32.const 4590372)) (i32.eqz (i32.load8_u (i32.const 4590374))))', // real or virtual-8086 only
    '(i32.load16_u (i32.const 4590400))', // 16-bit default operand/address size
    '(i32.ne (i32.load (i32.const 4590048)) (i32.add (i32.load (i32.const 4590052)) (i32.const 1)))', // no prefixes
    '(i32.load8_u (i32.const 228333))', // DMA working
    '(i32.gt_u (i32.load (i32.const 4590240)) (i32.const 606208))', // DS + 64K remains in ordinary RAM
    `(i32.lt_u (i32.load (i32.const 4590052)) (i32.const ${distance}))`, // no segment wrap
    '(i32.gt_u (local.get $pc) (i32.const 671741))',
    '(i32.lt_u (local.get $pc) (i32.const 9))',
    ...(game==='04'?[[0,0x72],[1,0xf7],[-7,0xa1],[-4,0x3b],[-3,6]]:[[0,0x77],[1,0xf5],[-9,0xa0],[-6,0xb4],[-5,0],[-4,0x3b],[-3,6]]).map(([offset,value])=>
      `(i32.ne (i32.load8_u (i32.add (local.get $pc) (i32.const ${2492848+offset}))) (i32.const ${value}))`),
    // The original operands must match the audited TH03 frame-delay/count fields.
    `(i32.ne (i32.load16_u (i32.add (local.get $pc) (i32.const ${2492848-distance+1}))) (i32.const ${game==='04'?counter:delay}))`,
    `(i32.ne (i32.load16_u (i32.add (local.get $pc) (i32.const 2492846))) (i32.const ${game==='04'?delay:counter}))`,
    `(i32.ne (i32.load16_u (i32.const 4590000)) (i32.load${game==='04'?'16':'8'}_u (i32.add (i32.load (i32.const 4590240)) (i32.const ${2492848+(game==='04'?counter:delay)}))))`,
    `(i32.${game==='04'?'ge':'le'}_u (i32.load16_u (i32.const 4590000)) (i32.load16_u (i32.add (i32.load (i32.const 4590240)) (i32.const ${2492848+(game==='04'?delay:counter)}))))`,
    `(i32.le_s (i32.load (i32.const 4590412)) (i32.const ${cycles}))`,
    // Reject ordinary gameplay branches before walking page tables. These
    // earlier reads are bounded, side-effect-free host RAM probes. A match
    // still requires identity pages for every operand before any cycles skip.
    ...[0,1,-distance].map(offset=>`(i32.eqz (call $idle_identity (i32.add (local.get $pc) (i32.const ${offset}))))`),
    ...[delay,delay+1,counter,counter+1].map(offset=>`(i32.eqz (call $idle_identity (i32.add (i32.load (i32.const 4590240)) (i32.const ${offset}))))`),
  ];
  // Guard both ends of every possible page crossing before eliding iterations.
  let helper=`
  (func $idle_physical (param $address i32) (result i32)
    (if (i32.le_u (local.get $address) (i32.const 671740)) (then
      (return (i32.load (i32.add (local.get $address) (i32.const 2492848))))))
    (if (i32.and (i32.ge_u (local.get $address) (i32.const 1114112))
      (i32.lt_u (local.get $address) (i32.sub (i32.load (i32.const 4590436)) (i32.const 3)))) (then
      (return (i32.load (i32.add (local.get $address) (i32.load (i32.const 4590432)))))))
    (i32.const 0))
  (func $idle_identity (param $address i32) (result i32) (local $pde i32) (local $pte i32)
    (if (i32.eqz (i32.load8_u (i32.const 4590373))) (then (return (i32.const 1))))
    (local.set $pde (call $idle_physical (i32.add (i32.load (i32.const 4590380)) (i32.and (i32.shr_u (local.get $address) (i32.const 20)) (i32.const 4092)))))
    (if (i32.ne (i32.and (local.get $pde) (i32.const 133)) (i32.const 5)) (then (return (i32.const 0))))
    (local.set $pte (call $idle_physical (i32.add (i32.and (local.get $pde) (i32.const -4096)) (i32.and (i32.shr_u (local.get $address) (i32.const 10)) (i32.const 4092)))))
    (i32.and (i32.eq (i32.and (local.get $pte) (i32.const 5)) (i32.const 5))
      (i32.eq (i32.and (local.get $pte) (i32.const -4096)) (i32.and (local.get $address) (i32.const -4096)))))
  (global $idle_yields (mut i32) (i32.const 0))
  (export "__idle_yields" (global $idle_yields))
  (func $idle_wait (result i32) (local $pc i32)
    (local.set $pc (i32.add (i32.load (i32.const 4590052)) (i32.load (i32.const 4590192))))
    (block $no
      ${checks.map(c=>`(br_if $no ${c})`).join('\n      ')}
      ;; Skip complete 19-cycle loops, leaving the final partial loop to the
      ;; original interpreter. CPU registers, flags and interrupt boundary stay exact.
      (i32.store (i32.const 4590412) (i32.add (i32.rem_u (i32.sub (i32.load (i32.const 4590412)) (i32.const 1)) (i32.const ${cycles})) (i32.const 1)))
      (global.set $idle_yields (i32.add (global.get $idle_yields) (i32.const 1)))
      (return (i32.const 1)))
    (i32.const 0))
`;
  helper=helper.replaceAll('$idle_wait','$idle_wait'+game).replaceAll('$idle_identity','$idle_identity'+game).replaceAll('$idle_physical','$idle_physical'+game).replaceAll('$idle_yields','$idle_yields'+game).replaceAll('"__idle_yields"','"__idle_yields'+game+'"');
  return wat.slice(0,wat.lastIndexOf(')'))+helper+')';
}
module.exports={transform};
