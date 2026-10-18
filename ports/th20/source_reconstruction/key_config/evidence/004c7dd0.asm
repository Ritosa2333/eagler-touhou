004c7dd0 55                       push       ebp
004c7dd1 8bec                     mov        ebp, esp
004c7dd3 83ec10                   sub        esp, 0x10
004c7dd6 56                       push       esi
004c7dd7 894df8                   mov        dword ptr [ebp - 8], ecx
004c7dda 8b45f8                   mov        eax, dword ptr [ebp - 8]
004c7ddd 8b880c010000             mov        ecx, dword ptr [eax + 0x10c]
004c7de3 51                       push       ecx
004c7de4 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c7dea e831e0ffff               call       0x4c5e20
004c7def 8bc8                     mov        ecx, eax
004c7df1 e85ae0ffff               call       0x4c5e50
004c7df6 85c0                     test       eax, eax
004c7df8 740b                     je         0x4c7e05
004c7dfa 8b55f8                   mov        edx, dword ptr [ebp - 8]
004c7dfd 83c260                   add        edx, 0x60
004c7e00 8955fc                   mov        dword ptr [ebp - 4], edx
004c7e03 eb39                     jmp        0x4c7e3e
004c7e05 8b45f8                   mov        eax, dword ptr [ebp - 8]
004c7e08 8b880c010000             mov        ecx, dword ptr [eax + 0x10c]
004c7e0e 51                       push       ecx
004c7e0f 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c7e15 e806e0ffff               call       0x4c5e20
004c7e1a 8bc8                     mov        ecx, eax
004c7e1c e88fe0ffff               call       0x4c5eb0
004c7e21 85c0                     test       eax, eax
004c7e23 740e                     je         0x4c7e33
004c7e25 8b55f8                   mov        edx, dword ptr [ebp - 8]
004c7e28 81c280000000             add        edx, 0x80
004c7e2e 8955fc                   mov        dword ptr [ebp - 4], edx
004c7e31 eb0b                     jmp        0x4c7e3e
004c7e33 8b45f8                   mov        eax, dword ptr [ebp - 8]
004c7e36 05a0000000               add        eax, 0xa0
004c7e3b 8945fc                   mov        dword ptr [ebp - 4], eax
004c7e3e 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004c7e41 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c7e44 0fbf044a                 movsx      eax, word ptr [edx + ecx*2]
004c7e48 3b450c                   cmp        eax, dword ptr [ebp + 0xc]
004c7e4b 7505                     jne        0x4c7e52
004c7e4d e9a4000000               jmp        0x4c7ef6
004c7e52 c745f400000000           mov        dword ptr [ebp - 0xc], 0
004c7e59 eb09                     jmp        0x4c7e64
004c7e5b 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c7e5e 83c101                   add        ecx, 1
004c7e61 894df4                   mov        dword ptr [ebp - 0xc], ecx
004c7e64 8b55f8                   mov        edx, dword ptr [ebp - 8]
004c7e67 8b820c010000             mov        eax, dword ptr [edx + 0x10c]
004c7e6d 50                       push       eax
004c7e6e 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c7e74 e8a7dfffff               call       0x4c5e20
004c7e79 8bc8                     mov        ecx, eax
004c7e7b e800e0ffff               call       0x4c5e80
004c7e80 85c0                     test       eax, eax
004c7e82 7409                     je         0x4c7e8d
004c7e84 c745f008000000           mov        dword ptr [ebp - 0x10], 8
004c7e8b eb07                     jmp        0x4c7e94
004c7e8d c745f004000000           mov        dword ptr [ebp - 0x10], 4
004c7e94 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c7e97 3b4df0                   cmp        ecx, dword ptr [ebp - 0x10]
004c7e9a 7d2d                     jge        0x4c7ec9
004c7e9c 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004c7e9f 3b5508                   cmp        edx, dword ptr [ebp + 8]
004c7ea2 7423                     je         0x4c7ec7
004c7ea4 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004c7ea7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c7eaa 0fbf1441                 movsx      edx, word ptr [ecx + eax*2]
004c7eae 3b550c                   cmp        edx, dword ptr [ebp + 0xc]
004c7eb1 7514                     jne        0x4c7ec7
004c7eb3 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004c7eb6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c7eb9 8b5508                   mov        edx, dword ptr [ebp + 8]
004c7ebc 8b75fc                   mov        esi, dword ptr [ebp - 4]
004c7ebf 668b1456                 mov        dx, word ptr [esi + edx*2]
004c7ec3 66891441                 mov        word ptr [ecx + eax*2], dx
004c7ec7 eb92                     jmp        0x4c7e5b
004c7ec9 8b4508                   mov        eax, dword ptr [ebp + 8]
004c7ecc 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c7ecf 668b550c                 mov        dx, word ptr [ebp + 0xc]
004c7ed3 66891441                 mov        word ptr [ecx + eax*2], dx
004c7ed7 6a00                     push       0
004c7ed9 6a07                     push       7
004c7edb b930a85b00               mov        ecx, 0x5ba830
004c7ee0 e88beef5ff               call       0x426d70
004c7ee5 6a08                     push       8
004c7ee7 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c7eea 81c1ec000000             add        ecx, 0xec
004c7ef0 e82bb6f5ff               call       0x423520
004c7ef5 90                       nop        
004c7ef6 5e                       pop        esi
004c7ef7 8be5                     mov        esp, ebp
004c7ef9 5d                       pop        ebp
004c7efa c20800                   ret        8
004c7efd cc                       int3       
004c7efe cc                       int3       
004c7eff cc                       int3       
