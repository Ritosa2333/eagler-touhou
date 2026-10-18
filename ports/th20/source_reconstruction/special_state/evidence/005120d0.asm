005120d0 55                       push       ebp
005120d1 8bec                     mov        ebp, esp
005120d3 6aff                     push       -1
005120d5 6865aa5600               push       0x56aa65
005120da 64a100000000             mov        eax, dword ptr fs:[0]
005120e0 50                       push       eax
005120e1 81ec1c010000             sub        esp, 0x11c
005120e7 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
005120ec 33c5                     xor        eax, ebp
005120ee 8945f0                   mov        dword ptr [ebp - 0x10], eax
005120f1 56                       push       esi
005120f2 50                       push       eax
005120f3 8d45f4                   lea        eax, [ebp - 0xc]
005120f6 64a300000000             mov        dword ptr fs:[0], eax
005120fc 894d88                   mov        dword ptr [ebp - 0x78], ecx
005120ff b968a55b00               mov        ecx, 0x5ba568
00512104 e84767f7ff               call       0x488850
00512109 0fb6c0                   movzx      eax, al
0051210c 85c0                     test       eax, eax
0051210e 741e                     je         0x51212e
00512110 6a00                     push       0
00512112 e8691ff5ff               call       0x464080
00512117 83c404                   add        esp, 4
0051211a 898508ffffff             mov        dword ptr [ebp - 0xf8], eax
00512120 6a00                     push       0
00512122 8b8d08ffffff             mov        ecx, dword ptr [ebp - 0xf8]
00512128 e8f3c4faff               call       0x4be620
0051212d 90                       nop        
0051212e 6a00                     push       0
00512130 e82b5ff6ff               call       0x478060
00512135 83c404                   add        esp, 4
00512138 85c0                     test       eax, eax
0051213a 0f848c000000             je         0x5121cc
00512140 6a00                     push       0
00512142 e8195ff6ff               call       0x478060
00512147 83c404                   add        esp, 4
0051214a 8bc8                     mov        ecx, eax
0051214c e8cf9cf8ff               call       0x49be20
00512151 0fb6c8                   movzx      ecx, al
00512154 85c9                     test       ecx, ecx
00512156 7474                     je         0x5121cc
00512158 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
0051215b e8e038f7ff               call       0x485a40
00512160 0fb6d0                   movzx      edx, al
00512163 85d2                     test       edx, edx
00512165 7565                     jne        0x5121cc
00512167 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
0051216d e8ee5ff6ff               call       0x478160
00512172 85c0                     test       eax, eax
00512174 7556                     jne        0x5121cc
00512176 8b4588                   mov        eax, dword ptr [ebp - 0x78]
00512179 83c028                   add        eax, 0x28
0051217c 8985ecfeffff             mov        dword ptr [ebp - 0x114], eax
00512182 6a03                     push       3
00512184 8b8decfeffff             mov        ecx, dword ptr [ebp - 0x114]
0051218a e8b1fef5ff               call       0x472040
0051218f 83f801                   cmp        eax, 1
00512192 7f1e                     jg         0x5121b2
00512194 6a00                     push       0
00512196 e8e51ef5ff               call       0x464080
0051219b 83c404                   add        esp, 4
0051219e 8985e8feffff             mov        dword ptr [ebp - 0x118], eax
005121a4 6a01                     push       1
005121a6 8b8de8feffff             mov        ecx, dword ptr [ebp - 0x118]
005121ac e8af5df6ff               call       0x477f60
005121b1 90                       nop        
005121b2 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
005121b5 83c128                   add        ecx, 0x28
005121b8 898d5cffffff             mov        dword ptr [ebp - 0xa4], ecx
005121be 6a00                     push       0
005121c0 8b8d5cffffff             mov        ecx, dword ptr [ebp - 0xa4]
005121c6 e87513f1ff               call       0x423540
005121cb 90                       nop        
005121cc 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
005121cf e86c38f7ff               call       0x485a40
005121d4 0fb6d0                   movzx      edx, al
005121d7 85d2                     test       edx, edx
005121d9 0f85c1040000             jne        0x5126a0
005121df 6a00                     push       0
005121e1 e89a1ef5ff               call       0x464080
005121e6 83c404                   add        esp, 4
005121e9 8bc8                     mov        ecx, eax
005121eb e83073f8ff               call       0x499520
005121f0 8bf0                     mov        esi, eax
005121f2 6a00                     push       0
005121f4 e8871ef5ff               call       0x464080
005121f9 83c404                   add        esp, 4
005121fc 8bc8                     mov        ecx, eax
005121fe e85d1c0000               call       0x513e60
00512203 3bf0                     cmp        esi, eax
00512205 0f8c95040000             jl         0x5126a0
0051220b 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
00512211 e84a5ff6ff               call       0x478160
00512216 85c0                     test       eax, eax
00512218 0f8582040000             jne        0x5126a0
0051221e 833d14615c0000           cmp        dword ptr [0x5c6114], 0
00512225 0f8575040000             jne        0x5126a0
0051222b 8d4d8c                   lea        ecx, [ebp - 0x74]
0051222e e8fd98f6ff               call       0x47bb30
00512233 c7458000000000           mov        dword ptr [ebp - 0x80], 0
0051223a c7857cffffffffffffff     mov        dword ptr [ebp - 0x84], 0xffffffff
00512244 c7458400000000           mov        dword ptr [ebp - 0x7c], 0
0051224b 6a00                     push       0
0051224d e82e1ef5ff               call       0x464080
00512252 83c404                   add        esp, 4
00512255 8bc8                     mov        ecx, eax
00512257 e8c41c0000               call       0x513f20
0051225c 85c0                     test       eax, eax
0051225e 0f8cac000000             jl         0x512310
00512264 6a00                     push       0
00512266 e8151ef5ff               call       0x464080
0051226b 83c404                   add        esp, 4
0051226e 8bc8                     mov        ecx, eax
00512270 e8ab1c0000               call       0x513f20
00512275 894580                   mov        dword ptr [ebp - 0x80], eax
00512278 8b4580                   mov        eax, dword ptr [ebp - 0x80]
0051227b 898570ffffff             mov        dword ptr [ebp - 0x90], eax
00512281 83bd70ffffff03           cmp        dword ptr [ebp - 0x90], 3
00512288 770d                     ja         0x512297
0051228a 8b8d70ffffff             mov        ecx, dword ptr [ebp - 0x90]
00512290 ff248d58295100           jmp        dword ptr [ecx*4 + 0x512958]
00512297 6a00                     push       0
00512299 e8e21df5ff               call       0x464080
0051229e 83c404                   add        esp, 4
005122a1 8bc8                     mov        ecx, eax
005122a3 e848f2ffff               call       0x5114f0
005122a8 894584                   mov        dword ptr [ebp - 0x7c], eax
005122ab eb40                     jmp        0x5122ed
005122ad 6a00                     push       0
005122af e8cc1df5ff               call       0x464080
005122b4 83c404                   add        esp, 4
005122b7 8bc8                     mov        ecx, eax
005122b9 e8a2f1ffff               call       0x511460
005122be 894584                   mov        dword ptr [ebp - 0x7c], eax
005122c1 eb2a                     jmp        0x5122ed
005122c3 6a00                     push       0
005122c5 e8b61df5ff               call       0x464080
005122ca 83c404                   add        esp, 4
005122cd 8bc8                     mov        ecx, eax
005122cf e85cf2ffff               call       0x511530
005122d4 894584                   mov        dword ptr [ebp - 0x7c], eax
005122d7 eb14                     jmp        0x5122ed
005122d9 6a00                     push       0
005122db e8a01df5ff               call       0x464080
005122e0 83c404                   add        esp, 4
005122e3 8bc8                     mov        ecx, eax
005122e5 e8b6f1ffff               call       0x5114a0
005122ea 894584                   mov        dword ptr [ebp - 0x7c], eax
005122ed 6a00                     push       0
005122ef e88c1df5ff               call       0x464080
005122f4 83c404                   add        esp, 4
005122f7 898558ffffff             mov        dword ptr [ebp - 0xa8], eax
005122fd 6aff                     push       -1
005122ff 8b8d58ffffff             mov        ecx, dword ptr [ebp - 0xa8]
00512305 e8661e0000               call       0x514170
0051230a 90                       nop        
0051230b e92c010000               jmp        0x51243c
00512310 6a00                     push       0
00512312 e8691df5ff               call       0x464080
00512317 83c404                   add        esp, 4
0051231a 8bc8                     mov        ecx, eax
0051231c e87f1b0000               call       0x513ea0
00512321 3b857cffffff             cmp        eax, dword ptr [ebp - 0x84]
00512327 7e32                     jle        0x51235b
00512329 6a00                     push       0
0051232b e8501df5ff               call       0x464080
00512330 83c404                   add        esp, 4
00512333 8bc8                     mov        ecx, eax
00512335 e8661b0000               call       0x513ea0
0051233a 89857cffffff             mov        dword ptr [ebp - 0x84], eax
00512340 6a00                     push       0
00512342 e8391df5ff               call       0x464080
00512347 83c404                   add        esp, 4
0051234a 8bc8                     mov        ecx, eax
0051234c e89ff1ffff               call       0x5114f0
00512351 894584                   mov        dword ptr [ebp - 0x7c], eax
00512354 c7458000000000           mov        dword ptr [ebp - 0x80], 0
0051235b 6a00                     push       0
0051235d e81e1df5ff               call       0x464080
00512362 83c404                   add        esp, 4
00512365 8bc8                     mov        ecx, eax
00512367 e8741a0000               call       0x513de0
0051236c 3b857cffffff             cmp        eax, dword ptr [ebp - 0x84]
00512372 7e32                     jle        0x5123a6
00512374 6a00                     push       0
00512376 e8051df5ff               call       0x464080
0051237b 83c404                   add        esp, 4
0051237e 8bc8                     mov        ecx, eax
00512380 e85b1a0000               call       0x513de0
00512385 89857cffffff             mov        dword ptr [ebp - 0x84], eax
0051238b 6a00                     push       0
0051238d e8ee1cf5ff               call       0x464080
00512392 83c404                   add        esp, 4
00512395 8bc8                     mov        ecx, eax
00512397 e8c4f0ffff               call       0x511460
0051239c 894584                   mov        dword ptr [ebp - 0x7c], eax
0051239f c7458001000000           mov        dword ptr [ebp - 0x80], 1
005123a6 6a00                     push       0
005123a8 e8d31cf5ff               call       0x464080
005123ad 83c404                   add        esp, 4
005123b0 8bc8                     mov        ecx, eax
005123b2 e8291b0000               call       0x513ee0
005123b7 3b857cffffff             cmp        eax, dword ptr [ebp - 0x84]
005123bd 7e32                     jle        0x5123f1
005123bf 6a00                     push       0
005123c1 e8ba1cf5ff               call       0x464080
005123c6 83c404                   add        esp, 4
005123c9 8bc8                     mov        ecx, eax
005123cb e8101b0000               call       0x513ee0
005123d0 89857cffffff             mov        dword ptr [ebp - 0x84], eax
005123d6 6a00                     push       0
005123d8 e8a31cf5ff               call       0x464080
005123dd 83c404                   add        esp, 4
005123e0 8bc8                     mov        ecx, eax
005123e2 e849f1ffff               call       0x511530
005123e7 894584                   mov        dword ptr [ebp - 0x7c], eax
005123ea c7458002000000           mov        dword ptr [ebp - 0x80], 2
005123f1 6a00                     push       0
005123f3 e8881cf5ff               call       0x464080
005123f8 83c404                   add        esp, 4
005123fb 8bc8                     mov        ecx, eax
005123fd e81e1a0000               call       0x513e20
00512402 3b857cffffff             cmp        eax, dword ptr [ebp - 0x84]
00512408 7e32                     jle        0x51243c
0051240a 6a00                     push       0
0051240c e86f1cf5ff               call       0x464080
00512411 83c404                   add        esp, 4
00512414 8bc8                     mov        ecx, eax
00512416 e8051a0000               call       0x513e20
0051241b 89857cffffff             mov        dword ptr [ebp - 0x84], eax
00512421 6a00                     push       0
00512423 e8581cf5ff               call       0x464080
00512428 83c404                   add        esp, 4
0051242b 8bc8                     mov        ecx, eax
0051242d e86ef0ffff               call       0x5114a0
00512432 894584                   mov        dword ptr [ebp - 0x7c], eax
00512435 c7458003000000           mov        dword ptr [ebp - 0x80], 3
0051243c b9a8a45b00               mov        ecx, 0x5ba4a8
00512441 e89a74f1ff               call       0x4298e0
00512446 d99d54ffffff             fstp       dword ptr [ebp - 0xac]
0051244c f30f108554ffffff         movss      xmm0, dword ptr [ebp - 0xac]
00512454 f30f5905d40a5700         mulss      xmm0, dword ptr [0x570ad4]
0051245c 51                       push       ecx
0051245d 0f57c9                   xorps      xmm1, xmm1
00512460 f30f110c24               movss      dword ptr [esp], xmm1
00512465 51                       push       ecx
00512466 f30f100dd00a5700         movss      xmm1, dword ptr [0x570ad0]
0051246e f30f110c24               movss      dword ptr [esp], xmm1
00512473 51                       push       ecx
00512474 f30f110424               movss      dword ptr [esp], xmm0
00512479 8d8dd8feffff             lea        ecx, [ebp - 0x128]
0051247f e84c09f1ff               call       0x422dd0
00512484 8b10                     mov        edx, dword ptr [eax]
00512486 89558c                   mov        dword ptr [ebp - 0x74], edx
00512489 8b4804                   mov        ecx, dword ptr [eax + 4]
0051248c 894d90                   mov        dword ptr [ebp - 0x70], ecx
0051248f 8b5008                   mov        edx, dword ptr [eax + 8]
00512492 895594                   mov        dword ptr [ebp - 0x6c], edx
00512495 6b458014                 imul       eax, dword ptr [ebp - 0x80], 0x14
00512499 8b4d84                   mov        ecx, dword ptr [ebp - 0x7c]
0051249c 8b9488f03f5700           mov        edx, dword ptr [eax + ecx*4 + 0x573ff0]
005124a3 8955a0                   mov        dword ptr [ebp - 0x60], edx
005124a6 8b4588                   mov        eax, dword ptr [ebp - 0x78]
005124a9 c680b000000001           mov        byte ptr [eax + 0xb0], 1
005124b0 6a00                     push       0
005124b2 e8a95bf6ff               call       0x478060
005124b7 83c404                   add        esp, 4
005124ba 89854cffffff             mov        dword ptr [ebp - 0xb4], eax
005124c0 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
005124c3 8b148dd8095b00           mov        edx, dword ptr [ecx*4 + 0x5b09d8]
005124ca 899550ffffff             mov        dword ptr [ebp - 0xb0], edx
005124d0 6a00                     push       0
005124d2 8d458c                   lea        eax, [ebp - 0x74]
005124d5 50                       push       eax
005124d6 8b8d50ffffff             mov        ecx, dword ptr [ebp - 0xb0]
005124dc 51                       push       ecx
005124dd 8b8d4cffffff             mov        ecx, dword ptr [ebp - 0xb4]
005124e3 e83864f9ff               call       0x4a8920
005124e8 90                       nop        
005124e9 8b5580                   mov        edx, dword ptr [ebp - 0x80]
005124ec 89956cffffff             mov        dword ptr [ebp - 0x94], edx
005124f2 83bd6cffffff03           cmp        dword ptr [ebp - 0x94], 3
005124f9 0f878b000000             ja         0x51258a
005124ff 8b856cffffff             mov        eax, dword ptr [ebp - 0x94]
00512505 ff248568295100           jmp        dword ptr [eax*4 + 0x512968]
0051250c 6a00                     push       0
0051250e e86d1bf5ff               call       0x464080
00512513 83c404                   add        esp, 4
00512516 898548ffffff             mov        dword ptr [ebp - 0xb8], eax
0051251c 6a00                     push       0
0051251e 8b8d48ffffff             mov        ecx, dword ptr [ebp - 0xb8]
00512524 e837c2faff               call       0x4be760
00512529 90                       nop        
0051252a eb5e                     jmp        0x51258a
0051252c 6a00                     push       0
0051252e e84d1bf5ff               call       0x464080
00512533 83c404                   add        esp, 4
00512536 898544ffffff             mov        dword ptr [ebp - 0xbc], eax
0051253c 6a00                     push       0
0051253e 8b8d44ffffff             mov        ecx, dword ptr [ebp - 0xbc]
00512544 e827c1faff               call       0x4be670
00512549 90                       nop        
0051254a eb3e                     jmp        0x51258a
0051254c 6a00                     push       0
0051254e e82d1bf5ff               call       0x464080
00512553 83c404                   add        esp, 4
00512556 898540ffffff             mov        dword ptr [ebp - 0xc0], eax
0051255c 6a00                     push       0
0051255e 8b8d40ffffff             mov        ecx, dword ptr [ebp - 0xc0]
00512564 e847c2faff               call       0x4be7b0
00512569 90                       nop        
0051256a eb1e                     jmp        0x51258a
0051256c 6a00                     push       0
0051256e e80d1bf5ff               call       0x464080
00512573 83c404                   add        esp, 4
00512576 89853cffffff             mov        dword ptr [ebp - 0xc4], eax
0051257c 6a00                     push       0
0051257e 8b8d3cffffff             mov        ecx, dword ptr [ebp - 0xc4]
00512584 e837c1faff               call       0x4be6c0
00512589 90                       nop        
0051258a 6a00                     push       0
0051258c e8ef1af5ff               call       0x464080
00512591 83c404                   add        esp, 4
00512594 898534ffffff             mov        dword ptr [ebp - 0xcc], eax
0051259a 6a00                     push       0
0051259c e8df1af5ff               call       0x464080
005125a1 83c404                   add        esp, 4
005125a4 8bc8                     mov        ecx, eax
005125a6 e8b5180000               call       0x513e60
005125ab 99                       cdq        
005125ac b903000000               mov        ecx, 3
005125b1 f7f9                     idiv       ecx
005125b3 898538ffffff             mov        dword ptr [ebp - 0xc8], eax
005125b9 8b9538ffffff             mov        edx, dword ptr [ebp - 0xc8]
005125bf 52                       push       edx
005125c0 8b8d34ffffff             mov        ecx, dword ptr [ebp - 0xcc]
005125c6 e8951c0000               call       0x514260
005125cb 6a00                     push       0
005125cd 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
005125d0 83c118                   add        ecx, 0x18
005125d3 e8480ff1ff               call       0x423520
005125d8 90                       nop        
005125d9 8b4588                   mov        eax, dword ptr [ebp - 0x78]
005125dc 83c038                   add        eax, 0x38
005125df 898528ffffff             mov        dword ptr [ebp - 0xd8], eax
005125e5 0f57c0                   xorps      xmm0, xmm0
005125e8 f30f118530ffffff         movss      dword ptr [ebp - 0xd0], xmm0
005125f0 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
005125f8 f30f11852cffffff         movss      dword ptr [ebp - 0xd4], xmm0
00512600 8d8d30ffffff             lea        ecx, [ebp - 0xd0]
00512606 51                       push       ecx
00512607 8d952cffffff             lea        edx, [ebp - 0xd4]
0051260d 52                       push       edx
0051260e 6a04                     push       4
00512610 6a3c                     push       0x3c
00512612 8b8d28ffffff             mov        ecx, dword ptr [ebp - 0xd8]
00512618 e8336ff2ff               call       0x439550
0051261d 90                       nop        
0051261e 8b4588                   mov        eax, dword ptr [ebp - 0x78]
00512621 83c064                   add        eax, 0x64
00512624 898524ffffff             mov        dword ptr [ebp - 0xdc], eax
0051262a c6857affffff00           mov        byte ptr [ebp - 0x86], 0
00512631 c6857bffffffff           mov        byte ptr [ebp - 0x85], 0xff
00512638 8d8d7affffff             lea        ecx, [ebp - 0x86]
0051263e 51                       push       ecx
0051263f 8d957bffffff             lea        edx, [ebp - 0x85]
00512645 52                       push       edx
00512646 6a04                     push       4
00512648 6a78                     push       0x78
0051264a 8b8d24ffffff             mov        ecx, dword ptr [ebp - 0xdc]
00512650 e88b1b0000               call       0x5141e0
00512655 90                       nop        
00512656 8b4588                   mov        eax, dword ptr [ebp - 0x78]
00512659 0584000000               add        eax, 0x84
0051265e 898518ffffff             mov        dword ptr [ebp - 0xe8], eax
00512664 0f57c0                   xorps      xmm0, xmm0
00512667 f30f118520ffffff         movss      dword ptr [ebp - 0xe0], xmm0
0051266f f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
00512677 f30f11851cffffff         movss      dword ptr [ebp - 0xe4], xmm0
0051267f 8d8d20ffffff             lea        ecx, [ebp - 0xe0]
00512685 51                       push       ecx
00512686 8d951cffffff             lea        edx, [ebp - 0xe4]
0051268c 52                       push       edx
0051268d 6a05                     push       5
0051268f 6828050000               push       0x528
00512694 8b8d18ffffff             mov        ecx, dword ptr [ebp - 0xe8]
0051269a e8b16ef2ff               call       0x439550
0051269f 90                       nop        
005126a0 c78574ffffff00000000     mov        dword ptr [ebp - 0x8c], 0
005126aa 8b4588                   mov        eax, dword ptr [ebp - 0x78]
005126ad 898568ffffff             mov        dword ptr [ebp - 0x98], eax
005126b3 6a08                     push       8
005126b5 8d4de8                   lea        ecx, [ebp - 0x18]
005126b8 e8c399efff               call       0x40c080
005126bd 8d4de8                   lea        ecx, [ebp - 0x18]
005126c0 51                       push       ecx
005126c1 8b8d68ffffff             mov        ecx, dword ptr [ebp - 0x98]
005126c7 e8b4fbefff               call       0x412280
005126cc c745fc00000000           mov        dword ptr [ebp - 4], 0
005126d3 8b8d68ffffff             mov        ecx, dword ptr [ebp - 0x98]
005126d9 e862feefff               call       0x412540
005126de 898514ffffff             mov        dword ptr [ebp - 0xec], eax
005126e4 eb09                     jmp        0x5126ef
005126e6 8d4de8                   lea        ecx, [ebp - 0x18]
005126e9 e842f5efff               call       0x411c30
005126ee 90                       nop        
005126ef 8b9514ffffff             mov        edx, dword ptr [ebp - 0xec]
005126f5 52                       push       edx
005126f6 8d4de8                   lea        ecx, [ebp - 0x18]
005126f9 e8b2f4efff               call       0x411bb0
005126fe 0fb6c0                   movzx      eax, al
00512701 85c0                     test       eax, eax
00512703 7451                     je         0x512756
00512705 8d4de8                   lea        ecx, [ebp - 0x18]
00512708 e8f39befff               call       0x40c300
0051270d 898564ffffff             mov        dword ptr [ebp - 0x9c], eax
00512713 8b8d74ffffff             mov        ecx, dword ptr [ebp - 0x8c]
00512719 83c101                   add        ecx, 1
0051271c 898d74ffffff             mov        dword ptr [ebp - 0x8c], ecx
00512722 8b8d64ffffff             mov        ecx, dword ptr [ebp - 0x9c]
00512728 e8d39befff               call       0x40c300
0051272d 8bc8                     mov        ecx, eax
0051272f e84c020000               call       0x512980
00512734 85c0                     test       eax, eax
00512736 741c                     je         0x512754
00512738 8b8d64ffffff             mov        ecx, dword ptr [ebp - 0x9c]
0051273e e8bd9befff               call       0x40c300
00512743 8bc8                     mov        ecx, eax
00512745 e8e6070000               call       0x512f30
0051274a 8b5588                   mov        edx, dword ptr [ebp - 0x78]
0051274d c682b000000000           mov        byte ptr [edx + 0xb0], 0
00512754 eb90                     jmp        0x5126e6
00512756 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0051275d 8d4de8                   lea        ecx, [ebp - 0x18]
00512760 e89bf3efff               call       0x411b00
00512765 90                       nop        
00512766 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
00512769 e8d232f7ff               call       0x485a40
0051276e 0fb6c0                   movzx      eax, al
00512771 85c0                     test       eax, eax
00512773 0f84c4010000             je         0x51293d
00512779 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
0051277c 83c118                   add        ecx, 0x18
0051277f 898d10ffffff             mov        dword ptr [ebp - 0xf0], ecx
00512785 6a0a                     push       0xa
00512787 8b8d10ffffff             mov        ecx, dword ptr [ebp - 0xf0]
0051278d e8dee8f4ff               call       0x461070
00512792 0fb6d0                   movzx      edx, al
00512795 85d2                     test       edx, edx
00512797 7413                     je         0x5127ac
00512799 83bd74ffffff00           cmp        dword ptr [ebp - 0x8c], 0
005127a0 750a                     jne        0x5127ac
005127a2 8b4588                   mov        eax, dword ptr [ebp - 0x78]
005127a5 c680b000000000           mov        byte ptr [eax + 0xb0], 0
005127ac 6a00                     push       0
005127ae e8cd18f5ff               call       0x464080
005127b3 83c404                   add        esp, 4
005127b6 898500ffffff             mov        dword ptr [ebp - 0x100], eax
005127bc 6a00                     push       0
005127be e8bd18f5ff               call       0x464080
005127c3 83c404                   add        esp, 4
005127c6 8bc8                     mov        ecx, eax
005127c8 e893160000               call       0x513e60
005127cd f30f2ac0                 cvtsi2ss   xmm0, eax
005127d1 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
005127d4 81c184000000             add        ecx, 0x84
005127da f30f11850cffffff         movss      dword ptr [ebp - 0xf4], xmm0
005127e2 e809f5ffff               call       0x511cf0
005127e7 d99df4feffff             fstp       dword ptr [ebp - 0x10c]
005127ed f30f10850cffffff         movss      xmm0, dword ptr [ebp - 0xf4]
005127f5 f30f5985f4feffff         mulss      xmm0, dword ptr [ebp - 0x10c]
005127fd f30f2cc8                 cvttss2si  ecx, xmm0
00512801 898d04ffffff             mov        dword ptr [ebp - 0xfc], ecx
00512807 8b9504ffffff             mov        edx, dword ptr [ebp - 0xfc]
0051280d 52                       push       edx
0051280e 8b8d00ffffff             mov        ecx, dword ptr [ebp - 0x100]
00512814 e807befaff               call       0x4be620
00512819 8b4588                   mov        eax, dword ptr [ebp - 0x78]
0051281c 83c018                   add        eax, 0x18
0051281f 8985fcfeffff             mov        dword ptr [ebp - 0x104], eax
00512825 6a00                     push       0
00512827 8b8dfcfeffff             mov        ecx, dword ptr [ebp - 0x104]
0051282d e80e0df1ff               call       0x423540
00512832 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
00512835 83c138                   add        ecx, 0x38
00512838 e8d378f1ff               call       0x42a110
0051283d ddd8                     fstp       st(0)
0051283f 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
00512842 83c164                   add        ecx, 0x64
00512845 e8c60e0000               call       0x513710
0051284a 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
0051284d 81c184000000             add        ecx, 0x84
00512853 e8b878f1ff               call       0x42a110
00512858 ddd8                     fstp       st(0)
0051285a 90                       nop        
0051285b 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
0051285e 81c184000000             add        ecx, 0x84
00512864 e817180000               call       0x514080
00512869 0fb6c8                   movzx      ecx, al
0051286c 85c9                     test       ecx, ecx
0051286e 7427                     je         0x512897
00512870 8b5588                   mov        edx, dword ptr [ebp - 0x78]
00512873 83c218                   add        edx, 0x18
00512876 8995f8feffff             mov        dword ptr [ebp - 0x108], edx
0051287c 6808070000               push       0x708
00512881 8b8df8feffff             mov        ecx, dword ptr [ebp - 0x108]
00512887 e8340df1ff               call       0x4235c0
0051288c 0fb6c0                   movzx      eax, al
0051288f 85c0                     test       eax, eax
00512891 0f84a6000000             je         0x51293d
00512897 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
0051289a c681b000000000           mov        byte ptr [ecx + 0xb0], 0
005128a1 6a00                     push       0
005128a3 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
005128a6 83c118                   add        ecx, 0x18
005128a9 e8720cf1ff               call       0x423520
005128ae 90                       nop        
005128af 8b5588                   mov        edx, dword ptr [ebp - 0x78]
005128b2 899560ffffff             mov        dword ptr [ebp - 0xa0], edx
005128b8 6a08                     push       8
005128ba 8d4de0                   lea        ecx, [ebp - 0x20]
005128bd e8be97efff               call       0x40c080
005128c2 8d45e0                   lea        eax, [ebp - 0x20]
005128c5 50                       push       eax
005128c6 8b8d60ffffff             mov        ecx, dword ptr [ebp - 0xa0]
005128cc e8aff9efff               call       0x412280
005128d1 c745fc01000000           mov        dword ptr [ebp - 4], 1
005128d8 8b8d60ffffff             mov        ecx, dword ptr [ebp - 0xa0]
005128de e85dfcefff               call       0x412540
005128e3 8985e4feffff             mov        dword ptr [ebp - 0x11c], eax
005128e9 eb09                     jmp        0x5128f4
005128eb 8d4de0                   lea        ecx, [ebp - 0x20]
005128ee e83df3efff               call       0x411c30
005128f3 90                       nop        
005128f4 8b8de4feffff             mov        ecx, dword ptr [ebp - 0x11c]
005128fa 51                       push       ecx
005128fb 8d4de0                   lea        ecx, [ebp - 0x20]
005128fe e8adf2efff               call       0x411bb0
00512903 0fb6d0                   movzx      edx, al
00512906 85d2                     test       edx, edx
00512908 7423                     je         0x51292d
0051290a 8d4de0                   lea        ecx, [ebp - 0x20]
0051290d e8ee99efff               call       0x40c300
00512912 8985f0feffff             mov        dword ptr [ebp - 0x110], eax
00512918 8b8df0feffff             mov        ecx, dword ptr [ebp - 0x110]
0051291e e8dd99efff               call       0x40c300
00512923 8bc8                     mov        ecx, eax
00512925 e806060000               call       0x512f30
0051292a 90                       nop        
0051292b ebbe                     jmp        0x5128eb
0051292d c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
00512934 8d4de0                   lea        ecx, [ebp - 0x20]
00512937 e8c4f1efff               call       0x411b00
0051293c 90                       nop        
0051293d 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00512940 64890d00000000           mov        dword ptr fs:[0], ecx
00512947 59                       pop        ecx
00512948 5e                       pop        esi
00512949 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0051294c 33cd                     xor        ecx, ebp
0051294e e89b000300               call       0x5429ee
00512953 8be5                     mov        esp, ebp
00512955 5d                       pop        ebp
00512956 c3                       ret        
00512957 90                       nop        
00512958 97                       xchg       edi, eax
00512959 225100                   and        dl, byte ptr [ecx]
0051295c ad                       lodsd      eax, dword ptr [esi]
0051295d 225100                   and        dl, byte ptr [ecx]
00512960 c3                       ret        
00512961 225100                   and        dl, byte ptr [ecx]
00512964 d922                     fldenv     [edx]
00512966 51                       push       ecx
00512967 000c2551002c25           add        byte ptr [0x252c0051], cl
0051296e 51                       push       ecx
0051296f 004c2551                 add        byte ptr [ebp + 0x51], cl
00512973 006c2551                 add        byte ptr [ebp + 0x51], ch
00512977 00cc                     add        ah, cl
00512979 cc                       int3       
0051297a cc                       int3       
0051297b cc                       int3       
0051297c cc                       int3       
0051297d cc                       int3       
0051297e cc                       int3       
0051297f cc                       int3       
