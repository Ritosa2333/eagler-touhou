00508360 55                       push       ebp
00508361 8bec                     mov        ebp, esp
00508363 83ec1c                   sub        esp, 0x1c
00508366 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0050836b 33c5                     xor        eax, ebp
0050836d 8945fc                   mov        dword ptr [ebp - 4], eax
00508370 894dec                   mov        dword ptr [ebp - 0x14], ecx
00508373 833d28a85b0000           cmp        dword ptr [0x5ba828], 0
0050837a 750a                     jne        0x508386
0050837c b801000000               mov        eax, 1
00508381 e9e2000000               jmp        0x508468
00508386 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00508389 83781000                 cmp        dword ptr [eax + 0x10], 0
0050838d 7505                     jne        0x508394
0050838f e9cf000000               jmp        0x508463
00508394 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00508397 83791001                 cmp        dword ptr [ecx + 0x10], 1
0050839b 0f85c2000000             jne        0x508463
005083a1 51                       push       ecx
005083a2 0f57c0                   xorps      xmm0, xmm0
005083a5 f30f110424               movss      dword ptr [esp], xmm0
005083aa 51                       push       ecx
005083ab f30f10050c375700         movss      xmm0, dword ptr [0x57370c]
005083b3 f30f110424               movss      dword ptr [esp], xmm0
005083b8 51                       push       ecx
005083b9 f30f100508375700         movss      xmm0, dword ptr [0x573708]
005083c1 f30f110424               movss      dword ptr [esp], xmm0
005083c6 8d4df0                   lea        ecx, [ebp - 0x10]
005083c9 e802aaf1ff               call       0x422dd0
005083ce 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
005083d1 0fb6824c020000           movzx      eax, byte ptr [edx + 0x24c]
005083d8 f30f2ac0                 cvtsi2ss   xmm0, eax
005083dc f30f100d80f25600         movss      xmm1, dword ptr [0x56f280]
005083e4 0f2fc8                   comiss     xmm1, xmm0
005083e7 7609                     jbe        0x5083f2
005083e9 c745e4ff5050ff           mov        dword ptr [ebp - 0x1c], 0xff5050ff
005083f0 eb31                     jmp        0x508423
005083f2 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
005083f5 0fb6914c020000           movzx      edx, byte ptr [ecx + 0x24c]
005083fc f30f2ac2                 cvtsi2ss   xmm0, edx
00508400 f30f100d90ec5600         movss      xmm1, dword ptr [0x56ec90]
00508408 0f2fc8                   comiss     xmm1, xmm0
0050840b 7609                     jbe        0x508416
0050840d c745e8ffa0a0ff           mov        dword ptr [ebp - 0x18], 0xffa0a0ff
00508414 eb07                     jmp        0x50841d
00508416 c745e8ffffffff           mov        dword ptr [ebp - 0x18], 0xffffffff
0050841d 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
00508420 8945e4                   mov        dword ptr [ebp - 0x1c], eax
00508423 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00508426 51                       push       ecx
00508427 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0050842d e89e86f6ff               call       0x470ad0
00508432 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00508435 0fb6824c020000           movzx      eax, byte ptr [edx + 0x24c]
0050843c 50                       push       eax
0050843d 68a8345700               push       0x5734a8
00508442 8d4df0                   lea        ecx, [ebp - 0x10]
00508445 51                       push       ecx
00508446 8b1598065c00             mov        edx, dword ptr [0x5c0698]
0050844c 52                       push       edx
0050844d e83e45f6ff               call       0x46c990
00508452 83c410                   add        esp, 0x10
00508455 6aff                     push       -1
00508457 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0050845d e86e86f6ff               call       0x470ad0
00508462 90                       nop        
00508463 b801000000               mov        eax, 1
00508468 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0050846b 33cd                     xor        ecx, ebp
0050846d e87ca50300               call       0x5429ee
00508472 8be5                     mov        esp, ebp
00508474 5d                       pop        ebp
00508475 c3                       ret        
00508476 cc                       int3       
00508477 cc                       int3       
00508478 cc                       int3       
00508479 cc                       int3       
0050847a cc                       int3       
0050847b cc                       int3       
0050847c cc                       int3       
0050847d cc                       int3       
0050847e cc                       int3       
0050847f cc                       int3       
