0052d5c0 55                       push       ebp
0052d5c1 8bec                     mov        ebp, esp
0052d5c3 6aff                     push       -1
0052d5c5 688d825600               push       0x56828d
0052d5ca 64a100000000             mov        eax, dword ptr fs:[0]
0052d5d0 50                       push       eax
0052d5d1 83ec20                   sub        esp, 0x20
0052d5d4 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0052d5d9 33c5                     xor        eax, ebp
0052d5db 8945f0                   mov        dword ptr [ebp - 0x10], eax
0052d5de 50                       push       eax
0052d5df 8d45f4                   lea        eax, [ebp - 0xc]
0052d5e2 64a300000000             mov        dword ptr fs:[0], eax
0052d5e8 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
0052d5eb 6a01                     push       1
0052d5ed b940025c00               mov        ecx, 0x5c0240
0052d5f2 e819ededff               call       0x40c310
0052d5f7 50                       push       eax
0052d5f8 8d4dec                   lea        ecx, [ebp - 0x14]
0052d5fb e830e1edff               call       0x40b730
0052d600 c745fc00000000           mov        dword ptr [ebp - 4], 0
0052d607 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0052d60a 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0052d60d 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
0052d610 b904070000               mov        ecx, 0x704
0052d615 f7e1                     mul        ecx
0052d617 baffffffff               mov        edx, 0xffffffff
0052d61c 0f42c2                   cmovb      eax, edx
0052d61f 50                       push       eax
0052d620 e8dc530100               call       0x542a01
0052d625 83c404                   add        esp, 4
0052d628 8945e8                   mov        dword ptr [ebp - 0x18], eax
0052d62b 837de800                 cmp        dword ptr [ebp - 0x18], 0
0052d62f 741f                     je         0x52d650
0052d631 68c05c4200               push       0x425cc0
0052d636 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
0052d639 50                       push       eax
0052d63a 6804070000               push       0x704
0052d63f 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0052d642 51                       push       ecx
0052d643 e8d8e5edff               call       0x40bc20
0052d648 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
0052d64b 8955e0                   mov        dword ptr [ebp - 0x20], edx
0052d64e eb07                     jmp        0x52d657
0052d650 c745e000000000           mov        dword ptr [ebp - 0x20], 0
0052d657 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
0052d65a 8945dc                   mov        dword ptr [ebp - 0x24], eax
0052d65d 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0052d660 894dd8                   mov        dword ptr [ebp - 0x28], ecx
0052d663 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0052d66a 8d4dec                   lea        ecx, [ebp - 0x14]
0052d66d e88ee2edff               call       0x40b900
0052d672 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
0052d675 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0052d678 64890d00000000           mov        dword ptr fs:[0], ecx
0052d67f 59                       pop        ecx
0052d680 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052d683 33cd                     xor        ecx, ebp
0052d685 e864530100               call       0x5429ee
0052d68a 8be5                     mov        esp, ebp
0052d68c 5d                       pop        ebp
0052d68d c20800                   ret        8
