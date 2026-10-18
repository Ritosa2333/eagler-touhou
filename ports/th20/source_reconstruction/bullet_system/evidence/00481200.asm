00481200 55                       push       ebp
00481201 8bec                     mov        ebp, esp
00481203 83ec7c                   sub        esp, 0x7c
00481206 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0048120b 33c5                     xor        eax, ebp
0048120d 8945fc                   mov        dword ptr [ebp - 4], eax
00481210 894dc8                   mov        dword ptr [ebp - 0x38], ecx
00481213 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
00481216 05a0000000               add        eax, 0xa0
0048121b 8945b4                   mov        dword ptr [ebp - 0x4c], eax
0048121e 6a0b                     push       0xb
00481220 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
00481223 e858b4ffff               call       0x47c680
00481228 8945b0                   mov        dword ptr [ebp - 0x50], eax
0048122b 6a00                     push       0
0048122d 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00481230 e8eb81faff               call       0x429420
00481235 90                       nop        
00481236 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
00481239 81c1a0000000             add        ecx, 0xa0
0048123f 894dac                   mov        dword ptr [ebp - 0x54], ecx
00481242 6a0b                     push       0xb
00481244 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
00481247 e834b4ffff               call       0x47c680
0048124c 83783000                 cmp        dword ptr [eax + 0x30], 0
00481250 0f84cd040000             je         0x481723
00481256 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
00481259 8b4a1c                   mov        ecx, dword ptr [edx + 0x1c]
0048125c e86f6afbff               call       0x437cd0
00481261 f30f104048               movss      xmm0, dword ptr [eax + 0x48]
00481266 f30f1145a8               movss      dword ptr [ebp - 0x58], xmm0
0048126b 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
0048126e 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
00481271 e85a6afbff               call       0x437cd0
00481276 f30f10404c               movss      xmm0, dword ptr [eax + 0x4c]
0048127b f30f1145a4               movss      dword ptr [ebp - 0x5c], xmm0
00481280 51                       push       ecx
00481281 f30f1045a8               movss      xmm0, dword ptr [ebp - 0x58]
00481286 f30f110424               movss      dword ptr [esp], xmm0
0048128b 51                       push       ecx
0048128c f30f1045a4               movss      xmm0, dword ptr [ebp - 0x5c]
00481291 f30f110424               movss      dword ptr [esp], xmm0
00481296 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
00481299 83c164                   add        ecx, 0x64
0048129c 51                       push       ecx
0048129d e82e420000               call       0x4854d0
004812a2 83c40c                   add        esp, 0xc
004812a5 85c0                     test       eax, eax
004812a7 0f8476040000             je         0x481723
004812ad 8d4dd4                   lea        ecx, [ebp - 0x2c]
004812b0 e8eb85fbff               call       0x4398a0
004812b5 8d4dcc                   lea        ecx, [ebp - 0x34]
004812b8 e8e385fbff               call       0x4398a0
004812bd 33d2                     xor        edx, edx
004812bf 8955dc                   mov        dword ptr [ebp - 0x24], edx
004812c2 8955e0                   mov        dword ptr [ebp - 0x20], edx
004812c5 8955e4                   mov        dword ptr [ebp - 0x1c], edx
004812c8 8955e8                   mov        dword ptr [ebp - 0x18], edx
004812cb 33c0                     xor        eax, eax
004812cd 8945ec                   mov        dword ptr [ebp - 0x14], eax
004812d0 8945f0                   mov        dword ptr [ebp - 0x10], eax
004812d3 8945f4                   mov        dword ptr [ebp - 0xc], eax
004812d6 8945f8                   mov        dword ptr [ebp - 8], eax
004812d9 51                       push       ecx
004812da f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
004812e2 f30f110424               movss      dword ptr [esp], xmm0
004812e7 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004812ea 81c184000000             add        ecx, 0x84
004812f0 e8eb7ffaff               call       0x4292e0
004812f5 51                       push       ecx
004812f6 d91c24                   fstp       dword ptr [esp]
004812f9 8d4dcc                   lea        ecx, [ebp - 0x34]
004812fc 51                       push       ecx
004812fd e82e80fbff               call       0x439330
00481302 83c40c                   add        esp, 0xc
00481305 f30f1005a4cd5600         movss      xmm0, dword ptr [0x56cda4]
0048130d f30f114588               movss      dword ptr [ebp - 0x78], xmm0
00481312 f30f1005c0d75600         movss      xmm0, dword ptr [0x56d7c0]
0048131a f30f114584               movss      dword ptr [ebp - 0x7c], xmm0
0048131f 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
00481322 8b4a1c                   mov        ecx, dword ptr [edx + 0x1c]
00481325 e8a669fbff               call       0x437cd0
0048132a f30f10056cfd5600         movss      xmm0, dword ptr [0x56fd6c]
00481332 f30f5c404c               subss      xmm0, dword ptr [eax + 0x4c]
00481337 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
0048133f 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
00481342 f30f5c4064               subss      xmm0, dword ptr [eax + 0x64]
00481347 f30f1145d4               movss      dword ptr [ebp - 0x2c], xmm0
0048134c f30f1005c0d75600         movss      xmm0, dword ptr [0x56d7c0]
00481354 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
0048135c 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
0048135f 8b491c                   mov        ecx, dword ptr [ecx + 0x1c]
00481362 f30f1145a0               movss      dword ptr [ebp - 0x60], xmm0
00481367 e86469fbff               call       0x437cd0
0048136c f30f100570fd5600         movss      xmm0, dword ptr [0x56fd70]
00481374 f30f5c4048               subss      xmm0, dword ptr [eax + 0x48]
00481379 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00481381 f30f104da0               movss      xmm1, dword ptr [ebp - 0x60]
00481386 f30f58c8                 addss      xmm1, xmm0
0048138a 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
0048138d f30f5c4a68               subss      xmm1, dword ptr [edx + 0x68]
00481392 f30f114dd8               movss      dword ptr [ebp - 0x28], xmm1
00481397 8d45d4                   lea        eax, [ebp - 0x2c]
0048139a 50                       push       eax
0048139b 8d4dd4                   lea        ecx, [ebp - 0x2c]
0048139e 51                       push       ecx
0048139f e86cd5fbff               call       0x43e910
004813a4 83c408                   add        esp, 8
004813a7 8d55d4                   lea        edx, [ebp - 0x2c]
004813aa 52                       push       edx
004813ab 8d45cc                   lea        eax, [ebp - 0x34]
004813ae 50                       push       eax
004813af e87cf9ffff               call       0x480d30
004813b4 83c408                   add        esp, 8
004813b7 b904000000               mov        ecx, 4
004813bc 6bd100                   imul       edx, ecx, 0
004813bf d95c15dc                 fstp       dword ptr [ebp + edx - 0x24]
004813c3 8d45d4                   lea        eax, [ebp - 0x2c]
004813c6 50                       push       eax
004813c7 8d4dcc                   lea        ecx, [ebp - 0x34]
004813ca 51                       push       ecx
004813cb e8a0f9ffff               call       0x480d70
004813d0 83c408                   add        esp, 8
004813d3 ba04000000               mov        edx, 4
004813d8 6bc200                   imul       eax, edx, 0
004813db d95c05ec                 fstp       dword ptr [ebp + eax - 0x14]
004813df 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004813e2 8b491c                   mov        ecx, dword ptr [ecx + 0x1c]
004813e5 e8e668fbff               call       0x437cd0
004813ea f30f1005a4cd5600         movss      xmm0, dword ptr [0x56cda4]
004813f2 f30f58404c               addss      xmm0, dword ptr [eax + 0x4c]
004813f7 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004813ff 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
00481402 f30f5c4264               subss      xmm0, dword ptr [edx + 0x64]
00481407 f30f1145d4               movss      dword ptr [ebp - 0x2c], xmm0
0048140c f30f1005c0d75600         movss      xmm0, dword ptr [0x56d7c0]
00481414 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
0048141c 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
0048141f 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
00481422 f30f11459c               movss      dword ptr [ebp - 0x64], xmm0
00481427 e8a468fbff               call       0x437cd0
0048142c f30f100570fd5600         movss      xmm0, dword ptr [0x56fd70]
00481434 f30f5c4048               subss      xmm0, dword ptr [eax + 0x48]
00481439 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00481441 f30f104d9c               movss      xmm1, dword ptr [ebp - 0x64]
00481446 f30f58c8                 addss      xmm1, xmm0
0048144a 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
0048144d f30f5c4968               subss      xmm1, dword ptr [ecx + 0x68]
00481452 f30f114dd8               movss      dword ptr [ebp - 0x28], xmm1
00481457 8d55d4                   lea        edx, [ebp - 0x2c]
0048145a 52                       push       edx
0048145b 8d45d4                   lea        eax, [ebp - 0x2c]
0048145e 50                       push       eax
0048145f e8acd4fbff               call       0x43e910
00481464 83c408                   add        esp, 8
00481467 8d4dd4                   lea        ecx, [ebp - 0x2c]
0048146a 51                       push       ecx
0048146b 8d55cc                   lea        edx, [ebp - 0x34]
0048146e 52                       push       edx
0048146f e8bcf8ffff               call       0x480d30
00481474 83c408                   add        esp, 8
00481477 b804000000               mov        eax, 4
0048147c c1e000                   shl        eax, 0
0048147f d95c05dc                 fstp       dword ptr [ebp + eax - 0x24]
00481483 8d4dd4                   lea        ecx, [ebp - 0x2c]
00481486 51                       push       ecx
00481487 8d55cc                   lea        edx, [ebp - 0x34]
0048148a 52                       push       edx
0048148b e8e0f8ffff               call       0x480d70
00481490 83c408                   add        esp, 8
00481493 b804000000               mov        eax, 4
00481498 c1e000                   shl        eax, 0
0048149b d95c05ec                 fstp       dword ptr [ebp + eax - 0x14]
0048149f 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004814a2 8b491c                   mov        ecx, dword ptr [ecx + 0x1c]
004814a5 e82668fbff               call       0x437cd0
004814aa f30f10056cfd5600         movss      xmm0, dword ptr [0x56fd6c]
004814b2 f30f5c404c               subss      xmm0, dword ptr [eax + 0x4c]
004814b7 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004814bf 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
004814c2 f30f5c4264               subss      xmm0, dword ptr [edx + 0x64]
004814c7 f30f1145d4               movss      dword ptr [ebp - 0x2c], xmm0
004814cc f30f1005c0d75600         movss      xmm0, dword ptr [0x56d7c0]
004814d4 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004814dc 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
004814df 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
004814e2 f30f114598               movss      dword ptr [ebp - 0x68], xmm0
004814e7 e8e467fbff               call       0x437cd0
004814ec f30f1005c0d75600         movss      xmm0, dword ptr [0x56d7c0]
004814f4 f30f584048               addss      xmm0, dword ptr [eax + 0x48]
004814f9 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00481501 f30f104d98               movss      xmm1, dword ptr [ebp - 0x68]
00481506 f30f58c8                 addss      xmm1, xmm0
0048150a 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
0048150d f30f5c4968               subss      xmm1, dword ptr [ecx + 0x68]
00481512 f30f114dd8               movss      dword ptr [ebp - 0x28], xmm1
00481517 8d55d4                   lea        edx, [ebp - 0x2c]
0048151a 52                       push       edx
0048151b 8d45d4                   lea        eax, [ebp - 0x2c]
0048151e 50                       push       eax
0048151f e8ecd3fbff               call       0x43e910
00481524 83c408                   add        esp, 8
00481527 8d4dd4                   lea        ecx, [ebp - 0x2c]
0048152a 51                       push       ecx
0048152b 8d55cc                   lea        edx, [ebp - 0x34]
0048152e 52                       push       edx
0048152f e8fcf7ffff               call       0x480d30
00481534 83c408                   add        esp, 8
00481537 b804000000               mov        eax, 4
0048153c d1e0                     shl        eax, 1
0048153e d95c05dc                 fstp       dword ptr [ebp + eax - 0x24]
00481542 8d4dd4                   lea        ecx, [ebp - 0x2c]
00481545 51                       push       ecx
00481546 8d55cc                   lea        edx, [ebp - 0x34]
00481549 52                       push       edx
0048154a e821f8ffff               call       0x480d70
0048154f 83c408                   add        esp, 8
00481552 b804000000               mov        eax, 4
00481557 d1e0                     shl        eax, 1
00481559 d95c05ec                 fstp       dword ptr [ebp + eax - 0x14]
0048155d 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
00481560 8b491c                   mov        ecx, dword ptr [ecx + 0x1c]
00481563 e86867fbff               call       0x437cd0
00481568 f30f1005a4cd5600         movss      xmm0, dword ptr [0x56cda4]
00481570 f30f58404c               addss      xmm0, dword ptr [eax + 0x4c]
00481575 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
0048157d 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
00481580 f30f5c4264               subss      xmm0, dword ptr [edx + 0x64]
00481585 f30f1145d4               movss      dword ptr [ebp - 0x2c], xmm0
0048158a f30f1005c0d75600         movss      xmm0, dword ptr [0x56d7c0]
00481592 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
0048159a 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
0048159d 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
004815a0 f30f114594               movss      dword ptr [ebp - 0x6c], xmm0
004815a5 e82667fbff               call       0x437cd0
004815aa f30f1005c0d75600         movss      xmm0, dword ptr [0x56d7c0]
004815b2 f30f584048               addss      xmm0, dword ptr [eax + 0x48]
004815b7 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004815bf f30f104d94               movss      xmm1, dword ptr [ebp - 0x6c]
004815c4 f30f58c8                 addss      xmm1, xmm0
004815c8 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004815cb f30f5c4968               subss      xmm1, dword ptr [ecx + 0x68]
004815d0 f30f114dd8               movss      dword ptr [ebp - 0x28], xmm1
004815d5 8d55d4                   lea        edx, [ebp - 0x2c]
004815d8 52                       push       edx
004815d9 8d45d4                   lea        eax, [ebp - 0x2c]
004815dc 50                       push       eax
004815dd e82ed3fbff               call       0x43e910
004815e2 83c408                   add        esp, 8
004815e5 8d4dd4                   lea        ecx, [ebp - 0x2c]
004815e8 51                       push       ecx
004815e9 8d55cc                   lea        edx, [ebp - 0x34]
004815ec 52                       push       edx
004815ed e83ef7ffff               call       0x480d30
004815f2 83c408                   add        esp, 8
004815f5 b804000000               mov        eax, 4
004815fa 6bc803                   imul       ecx, eax, 3
004815fd d95c0ddc                 fstp       dword ptr [ebp + ecx - 0x24]
00481601 8d55d4                   lea        edx, [ebp - 0x2c]
00481604 52                       push       edx
00481605 8d45cc                   lea        eax, [ebp - 0x34]
00481608 50                       push       eax
00481609 e862f7ffff               call       0x480d70
0048160e 83c408                   add        esp, 8
00481611 b904000000               mov        ecx, 4
00481616 6bd103                   imul       edx, ecx, 3
00481619 d95c15ec                 fstp       dword ptr [ebp + edx - 0x14]
0048161d f30f10057cfd5600         movss      xmm0, dword ptr [0x56fd7c]
00481625 f30f1145bc               movss      dword ptr [ebp - 0x44], xmm0
0048162a c745c400000000           mov        dword ptr [ebp - 0x3c], 0
00481631 eb09                     jmp        0x48163c
00481633 8b45c4                   mov        eax, dword ptr [ebp - 0x3c]
00481636 83c001                   add        eax, 1
00481639 8945c4                   mov        dword ptr [ebp - 0x3c], eax
0048163c 837dc404                 cmp        dword ptr [ebp - 0x3c], 4
00481640 7d3e                     jge        0x481680
00481642 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
00481645 0f57c0                   xorps      xmm0, xmm0
00481648 0f2f448ddc               comiss     xmm0, dword ptr [ebp + ecx*4 - 0x24]
0048164d 722f                     jb         0x48167e
0048164f 8b55c4                   mov        edx, dword ptr [ebp - 0x3c]
00481652 f30f104495ec             movss      xmm0, dword ptr [ebp + edx*4 - 0x14]
00481658 0f2f45bc                 comiss     xmm0, dword ptr [ebp - 0x44]
0048165c 7620                     jbe        0x48167e
0048165e 8b45c4                   mov        eax, dword ptr [ebp - 0x3c]
00481661 f30f104485ec             movss      xmm0, dword ptr [ebp + eax*4 - 0x14]
00481667 0f2f05e8e05600           comiss     xmm0, dword ptr [0x56e0e8]
0048166e 720e                     jb         0x48167e
00481670 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
00481673 f30f10448dec             movss      xmm0, dword ptr [ebp + ecx*4 - 0x14]
00481679 f30f1145bc               movss      dword ptr [ebp - 0x44], xmm0
0048167e ebb3                     jmp        0x481633
00481680 f30f10057cfd5600         movss      xmm0, dword ptr [0x56fd7c]
00481688 f30f1145b8               movss      dword ptr [ebp - 0x48], xmm0
0048168d c745c000000000           mov        dword ptr [ebp - 0x40], 0
00481694 eb09                     jmp        0x48169f
00481696 8b55c0                   mov        edx, dword ptr [ebp - 0x40]
00481699 83c201                   add        edx, 1
0048169c 8955c0                   mov        dword ptr [ebp - 0x40], edx
0048169f 837dc004                 cmp        dword ptr [ebp - 0x40], 4
004816a3 7d43                     jge        0x4816e8
004816a5 8b45c0                   mov        eax, dword ptr [ebp - 0x40]
004816a8 f30f104485dc             movss      xmm0, dword ptr [ebp + eax*4 - 0x24]
004816ae 0f2f05e8e05600           comiss     xmm0, dword ptr [0x56e0e8]
004816b5 722f                     jb         0x4816e6
004816b7 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
004816ba f30f10448dec             movss      xmm0, dword ptr [ebp + ecx*4 - 0x14]
004816c0 0f2f45b8                 comiss     xmm0, dword ptr [ebp - 0x48]
004816c4 7620                     jbe        0x4816e6
004816c6 8b55c0                   mov        edx, dword ptr [ebp - 0x40]
004816c9 f30f104495ec             movss      xmm0, dword ptr [ebp + edx*4 - 0x14]
004816cf 0f2f05e8e05600           comiss     xmm0, dword ptr [0x56e0e8]
004816d6 720e                     jb         0x4816e6
004816d8 8b45c0                   mov        eax, dword ptr [ebp - 0x40]
004816db f30f104485ec             movss      xmm0, dword ptr [ebp + eax*4 - 0x14]
004816e1 f30f1145b8               movss      dword ptr [ebp - 0x48], xmm0
004816e6 ebae                     jmp        0x481696
004816e8 f30f100578fd5600         movss      xmm0, dword ptr [0x56fd78]
004816f0 0f2f45bc                 comiss     xmm0, dword ptr [ebp - 0x44]
004816f4 770e                     ja         0x481704
004816f6 f30f100578fd5600         movss      xmm0, dword ptr [0x56fd78]
004816fe 0f2f45b8                 comiss     xmm0, dword ptr [ebp - 0x48]
00481702 761f                     jbe        0x481723
00481704 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
00481707 8b9190000000             mov        edx, dword ptr [ecx + 0x90]
0048170d 81e2fffeffff             and        edx, 0xfffffeff
00481713 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
00481716 899090000000             mov        dword ptr [eax + 0x90], edx
0048171c b801000000               mov        eax, 1
00481721 eb4b                     jmp        0x48176e
00481723 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
00481726 81c1a0000000             add        ecx, 0xa0
0048172c 894d90                   mov        dword ptr [ebp - 0x70], ecx
0048172f 6a0b                     push       0xb
00481731 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
00481734 e847afffff               call       0x47c680
00481739 89458c                   mov        dword ptr [ebp - 0x74], eax
0048173c 6a00                     push       0
0048173e 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
00481741 e84a1efaff               call       0x423590
00481746 0fb6d0                   movzx      edx, al
00481749 85d2                     test       edx, edx
0048174b 741f                     je         0x48176c
0048174d 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
00481750 8b8890000000             mov        ecx, dword ptr [eax + 0x90]
00481756 81e1fffeffff             and        ecx, 0xfffffeff
0048175c 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
0048175f 898a90000000             mov        dword ptr [edx + 0x90], ecx
00481765 b801000000               mov        eax, 1
0048176a eb02                     jmp        0x48176e
0048176c 33c0                     xor        eax, eax
0048176e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00481771 33cd                     xor        ecx, ebp
00481773 e876120c00               call       0x5429ee
00481778 8be5                     mov        esp, ebp
0048177a 5d                       pop        ebp
0048177b c3                       ret        
0048177c cc                       int3       
0048177d cc                       int3       
0048177e cc                       int3       
0048177f cc                       int3       
