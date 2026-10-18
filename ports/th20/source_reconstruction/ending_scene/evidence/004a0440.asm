004a0440 55                       push       ebp
004a0441 8bec                     mov        ebp, esp
004a0443 83ec14                   sub        esp, 0x14
004a0446 894dfc                   mov        dword ptr [ebp - 4], ecx
004a0449 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a044c 8b4818                   mov        ecx, dword ptr [eax + 0x18]
004a044f e83c010000               call       0x4a0590
004a0454 85c0                     test       eax, eax
004a0456 7444                     je         0x4a049c
004a0458 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a045b 8b5120                   mov        edx, dword ptr [ecx + 0x20]
004a045e c1ea02                   shr        edx, 2
004a0461 83e201                   and        edx, 1
004a0464 7527                     jne        0x4a048d
004a0466 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a0469 8b4820                   mov        ecx, dword ptr [eax + 0x20]
004a046c 83c908                   or         ecx, 8
004a046f 8b55fc                   mov        edx, dword ptr [ebp - 4]
004a0472 894a20                   mov        dword ptr [edx + 0x20], ecx
004a0475 6a10                     push       0x10
004a0477 b9404d5c00               mov        ecx, 0x5c4d40
004a047c e82f0b0000               call       0x4a0fb0
004a0481 b801000000               mov        eax, 1
004a0486 e9f7000000               jmp        0x4a0582
004a048b eb0f                     jmp        0x4a049c
004a048d 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a0490 8b4820                   mov        ecx, dword ptr [eax + 0x20]
004a0493 83c908                   or         ecx, 8
004a0496 8b55fc                   mov        edx, dword ptr [ebp - 4]
004a0499 894a20                   mov        dword ptr [edx + 0x20], ecx
004a049c 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a049f 8b4824                   mov        ecx, dword ptr [eax + 0x24]
004a04a2 83c101                   add        ecx, 1
004a04a5 8b55fc                   mov        edx, dword ptr [ebp - 4]
004a04a8 894a24                   mov        dword ptr [edx + 0x24], ecx
004a04ab 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a04ae 8b4818                   mov        ecx, dword ptr [eax + 0x18]
004a04b1 e81a0a0000               call       0x4a0ed0
004a04b6 85c0                     test       eax, eax
004a04b8 0f85bf000000             jne        0x4a057d
004a04be 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a04c1 e8ea090000               call       0x4a0eb0
004a04c6 85c0                     test       eax, eax
004a04c8 0f85af000000             jne        0x4a057d
004a04ce 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a04d1 8b4918                   mov        ecx, dword ptr [ecx + 0x18]
004a04d4 e8170a0000               call       0x4a0ef0
004a04d9 85c0                     test       eax, eax
004a04db 0f849c000000             je         0x4a057d
004a04e1 33d2                     xor        edx, edx
004a04e3 7532                     jne        0x4a0517
004a04e5 b804000000               mov        eax, 4
004a04ea d1e0                     shl        eax, 1
004a04ec 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
004a04f3 7422                     je         0x4a0517
004a04f5 b904000000               mov        ecx, 4
004a04fa d1e1                     shl        ecx, 1
004a04fc 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
004a0502 8955f0                   mov        dword ptr [ebp - 0x10], edx
004a0505 6800020000               push       0x200
004a050a 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004a050d e88e90faff               call       0x4495a0
004a0512 8945f8                   mov        dword ptr [ebp - 8], eax
004a0515 eb07                     jmp        0x4a051e
004a0517 c745f800000000           mov        dword ptr [ebp - 8], 0
004a051e 837df800                 cmp        dword ptr [ebp - 8], 0
004a0522 7540                     jne        0x4a0564
004a0524 33c0                     xor        eax, eax
004a0526 752f                     jne        0x4a0557
004a0528 b904000000               mov        ecx, 4
004a052d d1e1                     shl        ecx, 1
004a052f 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
004a0536 741f                     je         0x4a0557
004a0538 ba04000000               mov        edx, 4
004a053d d1e2                     shl        edx, 1
004a053f 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
004a0545 8945ec                   mov        dword ptr [ebp - 0x14], eax
004a0548 6a00                     push       0
004a054a 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004a054d e8ce040000               call       0x4a0a20
004a0552 8945f4                   mov        dword ptr [ebp - 0xc], eax
004a0555 eb07                     jmp        0x4a055e
004a0557 c745f400000000           mov        dword ptr [ebp - 0xc], 0
004a055e 837df414                 cmp        dword ptr [ebp - 0xc], 0x14
004a0562 7219                     jb         0x4a057d
004a0564 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a0567 8b4124                   mov        eax, dword ptr [ecx + 0x24]
004a056a 99                       cdq        
004a056b b90c000000               mov        ecx, 0xc
004a0570 f7f9                     idiv       ecx
004a0572 85d2                     test       edx, edx
004a0574 7407                     je         0x4a057d
004a0576 b806000000               mov        eax, 6
004a057b eb05                     jmp        0x4a0582
004a057d b801000000               mov        eax, 1
004a0582 8be5                     mov        esp, ebp
004a0584 5d                       pop        ebp
004a0585 c3                       ret        
004a0586 cc                       int3       
004a0587 cc                       int3       
004a0588 cc                       int3       
004a0589 cc                       int3       
004a058a cc                       int3       
004a058b cc                       int3       
004a058c cc                       int3       
004a058d cc                       int3       
004a058e cc                       int3       
004a058f cc                       int3       
