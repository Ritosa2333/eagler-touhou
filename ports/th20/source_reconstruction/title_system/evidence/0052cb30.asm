0052cb30 55                       push       ebp
0052cb31 8bec                     mov        ebp, esp
0052cb33 83ec10                   sub        esp, 0x10
0052cb36 894df0                   mov        dword ptr [ebp - 0x10], ecx
0052cb39 c745fc00000000           mov        dword ptr [ebp - 4], 0
0052cb40 eb09                     jmp        0x52cb4b
0052cb42 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052cb45 83c001                   add        eax, 1
0052cb48 8945fc                   mov        dword ptr [ebp - 4], eax
0052cb4b 837dfc05                 cmp        dword ptr [ebp - 4], 5
0052cb4f 7d75                     jge        0x52cbc6
0052cb51 694d0804010000           imul       ecx, dword ptr [ebp + 8], 0x104
0052cb58 6b550c14                 imul       edx, dword ptr [ebp + 0xc], 0x14
0052cb5c 8d841120f25a00           lea        eax, [ecx + edx + 0x5af220]
0052cb63 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052cb66 833c8800                 cmp        dword ptr [eax + ecx*4], 0
0052cb6a 7d02                     jge        0x52cb6e
0052cb6c eb58                     jmp        0x52cbc6
0052cb6e 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
0052cb74 e887bbf5ff               call       0x488700
0052cb79 05080b0000               add        eax, 0xb08
0052cb7e 8945f4                   mov        dword ptr [ebp - 0xc], eax
0052cb81 69550804010000           imul       edx, dword ptr [ebp + 8], 0x104
0052cb88 6b450c14                 imul       eax, dword ptr [ebp + 0xc], 0x14
0052cb8c 8d8c0220f25a00           lea        ecx, [edx + eax + 0x5af220]
0052cb93 8b55fc                   mov        edx, dword ptr [ebp - 4]
0052cb96 8b0491                   mov        eax, dword ptr [ecx + edx*4]
0052cb99 8945f8                   mov        dword ptr [ebp - 8], eax
0052cb9c 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0052cb9f 51                       push       ecx
0052cba0 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0052cba3 e868a2f5ff               call       0x486e10
0052cba8 ba04000000               mov        edx, 4
0052cbad 6bca00                   imul       ecx, edx, 0
0052cbb0 83bc08c800000000         cmp        dword ptr [eax + ecx + 0xc8], 0
0052cbb8 7407                     je         0x52cbc1
0052cbba b801000000               mov        eax, 1
0052cbbf eb07                     jmp        0x52cbc8
0052cbc1 e97cffffff               jmp        0x52cb42
0052cbc6 33c0                     xor        eax, eax
0052cbc8 8be5                     mov        esp, ebp
0052cbca 5d                       pop        ebp
0052cbcb c20800                   ret        8
0052cbce cc                       int3       
0052cbcf cc                       int3       
